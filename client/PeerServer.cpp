#include "PeerServer.h"
#include "crypto.h"
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>

// ── Per-connection worker ────────────────────────────────────────────────────
class PeerWorker : public QRunnable {
public:
    PeerWorker(qintptr fd, const QString &dir)
        : m_fd(fd), m_dir(dir) { setAutoDelete(true); }

    void run() override {
        QTcpSocket sock;
        if (!sock.setSocketDescriptor(m_fd)) return;
        sock.waitForReadyRead(5000);

        QString line = QString::fromUtf8(sock.readLine()).trimmed();
        QStringList t = line.split(' ', Qt::SkipEmptyParts);

        if (t.size() >= 2 && t[0] == "GET") {
            QString fname = t[1];
            if (!Crypto::safeFilename(fname)) {
                sock.write("ERROR forbidden\n"); sock.flush(); return;
            }
            qint64 offset = (t.size() >= 3) ? t[2].toLongLong() : 0;
            qint64 length = (t.size() >= 4) ? t[3].toLongLong() : -1;

            QFile f(m_dir + "/" + fname);
            if (!f.open(QIODevice::ReadOnly)) {
                sock.write("ERROR not found\n"); sock.flush(); return;
            }
            if (offset > 0) f.seek(offset);
            qint64 remaining = (length < 0) ? f.size() - offset : length;
            char buf[65536];
            while (remaining > 0 && sock.state() == QAbstractSocket::ConnectedState) {
                qint64 want = qMin((qint64)sizeof(buf), remaining);
                qint64 n    = f.read(buf, want);
                if (n <= 0) break;
                sock.write(buf, n);
                remaining -= n;
            }
            sock.flush();
        }
        sock.waitForDisconnected(3000);
    }

private:
    qintptr m_fd;
    QString m_dir;
};

// ── PeerServer ───────────────────────────────────────────────────────────────
PeerServer::PeerServer(QObject *parent) : QObject(parent) {
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &PeerServer::onNewConnection);
}

bool PeerServer::start(int port, const QString &sharedDir) {
    m_dir  = sharedDir;
    m_port = port;
    if (!m_server->listen(QHostAddress::Any, static_cast<quint16>(port))) {
        emit log("PeerServer: failed to listen on port " + QString::number(port));
        return false;
    }
    emit log("PeerServer: listening on port " + QString::number(port));
    return true;
}

void PeerServer::stop() {
    m_server->close();
}

void PeerServer::onNewConnection() {
    while (m_server->hasPendingConnections()) {
        QTcpSocket *s = m_server->nextPendingConnection();
        qintptr fd = s->socketDescriptor();
        s->setParent(nullptr);
        delete s; // worker takes over the fd
        QThreadPool::globalInstance()->start(new PeerWorker(fd, m_dir));
    }
}
