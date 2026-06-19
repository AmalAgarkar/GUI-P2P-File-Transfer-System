#include "MainWindow.h"
#include "LoginDialog.h"
#include "TrackerSocket.h"
#include "TransferManager.h"
#include "PeerServer.h"
#include "crypto.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QTabWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QProgressBar>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QGroupBox>
#include <QFrame>
#include <QTimer>
#include <QDateTime>

// ─── Constructor ─────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("P2P File Transfer — Client");
    resize(1100, 680);

    m_tracker = new TrackerSocket(this);
    m_xfer    = new TransferManager(this);
    m_server  = new PeerServer(this);

    connect(m_tracker, &TrackerSocket::lineReceived,  this, &MainWindow::onTrackerLine);
    connect(m_tracker, &TrackerSocket::connected,     this, &MainWindow::onTrackerConnected);
    connect(m_tracker, &TrackerSocket::disconnected,  this, &MainWindow::onTrackerDisconnected);
    connect(m_tracker, &TrackerSocket::errorOccurred, this, &MainWindow::onTrackerError);

    connect(m_xfer, &TransferManager::jobUpdated,  this, &MainWindow::onJobUpdated);
    connect(m_xfer, &TransferManager::jobFinished, this, &MainWindow::onJobFinished);
    connect(m_xfer, &TransferManager::log, this, &MainWindow::appendLog);
    connect(m_server, &PeerServer::log, this, &MainWindow::appendLog);

    buildUi();
    setConnected(false);

    // Show login dialog on startup
    QTimer::singleShot(0, this, [this]() {
        LoginDialog dlg(this);
        if (dlg.exec() != QDialog::Accepted) { close(); return; }

        m_username  = dlg.username();
        m_sharedDir = QDir::homePath() + "/p2p_shared";
        QDir().mkpath(m_sharedDir);

        m_userLabel->setText("👤  " + m_username);
        m_tracker->connectToTracker(dlg.trackerIp(), dlg.trackerPort());

        // After connect, auto-login (store credentials so we don't copy the dialog)
        QString loginUser = dlg.username();
        QString loginPass = dlg.password();
        connect(m_tracker, &TrackerSocket::connected, this, [this, loginUser, loginPass]() {
            sendCmd("login " + loginUser + " " + loginPass);
        }, Qt::SingleShotConnection);
    });
}

