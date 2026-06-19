#include "TrackerBackend.h"
#include "crypto.h"
#include <QThread>
#include <QMutexLocker>
#include <QHostAddress>
#include <QFileInfo>

TrackerBackend::TrackerBackend(QObject *parent) : QObject(parent) {
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &TrackerBackend::onNewConnection);
}

bool TrackerBackend::start(int port) {
    m_port = port;
    if (!m_server->listen(QHostAddress::Any, static_cast<quint16>(port))) {
        emit logMessage("Failed to listen on port " + QString::number(port) +
                        ": " + m_server->errorString());
        return false;
    }
    emit logMessage("Tracker listening on port " + QString::number(port));
    return true;
}

void TrackerBackend::stop() {
    m_server->close();
    emit logMessage("Tracker stopped");
}

bool TrackerBackend::isRunning() const { return m_server->isListening(); }

TrackerStats TrackerBackend::stats() const {
    QMutexLocker lk(&m_mx);
    TrackerStats s;
    s.users     = m_users.size();
    s.groups    = m_groups.size();
    s.connected = m_connected.size();
    for (auto &v : m_files) s.files += v.size();
    return s;
}

QStringList TrackerBackend::connectedUsers() const {
    QMutexLocker lk(&m_mx);
    return QStringList(m_connected.begin(), m_connected.end());
}

QStringList TrackerBackend::groupList() const {
    QMutexLocker lk(&m_mx);
    return m_groups.keys();
}

QList<FEntry> TrackerBackend::filesInGroup(const QString &gid) const {
    QMutexLocker lk(&m_mx);
    return m_files.value(gid);
}

void TrackerBackend::onNewConnection() {
    while (m_server->hasPendingConnections()) {
        QTcpSocket *sock = m_server->nextPendingConnection();
        QString addr = sock->peerAddress().toString() + ":" +
                       QString::number(sock->peerPort());
        {
            QMutexLocker lk(&m_mx);
            m_connected.insert(addr);
        }
        emit clientConnected(addr);
        emit statsChanged();

        // Handle on a background thread so multiple clients work concurrently
        QThread *t = QThread::create([this, sock, addr]() {
            handleClient(sock);
            {
                QMutexLocker lk(&m_mx);
                m_connected.remove(addr);
            }
            emit clientDisconnected(addr);
            emit statsChanged();
            sock->deleteLater();
        });
        sock->moveToThread(t);
        connect(t, &QThread::finished, t, &QObject::deleteLater);
        t->start();
    }
}

void TrackerBackend::handleClient(QTcpSocket *sock) {
    QString sessionUser;
    QString peerIp = sock->peerAddress().toString();
    QString buf;

    while (sock->state() == QAbstractSocket::ConnectedState ||
           sock->bytesAvailable() > 0) {
        if (!sock->waitForReadyRead(30000)) break;
        buf += QString::fromUtf8(sock->readAll());

        while (true) {
            int idx = buf.indexOf('\n');
            if (idx < 0) break;
            QString line = buf.left(idx).trimmed();
            buf = buf.mid(idx + 1);
            if (line.isEmpty()) continue;

            QString resp = processLine(line, sessionUser, peerIp);
            if (!resp.isEmpty()) {
                sock->write((resp + "\n").toUtf8());
                sock->flush();
            }
        }
    }

    if (!sessionUser.isEmpty()) {
        QMutexLocker lk(&m_mx);
        m_loggedIn.remove(sessionUser);
    }
}

