#pragma once
#include <QString>
#include <QStringList>

// ── Wire protocol helpers ────────────────────────────────────────────────────
namespace Protocol {

constexpr int DEFAULT_CHUNK_SIZE = 1024 * 1024; // 1 MB
constexpr int MAX_RETRIES        = 3;

struct FileEntry {
    QString filename;
    QString owner;
    QString filepath;
    qint64  filesize = 0;
    QString sha256;
    QString peerIp;
    int     peerPort = 0;
};

struct DownloadEntry {
    QString gid;
    QString filename;
    QString destination;
    QString status; // "queued" | "downloading" | "completed" | "failed"
    int     progress = 0;
};

// Parse a DOWNLOAD reply from the tracker:
// DOWNLOAD <ip:port> <filename> <dest> <filesize> <sha256>
inline bool parseDownload(const QString &line, FileEntry &out, QString &dest) {
    QStringList t = line.split(' ', Qt::SkipEmptyParts);
    if (t.size() < 6 || t[0] != "DOWNLOAD") return false;
    auto ep = t[1].split(':');
    if (ep.size() < 2) return false;
    out.peerIp   = ep[0];
    out.peerPort = ep[1].toInt();
    out.filename = t[2];
    dest         = t[3];
    out.filesize = t[4].toLongLong();
    out.sha256   = t[5];
    return true;
}

} // namespace Protocol