// ─── UI construction ─────────────────────────────────────────────────────────
void MainWindow::buildUi() {
    buildMenuBar();

    auto *central  = new QWidget(this);
    auto *rootHbox = new QHBoxLayout(central);
    rootHbox->setContentsMargins(0,0,0,0);
    rootHbox->setSpacing(0);
    setCentralWidget(central);

    buildSidebar();
    buildMainArea();
    buildTransferPanel();

    // Sidebar
    auto *sidebar = new QFrame;
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(200);
    auto *sl = new QVBoxLayout(sidebar);
    sl->setContentsMargins(0,0,0,0);
    sl->setSpacing(0);

    auto *grpLbl = new QLabel("  GROUPS");
    grpLbl->setObjectName("labelMuted");
    grpLbl->setStyleSheet("font-size:10px;letter-spacing:.08em;padding:10px 12px 4px;");
    sl->addWidget(grpLbl);

    m_groupList = new QListWidget;
    m_groupList->setFrameShape(QFrame::NoFrame);
    m_groupList->setStyleSheet("QListWidget::item{padding:7px 14px;border-left:2px solid transparent;}"
                               "QListWidget::item:selected{background:#1f6feb14;color:#58a6ff;"
                               "border-left:2px solid #58a6ff;}");
    sl->addWidget(m_groupList, 1);

    auto *grpBtns = new QFrame;
    grpBtns->setStyleSheet("background:#161b22;border-top:1px solid #21262d;");
    auto *gbl = new QHBoxLayout(grpBtns);
    gbl->setContentsMargins(8,6,8,6); gbl->setSpacing(4);
    auto *btnCG = new QPushButton("+ Group");
    auto *btnJG = new QPushButton("Join");
    btnCG->setFixedHeight(26); btnJG->setFixedHeight(26);
    gbl->addWidget(btnCG); gbl->addWidget(btnJG);
    sl->addWidget(grpBtns);

    connect(btnCG, &QPushButton::clicked, this, &MainWindow::doCreateGroup);
    connect(btnJG, &QPushButton::clicked, this, &MainWindow::doJoinGroup);
    connect(m_groupList, &QListWidget::itemClicked, this, &MainWindow::onGroupSelected);

    // Centre area with tabs
    auto *centre = new QWidget;
    auto *cl     = new QVBoxLayout(centre);
    cl->setContentsMargins(0,0,0,0); cl->setSpacing(0);

    // Toolbar
    auto *toolbar = new QFrame;
    toolbar->setStyleSheet("background:#161b22;border-bottom:1px solid #21262d;");
    auto *tl = new QHBoxLayout(toolbar);
    tl->setContentsMargins(10,6,10,6); tl->setSpacing(6);

    auto *btnUp   = new QPushButton("⬆  Upload");
    m_btnDownload = new QPushButton("⬇  Download");
    m_btnStop     = new QPushButton("✕  Stop share");
    auto *btnReq  = new QPushButton("📋  Requests");
    auto *btnLeave= new QPushButton("Leave group");

    btnUp->setObjectName("btnPrimary");
    m_btnStop->setObjectName("btnDanger");

    auto *search = new QLineEdit;
    search->setPlaceholderText("🔍  Filter files…");
    search->setFixedHeight(28);

    tl->addWidget(btnUp); tl->addWidget(m_btnDownload);
    tl->addWidget(m_btnStop); tl->addWidget(btnReq);
    tl->addWidget(btnLeave); tl->addStretch();
    tl->addWidget(search);
    cl->addWidget(toolbar);

    connect(btnUp,        &QPushButton::clicked, this, &MainWindow::doUpload);
    connect(m_btnDownload,&QPushButton::clicked, this, &MainWindow::doDownload);
    connect(m_btnStop,    &QPushButton::clicked, this, &MainWindow::doStopShare);
    connect(btnReq,       &QPushButton::clicked, this, &MainWindow::doListRequests);
    connect(btnLeave,     &QPushButton::clicked, this, &MainWindow::doLeaveGroup);

    // File table
    m_fileTable = new QTableWidget(0, 6);
    m_fileTable->setHorizontalHeaderLabels({"", "Filename", "Owner", "Size", "SHA-256", "Peer"});
    m_fileTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_fileTable->horizontalHeader()->setStretchLastSection(false);
    m_fileTable->setColumnWidth(0, 28);
    m_fileTable->setColumnWidth(2, 90);
    m_fileTable->setColumnWidth(3, 70);
    m_fileTable->setColumnWidth(4, 130);
    m_fileTable->setColumnWidth(5, 150);
    m_fileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_fileTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_fileTable->verticalHeader()->setVisible(false);
    m_fileTable->setShowGrid(false);
    m_fileTable->setAlternatingRowColors(false);
    connect(m_fileTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::onFileSelected);
    connect(search, &QLineEdit::textChanged, this, [this](const QString &t){
        for (int r = 0; r < m_fileTable->rowCount(); ++r) {
            auto *it = m_fileTable->item(r, 1);
            m_fileTable->setRowHidden(r, it && !it->text().contains(t, Qt::CaseInsensitive));
        }
    });
    cl->addWidget(m_fileTable, 1);

    // Right panel — tabs for Transfers and Log
    auto *rightPanel = new QFrame;
    rightPanel->setObjectName("rightPanel");
    rightPanel->setFixedWidth(280);
    auto *rl = new QVBoxLayout(rightPanel);
    rl->setContentsMargins(0,0,0,0); rl->setSpacing(0);

    auto *tabs = new QTabWidget;
    rl->addWidget(tabs, 1);

    // Transfers tab
    m_transferTable = new QTableWidget(0, 3);
    m_transferTable->setHorizontalHeaderLabels({"File", "Progress", "Status"});
    m_transferTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_transferTable->setColumnWidth(1, 80); m_transferTable->setColumnWidth(2, 70);
    m_transferTable->verticalHeader()->setVisible(false);
    m_transferTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_transferTable->setShowGrid(false);
    tabs->addTab(m_transferTable, "Transfers");

    // Log tab
    m_logView = new QTextEdit;
    m_logView->setReadOnly(true);
    m_logView->setStyleSheet("background:#0d1117;color:#3fb950;font-size:11px;border:none;");
    tabs->addTab(m_logView, "Log");

    // Status bar
    m_statusLabel = new QLabel("⚫  Disconnected");
    m_userLabel   = new QLabel;
    statusBar()->addWidget(m_statusLabel);
    statusBar()->addPermanentWidget(m_userLabel);
    statusBar()->addPermanentWidget(new QLabel("🛡 SHA-256 on  🔒 Passwords hashed"));

    rootHbox->addWidget(sidebar);
    rootHbox->addWidget(centre, 1);
    rootHbox->addWidget(rightPanel);
}

