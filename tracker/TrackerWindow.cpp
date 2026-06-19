#include "TrackerWindow.h"
#include "TrackerBackend.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QTextEdit>
#include <QTabWidget>
#include <QFrame>
#include <QGroupBox>
#include <QTimer>
#include <QDateTime>
#include <QSplitter>
#include <QStatusBar>

// ── Stat card helper ─────────────────────────────────────────────────────────
static QFrame *makeStatCard(const QString &title, QLabel *&valueLabel) {
    auto *card = new QFrame;
    card->setStyleSheet(
        "QFrame{background:#161b22;border:1px solid #21262d;"
        "border-radius:8px;padding:4px;}"
    );
    auto *vl = new QVBoxLayout(card);
    vl->setContentsMargins(14, 10, 14, 10);
    vl->setSpacing(4);

    auto *lbl = new QLabel(title);
    lbl->setStyleSheet("font-size:10px;color:#484f58;text-transform:uppercase;letter-spacing:.06em;");

    valueLabel = new QLabel("0");
    valueLabel->setStyleSheet("font-size:26px;font-weight:600;color:#e6edf3;");

    vl->addWidget(lbl);
    vl->addWidget(valueLabel);
    return card;
}

// ─── TrackerWindow ───────────────────────────────────────────────────────────
TrackerWindow::TrackerWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("P2P File Transfer — Tracker");
    resize(980, 660);

    m_backend = new TrackerBackend(this);
    connect(m_backend, &TrackerBackend::statsChanged,       this, &TrackerWindow::onStatsChanged);
    connect(m_backend, &TrackerBackend::logMessage,         this, &TrackerWindow::onLog);
    connect(m_backend, &TrackerBackend::clientConnected,    this, &TrackerWindow::onClientConnected);
    connect(m_backend, &TrackerBackend::clientDisconnected, this, &TrackerWindow::onClientDisconnected);

    buildUi();

    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &TrackerWindow::refreshGroups);
    m_refreshTimer->start(3000);
}

