#pragma once
#include <QWidget>

class QTableWidget;
class QLineEdit;
class QPushButton;
class QLabel;
class QStackedWidget;
class QDoubleSpinBox;
class QSpinBox;
class QTextEdit;

class AdminDashboard : public QWidget {
    Q_OBJECT
public:
    explicit AdminDashboard(QWidget* parent = nullptr);
    void loadAdmin(const QString& adminId);

signals:
    void loggedOut();

protected:
    void paintEvent(QPaintEvent*) override;

private slots:
    void onAddItem();
    void onRemoveItem();
    void onUpdateItem();

private:
    QString m_adminId;

    QList<QPushButton*> m_navBtns;
    QStackedWidget*     m_stack;
    QLabel*             m_adminLabel;

    // Inventory
    QTableWidget*   m_table;
    QLineEdit*      m_fId, *m_fType, *m_fDest;
    QDoubleSpinBox* m_fPrice;
    QSpinBox*       m_fAvail;
    QLabel*         m_invMsg;

    // Stats
    QLabel* m_sUsers, *m_sItems;

    // Logs
    QTextEdit* m_logView;

    QWidget* buildOverview();
    QWidget* buildInventory();
    QWidget* buildLogs();

    void refreshTable();
    void refreshStats();
    void setNav(int idx);
    QString dp(const QString& f) const;
};