void MainWindow::buildMenuBar() {
    auto *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("Set shared folder", this, [this](){
        QString d = QFileDialog::getExistingDirectory(this, "Shared folder", m_sharedDir);
        if (!d.isEmpty()) { m_sharedDir = d; appendLog("Shared dir: " + d); }
    });
    fileMenu->addSeparator();
    fileMenu->addAction("Quit", qApp, &QApplication::quit);

    auto *p2pMenu = menuBar()->addMenu("P2P Server");
    p2pMenu->addAction("Start file server", this, [this](){
        bool ok;
        int port = QInputDialog::getInt(this, "P2P Port", "Listen port:", m_p2pPort, 1024, 65535, 1, &ok);
        if (!ok) return;
        m_p2pPort = port;
        m_server->stop();
        if (m_server->start(port, m_sharedDir))
            appendLog("P2P server started on :" + QString::number(port));
    });
    p2pMenu->addAction("Stop file server", this, [this](){
        m_server->stop(); appendLog("P2P server stopped");
    });
}

void MainWindow::buildSidebar()  {}
void MainWindow::buildMainArea() {}
void MainWindow::buildTransferPanel() {}
void MainWindow::buildStatusBar() {}

// ─── Tracker responses ───────────────────────────────────────────────────────
void MainWindow::onTrackerLine(const QString &line) {
    appendLog("← " + line);

    // Accumulate multi-line responses
    if (line == "END") {
        if (m_pendingCmd == "list_files") {
            // File list already rendered row-by-row; nothing extra to do
        } else if (m_pendingCmd == "list_groups") {
            // Groups already added
        } else if (m_pendingCmd == "list_requests") {
            if (m_pendingResponse.isEmpty()) {
                QMessageBox::information(this, "Requests", "No pending join requests.");
            } else {
                bool ok;
                QString uid = QInputDialog::getItem(this, "Accept request",
                    "Choose user to accept:", m_pendingResponse, 0, false, &ok);
                if (ok && !uid.isEmpty())
                    sendCmd("accept request " + m_currentGroup + " " + uid);
            }
        }
        m_pendingResponse.clear();
        m_pendingCmd.clear();
        return;
    }

    // DOWNLOAD directive — start transfer
    if (line.startsWith("DOWNLOAD ")) {
        Protocol::FileEntry entry;
        QString dest;
        if (Protocol::parseDownload(line, entry, dest)) {
            if (dest.isEmpty()) dest = QDir::homePath() + "/Downloads";
            m_xfer->startDownload(entry, dest);
            // Add row to transfer table
            int row = m_transferTable->rowCount();
            m_transferTable->insertRow(row);
            m_transferTable->setItem(row, 0, new QTableWidgetItem(entry.filename));
            auto *bar = new QProgressBar;
            bar->setRange(0, 100); bar->setValue(0);
            bar->setProperty("status", "active");
            m_transferTable->setCellWidget(row, 1, bar);
            m_transferTable->setItem(row, 2, new QTableWidgetItem("Downloading"));
        }
        return;
    }

    // File list row:  <filename> (owner: X, size: Y, sha256: Z...)
    if (m_pendingCmd == "list_files") {
        // Format: "filename (owner: X, size: Y, sha256: HASH...)"
        // Add row to file table
        int row = m_fileTable->rowCount();
        m_fileTable->insertRow(row);
        auto *icon = new QTableWidgetItem("📄");
        icon->setTextAlignment(Qt::AlignCenter);
        m_fileTable->setItem(row, 0, icon);

        // Extract filename (everything before " (owner:")
        int paren = line.indexOf(" (owner:");
        QString fname = (paren > 0) ? line.left(paren) : line;
        m_fileTable->setItem(row, 1, new QTableWidgetItem(fname));

        // Extract owner
        QRegularExpression re(R"(owner:\s*(\S+),)");
        auto m = re.match(line);
        if (m.hasMatch()) {
            auto *ownerItem = new QTableWidgetItem(m.captured(1));
            ownerItem->setForeground(QColor("#58a6ff"));
            m_fileTable->setItem(row, 2, ownerItem);
        }

        // Extract size
        QRegularExpression reSz(R"(size:\s*(\d+))");
        auto mSz = reSz.match(line);
        if (mSz.hasMatch()) {
            qint64 sz = mSz.captured(1).toLongLong();
            QString human;
            if (sz > 1024*1024*1024) human = QString::number(sz/1024/1024/1024.0, 'f', 1) + " GB";
            else if (sz > 1024*1024) human = QString::number(sz/1024/1024.0, 'f', 1) + " MB";
            else if (sz > 1024)      human = QString::number(sz/1024.0, 'f', 1) + " KB";
            else                     human = QString::number(sz) + " B";
            m_fileTable->setItem(row, 3, new QTableWidgetItem(human));
        }

        // Extract hash
        QRegularExpression reH(R"(sha256:\s*([0-9a-f]+))");
        auto mH = reH.match(line);
        if (mH.hasMatch()) {
            auto *hashItem = new QTableWidgetItem(mH.captured(1) + "...");
            hashItem->setForeground(QColor("#3fb950"));
            hashItem->setFont(QFont("monospace", 10));
            m_fileTable->setItem(row, 4, hashItem);
        }
        m_pendingResponse.append(fname);
        return;
    }

    // Group list row
    if (m_pendingCmd == "list_groups") {
        if (!line.startsWith("No ")) {
            bool found = false;
            for (int i = 0; i < m_groupList->count(); ++i)
                if (m_groupList->item(i)->text() == line) { found = true; break; }
            if (!found) {
                auto *item = new QListWidgetItem("👥  " + line);
                item->setData(Qt::UserRole, line);
                m_groupList->addItem(item);
            }
        }
        return;
    }

    // Requests list
    if (m_pendingCmd == "list_requests") {
        if (!line.startsWith("No ")) m_pendingResponse.append(line);
        return;
    }

    // Single-line responses
    if (line.startsWith("OK ")) {
        m_statusLabel->setText("✅  " + line.mid(3));
        // After login success, refresh groups
        if (line == "OK User logged in") {
            QTimer::singleShot(200, this, [this](){
                m_pendingCmd = "list_groups";
                sendCmd("list groups");
            });
        }
        // After group creation / join accepted, refresh
        if (line.startsWith("OK Group") || line.startsWith("OK Join") ||
            line.startsWith("OK Request accepted")) {
            QTimer::singleShot(200, this, [this](){
                m_pendingCmd = "list_groups";
                sendCmd("list groups");
            });
        }
    } else if (line.startsWith("ERROR ")) {
        QMessageBox::warning(this, "Error", line.mid(6));
    }
}