void TrackerWindow::buildUi() {
    auto *central = new QWidget(this);
    auto *root    = new QVBoxLayout(central);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);
    setCentralWidget(central);

    // ── Top toolbar ──────────────────────────────────────────────────────────
    auto *toolbar = new QFrame;
    toolbar->setStyleSheet("background:#161b22;border-bottom:1px solid #21262d;");
    auto *tl = new QHBoxLayout(toolbar);
    tl->setContentsMargins(14,8,14,8); tl->setSpacing(10);

    auto *logoLbl = new QLabel("📡  P2P Tracker");
    logoLbl->setStyleSheet("font-size:15px;font-weight:600;color:#e6edf3;");

    auto *portLbl = new QLabel("Port:");
    portLbl->setStyleSheet("color:#8b949e;font-size:12px;");
    m_portSpin = new QSpinBox;
    m_portSpin->setRange(1024, 65535);
    m_portSpin->setValue(9000);
    m_portSpin->setFixedWidth(80);

    m_btnStartStop = new QPushButton("▶  Start");
    m_btnStartStop->setObjectName("btnPrimary");
    m_btnStartStop->setFixedHeight(30);
    m_btnStartStop->setMinimumWidth(90);

    tl->addWidget(logoLbl);
    tl->addStretch();
    tl->addWidget(portLbl);
    tl->addWidget(m_portSpin);
    tl->addWidget(m_btnStartStop);

    connect(m_btnStartStop, &QPushButton::clicked, this, &TrackerWindow::onStartStop);
    root->addWidget(toolbar);

    // ── Stat cards ────────────────────────────────────────────────────────────
    auto *statsRow = new QFrame;
    statsRow->setStyleSheet("background:#0d1117;padding:12px;");
    auto *sl = new QHBoxLayout(statsRow);
    sl->setContentsMargins(14,12,14,12); sl->setSpacing(10);
    sl->addWidget(makeStatCard("Registered users", m_cardUsers));
    sl->addWidget(makeStatCard("Groups",           m_cardGroups));
    sl->addWidget(makeStatCard("Shared files",     m_cardFiles));
    sl->addWidget(makeStatCard("Connected peers",  m_cardPeers));
    root->addWidget(statsRow);

    // ── Main split area ───────────────────────────────────────────────────────
    auto *splitter = new QSplitter(Qt::Horizontal);
    splitter->setHandleWidth(1);
    splitter->setStyleSheet("QSplitter::handle{background:#21262d;}");

    // Left: tab widget with connected / groups / files
    auto *tabs = new QTabWidget;
    tabs->setStyleSheet("QTabWidget::pane{border:none;}");

    // Connected peers table
    m_connTable = new QTableWidget(0, 3);
    m_connTable->setHorizontalHeaderLabels({"Address", "Connected at", "Status"});
    m_connTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_connTable->setColumnWidth(1, 140); m_connTable->setColumnWidth(2, 80);
    m_connTable->verticalHeader()->setVisible(false);
    m_connTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_connTable->setShowGrid(false);
    tabs->addTab(m_connTable, "🔌  Peers");

    // Groups table
    m_groupTable = new QTableWidget(0, 3);
    m_groupTable->setHorizontalHeaderLabels({"Group", "Owner", "Members"});
    m_groupTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_groupTable->setColumnWidth(1, 110); m_groupTable->setColumnWidth(2, 70);
    m_groupTable->verticalHeader()->setVisible(false);
    m_groupTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_groupTable->setShowGrid(false);
    connect(m_groupTable, &QTableWidget::itemSelectionChanged, this, &TrackerWindow::refreshFiles);
    tabs->addTab(m_groupTable, "👥  Groups");

    // Files table
    m_fileTable = new QTableWidget(0, 5);
    m_fileTable->setHorizontalHeaderLabels({"Filename", "Owner", "Size", "SHA-256", "Peer"});
    m_fileTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_fileTable->setColumnWidth(1, 80); m_fileTable->setColumnWidth(2, 70);
    m_fileTable->setColumnWidth(3, 120); m_fileTable->setColumnWidth(4, 130);
    m_fileTable->verticalHeader()->setVisible(false);
    m_fileTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_fileTable->setShowGrid(false);
    tabs->addTab(m_fileTable, "📁  Files");

    splitter->addWidget(tabs);

    // Right: log
    auto *logBox = new QFrame;
    logBox->setStyleSheet("background:#0d1117;");
    auto *ll = new QVBoxLayout(logBox);
    ll->setContentsMargins(0,0,0,0); ll->setSpacing(0);
    auto *logHdr = new QLabel("  📋  Event log");
    logHdr->setStyleSheet("background:#161b22;border-bottom:1px solid #21262d;"
                          "padding:8px 12px;font-size:11px;color:#8b949e;");
    m_logView = new QTextEdit;
    m_logView->setReadOnly(true);
    m_logView->setStyleSheet("background:#0d1117;color:#3fb950;font-size:11px;border:none;padding:6px;");
    ll->addWidget(logHdr);
    ll->addWidget(m_logView, 1);

    auto *clearBtn = new QPushButton("Clear log");
    clearBtn->setStyleSheet("margin:6px;");
    connect(clearBtn, &QPushButton::clicked, m_logView, &QTextEdit::clear);
    ll->addWidget(clearBtn);

    splitter->addWidget(logBox);
    splitter->setSizes({620, 340});

    root->addWidget(splitter, 1);

    // Status bar
    statusBar()->showMessage("⚫  Tracker not running");
    statusBar()->setStyleSheet("background:#1f6feb;color:#fff;font-size:11px;");
}

// ─── Slots ───────────────────────────────────────────────────────────────────
void TrackerWindow::onStartStop() {
    if (m_backend->isRunning()) {
        m_backend->stop();
        m_btnStartStop->setText("▶  Start");
        m_btnStartStop->setObjectName("btnPrimary");
        m_portSpin->setEnabled(true);
        statusBar()->showMessage("⚫  Tracker stopped");
        onLog("Tracker stopped");
    } else {
        int port = m_portSpin->value();
        if (m_backend->start(port)) {
            m_btnStartStop->setText("⏹  Stop");
            m_btnStartStop->setObjectName("btnDanger");
            m_portSpin->setEnabled(false);
            statusBar()->showMessage("🟢  Tracker running on port " + QString::number(port));
        }
    }
    // Force stylesheet refresh
    m_btnStartStop->style()->unpolish(m_btnStartStop);
    m_btnStartStop->style()->polish(m_btnStartStop);
}

