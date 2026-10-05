#include "MainWindow.h"
#include "SplashPage.h"
#include "LoginPage.h"
#include "AdminDashboard.h"
#include "CustomerDashboard.h"
#include <QScreen>
#include <QPalette>
#include <QGuiApplication>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Quadrata");
    setMinimumSize(960, 640);

    QScreen* scr = QGuiApplication::primaryScreen();
    QRect sg = scr->availableGeometry();
    resize(1100, 700);
    move((sg.width() - 1100) / 2, (sg.height() - 700) / 2);

    m_stack    = new QStackedWidget(this);
    m_splash   = new SplashPage(this);
    m_login    = new LoginPage(this);
    m_admin    = new AdminDashboard(this);
    m_customer = new CustomerDashboard(this);

    m_stack->addWidget(m_splash);    // 0
    m_stack->addWidget(m_login);     // 1
    m_stack->addWidget(m_admin);     // 2
    m_stack->addWidget(m_customer);  // 3
    setCentralWidget(m_stack);

    // Force white background on the window
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
    setAutoFillBackground(true);

    connect(m_splash,   &SplashPage::enterClicked,         this, &MainWindow::showLogin);
    connect(m_login,    &LoginPage::adminLoggedIn,          this, &MainWindow::showAdminDashboard);
    connect(m_login,    &LoginPage::customerLoggedIn,       this, &MainWindow::showCustomerDashboard);
    connect(m_login,    &LoginPage::backToSplash,           this, &MainWindow::showSplash);
    connect(m_admin,    &AdminDashboard::loggedOut,         this, &MainWindow::showSplash);
    connect(m_customer, &CustomerDashboard::loggedOut,      this, &MainWindow::showSplash);

    m_stack->setCurrentIndex(0);
}

void MainWindow::showLogin()                              { m_login->reset(); m_stack->setCurrentIndex(1); }
void MainWindow::showAdminDashboard(const QString& id)   { m_admin->loadAdmin(id);       m_stack->setCurrentIndex(2); }
void MainWindow::showCustomerDashboard(const QString& u) { m_customer->loadCustomer(u);  m_stack->setCurrentIndex(3); }
void MainWindow::showSplash()                            { m_stack->setCurrentIndex(0); }
