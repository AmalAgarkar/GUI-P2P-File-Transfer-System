#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QString>
#include "protocol.h"

class TrackerSocket : public QObject {
    Q_OBJECT
public:
    explicit TrackerSocket(QObject *parent = nullptr);

    void connectToTracker(const QString &ip, int port);
    void sendCommand(const QString &cmd);
    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void lineReceived(const QString &line);
    void errorOccurred(const QString &msg);

private slots:
    void onReadyRead();
    void onError(QAbstractSocket::SocketError err);

private:
    QTcpSocket *m_sock;
    QString     m_buf;
};