QString TrackerBackend::processLine(const QString &line, QString &sessionUser,
                                    const QString &peerIp) {
    QStringList t = line.split(' ', Qt::SkipEmptyParts);
    if (t.isEmpty()) return {};
    const QString &cmd = t[0];
    emit logMessage("← [" + peerIp + "] " + line);

    QMutexLocker lk(&m_mx);

    // ── CREATE USER ──────────────────────────────────────────────────────────
    if (cmd == "create" && t.size() >= 4 && t[1] == "user") {
        const QString &uid = t[2], &pwd = t[3];
        if (m_users.contains(uid)) return "ERROR User exists";
        m_users[uid] = Crypto::sha256Hex(pwd);
        lk.unlock(); emit statsChanged();
        return "OK User created";
    }
    // ── LOGIN ────────────────────────────────────────────────────────────────
    if (cmd == "login" && t.size() >= 3) {
        if (!m_users.contains(t[1])) return "ERROR Invalid credentials";
        if (m_users[t[1]] != Crypto::sha256Hex(t[2])) return "ERROR Invalid credentials";
        sessionUser = t[1];
        m_loggedIn.insert(t[1]);
        lk.unlock(); emit statsChanged();
        return "OK User logged in";
    }
    // ── LOGOUT ───────────────────────────────────────────────────────────────
    if (cmd == "logout") {
        if (!sessionUser.isEmpty()) m_loggedIn.remove(sessionUser);
        sessionUser.clear();
        return "OK User logged out";
    }
    // All commands below require login
    if (sessionUser.isEmpty()) return "ERROR Not logged in";

    // ── CREATE GROUP ─────────────────────────────────────────────────────────
    if (cmd == "create" && t.size() >= 3 && t[1] == "group") {
        if (m_groups.contains(t[2])) return "ERROR Group exists";
        GrpInfo g; g.owner = sessionUser; g.members.insert(sessionUser);
        m_groups[t[2]] = g;
        lk.unlock(); emit statsChanged();
        return "OK Group created";
    }
    // ── JOIN GROUP ───────────────────────────────────────────────────────────
    if (cmd == "join" && t.size() >= 3 && t[1] == "group") {
        if (!m_groups.contains(t[2])) return "ERROR Group not found";
        auto &g = m_groups[t[2]];
        if (g.members.contains(sessionUser)) return "ERROR Already a member";
        g.requests.insert(sessionUser);
        return "OK Join request sent";
    }
    // ── LEAVE GROUP ──────────────────────────────────────────────────────────
    if (cmd == "leave" && t.size() >= 3 && t[1] == "group") {
        if (!m_groups.contains(t[2])) return "ERROR Group not found";
        auto &g = m_groups[t[2]];
        g.members.remove(sessionUser);
        if (g.owner == sessionUser) {
            if (g.members.isEmpty()) { m_groups.remove(t[2]); }
            else g.owner = *g.members.begin();
        }
        lk.unlock(); emit statsChanged();
        return "OK Left group";
    }
    // ── LIST GROUPS ──────────────────────────────────────────────────────────
    if (cmd == "list" && t.size() >= 2 && t[1] == "groups") {
        QStringList res;
        for (auto it = m_groups.begin(); it != m_groups.end(); ++it)
            res.append(it.key());
        return (res.isEmpty() ? "No groups available" : res.join("\n")) + "\nEND";
    }
    // ── LIST REQUESTS ────────────────────────────────────────────────────────
    if (cmd == "list" && t.size() >= 3 && t[1] == "requests") {
        if (!m_groups.contains(t[2])) return "ERROR Group not found\nEND";
        auto &g = m_groups[t[2]];
        if (g.owner != sessionUser) return "ERROR Not group owner\nEND";
        QStringList r(g.requests.begin(), g.requests.end());
        return (r.isEmpty() ? "No pending requests" : r.join("\n")) + "\nEND";
    }
    // ── ACCEPT REQUEST ───────────────────────────────────────────────────────
    if (cmd == "accept" && t.size() >= 4 && t[1] == "request") {
        if (!m_groups.contains(t[2])) return "ERROR Group not found";
        auto &g = m_groups[t[2]];
        if (g.owner != sessionUser) return "ERROR Not group owner";
        if (!g.requests.contains(t[3])) return "ERROR No such request";
        g.requests.remove(t[3]); g.members.insert(t[3]);
        return "OK Request accepted";
    }
    // ── UPLOAD FILE ──────────────────────────────────────────────────────────
    if (cmd == "upload" && t.size() >= 5 && t[1] == "file") {
        const QString &gid = t[2], &filepath = t[3];
        int port; bool ok; port = t[4].toInt(&ok);
        if (!ok) return "ERROR Invalid port";
        if (!m_groups.contains(gid)) return "ERROR Group not found";
        if (!m_groups[gid].members.contains(sessionUser)) return "ERROR Not a member";
        QFileInfo fi(filepath);
        if (!fi.exists()) return "ERROR File not found on uploader";
        QString fname  = fi.fileName();
        QString sha256 = Crypto::sha256File(filepath);
        FEntry e; e.filename = fname; e.owner = sessionUser;
        e.filepath = filepath; e.filesize = fi.size();
        e.sha256 = sha256; e.peerIp = peerIp; e.peerPort = port;
        auto &flist = m_files[gid];
        for (auto &f : flist) {
            if (f.filename == fname && f.owner == sessionUser) { f = e; lk.unlock(); emit statsChanged(); return "OK File uploaded"; }
        }
        flist.append(e);
        lk.unlock(); emit statsChanged();
        return "OK File uploaded";
    }
    // ── LIST FILES ───────────────────────────────────────────────────────────
    if (cmd == "list" && t.size() >= 3 && t[1] == "files") {
        auto &flist = m_files[t[2]];
        if (flist.isEmpty()) return "No files available\nEND";
        QStringList res;
        for (auto &f : flist)
            res.append(f.filename + " (owner: " + f.owner +
                       ", size: " + QString::number(f.filesize) +
                       ", sha256: " + f.sha256.left(12) + "...)");
        return res.join("\n") + "\nEND";
    }
    // ── STOP SHARE ───────────────────────────────────────────────────────────
    if (cmd == "stop" && t.size() >= 4 && t[1] == "share") {
        auto &flist = m_files[t[2]];
        for (int i = 0; i < flist.size(); ++i)
            if (flist[i].filename == t[3] && flist[i].owner == sessionUser) {
                flist.removeAt(i); lk.unlock(); emit statsChanged(); return "OK Stopped sharing";
            }
        return "ERROR File not found";
    }
    // ── DOWNLOAD FILE ────────────────────────────────────────────────────────
    if (cmd == "download" && t.size() >= 5 && t[1] == "file") {
        const QString &gid = t[2], &fname = t[3], &dest = t[4];
        if (!m_files.contains(gid)) return "ERROR File not found";
        for (auto &f : m_files[gid]) {
            if (f.filename == fname)
                return QString("DOWNLOAD %1:%2 %3 %4 %5 %6")
                    .arg(f.peerIp).arg(f.peerPort).arg(f.filename)
                    .arg(dest).arg(f.filesize).arg(f.sha256);
        }
        return "ERROR File not found";
    }
    // ── SHOW DOWNLOADS ───────────────────────────────────────────────────────
    if (cmd == "show" && t.size() >= 2 && t[1] == "downloads")
        return "No downloads\nEND";

    return "ERROR Unknown command";
}
