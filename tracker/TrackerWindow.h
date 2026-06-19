#pragma once
#include <QMainWindow>

class TrackerBackend;
class QLabel;
class QTableWidget;
class QTextEdit;
class QSpinBox;
class QPushButton;
class QTimer;

class TrackerWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit TrackerWindow(QWidget *parent = nullptr);

private slots:
    void onStartStop();
    void onStatsChanged();
    void onLog(const QString &msg);
    void onClientConnected(const QString &addr);
    void onClientDisconnected(const QString &addr);
    void refreshGroups();
    void refreshFiles();

private:
    void buildUi();

    TrackerBackend *m_backend;

    // Toolbar
    QSpinBox   *m_portSpin;
    QPushButton *m_btnStartStop;

    // Stat cards
    QLabel *m_cardUsers;
    QLabel *m_cardGroups;
    QLabel *m_cardFiles;
    QLabel *m_cardPeers;

    // Tables
    QTableWidget *m_connTable;
    QTableWidget *m_groupTable;
    QTableWidget *m_fileTable;

    // Log
    QTextEdit *m_logView;

    QTimer *m_refreshTimer;
};
