#include "AdminDashboard.h"
#include "JsonUtils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QPainter>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFrame>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>

// ── Style helpers ─────────────────────────────────────────────────────────────
static const char* S_NAV_ON = R"(
    QPushButton {
        background: #eeeeee; color: #111111;
        border: none; border-left: 2px solid #111111;
        text-align: left; padding: 13px 20px;
        font-size: 12px; font-weight: 600; letter-spacing: 1px; border-radius: 0;
    }
)";
static const char* S_NAV_OFF = R"(
    QPushButton {
        background: transparent; color: #aaaaaa;
        border: none; border-left: 2px solid transparent;
        text-align: left; padding: 13px 20px;
        font-size: 12px; font-weight: 400; letter-spacing: 1px; border-radius: 0;
    }
    QPushButton:hover { background: #f5f5f5; color: #888888; }
)";
static const char* S_TABLE = R"(
    QTableWidget {
        background: #ffffff;
        border: 1px solid #e8e8e8;
        border-radius: 10px;
        gridline-color: #eeeeee;
        color: #333333;
        font-size: 13px;
        selection-background-color: #e0e0e0;
    }
    QTableWidget::item { padding: 8px 12px; border: none; }
    QTableWidget::item:selected { background: #e0e0e0; color: #111111; }
    QHeaderView::section {
        background: #f5f5f5;
        color: #999999;
        border: none;
        border-bottom: 1px solid #e8e8e8;
        padding: 10px 12px;
        font-size: 10px; font-weight: 700; letter-spacing: 2px;
    }
)";
static const char* S_INPUT = R"(
    QLineEdit, QDoubleSpinBox, QSpinBox {
        background: #f5f5f5;
        border: 1px solid #dddddd;
        border-radius: 6px; color: #111111; padding: 8px 10px; font-size: 12px;
    }
    QLineEdit:focus, QDoubleSpinBox:focus, QSpinBox:focus {
        border: 1px solid #999999; background: #ffffff;
    }
    QDoubleSpinBox::up-button, QDoubleSpinBox::down-button,
    QSpinBox::up-button, QSpinBox::down-button {
        background: #eeeeee; border: none; width: 16px;
    }
)";
static const char* S_BTN = R"(
    QPushButton {
        background: #f0f0f0; color: #333333;
        border: 1px solid #dddddd; border-radius: 7px;
        font-size: 12px; font-weight: 600; padding: 8px 16px;
    }
    QPushButton:hover { background: #e0e0e0; border-color: #bbbbbb; color:#000000; }
    QPushButton:pressed { background: #e8e8e8; }
)";
static const char* S_BTN_RED = R"(
    QPushButton {
        background: #fff0f0; color: #cc3333;
        border: 1px solid #ffcccc; border-radius: 7px;
        font-size: 12px; font-weight: 600; padding: 8px 16px;
    }
    QPushButton:hover { background: #ffe0e0; color:#cc0000; }
    QPushButton:pressed { background: #ffd0d0; }
)";

static QLabel* sectionLabel(const QString& t) {
    QLabel* l = new QLabel(t);
    l->setStyleSheet("color: rgba(160,190,220,140); font-size: 10px; "
                     "letter-spacing: 1px; background: transparent;");
    return l;
}

QString AdminDashboard::dp(const QString& f) const {
    QStringList c = {
        QCoreApplication::applicationDirPath() + "/" + f,
        QDir::currentPath() + "/" + f,
        "C:/Users/Lenovo/Downloads/Project Quadrata/Project Quadrata/Project Quadrata/" + f
    };
    for (auto& p : c) if (QFile::exists(p)) return p;
    return c.last();
}

AdminDashboard::AdminDashboard(QWidget* parent) : QWidget(parent) {

    // ── Sidebar ───────────────────────────────────────────────────────────────
    QWidget* sidebar = new QWidget(this);
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet(
        "QWidget { background: #f8f8f8; "
        "border-right: 1px solid #e8e8e8; }"
    );

    // Small logo
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
    div->setStyleSheet("background: #e8e8e8; max-height:1px; border:none;");

    m_adminLabel = new QLabel("", sidebar);
    m_adminLabel->setStyleSheet(
        "color: #444444; font-size:10px; letter-spacing:1px; "
        "padding: 0 20px; background:transparent;"
    );

    QStringList navItems = { "Overview", "Inventory", "System Logs" };
    QVBoxLayout* sideVL = new QVBoxLayout(sidebar);
    sideVL->setContentsMargins(0, 16, 0, 0);
    sideVL->setSpacing(0);
    sideVL->addWidget(logoLbl);
    sideVL->addSpacing(12);
    sideVL->addWidget(div);
    sideVL->addSpacing(12);
    sideVL->addWidget(m_adminLabel);
    sideVL->addSpacing(8);

    for (int i = 0; i < navItems.size(); ++i) {
        QPushButton* b = new QPushButton(navItems[i], sidebar);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(i == 0 ? S_NAV_ON : S_NAV_OFF);
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
    connect(logoutBtn, &QPushButton::clicked, this, &AdminDashboard::loggedOut);

    // ── Content ───────────────────────────────────────────────────────────────
    m_stack = new QStackedWidget(this);
    m_stack->addWidget(buildOverview());
    m_stack->addWidget(buildInventory());
    m_stack->addWidget(buildLogs());

    QHBoxLayout* hl = new QHBoxLayout(this);
    hl->setContentsMargins(0,0,0,0); hl->setSpacing(0);
    hl->addWidget(sidebar);
    hl->addWidget(m_stack);
}

void AdminDashboard::setNav(int idx) {
    m_stack->setCurrentIndex(idx);
    for (int i = 0; i < m_navBtns.size(); ++i)
        m_navBtns[i]->setStyleSheet(i == idx ? S_NAV_ON : S_NAV_OFF);
    if (idx == 0) refreshStats();
    if (idx == 2) {
        QFile f(dp("log.txt"));
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            m_logView->setPlainText(QString::fromUtf8(f.readAll())); f.close();
            m_logView->moveCursor(QTextCursor::End);
        }
    }
}

QWidget* AdminDashboard::buildOverview() {
    QWidget* page = new QWidget();
    QVBoxLayout* vl = new QVBoxLayout(page);
    vl->setContentsMargins(36, 32, 36, 32); vl->setSpacing(24);

    QLabel* title = new QLabel("Overview");
    title->setStyleSheet("color:#111111; font-size:20px; font-weight:700; background:transparent;");
    vl->addWidget(title);

    QFrame* div = new QFrame(); div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("background:#eeeeee; max-height:1px; border:none;");
    vl->addWidget(div);

    // Stat cards
    auto makeCard = [](const QString& label, QLabel*& val) -> QWidget* {
        QWidget* c = new QWidget();
        c->setStyleSheet(
            "QWidget { background: #f8f8f8; "
            "border: 1px solid #e8e8e8; border-radius: 12px; }"
        );
        QVBoxLayout* cl = new QVBoxLayout(c);
        cl->setContentsMargins(24, 20, 24, 20); cl->setSpacing(6);
        val = new QLabel("—");
        val->setStyleSheet("color:#111111; font-size:30px; font-weight:700; background:transparent;");
        QLabel* lbl = new QLabel(label);
        lbl->setStyleSheet("color:#999999; font-size:10px; letter-spacing:2px; background:transparent;");
        cl->addWidget(val); cl->addWidget(lbl);
        return c;
    };

    QHBoxLayout* row = new QHBoxLayout(); row->setSpacing(16);
    row->addWidget(makeCard("REGISTERED USERS",   m_sUsers));
    row->addWidget(makeCard("INVENTORY ITEMS",     m_sItems));
    vl->addLayout(row);
    vl->addStretch();
    return page;
}

QWidget* AdminDashboard::buildInventory() {
    QWidget* page = new QWidget();
    QVBoxLayout* vl = new QVBoxLayout(page);
    vl->setContentsMargins(36, 32, 36, 32); vl->setSpacing(16);

    QLabel* title = new QLabel("Inventory");
    title->setStyleSheet("color:#111111; font-size:20px; font-weight:700; background:transparent;");
    vl->addWidget(title);

    QFrame* div = new QFrame(); div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("background:#eeeeee; max-height:1px; border:none;");
    vl->addWidget(div);

    // Table
    m_table = new QTableWidget(0, 5, page);
    m_table->setHorizontalHeaderLabels({"ID", "TYPE", "DESTINATION", "PRICE", "AVAILABLE"});
    m_table->setStyleSheet(S_TABLE);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setStyleSheet(QString(S_TABLE) + "QTableWidget{alternate-background-color:#fafafa;}");
    vl->addWidget(m_table, 1);

    // Form
    QWidget* form = new QWidget();
    form->setStyleSheet(
        "QWidget { background: #f8f8f8; "
        "border: 1px solid #e8e8e8; border-radius: 10px; }"
    );
    QGridLayout* gl = new QGridLayout(form);
    gl->setContentsMargins(20, 16, 20, 16); gl->setSpacing(10);

    m_fId    = new QLineEdit(); m_fId->setPlaceholderText("ID");          m_fId->setStyleSheet(S_INPUT);
    m_fType  = new QLineEdit(); m_fType->setPlaceholderText("Type");      m_fType->setStyleSheet(S_INPUT);
    m_fDest  = new QLineEdit(); m_fDest->setPlaceholderText("Destination"); m_fDest->setStyleSheet(S_INPUT);
    m_fPrice = new QDoubleSpinBox(); m_fPrice->setRange(0,99999); m_fPrice->setDecimals(2); m_fPrice->setStyleSheet(S_INPUT);
    m_fAvail = new QSpinBox();       m_fAvail->setRange(0,9999);  m_fAvail->setStyleSheet(S_INPUT);

    gl->addWidget(sectionLabel("ID"),          0,0); gl->addWidget(m_fId,    1,0);
    gl->addWidget(sectionLabel("TYPE"),        0,1); gl->addWidget(m_fType,  1,1);
    gl->addWidget(sectionLabel("DESTINATION"), 0,2); gl->addWidget(m_fDest,  1,2);
    gl->addWidget(sectionLabel("PRICE"),       0,3); gl->addWidget(m_fPrice, 1,3);
    gl->addWidget(sectionLabel("AVAILABLE"),   0,4); gl->addWidget(m_fAvail, 1,4);

    m_invMsg = new QLabel(""); m_invMsg->setStyleSheet("font-size:11px; background:transparent;"); m_invMsg->hide();

    QPushButton* addBtn = new QPushButton("Add",    form); addBtn->setStyleSheet(S_BTN); addBtn->setCursor(Qt::PointingHandCursor);
    QPushButton* updBtn = new QPushButton("Update", form); updBtn->setStyleSheet(S_BTN); updBtn->setCursor(Qt::PointingHandCursor);
    QPushButton* delBtn = new QPushButton("Remove", form); delBtn->setStyleSheet(S_BTN_RED); delBtn->setCursor(Qt::PointingHandCursor);
    QPushButton* refBtn = new QPushButton("Refresh",form); refBtn->setStyleSheet(S_BTN); refBtn->setCursor(Qt::PointingHandCursor);

    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);
    btnRow->addWidget(addBtn); btnRow->addWidget(updBtn); btnRow->addWidget(delBtn);
    btnRow->addStretch(); btnRow->addWidget(refBtn);

    gl->addWidget(m_invMsg, 2, 0, 1, 5);
    gl->addLayout(btnRow,   3, 0, 1, 5);
    vl->addWidget(form);

    connect(addBtn, &QPushButton::clicked, this, &AdminDashboard::onAddItem);
    connect(updBtn, &QPushButton::clicked, this, &AdminDashboard::onUpdateItem);
    connect(delBtn, &QPushButton::clicked, this, &AdminDashboard::onRemoveItem);
    connect(refBtn, &QPushButton::clicked, this, &AdminDashboard::refreshTable);

    connect(m_table, &QTableWidget::currentCellChanged, this, [this](int row,int,int,int){
        if (row < 0) return;
        m_fId->setText(m_table->item(row,0)->text());
        m_fType->setText(m_table->item(row,1)->text());
        m_fDest->setText(m_table->item(row,2)->text());
        m_fPrice->setValue(m_table->item(row,3)->text().toDouble());
        m_fAvail->setValue(m_table->item(row,4)->text().toInt());
    });
    return page;
}

QWidget* AdminDashboard::buildLogs() {
    QWidget* page = new QWidget();
    QVBoxLayout* vl = new QVBoxLayout(page);
    vl->setContentsMargins(36, 32, 36, 32); vl->setSpacing(16);

    QLabel* title = new QLabel("System Logs");
    title->setStyleSheet("color:#111111; font-size:20px; font-weight:700; background:transparent;");
    vl->addWidget(title);

    QFrame* div = new QFrame(); div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("background:#eeeeee; max-height:1px; border:none;");
    vl->addWidget(div);

    m_logView = new QTextEdit();
    m_logView->setReadOnly(true);
    m_logView->setStyleSheet(R"(
        QTextEdit {
            background: #fafafa;
            border: 1px solid #e8e8e8;
            border-radius: 10px;
            color: #226622;
            font-family: 'Consolas', monospace;
            font-size: 12px;
            padding: 12px;
        }
    )");

    QPushButton* refBtn = new QPushButton("Refresh", page);
    refBtn->setStyleSheet(S_BTN); refBtn->setCursor(Qt::PointingHandCursor); refBtn->setFixedWidth(100);
    connect(refBtn, &QPushButton::clicked, this, [this]{
        QFile f(dp("log.txt"));
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            m_logView->setPlainText(QString::fromUtf8(f.readAll())); f.close();
            m_logView->moveCursor(QTextCursor::End);
        }
    });

    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->addWidget(refBtn); btnRow->addStretch();
    vl->addLayout(btnRow);
    vl->addWidget(m_logView, 1);
    return page;
}

