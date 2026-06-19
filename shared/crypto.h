#pragma once
#include <QString>
#include <QByteArray>

namespace Crypto {
    QString sha256Hex(const QString &text);
    QString sha256File(const QString &filePath);
    bool safeFilename(const QString &fname);
}