void MainWindow::onTrackerConnected() {
    setConnected(true);
    appendLog("Connected to tracker");
}

void MainWindow::onTrackerDisconnected() {
    setConnected(false);
    appendLog("Disconnected from tracker");
}

void MainWindow::onTrackerError(const QString &msg) {
    appendLog("Tracker error: " + msg);
    QMessageBox::critical(this, "Connection error", msg);
}

// ─── Transfer callbacks ───────────────────────────────────────────────────────
void MainWindow::onJobUpdated(int idx) {
    if (idx >= m_transferTable->rowCount()) return;
    const auto &job = m_xfer->jobs().at(idx);
    if (auto *bar = qobject_cast<QProgressBar*>(m_transferTable->cellWidget(idx, 1)))
        bar->setValue(job.progress);
}

void MainWindow::onJobFinished(int idx, bool ok) {
    if (idx >= m_transferTable->rowCount()) return;
    const auto &job = m_xfer->jobs().at(idx);
    if (auto *bar = qobject_cast<QProgressBar*>(m_transferTable->cellWidget(idx, 1))) {
        bar->setValue(ok ? 100 : job.progress);
        bar->setProperty("status", ok ? "done" : "failed");
        bar->style()->unpolish(bar); bar->style()->polish(bar);
    }
    auto *statusItem = m_transferTable->item(idx, 2);
    if (statusItem) statusItem->setText(ok ? "✅ Done" : "❌ Failed");
    appendLog(ok ? "Download complete: " + job.entry.filename
                 : "Download failed: " + job.entry.filename + " — " + job.errorMsg);
}

