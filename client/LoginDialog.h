#pragma once
#include <QDialog>
#include <QString>

class QLineEdit;
class QSpinBox;

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);

    QString trackerIp()   const;
    int     trackerPort() const;
    QString username()    const;
    QString password()    const;

private:
    QLineEdit *m_ip;
    QSpinBox  *m_port;
    QLineEdit *m_user;
    QLineEdit *m_pass;
};
