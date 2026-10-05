#pragma once
#include <QMainWindow>
#include <QStackedWidget>

class SplashPage;
class LoginPage;
class AdminDashboard;
class CustomerDashboard;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

public slots:
    void showLogin();
    void showAdminDashboard(const QString& adminId);
    void showCustomerDashboard(const QString& username);
    void showSplash();

private:
    QStackedWidget*    m_stack;
    SplashPage*        m_splash;
    LoginPage*         m_login;
    AdminDashboard*    m_admin;
    CustomerDashboard* m_customer;
};