void AdminDashboard::loadAdmin(const QString& id) {
    m_adminId = id;
    m_adminLabel->setText("ADMIN  ·  " + id.toUpper());
    refreshTable(); refreshStats();
}

void AdminDashboard::refreshTable() {
    m_table->setRowCount(0);
    QJsonObject root = jsonRead(dp("Inventory.json"));
    QJsonArray arr = root["Inventory"].toArray();
    for (const QJsonValue& v : arr) {
        QJsonObject o = v.toObject();
        int row = m_table->rowCount(); m_table->insertRow(row);
        auto cell = [](const QString& t){ auto* i = new QTableWidgetItem(t); i->setTextAlignment(Qt::AlignCenter); return i; };
        m_table->setItem(row,0,cell(o["id"].toString()));
        m_table->setItem(row,1,cell(o["type"].toString()));
        m_table->setItem(row,2,cell(o["destination"].toString()));
        m_table->setItem(row,3,cell(QString::number(o["price"].toDouble(),'f',2)));
        m_table->setItem(row,4,cell(QString::number(o["available"].toInt())));
    }
}

void AdminDashboard::refreshStats() {
    QJsonObject uRoot = jsonRead(dp("Users.json"));
    int users = 0;
    if (!uRoot.isEmpty()) {
        users = uRoot["Clients"].toObject().size() + uRoot["Admins"].toObject().size();
    }
    m_sUsers->setText(QString::number(users));

    QJsonObject invRoot = jsonRead(dp("Inventory.json"));
    int items = invRoot.isEmpty() ? 0 : invRoot["Inventory"].toArray().size();
    m_sItems->setText(QString::number(items));
}

