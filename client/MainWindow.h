#pragma once
#include <QMainWindow>
#include <QStringList>
#include "protocol.h"

class TrackerSocket;
class TransferManager;
class PeerServer;
class QListWidget;
class QTableWidget;
class QProgressBar;
class QLabel;
class QSplitter;
class QTabWidget;
class QTextEdit;
class QPushButton;
class QListWidgetItem;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onTrackerLine(const QString &line);
    void onTrackerConnected();
    void onTrackerDisconnected();
    void onTrackerError(const QString &msg);

    void onJobUpdated(int idx);
    void onJobFinished(int idx, bool ok);

    void onGroupSelected(QListWidgetItem *item);
    void onFileSelected();

    void doUpload();
    void doDownload();
    void doStopShare();
    void doCreateGroup();
    void doJoinGroup();
    void doLeaveGroup();
    void doListRequests();
    void doAcceptRequest();

    void refreshFiles();

private:
    void buildUi();
    void buildMenuBar();
    void buildSidebar();
    void buildMainArea();
    void buildTransferPanel();
    void buildStatusBar();

    void sendCmd(const QString &cmd);
    void appendLog(const QString &msg);
    void updateTransferRow(int idx);
    void setConnected(bool yes);

    // Widgets
    QListWidget    *m_groupList   = nullptr;
    QTableWidget   *m_fileTable   = nullptr;
    QTableWidget   *m_transferTable = nullptr;
    QTextEdit      *m_logView     = nullptr;
    QLabel         *m_statusLabel = nullptr;
    QLabel         *m_userLabel   = nullptr;
    QPushButton    *m_btnDownload = nullptr;
    QPushButton    *m_btnStop     = nullptr;

    // Network
    TrackerSocket  *m_tracker  = nullptr;
    TransferManager *m_xfer    = nullptr;
    PeerServer      *m_server  = nullptr;

    QString         m_username;
    QString         m_currentGroup;
    QStringList     m_pendingResponse;
    QString         m_pendingCmd;
    int             m_p2pPort = 10000;
    QString         m_sharedDir;
};
