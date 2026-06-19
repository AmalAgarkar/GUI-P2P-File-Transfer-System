#pragma once
#include <QObject>
#include <QList>
#include "protocol.h"

struct TransferJob {
    Protocol::FileEntry entry;
    QString             destDir;
    int                 progress = 0;   // 0-100
    QString             status;         // "downloading" | "completed" | "failed"
    QString             errorMsg;
};

class TransferManager : public QObject {
    Q_OBJECT
public:
    explicit TransferManager(QObject *parent = nullptr);
    void startDownload(const Protocol::FileEntry &entry, const QString &destDir);
    const QList<TransferJob> &jobs() const { return m_jobs; }

signals:
    void jobUpdated(int index);
    void jobFinished(int index, bool success);
    void log(const QString &msg);

private:
    void downloadAsync(int jobIndex);
    bool downloadChunk(const QString &ip, int port, const QString &fname,
                       const QString &tmpPath, qint64 offset, qint64 length);

    QList<TransferJob> m_jobs;
};