void AdminDashboard::onAddItem() {
    m_invMsg->hide();
    QString id = m_fId->text().trimmed(), type = m_fType->text().trimmed(), dest = m_fDest->text().trimmed();
    if (id.isEmpty() || type.isEmpty() || dest.isEmpty()) {
        m_invMsg->setStyleSheet("color:#cc3333; font-size:11px; background:transparent;");
        m_invMsg->setText("Fill in ID, Type and Destination."); m_invMsg->show(); return;
    }
    QJsonObject root = jsonRead(dp("Inventory.json"));
    QJsonArray arr = root["Inventory"].toArray();
    for (const QJsonValue& v : arr) if (v.toObject()["id"].toString() == id) {
        m_invMsg->setStyleSheet("color:#cc3333; font-size:11px; background:transparent;");
        m_invMsg->setText("ID already exists."); m_invMsg->show(); return;
    }
    QJsonObject o; o["id"]=id; o["type"]=type; o["destination"]=dest;
    o["price"]=m_fPrice->value(); o["available"]=m_fAvail->value();
    arr.append(o); root["Inventory"]=arr;
    if (!jsonWrite(dp("Inventory.json"), root)) return;
    m_invMsg->setStyleSheet("color:#228844; font-size:11px; background:transparent;");
    m_invMsg->setText("Item added."); m_invMsg->show(); refreshTable();
}

