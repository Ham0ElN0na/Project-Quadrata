#pragma once
#include <QDialog>

class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;
class QTextEdit;

// Modal dialog shown before confirming a booking.
// Handles coupon code, loyalty points, price breakdown, receipt display.
class BookingDialog : public QDialog {
    Q_OBJECT
public:
    struct BookingInfo {
        QString itemType;
        QString destination;
        double  basePrice    = 0.0;
        int     loyaltyPts   = 0;
        double  balance      = 0.0;
        QString username;
        QString email;
    };

    explicit BookingDialog(const BookingInfo& info, QWidget* parent = nullptr);

    // Results after dialog accepted
    double  finalPrice()      const { return m_finalPrice; }
    bool    usedLoyalty()     const { return m_usedLoyalty; }
    QString appliedCoupon()   const { return m_appliedCoupon; }
    QString receiptText()     const { return m_receiptText; }

private slots:
    void onApplyCoupon();
    void onConfirm();
    void recalculate();

private:
    BookingInfo m_info;
    double      m_finalPrice    = 0.0;
    bool        m_usedLoyalty   = false;
    QString     m_appliedCoupon;
    QString     m_receiptText;

    // Widgets
    QLabel*    m_baseLabel;
    QLabel*    m_couponLabel;
    QLabel*    m_loyaltyLabel;
    QLabel*    m_totalLabel;
    QLabel*    m_balanceAfterLabel;
    QLabel*    m_errorLabel;
    QLineEdit* m_couponField;
    QCheckBox* m_loyaltyCheck;
    QPushButton* m_applyBtn;
    QPushButton* m_confirmBtn;

    QString buildReceipt() const;
};