void TrackerWindow::onStatsChanged() {
    auto s = m_backend->stats();
    m_cardUsers->setText(QString::number(s.users));
    m_cardGroups->setText(QString::number(s.groups));
    m_cardFiles->setText(QString::number(s.files));
    m_cardPeers->setText(QString::number(s.connected));
}

void TrackerWindow::onLog(const QString &msg) {
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_logView->append("[" + ts + "] " + msg);
}

void TrackerWindow::onClientConnected(const QString &addr) {
    int row = m_connTable->rowCount();
    m_connTable->insertRow(row);
    m_connTable->setItem(row, 0, new QTableWidgetItem(addr));
    m_connTable->setItem(row, 1, new QTableWidgetItem(
        QDateTime::currentDateTime().toString("hh:mm:ss")));
    auto *status = new QTableWidgetItem("🟢 online");
    status->setForeground(QColor("#3fb950"));
    m_connTable->setItem(row, 2, status);
    onLog("Client connected: " + addr);
}

void TrackerWindow::onClientDisconnected(const QString &addr) {
    for (int r = 0; r < m_connTable->rowCount(); ++r) {
        if (m_connTable->item(r, 0) &&
            m_connTable->item(r, 0)->text() == addr) {
            auto *status = m_connTable->item(r, 2);
            if (status) { status->setText("⚫ gone"); status->setForeground(QColor("#484f58")); }
            break;
        }
    }
    onLog("Client disconnected: " + addr);
}

void TrackerWindow::refreshGroups() {
    if (!m_backend->isRunning()) return;
    QStringList groups = m_backend->groupList();
    m_groupTable->setRowCount(0);
    for (auto &g : groups) {
        int row = m_groupTable->rowCount();
        m_groupTable->insertRow(row);
        m_groupTable->setItem(row, 0, new QTableWidgetItem(g));
        // Owner and members would come from backend; placeholder
        m_groupTable->setItem(row, 1, new QTableWidgetItem("—"));
        auto files = m_backend->filesInGroup(g);
        m_groupTable->setItem(row, 2, new QTableWidgetItem(
            QString::number(files.size()) + " files"));
    }
}

void TrackerWindow::refreshFiles() {
    m_fileTable->setRowCount(0);
    auto sel = m_groupTable->selectedItems();
    if (sel.isEmpty()) return;
    QString gid = m_groupTable->item(sel.first()->row(), 0)->text();
    auto files = m_backend->filesInGroup(gid);
    for (auto &f : files) {
        int row = m_fileTable->rowCount();
        m_fileTable->insertRow(row);
        m_fileTable->setItem(row, 0, new QTableWidgetItem(f.filename));
        auto *ownerItem = new QTableWidgetItem(f.owner);
        ownerItem->setForeground(QColor("#58a6ff"));
        m_fileTable->setItem(row, 1, ownerItem);

        QString human;
        if (f.filesize > 1024*1024*1024LL) human = QString::number(f.filesize/1024/1024/1024.0,'f',1)+" GB";
        else if (f.filesize > 1024*1024) human = QString::number(f.filesize/1024/1024.0,'f',1)+" MB";
        else if (f.filesize > 1024)      human = QString::number(f.filesize/1024.0,'f',1)+" KB";
        else                             human = QString::number(f.filesize)+" B";
        m_fileTable->setItem(row, 2, new QTableWidgetItem(human));

        auto *hashItem = new QTableWidgetItem(f.sha256.left(12) + "...");
        hashItem->setForeground(QColor("#3fb950"));
        m_fileTable->setItem(row, 3, hashItem);
        m_fileTable->setItem(row, 4, new QTableWidgetItem(f.peerIp+":"+QString::number(f.peerPort)));
    }
}
