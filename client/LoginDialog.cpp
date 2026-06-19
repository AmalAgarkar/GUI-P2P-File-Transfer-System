#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QFrame>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("P2P File Transfer — Connect");
    setFixedSize(320, 320);
    setModal(true);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 28, 28, 28);
    root->setSpacing(16);

    // Header
    auto *title = new QLabel("🔗  P2P File Transfer");
    title->setStyleSheet("font-size:16px; font-weight:600; color:#e6edf3;");
    auto *sub = new QLabel("Connect to a tracker and sign in");
    sub->setObjectName("labelMuted");
    root->addWidget(title);
    root->addWidget(sub);

    // Divider
    auto *div = new QFrame; div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("border-color:#21262d;");
    root->addWidget(div);

    // Form
    auto *form = new QFormLayout;
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignRight);

    m_ip = new QLineEdit("127.0.0.1");
    m_port = new QSpinBox;
    m_port->setRange(1, 65535);
    m_port->setValue(9000);

    auto *trackerRow = new QHBoxLayout;
    trackerRow->addWidget(m_ip);
    trackerRow->addWidget(m_port);

    m_user = new QLineEdit;
    m_user->setPlaceholderText("username");

    m_pass = new QLineEdit;
    m_pass->setPlaceholderText("password");
    m_pass->setEchoMode(QLineEdit::Password);

    form->addRow("Tracker IP", m_ip);
    form->addRow("Port",       m_port);
    form->addRow("Username",   m_user);
    form->addRow("Password",   m_pass);

    root->addLayout(form);
    root->addStretch();

    auto *btn = new QPushButton("Sign in");
    btn->setObjectName("btnPrimary");
    btn->setMinimumHeight(36);
    btn->setDefault(true);
    root->addWidget(btn);

    connect(btn, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_pass, &QLineEdit::returnPressed, this, &QDialog::accept);
}

QString LoginDialog::trackerIp()   const { return m_ip->text().trimmed(); }
int     LoginDialog::trackerPort() const { return m_port->value(); }
QString LoginDialog::username()    const { return m_user->text().trimmed(); }
QString LoginDialog::password()    const { return m_pass->text(); }
