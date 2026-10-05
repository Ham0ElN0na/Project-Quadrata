#pragma once
#include <QWidget>
#include <QStackedWidget>

class QLineEdit;
class QPushButton;
class QLabel;
class QCheckBox;

class LoginPage : public QWidget {
    Q_OBJECT
public:
    explicit LoginPage(QWidget* parent = nullptr);
    void reset();

signals:
    void adminLoggedIn(const QString& adminId);
    void customerLoggedIn(const QString& username);
    void backToSplash();

protected:
    void paintEvent(QPaintEvent*) override;

private slots:
    void onLoginClicked();
    void onSignUpClicked();

private:
    // Tab buttons
    QPushButton*    m_loginTab;
    QPushButton*    m_signupTab;
    QStackedWidget* m_formStack;

    // Login form
    QCheckBox*   m_adminToggle;   // checked = Admin, unchecked = Customer
    QLabel*      m_idLabel;
    QLineEdit*   m_idField;
    QLineEdit*   m_passField;
    QLabel*      m_loginError;

    // Sign-up form (customers only)
    QLineEdit*   m_suId;
    QLineEdit*   m_suName;
    QLineEdit*   m_suEmail;
    QLineEdit*   m_suPass;
    QLineEdit*   m_suAge;
    QLineEdit*   m_suBalance;
    QLabel*      m_signupMsg;

    void buildLoginForm(QWidget* w);
    void buildSignupForm(QWidget* w);
    void setTab(int idx);
    QString dataPath(const QString& f) const;
};
