#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
#include <QMap>
#include <QSet>
#include <QList>
#include <QString>

struct GrpInfo {
    QString      owner;
    QSet<QString> members;
    QSet<QString> requests;
};

struct FEntry {
    QString filename;
    QString owner;
    QString filepath;
    qint64  filesize = 0;
    QString sha256;
    QString peerIp;
    int     peerPort = 0;
};

// Stats emitted to the GUI
struct TrackerStats {
    int users     = 0;
    int groups    = 0;
    int files     = 0;
    int connected = 0;
};

class TrackerBackend : public QObject {
    Q_OBJECT
public:
    explicit TrackerBackend(QObject *parent = nullptr);

    bool start(int port);
    void stop();
    bool isRunning() const;
    int  port() const { return m_port; }

    TrackerStats stats() const;
    QStringList  connectedUsers() const;
    QStringList  groupList() const;
    QList<FEntry> filesInGroup(const QString &gid) const;

signals:
    void statsChanged();
    void logMessage(const QString &msg);
    void clientConnected(const QString &addr);
    void clientDisconnected(const QString &addr);

private slots:
    void onNewConnection();

private:
    void handleClient(QTcpSocket *sock);
    QString processLine(const QString &line, QString &sessionUser, const QString &peerIp);

    // State (mutex-protected)
    mutable QMutex                  m_mx;
    QMap<QString, QString>          m_users;      // uid → hashed pwd
    QSet<QString>                   m_loggedIn;
    QMap<QString, GrpInfo>          m_groups;
    QMap<QString, QList<FEntry>>    m_files;
    QSet<QString>                   m_connected;  // addresses

    QTcpServer *m_server;
    int         m_port = 0;
};
