#ifndef PEERSERVER_H
#define PEERSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QString>

class PeerServer : public QObject {
    Q_OBJECT
public:
    explicit PeerServer(QObject *parent = nullptr);
    bool start(int port, const QString &sharedDir);
    void stop();
    int port() const { return m_port; }

signals:
    void log(const QString &msg);

private slots:
    void onNewConnection();

private:
    QTcpServer *m_server = nullptr;
    QString m_dir;
    int m_port = 0;
};

#endif // PEERSERVER_H