// ─── Group / file selection ──────────────────────────────────────────────────
void MainWindow::onGroupSelected(QListWidgetItem *item) {
    m_currentGroup = item->data(Qt::UserRole).toString();
    m_fileTable->setRowCount(0);
    m_pendingCmd = "list_files";
    sendCmd("list files " + m_currentGroup);
}

void MainWindow::onFileSelected() {
    bool hasSelection = !m_fileTable->selectedItems().isEmpty();
    m_btnDownload->setEnabled(hasSelection);
    m_btnStop->setEnabled(hasSelection);
}

// ─── Actions ─────────────────────────────────────────────────────────────────
void MainWindow::doUpload() {
    if (m_currentGroup.isEmpty()) {
        QMessageBox::information(this, "Upload", "Select a group first."); return;
    }
    QString file = QFileDialog::getOpenFileName(this, "Choose file to upload", m_sharedDir);
    if (file.isEmpty()) return;

    // Auto-start peer server if not running
    if (!m_server->port()) {
        m_server->start(m_p2pPort, m_sharedDir);
        appendLog("Auto-started P2P server on :" + QString::number(m_p2pPort));
    }
    sendCmd("upload file " + m_currentGroup + " " + file + " " + QString::number(m_p2pPort));
}

void MainWindow::doDownload() {
    int row = m_fileTable->currentRow();
    if (row < 0) return;
    auto *fnItem = m_fileTable->item(row, 1);
    if (!fnItem) return;

    QString dest = QFileDialog::getExistingDirectory(this, "Save to", QDir::homePath() + "/Downloads");
    if (dest.isEmpty()) return;
    sendCmd("download file " + m_currentGroup + " " + fnItem->text() + " " + dest);
}

void MainWindow::doStopShare() {
    int row = m_fileTable->currentRow();
    if (row < 0) return;
    auto *fnItem = m_fileTable->item(row, 1);
    if (!fnItem) return;
    if (QMessageBox::question(this, "Stop sharing",
        "Stop sharing "" + fnItem->text() + ""?") != QMessageBox::Yes) return;
    sendCmd("stop share " + m_currentGroup + " " + fnItem->text());
    m_fileTable->removeRow(row);
}

void MainWindow::doCreateGroup() {
    bool ok;
    QString gid = QInputDialog::getText(this, "Create group", "Group name:", QLineEdit::Normal, "", &ok);
    if (!ok || gid.isEmpty()) return;
    sendCmd("create group " + gid);
}

void MainWindow::doJoinGroup() {
    bool ok;
    QString gid = QInputDialog::getText(this, "Join group", "Group name:", QLineEdit::Normal, "", &ok);
    if (!ok || gid.isEmpty()) return;
    sendCmd("join group " + gid);
}

void MainWindow::doLeaveGroup() {
    if (m_currentGroup.isEmpty()) return;
    if (QMessageBox::question(this, "Leave group",
        "Leave group " + m_currentGroup + "?") != QMessageBox::Yes) return;
    sendCmd("leave group " + m_currentGroup);
    for (int i = 0; i < m_groupList->count(); ++i) {
        if (m_groupList->item(i)->data(Qt::UserRole).toString() == m_currentGroup) {
            delete m_groupList->takeItem(i); break;
        }
    }
    m_currentGroup.clear();
    m_fileTable->setRowCount(0);
}

void MainWindow::doListRequests() {
    if (m_currentGroup.isEmpty()) {
        QMessageBox::information(this, "Requests", "Select a group first."); return;
    }
    m_pendingCmd = "list_requests";
    m_pendingResponse.clear();
    sendCmd("list requests " + m_currentGroup);
}

void MainWindow::doAcceptRequest() {}

void MainWindow::refreshFiles() {
    if (!m_currentGroup.isEmpty()) {
        m_fileTable->setRowCount(0);
        m_pendingCmd = "list_files";
        sendCmd("list files " + m_currentGroup);
    }
}

// ─── Helpers ─────────────────────────────────────────────────────────────────
void MainWindow::sendCmd(const QString &cmd) {
    appendLog("→ " + cmd);
    m_tracker->sendCommand(cmd);
}

void MainWindow::appendLog(const QString &msg) {
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_logView->append("[" + ts + "] " + msg);
}

void MainWindow::setConnected(bool yes) {
    m_statusLabel->setText(yes ? "🟢  Connected" : "⚫  Disconnected");
}