void AdminDashboard::onRemoveItem() {
    m_invMsg->hide();
    int row = m_table->currentRow();
    if (row < 0) { m_invMsg->setStyleSheet("color:#cc3333; font-size:11px; background:transparent;");
        m_invMsg->setText("Select a row first."); m_invMsg->show(); return; }
    QString id = m_table->item(row,0)->text();
    QJsonObject root = jsonRead(dp("Inventory.json"));
    QJsonArray arr = root["Inventory"].toArray(), newArr;
    for (const QJsonValue& v : arr) if (v.toObject()["id"].toString() != id) newArr.append(v);
    root["Inventory"]=newArr;
    if (!jsonWrite(dp("Inventory.json"), root)) return;
    m_invMsg->setStyleSheet("color:#228844; font-size:11px; background:transparent;");
    m_invMsg->setText("Item removed."); m_invMsg->show(); refreshTable();
}

void AdminDashboard::onUpdateItem() {
    m_invMsg->hide();
    int row = m_table->currentRow();
    if (row < 0) { m_invMsg->setStyleSheet("color:#cc3333; font-size:11px; background:transparent;");
        m_invMsg->setText("Select a row first."); m_invMsg->show(); return; }
    QString origId = m_table->item(row,0)->text();
    QJsonObject root = jsonRead(dp("Inventory.json"));
    QJsonArray arr = root["Inventory"].toArray();
    for (int i = 0; i < arr.size(); ++i) {
        QJsonObject o = arr[i].toObject();
        if (o["id"].toString() == origId) {
            o["id"]=m_fId->text().trimmed(); o["type"]=m_fType->text().trimmed();
            o["destination"]=m_fDest->text().trimmed();
            o["price"]=m_fPrice->value(); o["available"]=m_fAvail->value();
            arr[i]=o; break;
        }
    }
    root["Inventory"]=arr;
    if (!jsonWrite(dp("Inventory.json"), root)) return;
    m_invMsg->setStyleSheet("color:#228844; font-size:11px; background:transparent;");
    m_invMsg->setText("Item updated."); m_invMsg->show(); refreshTable();
}

void AdminDashboard::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), QColor(255, 255, 255));
}
