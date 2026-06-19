#include "crypto.h"
#include <openssl/sha.h>
#include <QFile>
#include <QCryptographicHash>

namespace Crypto {

QString sha256Hex(const QString &text) {
    return QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Sha256).toHex();
}

QString sha256File(const QString &filePath) {
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) return {};
    QCryptographicHash hash(QCryptographicHash::Sha256);
    while (!f.atEnd()) {
        QByteArray chunk = f.read(65536);
        hash.addData(chunk);
    }
    return hash.result().toHex();
}

bool safeFilename(const QString &fname) {
    if (fname.isEmpty()) return false;
    if (fname.contains('/') || fname.contains('\\')) return false;
    if (fname == ".." || fname == ".") return false;
    if (fname.contains("..")) return false;
    return true;
}

} // namespace Crypto
