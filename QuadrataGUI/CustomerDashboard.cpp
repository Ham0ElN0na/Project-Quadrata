#include "CustomerDashboard.h"
#include "JsonUtils.h"
#include "BookingDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QDoubleSpinBox>
#include <QPainter>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFrame>
#include <QPixmap>
#include <QMessageBox>
#include <QDateTime>
#include <QDate>
#include <QInputDialog>
#include <QRegularExpression>
#include <QTextEdit>
#include <QDialog>

// ── Shared styles (pure black theme) ─────────────────────────────────────────
static const char* CN_ON = R"(
    QPushButton {
        background: #eeeeee; color: #111111;
        border: none; border-left: 2px solid #111111;
        text-align: left; padding: 13px 20px;
        font-size: 12px; font-weight: 600; letter-spacing: 1px; border-radius: 0;
    }
)";
static const char* CN_OFF = R"(
    QPushButton {
        background: transparent; color: #aaaaaa;
        border: none; border-left: 2px solid transparent;
        text-align: left; padding: 13px 20px;
        font-size: 12px; font-weight: 400; letter-spacing: 1px; border-radius: 0;
    }
    QPushButton:hover { background: #f5f5f5; color: #888888; }
)";
static const char* C_TBL = R"(
    QTableWidget {
        background: #ffffff;
        border: 1px solid #e8e8e8;
        border-radius: 10px;
        gridline-color: #eeeeee;
        color: #333333; font-size: 13px;
        selection-background-color: #e0e0e0;
    }
    QTableWidget::item { padding: 8px 12px; border: none; }
    QTableWidget::item:selected { background: #e0e0e0; color: #111111; }
    QHeaderView::section {
        background: #f5f5f5; color: #999999;
        border: none; border-bottom: 1px solid #e8e8e8;
        padding: 10px 12px; font-size: 10px; font-weight: 700; letter-spacing: 2px;
    }
    QTableWidget { alternate-background-color: #fafafa; }
)";
static const char* C_INP = R"(
    QLineEdit, QDoubleSpinBox {
        background: #f5f5f5;
        border: 1px solid #dddddd;
        border-radius: 7px; color: #111111; padding: 10px 12px; font-size: 13px;
    }
    QLineEdit:focus, QDoubleSpinBox:focus {
        border: 1px solid #999999; background: #ffffff;
    }
    QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
        background: #eeeeee; border: none; width: 16px;
    }
)";
static const char* C_BTN = R"(
    QPushButton {
        background: #f0f0f0; color: #333333;
        border: 1px solid #dddddd; border-radius: 7px;
        font-size: 12px; font-weight: 600; padding: 9px 18px;
    }
    QPushButton:hover { background: #e0e0e0; border-color: #bbbbbb; color:#000000; }
    QPushButton:pressed { background: #f0f0f0; }
)";
static const char* C_BTN_G = R"(
    QPushButton {
        background: #f0fff0; color: #228822;
        border: 1px solid #aaddaa; border-radius: 7px;
        font-size: 12px; font-weight: 600; padding: 9px 18px;
    }
    QPushButton:hover { background: #e0ffe0; color:#116611; }
    QPushButton:pressed { background: #d0f0d0; }
)";
static const char* C_BTN_RED = R"(
    QPushButton {
        background: #fff0f0; color: #cc3333;
        border: 1px solid #ffcccc; border-radius: 7px;
        font-size: 12px; font-weight: 600; padding: 9px 18px;
    }
    QPushButton:hover { background: #ffe0e0; color:#cc0000; }
    QPushButton:pressed { background: #ffd0d0; }
)";

static QLabel* fldLbl(const QString& t) {
    QLabel* l = new QLabel(t);
    l->setStyleSheet("color:#666666; font-size:10px; font-weight:600; "
                     "letter-spacing:1px; background:transparent;");
    return l;
}

QString CustomerDashboard::dp(const QString& f) const {
    QStringList c = {
        QCoreApplication::applicationDirPath() + "/" + f,
        QDir::currentPath() + "/" + f,
        "C:/Users/Lenovo/Downloads/Project Quadrata/Project Quadrata/Project Quadrata/" + f
    };
    for (auto& p : c) if (QFile::exists(p)) return p;
    return c.last();
}

CustomerDashboard::CustomerDashboard(QWidget* parent) : QWidget(parent) {

    // ── Sidebar ───────────────────────────────────────────────────────────────
    QWidget* sidebar = new QWidget(this);
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet(
        "QWidget { background: #f8f8f8; "
        "border-right: 1px solid #e8e8e8; }"
    );

    QLabel* logoLbl = new QLabel(sidebar);
    QPixmap logo(":/logo.png");
    if (!logo.isNull())
        logoLbl->setPixmap(logo.scaledToWidth(160, Qt::SmoothTransformation));
    else {
        logoLbl->setText("QUADRATA");
        logoLbl->setStyleSheet("color:#ffffff; font-size:14px; font-weight:900; "
                               "letter-spacing:4px; background:transparent;");
    }
    logoLbl->setAlignment(Qt::AlignCenter);

    QFrame* div = new QFrame(sidebar);
    div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("background:#e8e8e8; max-height:1px; border:none;");

    m_userLabel = new QLabel("", sidebar);
    m_userLabel->setStyleSheet(
        "color:#444444; font-size:10px; letter-spacing:1px; "
        "padding: 0 20px; background:transparent;"
    );

    QStringList navItems = { "Search & Book", "My Profile" };
    QVBoxLayout* sideVL = new QVBoxLayout(sidebar);
    sideVL->setContentsMargins(0, 16, 0, 0);
    sideVL->setSpacing(0);
    sideVL->addWidget(logoLbl);
    sideVL->addSpacing(12);
    sideVL->addWidget(div);
    sideVL->addSpacing(12);
    sideVL->addWidget(m_userLabel);
    sideVL->addSpacing(8);

    for (int i = 0; i < navItems.size(); ++i) {
        QPushButton* b = new QPushButton(navItems[i], sidebar);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(i == 0 ? CN_ON : CN_OFF);
        connect(b, &QPushButton::clicked, this, [this, i]{ setNav(i); });
        m_navBtns.append(b);
        sideVL->addWidget(b);
    }
    sideVL->addStretch();

    QPushButton* logoutBtn = new QPushButton("Logout", sidebar);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setStyleSheet(R"(
        QPushButton {
            background: transparent; color: #cc6666;
            border: none; border-top: 1px solid #eeeeee;
            text-align: left; padding: 13px 20px;
            font-size: 12px; letter-spacing: 1px; border-radius: 0;
        }
        QPushButton:hover { color: #cc3333; background: #fff5f5; }
    )");
    sideVL->addWidget(logoutBtn);
    sideVL->addSpacing(8);
    connect(logoutBtn, &QPushButton::clicked, this, &CustomerDashboard::loggedOut);

    // ── Content ───────────────────────────────────────────────────────────────
    m_stack = new QStackedWidget(this);
    m_stack->addWidget(buildSearchPage());
    m_stack->addWidget(buildProfilePage());

    QHBoxLayout* hl = new QHBoxLayout(this);
    hl->setContentsMargins(0,0,0,0); hl->setSpacing(0);
    hl->addWidget(sidebar);
    hl->addWidget(m_stack);
}

QWidget* CustomerDashboard::buildSearchPage() {
    QWidget* page = new QWidget();
    QVBoxLayout* vl = new QVBoxLayout(page);
    vl->setContentsMargins(36, 32, 36, 32); vl->setSpacing(16);

    QLabel* title = new QLabel("Search & Book");
    title->setStyleSheet("color:#111111; font-size:20px; font-weight:700; background:transparent;");
    vl->addWidget(title);

    QFrame* div = new QFrame(); div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("background:#eeeeee; max-height:1px; border:none;");
    vl->addWidget(div);

    // Search bar card
    QWidget* bar = new QWidget();
    bar->setStyleSheet(
        "QWidget { background: #f8f8f8; "
        "border: 1px solid #e8e8e8; border-radius: 10px; }"
    );
    QHBoxLayout* bl = new QHBoxLayout(bar);
    bl->setContentsMargins(20, 14, 20, 14); bl->setSpacing(16);

    QVBoxLayout* d1 = new QVBoxLayout();
    m_dest = new QLineEdit(); m_dest->setPlaceholderText("Destination"); m_dest->setStyleSheet(C_INP);
    d1->addWidget(fldLbl("DESTINATION")); d1->addWidget(m_dest);

    QVBoxLayout* d2 = new QVBoxLayout();
    m_minP = new QLineEdit(); m_minP->setPlaceholderText("0"); m_minP->setStyleSheet(C_INP); m_minP->setFixedWidth(100);
    d2->addWidget(fldLbl("MIN ($)")); d2->addWidget(m_minP);

    QVBoxLayout* d3 = new QVBoxLayout();
    m_maxP = new QLineEdit(); m_maxP->setPlaceholderText("Any"); m_maxP->setStyleSheet(C_INP); m_maxP->setFixedWidth(100);
    d3->addWidget(fldLbl("MAX ($)")); d3->addWidget(m_maxP);

    QPushButton* searchBtn = new QPushButton("Search", bar);
    searchBtn->setStyleSheet(C_BTN); searchBtn->setCursor(Qt::PointingHandCursor);
    searchBtn->setFixedHeight(40);

    bl->addLayout(d1, 3); bl->addLayout(d2); bl->addLayout(d3); bl->addWidget(searchBtn);
    vl->addWidget(bar);

    m_searchMsg = new QLabel("", page);
    m_searchMsg->setStyleSheet("color:#888888; font-size:11px; background:transparent;");
    m_searchMsg->hide();
    vl->addWidget(m_searchMsg);

    // Results table
    m_results = new QTableWidget(0, 5, page);
    m_results->setHorizontalHeaderLabels({"ID", "TYPE", "DESTINATION", "PRICE ($)", "AVAILABLE"});
    m_results->setStyleSheet(C_TBL);
    m_results->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_results->verticalHeader()->setVisible(false);
    m_results->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_results->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_results->setAlternatingRowColors(true);
    vl->addWidget(m_results, 1);

    // Book button
    QPushButton* bookBtn = new QPushButton("Book Selected", page);
    bookBtn->setStyleSheet(C_BTN_G); bookBtn->setCursor(Qt::PointingHandCursor);
    bookBtn->setFixedHeight(44); bookBtn->setFixedWidth(180);

    QHBoxLayout* brow = new QHBoxLayout();
    brow->addStretch(); brow->addWidget(bookBtn);
    vl->addLayout(brow);

    connect(searchBtn, &QPushButton::clicked, this, &CustomerDashboard::onSearch);
    connect(m_dest,    &QLineEdit::returnPressed, this, &CustomerDashboard::onSearch);
    connect(bookBtn,   &QPushButton::clicked, this, &CustomerDashboard::onBook);

    return page;
}

QWidget* CustomerDashboard::buildProfilePage() {
    QWidget* page = new QWidget();
    QVBoxLayout* vl = new QVBoxLayout(page);
    vl->setContentsMargins(36, 32, 36, 32); vl->setSpacing(20);

    QLabel* title = new QLabel("My Profile");
    title->setStyleSheet("color:#111111; font-size:20px; font-weight:700; background:transparent;");
    vl->addWidget(title);

    QFrame* div = new QFrame(); div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("background:#eeeeee; max-height:1px; border:none;");
    vl->addWidget(div);

    // Info card
    QWidget* card = new QWidget();
    card->setStyleSheet(
        "QWidget { background: #f8f8f8; "
        "border: 1px solid #e8e8e8; border-radius: 12px; }"
    );
    QGridLayout* gl = new QGridLayout(card);
    gl->setContentsMargins(28, 22, 28, 22); gl->setSpacing(20);

    auto makeField = [](const QString& lbl, QLabel*& val) -> QWidget* {
        QWidget* w = new QWidget(); w->setStyleSheet("background:transparent;");
        QVBoxLayout* l = new QVBoxLayout(w); l->setContentsMargins(0,0,0,0); l->setSpacing(4);
        QLabel* lb = new QLabel(lbl);
        lb->setStyleSheet("color:#999999; font-size:10px; letter-spacing:1px; background:transparent;");
        val = new QLabel("—");
        val->setStyleSheet("color:#111111; font-size:16px; font-weight:600; background:transparent;");
        l->addWidget(lb); l->addWidget(val);
        return w;
    };

    gl->addWidget(makeField("USERNAME",       m_pName),    0, 0);
    gl->addWidget(makeField("EMAIL",          m_pEmail),   0, 1);
    gl->addWidget(makeField("AGE",            m_pAge),     0, 2);
    gl->addWidget(makeField("BALANCE",        m_pBalance), 1, 0);
    gl->addWidget(makeField("LOYALTY POINTS", m_pPoints),  1, 1);
    vl->addWidget(card);

    // Add funds row
    QWidget* fundsRow = new QWidget();
    fundsRow->setStyleSheet(
        "QWidget { background: #f8f8f8; "
        "border: 1px solid #e8e8e8; border-radius: 10px; }"
    );
    QHBoxLayout* fl = new QHBoxLayout(fundsRow);
    fl->setContentsMargins(20, 14, 20, 14); fl->setSpacing(12);

    QLabel* fundsLbl = new QLabel("Add Funds:");
    fundsLbl->setStyleSheet("color:#333333; font-size:13px; background:transparent;");
    m_fundsAmt = new QDoubleSpinBox();
    m_fundsAmt->setRange(0, 999999); m_fundsAmt->setDecimals(2);
    m_fundsAmt->setStyleSheet(C_INP); m_fundsAmt->setFixedWidth(150);

    QPushButton* addBtn = new QPushButton("Add", fundsRow);
    addBtn->setStyleSheet(C_BTN_G); addBtn->setCursor(Qt::PointingHandCursor);

    m_fundsMsg = new QLabel("", fundsRow);
    m_fundsMsg->setStyleSheet("font-size:11px; background:transparent;"); m_fundsMsg->hide();

    fl->addWidget(fundsLbl); fl->addWidget(m_fundsAmt); fl->addWidget(addBtn);
    fl->addWidget(m_fundsMsg); fl->addStretch();
    vl->addWidget(fundsRow);

    // History + Cancellation
    QHBoxLayout* histHeader = new QHBoxLayout();
    QLabel* histTitle = new QLabel("Booking History");
    histTitle->setStyleSheet("color:#111111; font-size:14px; font-weight:600; background:transparent;");

    m_cancelBtn = new QPushButton("Cancel Selected Booking", page);
    m_cancelBtn->setStyleSheet(C_BTN_RED);
    m_cancelBtn->setCursor(Qt::PointingHandCursor);
    m_cancelBtn->setFixedHeight(36);

    histHeader->addWidget(histTitle);
    histHeader->addStretch();
    histHeader->addWidget(m_cancelBtn);
    vl->addLayout(histHeader);

    m_histTable = new QTableWidget(0, 2, page);
    m_histTable->setHorizontalHeaderLabels({"#", "ACTION"});
    m_histTable->setStyleSheet(C_TBL);
    m_histTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_histTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_histTable->verticalHeader()->setVisible(false);
    m_histTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_histTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_histTable->setMaximumHeight(200);
    vl->addWidget(m_histTable);
    vl->addStretch();

    connect(addBtn,       &QPushButton::clicked, this, &CustomerDashboard::onAddFunds);
    connect(m_cancelBtn,  &QPushButton::clicked, this, &CustomerDashboard::onCancelBooking);
    return page;
}

void CustomerDashboard::loadCustomer(const QString& username) {
    m_username = username;
    m_userLabel->setText("USER  ·  " + username.toUpper());
    refreshProfile();
    setNav(0);
}

void CustomerDashboard::setNav(int idx) {
    m_stack->setCurrentIndex(idx);
    for (int i = 0; i < m_navBtns.size(); ++i)
        m_navBtns[i]->setStyleSheet(i == idx ? CN_ON : CN_OFF);
    if (idx == 1) refreshProfile();
}

void CustomerDashboard::refreshProfile() {
    QJsonObject root = jsonRead(dp("Users.json"));
    if (root.isEmpty()) return;
    QJsonObject clients = root["Clients"].toObject();
    if (!clients.contains(m_username)) return;
    m_custData = clients[m_username].toObject();

    m_pName->setText(m_username);
    // Support both PascalCase (C++ app) and lowercase (GUI-created) keys
    m_pEmail->setText(m_custData.contains("Email")
        ? m_custData["Email"].toString() : m_custData["email"].toString());
    m_pAge->setText(QString::number(m_custData.contains("Age")
        ? m_custData["Age"].toInt() : m_custData["age"].toInt()));
    double bal = m_custData.contains("Balance")
        ? m_custData["Balance"].toDouble() : m_custData["balance"].toDouble();
    m_pBalance->setText("$" + QString::number(bal, 'f', 2));
    int pts = m_custData.contains("LoyaltyPoints")
        ? m_custData["LoyaltyPoints"].toInt() : m_custData["loyaltyPoints"].toInt();
    m_pPoints->setText(QString::number(pts) + " pts");
    populateHistory();
}

void CustomerDashboard::populateHistory() {
    m_histTable->setRowCount(0);
    QJsonArray hist = m_custData.contains("History")
        ? m_custData["History"].toArray() : QJsonArray();
    for (int i = 0; i < hist.size(); ++i) {
        const QJsonValue& v = hist[i];
        // C++ app stores {"Action":"..."}, GUI stores plain strings
        QString text = v.isObject() ? v.toObject()["Action"].toString() : v.toString();
        int row = m_histTable->rowCount();
        m_histTable->insertRow(row);
        auto* numItem = new QTableWidgetItem(QString::number(i + 1));
        numItem->setTextAlignment(Qt::AlignCenter);
        auto* txtItem = new QTableWidgetItem(text);
        txtItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_histTable->setItem(row, 0, numItem);
        m_histTable->setItem(row, 1, txtItem);
    }
}

void CustomerDashboard::onSearch() {
    m_results->setRowCount(0);
    QString dest = m_dest->text().trimmed().toLower();
    double minP  = m_minP->text().isEmpty() ? 0.0      : m_minP->text().toDouble();
    double maxP  = m_maxP->text().isEmpty() ? 999999.0 : m_maxP->text().toDouble();

    QJsonObject invObj = jsonRead(dp("Inventory.json"));
    if (invObj.isEmpty()) {
        m_searchMsg->setStyleSheet("color:#cc4444; font-size:11px; background:transparent;");
        m_searchMsg->setText("Cannot open Inventory.json"); m_searchMsg->show(); return;
    }
    QJsonArray arr = invObj["Inventory"].toArray();

    int found = 0;
    for (const QJsonValue& v : arr) {
        QJsonObject o = v.toObject();
        double price = o["price"].toDouble();
        int avail    = o["available"].toInt();
        if (avail <= 0) continue;
        if (!dest.isEmpty() && !o["destination"].toString().toLower().contains(dest)) continue;
        if (price < minP || price > maxP) continue;

        int row = m_results->rowCount(); m_results->insertRow(row);
        auto cell = [](const QString& t) {
            auto* i = new QTableWidgetItem(t); i->setTextAlignment(Qt::AlignCenter); return i;
        };
        m_results->setItem(row, 0, cell(o["id"].toString()));
        m_results->setItem(row, 1, cell(o["type"].toString()));
        m_results->setItem(row, 2, cell(o["destination"].toString()));
        m_results->setItem(row, 3, cell(QString::number(price, 'f', 2)));
        m_results->setItem(row, 4, cell(QString::number(avail)));
        ++found;
    }

    m_searchMsg->setStyleSheet("color:#888888; font-size:11px; background:transparent;");
    m_searchMsg->setText(found > 0
        ? QString("%1 result(s) found").arg(found)
        : "No results found.");
    m_searchMsg->show();
}

void CustomerDashboard::onBook() {
    int row = m_results->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No Selection", "Select an item from the results first.");
        return;
    }

    QString itemId   = m_results->item(row, 0)->text();
    QString itemType = m_results->item(row, 1)->text();
    QString itemDest = m_results->item(row, 2)->text();
    double  price    = m_results->item(row, 3)->text().toDouble();
    int     avail    = m_results->item(row, 4)->text().toInt();
    double  balance  = m_custData.contains("Balance")
        ? m_custData["Balance"].toDouble() : m_custData["balance"].toDouble();

    if (balance < price) {
        QMessageBox::warning(this, "Insufficient Balance",
            QString("Your balance ($%1) is less than the price ($%2).\nAdd funds from your profile.")
            .arg(balance, 0, 'f', 2).arg(price, 0, 'f', 2));
        return;
    }

    // ── Show booking dialog ───────────────────────────────────────────────────
    BookingDialog::BookingInfo info;
    info.itemType   = itemType;
    info.destination= itemDest;
    info.basePrice  = price;
    info.balance    = balance;
    info.username   = m_username;
    info.loyaltyPts = m_custData.contains("LoyaltyPoints")
        ? m_custData["LoyaltyPoints"].toInt() : m_custData["loyaltyPoints"].toInt();
    info.email      = m_custData.contains("Email")
        ? m_custData["Email"].toString() : m_custData["email"].toString();

    BookingDialog dlg(info, this);
    if (dlg.exec() != QDialog::Accepted) return;

    double finalPrice = dlg.finalPrice();

    // ── Update Users.json ─────────────────────────────────────────────────────
    QJsonObject uRoot = jsonRead(dp("Users.json"));
    QJsonObject clients = uRoot["Clients"].toObject();
    QJsonObject cust    = clients[m_username].toObject();
    QString balKey  = cust.contains("Balance")       ? "Balance"       : "balance";
    QString ptsKey  = cust.contains("LoyaltyPoints") ? "LoyaltyPoints" : "loyaltyPoints";
    QString histKey = cust.contains("History")       ? "History"       : "history";

    cust[balKey] = balance - finalPrice;

    // If loyalty was used, zero out the points
    if (dlg.usedLoyalty())
        cust[ptsKey] = 0;
    else
        cust[ptsKey] = cust[ptsKey].toInt() + static_cast<int>(finalPrice);

    QJsonArray hist = cust[histKey].toArray();
    hist.append(QString("Booked %1 to %2  ·  $%3  ·  %4")
        .arg(itemType, itemDest)
        .arg(finalPrice, 0, 'f', 2)
        .arg(QDateTime::currentDateTime().toString("dd MMM yyyy  hh:mm")));
    cust[histKey] = hist;
    clients[m_username] = cust; uRoot["Clients"] = clients;
    jsonWrite(dp("Users.json"), uRoot);

    // ── Update Inventory.json ─────────────────────────────────────────────────
    QJsonObject invRoot = jsonRead(dp("Inventory.json"));
    QJsonArray arr = invRoot["Inventory"].toArray();
    for (int i = 0; i < arr.size(); ++i) {
        QJsonObject o = arr[i].toObject();
        if (o["id"].toString() == itemId) { o["available"] = avail - 1; arr[i] = o; break; }
    }
    invRoot["Inventory"] = arr;
    jsonWrite(dp("Inventory.json"), invRoot);

    m_custData = cust;

    // ── Show receipt ──────────────────────────────────────────────────────────
    QDialog* receiptDlg = new QDialog(this);
    receiptDlg->setWindowTitle("Booking Receipt");
    receiptDlg->setMinimumSize(500, 420);
    receiptDlg->setStyleSheet("QDialog{background:#ffffff;}");

    QVBoxLayout* rl = new QVBoxLayout(receiptDlg);
    rl->setContentsMargins(24, 20, 24, 20);

    QTextEdit* receiptView = new QTextEdit(receiptDlg);
    receiptView->setReadOnly(true);
    receiptView->setPlainText(dlg.receiptText());
    receiptView->setStyleSheet(R"(
        QTextEdit {
            background: #fafafa;
            border: 1px solid #e8e8e8;
            border-radius: 8px;
            font-family: 'Consolas', monospace;
            font-size: 12px;
            color: #222222;
            padding: 12px;
        }
    )");

    QPushButton* closeBtn = new QPushButton("Close", receiptDlg);
    closeBtn->setStyleSheet(R"(
        QPushButton {
            background:#111111; color:#ffffff; border:none;
            border-radius:8px; font-size:13px; font-weight:700; padding:12px 32px;
        }
        QPushButton:hover{background:#333333;}
    )");
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, receiptDlg, &QDialog::accept);

    QHBoxLayout* closeBtnRow = new QHBoxLayout();
    closeBtnRow->addStretch(); closeBtnRow->addWidget(closeBtn);

    rl->addWidget(receiptView, 1);
    rl->addLayout(closeBtnRow);
    receiptDlg->exec();

    onSearch(); // refresh availability
}

void CustomerDashboard::onAddFunds() {
    m_fundsMsg->hide();
    double amount = m_fundsAmt->value();
    if (amount <= 0) {
        m_fundsMsg->setStyleSheet("color:#cc3333; font-size:11px; background:transparent;");
        m_fundsMsg->setText("Enter a positive amount."); m_fundsMsg->show(); return;
    }

    QJsonObject root = jsonRead(dp("Users.json"));
    QJsonObject clients = root["Clients"].toObject();
    QJsonObject cust    = clients[m_username].toObject();
    QString balKey      = cust.contains("Balance") ? "Balance" : "balance";
    cust[balKey]        = cust[balKey].toDouble() + amount;
    clients[m_username] = cust; root["Clients"] = clients;

    if (!jsonWrite(dp("Users.json"), root)) { return; }

    m_custData = cust;
    m_fundsMsg->setStyleSheet("color:#228844; font-size:11px; background:transparent;");
    m_fundsMsg->setText(QString("+$%1 added").arg(amount, 0, 'f', 2)); m_fundsMsg->show();
    m_fundsAmt->setValue(0);
    refreshProfile();
}

void CustomerDashboard::onCancelBooking() {
    int row = m_histTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No Selection",
            "Select a booking from the history table to cancel.");
        return;
    }

    // Column 1 holds the action text
    QString action = m_histTable->item(row, 1)->text();

    // Only bookings made via the GUI contain "·  $" — extract price
    // Format: "Booked TYPE to DEST  ·  $PRICE  ·  DATE"
    // Also handle C++ app format: "Booked item ITEMID"
    double bookingPrice = 0.0;
    bool   priceFound   = false;

    // Try GUI format first: look for "·  $"
    QRegularExpression re(R"(\·\s+\$([0-9]+\.?[0-9]*))");
    QRegularExpressionMatch m = re.match(action);
    if (m.hasMatch()) {
        bookingPrice = m.captured(1).toDouble();
        priceFound   = true;
    }

    if (!priceFound) {
        // C++ console app format — no price stored in history
        bool ok;
        bookingPrice = QInputDialog::getDouble(this,
            "Enter Booking Price",
            "Price not stored in this record.\nEnter the original booking price for refund calculation:",
            0.0, 0.0, 999999.0, 2, &ok);
        if (!ok) return;
        priceFound = true;
    }

    // Ask for travel date to compute refund tier
    QString travelDateStr = QInputDialog::getText(this,
        "Travel Date",
        "Enter the travel date (YYYY-MM-DD) to calculate refund:",
        QLineEdit::Normal, QDate::currentDate().addDays(7).toString("yyyy-MM-dd"));
    if (travelDateStr.isEmpty()) return;

    QDate travelDate = QDate::fromString(travelDateStr, "yyyy-MM-dd");
    if (!travelDate.isValid()) {
        QMessageBox::warning(this, "Invalid Date", "Please enter a valid date in YYYY-MM-DD format.");
        return;
    }

    // Calculate refund based on hours until travel (mirrors cancellationService logic)
    qint64 hoursUntilTravel = QDateTime::currentDateTime()
        .secsTo(QDateTime(travelDate, QTime(0,0))) / 3600;

    double refund = 0.0;
    QString tier;
    if (hoursUntilTravel >= 168) {
        refund = bookingPrice;
        tier   = "Full refund (>7 days before travel)";
    } else if (hoursUntilTravel >= 72) {
        refund = bookingPrice * 0.90;
        tier   = "90% refund (3–7 days before travel)";
    } else if (hoursUntilTravel > 0) {
        refund = bookingPrice * 0.50;
        tier   = "50% refund (<3 days before travel)";
    } else {
        refund = 0.0;
        tier   = "No refund (travel date has passed)";
    }

    int ret = QMessageBox::question(this, "Confirm Cancellation",
        QString("Booking:  %1\n\nOriginal price:  $%2\nRefund policy:  %3\nRefund amount:  $%4\n\nProceed with cancellation?")
        .arg(action)
        .arg(bookingPrice, 0, 'f', 2)
        .arg(tier)
        .arg(refund, 0, 'f', 2),
        QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    // Apply refund to balance and remove from history
    QJsonObject root    = jsonRead(dp("Users.json"));
    QJsonObject clients = root["Clients"].toObject();
    QJsonObject cust    = clients[m_username].toObject();
    QString balKey      = cust.contains("Balance") ? "Balance" : "balance";
    QString histKey     = cust.contains("History")  ? "History"  : "history";

    double newBalance   = cust[balKey].toDouble() + refund;
    cust[balKey]        = newBalance;

    // Remove the cancelled entry from history
    QJsonArray oldHist  = cust[histKey].toArray();
    QJsonArray newHist;
    for (int i = 0; i < oldHist.size(); ++i)
        if (i != row) newHist.append(oldHist[i]);
    cust[histKey] = newHist;

    clients[m_username] = cust;
    root["Clients"]     = clients;
    jsonWrite(dp("Users.json"), root);

    m_custData = cust;
    QMessageBox::information(this, "Cancellation Confirmed",
        QString("Booking cancelled.\n\nRefund: $%1\nNew balance: $%2")
        .arg(refund, 0, 'f', 2)
        .arg(newBalance, 0, 'f', 2));

    refreshProfile();
}

void CustomerDashboard::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), QColor(255, 255, 255));
}
