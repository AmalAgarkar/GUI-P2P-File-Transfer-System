#include "TrackerSocket.h"

TrackerSocket::TrackerSocket(QObject *parent) : QObject(parent) {
    m_sock = new QTcpSocket(this);
    connect(m_sock, &QTcpSocket::connected,    this, &TrackerSocket::connected);
    connect(m_sock, &QTcpSocket::disconnected, this, &TrackerSocket::disconnected);
    connect(m_sock, &QTcpSocket::readyRead,    this, &TrackerSocket::onReadyRead);
    connect(m_sock, &QAbstractSocket::errorOccurred, this, &TrackerSocket::onError);
}

void TrackerSocket::connectToTracker(const QString &ip, int port) {
    m_sock->connectToHost(ip, static_cast<quint16>(port));
}

void TrackerSocket::sendCommand(const QString &cmd) {
    if (m_sock->state() != QAbstractSocket::ConnectedState) return;
    m_sock->write((cmd + "\n").toUtf8());
}

bool TrackerSocket::isConnected() const {
    return m_sock->state() == QAbstractSocket::ConnectedState;
}

void TrackerSocket::onReadyRead() {
    m_buf += QString::fromUtf8(m_sock->readAll());
    while (true) {
        int idx = m_buf.indexOf('\n');
        if (idx < 0) break;
        QString line = m_buf.left(idx).trimmed();
        m_buf = m_buf.mid(idx + 1);
        if (!line.isEmpty())
            emit lineReceived(line);
    }
}

void TrackerSocket::onError(QAbstractSocket::SocketError) {
    emit errorOccurred(m_sock->errorString());
}
