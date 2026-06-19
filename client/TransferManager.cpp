#include "TransferManager.h"
#include "crypto.h"
#include <QThread>
#include <QTcpSocket>
#include <QFile>
#include <QDir>
#include <QAtomicInt>
#include <QMutex>
#include <QList>
#include <functional>

TransferManager::TransferManager(QObject *parent) : QObject(parent) {}

void TransferManager::startDownload(const Protocol::FileEntry &entry, const QString &destDir) {
    TransferJob job;
    job.entry   = entry;
    job.destDir = destDir;
    job.status  = "downloading";
    job.progress = 0;
    m_jobs.append(job);
    int idx = m_jobs.size() - 1;

    // Run entire download on a background thread so UI stays responsive
    QThread *t = QThread::create([this, idx]() { downloadAsync(idx); });
    t->setParent(this);
    connect(t, &QThread::finished, t, &QObject::deleteLater);
    t->start();
}

bool TransferManager::downloadChunk(const QString &ip, int port,
                                     const QString &fname,
                                     const QString &tmpPath,
                                     qint64 offset, qint64 length) {
    for (int attempt = 1; attempt <= Protocol::MAX_RETRIES; ++attempt) {
        QTcpSocket sock;
        sock.connectToHost(ip, static_cast<quint16>(port));
        if (!sock.waitForConnected(5000)) continue;

        QString req = QString("GET %1 %2 %3\n").arg(fname).arg(offset).arg(length);
        sock.write(req.toUtf8());
        if (!sock.waitForBytesWritten(3000)) continue;

        QFile f(tmpPath);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;

        qint64 remaining = length;
        while (remaining > 0) {
            if (!sock.waitForReadyRead(5000)) break;
            QByteArray data = sock.read(qMin(remaining, (qint64)65536));
            if (data.isEmpty()) break;
            f.write(data);
            remaining -= data.size();
        }
        f.close();
        if (remaining == 0) return true;
        // Retry
        QThread::msleep(1000);
    }
    return false;
}

void TransferManager::downloadAsync(int idx) {
    TransferJob &job = m_jobs[idx];
    const Protocol::FileEntry &e = job.entry;

    qint64 filesize   = e.filesize;
    int    numChunks  = (int)((filesize + Protocol::DEFAULT_CHUNK_SIZE - 1)
                              / Protocol::DEFAULT_CHUNK_SIZE);
    if (numChunks == 0) numChunks = 1;

    QString destPath = job.destDir + "/" + e.filename;
    QDir().mkpath(job.destDir);

    QAtomicInt completed(0);
    QAtomicInt failed(0);

    // Download chunks in parallel threads
    QList<QThread *> threads;
    for (int i = 0; i < numChunks; ++i) {
        qint64 offset = (qint64)i * Protocol::DEFAULT_CHUNK_SIZE;
        qint64 length = qMin((qint64)Protocol::DEFAULT_CHUNK_SIZE, filesize - offset);
        QString tmp   = destPath + QString(".part%1").arg(i);

        QThread *t = QThread::create([this, idx, i, numChunks, offset, length,
                                       tmp, &e, &completed, &failed]() {
            bool ok = downloadChunk(e.peerIp, e.peerPort, e.filename, tmp, offset, length);
            if (!ok) {
                failed.fetchAndAddOrdered(1);
                emit log(QString("Chunk %1/%2 failed for %3").arg(i+1).arg(numChunks).arg(e.filename));
            } else {
                int done = completed.fetchAndAddOrdered(1) + 1;
                int pct  = done * 100 / numChunks;
                m_jobs[idx].progress = pct;
                emit jobUpdated(idx);
                emit log(QString("[%1/%2] %3% — %4").arg(done).arg(numChunks).arg(pct).arg(e.filename));
            }
        });
        threads.append(t);
        t->start();
    }
    for (auto *t : threads) { t->wait(); delete t; }

    if (failed.loadRelaxed() > 0) {
        // Clean up partial files
        for (int i = 0; i < numChunks; ++i)
            QFile::remove(destPath + QString(".part%1").arg(i));
        m_jobs[idx].status  = "failed";
        m_jobs[idx].errorMsg = "One or more chunks failed to download";
        emit jobFinished(idx, false);
        return;
    }

    // Merge chunks
    QFile out(destPath);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        m_jobs[idx].status = "failed";
        emit jobFinished(idx, false);
        return;
    }
    for (int i = 0; i < numChunks; ++i) {
        QString tmp = destPath + QString(".part%1").arg(i);
        QFile   in(tmp);
        if (in.open(QIODevice::ReadOnly)) {
            while (!in.atEnd()) out.write(in.read(65536));
            in.close();
        }
        QFile::remove(tmp);
    }
    out.close();

    // Integrity check
    if (!e.sha256.isEmpty()) {
        QString actual = Crypto::sha256File(destPath);
        if (actual != e.sha256) {
            QFile::remove(destPath);
            m_jobs[idx].status   = "failed";
            m_jobs[idx].errorMsg = "SHA-256 mismatch — file corrupted";
            emit log("Integrity FAILED for " + e.filename);
            emit jobFinished(idx, false);
            return;
        }
        emit log("Integrity OK for " + e.filename);
    }

    m_jobs[idx].progress = 100;
    m_jobs[idx].status   = "completed";
    emit jobUpdated(idx);
    emit jobFinished(idx, true);
}
