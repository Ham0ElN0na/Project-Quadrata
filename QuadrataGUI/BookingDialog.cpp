#include "BookingDialog.h"
#include "JsonUtils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QTextEdit>
#include <QFrame>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <cstdlib>
#include <ctime>

static const char* DLG_INPUT = R"(
    QLineEdit {
        background: #f5f5f5;
        border: 1px solid #dddddd;
        border-radius: 7px;
        color: #111111;
        padding: 10px 14px;
        font-size: 13px;
    }
    QLineEdit:focus { border: 1px solid #999999; background: #ffffff; }
)";

static const char* DLG_BTN = R"(
    QPushButton {
        background: #f0f0f0;
        color: #333333;
        border: 1px solid #dddddd;
        border-radius: 7px;
        font-size: 12px;
        font-weight: 600;
        padding: 9px 18px;
    }
    QPushButton:hover  { background: #e0e0e0; border-color: #bbbbbb; }
    QPushButton:pressed{ background: #d0d0d0; }
)";

static const char* DLG_BTN_PRIMARY = R"(
    QPushButton {
        background: #111111;
        color: #ffffff;
        border: none;
        border-radius: 8px;
        font-size: 14px;
        font-weight: 700;
        padding: 13px;
    }
    QPushButton:hover  { background: #333333; }
    QPushButton:pressed{ background: #000000; }
)";

BookingDialog::BookingDialog(const BookingInfo& info, QWidget* parent)
    : QDialog(parent), m_info(info), m_finalPrice(info.basePrice)
{
    setWindowTitle("Confirm Booking");
    setModal(true);
    setMinimumWidth(460);
    setStyleSheet("QDialog { background: #ffffff; }");

    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->setContentsMargins(32, 28, 32, 28);
    vl->setSpacing(16);

    // ── Title ─────────────────────────────────────────────────────────────────
    QLabel* title = new QLabel(
        QString("%1  →  %2").arg(info.itemType, info.destination), this);
    title->setStyleSheet("color:#111111; font-size:18px; font-weight:700; background:transparent;");
    vl->addWidget(title);

    QFrame* div = new QFrame(this);
    div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("background:#eeeeee; max-height:1px; border:none;");
    vl->addWidget(div);

    // ── Price breakdown card ──────────────────────────────────────────────────
    QWidget* card = new QWidget(this);
    card->setStyleSheet("QWidget{background:#f8f8f8; border:1px solid #eeeeee; border-radius:10px;}");
    QGridLayout* gl = new QGridLayout(card);
    gl->setContentsMargins(20, 16, 20, 16);
    gl->setSpacing(8);

    auto rowLabel = [](const QString& t) {
        QLabel* l = new QLabel(t);
        l->setStyleSheet("color:#888888; font-size:12px; background:transparent;");
        return l;
    };
    auto rowValue = [](QLabel*& lbl, const QString& init) {
        lbl = new QLabel(init);
        lbl->setStyleSheet("color:#111111; font-size:13px; font-weight:600; background:transparent;");
        lbl->setAlignment(Qt::AlignRight);
        return lbl;
    };

    gl->addWidget(rowLabel("Base price"),    0, 0);
    gl->addWidget(rowValue(m_baseLabel,    QString("$%1").arg(info.basePrice, 0, 'f', 2)), 0, 1);

    gl->addWidget(rowLabel("Coupon discount"), 1, 0);
    gl->addWidget(rowValue(m_couponLabel,  "—"), 1, 1);

    gl->addWidget(rowLabel("Loyalty discount"), 2, 0);
    gl->addWidget(rowValue(m_loyaltyLabel, "—"), 2, 1);

    QFrame* innerDiv = new QFrame(card);
    innerDiv->setFrameShape(QFrame::HLine);
    innerDiv->setStyleSheet("background:#e0e0e0; max-height:1px; border:none;");
    gl->addWidget(innerDiv, 3, 0, 1, 2);

    QLabel* totalLbl = new QLabel("Total");
    totalLbl->setStyleSheet("color:#111111; font-size:14px; font-weight:700; background:transparent;");
    gl->addWidget(totalLbl, 4, 0);
    m_totalLabel = new QLabel(QString("$%1").arg(info.basePrice, 0, 'f', 2));
    m_totalLabel->setStyleSheet("color:#111111; font-size:16px; font-weight:800; background:transparent;");
    m_totalLabel->setAlignment(Qt::AlignRight);
    gl->addWidget(m_totalLabel, 4, 1);

    gl->addWidget(rowLabel("Balance after"), 5, 0);
    m_balanceAfterLabel = new QLabel(
        QString("$%1").arg(info.balance - info.basePrice, 0, 'f', 2));
    m_balanceAfterLabel->setStyleSheet("color:#555555; font-size:12px; background:transparent;");
    m_balanceAfterLabel->setAlignment(Qt::AlignRight);
    gl->addWidget(m_balanceAfterLabel, 5, 1);

    vl->addWidget(card);

    // ── Coupon code ───────────────────────────────────────────────────────────
    QLabel* couponTitle = new QLabel("Coupon Code", this);
    couponTitle->setStyleSheet("color:#444444; font-size:11px; font-weight:600; letter-spacing:1px; background:transparent;");
    vl->addWidget(couponTitle);

    QHBoxLayout* couponRow = new QHBoxLayout();
    m_couponField = new QLineEdit(this);
    m_couponField->setPlaceholderText("e.g. AMMM4 or MAMM4");
    m_couponField->setStyleSheet(DLG_INPUT);
    m_applyBtn = new QPushButton("Apply", this);
    m_applyBtn->setStyleSheet(DLG_BTN);
    m_applyBtn->setCursor(Qt::PointingHandCursor);
    m_applyBtn->setFixedWidth(80);
    couponRow->addWidget(m_couponField);
    couponRow->addWidget(m_applyBtn);
    vl->addLayout(couponRow);

    // ── Loyalty points ────────────────────────────────────────────────────────
    if (info.loyaltyPts > 1000) {
        m_loyaltyCheck = new QCheckBox(
            QString("Use %1 loyalty points  (−$%2)")
            .arg(info.loyaltyPts)
            .arg(info.loyaltyPts / 10.0, 0, 'f', 2), this);
        m_loyaltyCheck->setStyleSheet(R"(
            QCheckBox { color:#444444; font-size:12px; spacing:8px; background:transparent; }
            QCheckBox::indicator { width:16px; height:16px; border-radius:4px;
                border:1px solid #cccccc; background:#f5f5f5; }
            QCheckBox::indicator:checked { background:#111111; border:1px solid #111111; }
        )");
        m_loyaltyCheck->setCursor(Qt::PointingHandCursor);
        vl->addWidget(m_loyaltyCheck);
        connect(m_loyaltyCheck, &QCheckBox::toggled, this, &BookingDialog::recalculate);
    } else {
        m_loyaltyCheck = nullptr;
        QLabel* noLoyalty = new QLabel(
            QString("Loyalty points: %1  (need >1000 to redeem)").arg(info.loyaltyPts), this);
        noLoyalty->setStyleSheet("color:#aaaaaa; font-size:11px; background:transparent;");
        vl->addWidget(noLoyalty);
    }

    // ── Error label ───────────────────────────────────────────────────────────
    m_errorLabel = new QLabel("", this);
    m_errorLabel->setStyleSheet("color:#cc3333; font-size:12px; background:transparent;");
    m_errorLabel->hide();
    vl->addWidget(m_errorLabel);

    // ── Buttons ───────────────────────────────────────────────────────────────
    QHBoxLayout* btnRow = new QHBoxLayout();
    QPushButton* cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->setStyleSheet(DLG_BTN);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    m_confirmBtn = new QPushButton("Confirm & Book", this);
    m_confirmBtn->setStyleSheet(DLG_BTN_PRIMARY);
    m_confirmBtn->setCursor(Qt::PointingHandCursor);
    m_confirmBtn->setFixedHeight(46);
    btnRow->addWidget(cancelBtn);
    btnRow->addStretch();
    btnRow->addWidget(m_confirmBtn);
    vl->addLayout(btnRow);

    connect(m_applyBtn,   &QPushButton::clicked, this, &BookingDialog::onApplyCoupon);
    connect(m_couponField,&QLineEdit::returnPressed, this, &BookingDialog::onApplyCoupon);
    connect(m_confirmBtn, &QPushButton::clicked, this, &BookingDialog::onConfirm);
    connect(cancelBtn,    &QPushButton::clicked, this, &QDialog::reject);
}

void BookingDialog::onApplyCoupon() {
    m_errorLabel->hide();
    QString code = m_couponField->text().trimmed().toUpper();
    if (code.isEmpty()) return;

    if (code == "AMMM4") {
        m_appliedCoupon = "AMMM4";
    } else if (code == "MAMM4") {
        m_appliedCoupon = "MAMM4";
    } else {
        m_errorLabel->setText("Invalid coupon code.");
        m_errorLabel->show();
        m_appliedCoupon.clear();
    }
    recalculate();
}

void BookingDialog::recalculate() {
    double price = m_info.basePrice;
    double couponAmt  = 0.0;
    double loyaltyAmt = 0.0;

    // Coupon
    if (m_appliedCoupon == "AMMM4") {
        couponAmt = price * 0.15;
        price    -= couponAmt;
        m_couponLabel->setText(QString("−$%1  (15% off)").arg(couponAmt, 0, 'f', 2));
        m_couponLabel->setStyleSheet("color:#228844; font-size:13px; font-weight:600; background:transparent;");
    } else if (m_appliedCoupon == "MAMM4") {
        couponAmt = price * 0.05;
        price    -= couponAmt;
        m_couponLabel->setText(QString("−$%1  (5% off)").arg(couponAmt, 0, 'f', 2));
        m_couponLabel->setStyleSheet("color:#228844; font-size:13px; font-weight:600; background:transparent;");
    } else {
        m_couponLabel->setText("—");
        m_couponLabel->setStyleSheet("color:#111111; font-size:13px; font-weight:600; background:transparent;");
    }

    // Loyalty
    if (m_loyaltyCheck && m_loyaltyCheck->isChecked()) {
        loyaltyAmt = m_info.loyaltyPts / 10.0;
        price     -= loyaltyAmt;
        if (price < 0) price = 0;
        m_loyaltyLabel->setText(QString("−$%1").arg(loyaltyAmt, 0, 'f', 2));
        m_loyaltyLabel->setStyleSheet("color:#228844; font-size:13px; font-weight:600; background:transparent;");
        m_usedLoyalty = true;
    } else {
        m_loyaltyLabel->setText("—");
        m_loyaltyLabel->setStyleSheet("color:#111111; font-size:13px; font-weight:600; background:transparent;");
        m_usedLoyalty = false;
    }

    m_finalPrice = price;
    m_totalLabel->setText(QString("$%1").arg(price, 0, 'f', 2));
    double balAfter = m_info.balance - price;
    m_balanceAfterLabel->setText(QString("$%1").arg(balAfter, 0, 'f', 2));

    // Warn if insufficient balance
    if (balAfter < 0) {
        m_balanceAfterLabel->setStyleSheet("color:#cc3333; font-size:12px; background:transparent;");
        m_confirmBtn->setEnabled(false);
        m_errorLabel->setText("Insufficient balance for this booking.");
        m_errorLabel->show();
    } else {
        m_balanceAfterLabel->setStyleSheet("color:#555555; font-size:12px; background:transparent;");
        m_confirmBtn->setEnabled(true);
        m_errorLabel->hide();
    }
}

void BookingDialog::onConfirm() {
    if (m_info.balance < m_finalPrice) {
        m_errorLabel->setText("Insufficient balance.");
        m_errorLabel->show();
        return;
    }
    m_receiptText = buildReceipt();
    accept();
}

QString BookingDialog::buildReceipt() const {
    srand(static_cast<unsigned>(time(nullptr)));
    int receiptId = 10000000 + rand() % 89999999;

    // Helper: left-pad a label and right-align a price on a 50-char line
    auto line = [](const QString& label, const QString& value) -> QString {
        // label takes up to 36 chars, value right-aligned in remaining space
        QString l = label.leftJustified(36, ' ', true);
        QString v = value.rightJustified(12, ' ');
        return l + v + "\n";
    };

    QString sep  = QString(50, '-');
    QString dsep = QString(50, '=');
    QString r;

    r += "\n" + dsep + "\n";
    r += "         QUADRATA TRAVEL RECEIPT\n";
    r += dsep + "\n";
    r += QString("Receipt ID  : #%1\n").arg(receiptId);
    r += QString("Customer    : %1\n").arg(m_info.username);
    if (!m_info.email.isEmpty())
        r += QString("Email       : %1\n").arg(m_info.email);
    r += QString("Date        : %1\n").arg(
        QDateTime::currentDateTime().toString("dd MMM yyyy  hh:mm"));
    r += sep + "\n";

    // Base price
    r += line("    " + m_info.itemType + " \u2192 " + m_info.destination,
              QString("$%1").arg(m_info.basePrice, 0, 'f', 2));

    // Coupon discount
    if (!m_appliedCoupon.isEmpty()) {
        double disc = (m_appliedCoupon == "AMMM4") ? m_info.basePrice * 0.15
                                                   : m_info.basePrice * 0.05;
        QString pct = (m_appliedCoupon == "AMMM4") ? "15%" : "5%";
        r += line("  - Coupon " + m_appliedCoupon + " (" + pct + " off)",
                  QString("-$%1").arg(disc, 0, 'f', 2));
    }

    // Loyalty discount
    if (m_usedLoyalty) {
        double loyaltyAmt = m_info.loyaltyPts / 10.0;
        r += line(QString("  - Loyalty points (%1 pts)").arg(m_info.loyaltyPts),
                  QString("-$%1").arg(loyaltyAmt, 0, 'f', 2));
    }

    r += sep + "\n";
    r += line("    TOTAL", QString("$%1").arg(m_finalPrice, 0, 'f', 2));
    r += dsep + "\n";
    r += "  Thank you for choosing Quadrata!\n";
    r += "  Have a wonderful trip.\n";
    r += dsep + "\n";
    return r;
}
