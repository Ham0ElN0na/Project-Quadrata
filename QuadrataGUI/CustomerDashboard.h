#pragma once
#include <QWidget>
#include <QJsonObject>

class QTableWidget;
class QLineEdit;
class QPushButton;
class QLabel;
class QStackedWidget;
class QDoubleSpinBox;
class CustomerDashboard : public QWidget {
    Q_OBJECT
public:
    explicit CustomerDashboard(QWidget* parent = nullptr);
    void loadCustomer(const QString& username);

signals:
    void loggedOut();

protected:
    void paintEvent(QPaintEvent*) override;

private slots:
    void onSearch();
    void onBook();
    void onAddFunds();
    void onCancelBooking();

private:
    QString     m_username;
    QJsonObject m_custData;

    QList<QPushButton*> m_navBtns;
    QStackedWidget*     m_stack;
    QLabel*             m_userLabel;

    // Search page
    QLineEdit*    m_dest;
    QLineEdit*    m_minP;
    QLineEdit*    m_maxP;
    QTableWidget* m_results;
    QLabel*       m_searchMsg;

    // Profile page
    QLabel*         m_pName;
    QLabel*         m_pEmail;
    QLabel*         m_pAge;
    QLabel*         m_pBalance;
    QLabel*         m_pPoints;
    QDoubleSpinBox* m_fundsAmt;
    QLabel*         m_fundsMsg;
    QTableWidget*   m_histTable;
    QPushButton*    m_cancelBtn;   // cancel selected booking

    QWidget* buildSearchPage();
    QWidget* buildProfilePage();

    void setNav(int idx);
    void refreshProfile();
    void populateHistory();
    QString dp(const QString& f) const;
};
