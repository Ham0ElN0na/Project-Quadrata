#include "LoginPage.h"
#include "JsonUtils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QStackedWidget>
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QScrollArea>

// ── Styles ────────────────────────────────────────────────────────────────────
static const char* S_INPUT = R"(
    QLineEdit {
        background: #f5f5f5;
        border: 1px solid #dddddd;
        border-radius: 8px;
        color: #111111;
        padding: 14px 16px;
        font-size: 14px;
        selection-background-color: #333333;
    }
    QLineEdit:focus {
        border: 1px solid #999999;
        background: #ffffff;
        color: #000000;
    }
    QLineEdit::placeholder { color: #aaaaaa; }
)";

static const char* S_BTN_PRIMARY = R"(
    QPushButton {
        background: #f0f0f0;
        color: #ffffff;
        border: none;
        border-radius: 8px;
        font-size: 14px;
        font-weight: 700;
        padding: 15px;
    }
    QPushButton:hover  { background: #333333; }
    QPushButton:pressed{ background: #000000; }
)";

static const char* S_TAB_ON = R"(
    QPushButton {
        background: transparent; color: #111111;
        border: none; border-bottom: 2px solid #111111;
        font-size: 12px; font-weight: 700; letter-spacing: 1px;
        padding: 8px 20px; border-radius: 0;
    }
)";
static const char* S_TAB_OFF = R"(
    QPushButton {
        background: transparent; color: #aaaaaa;
        border: none; border-bottom: 2px solid transparent;
        font-size: 12px; font-weight: 400; letter-spacing: 1px;
        padding: 8px 20px; border-radius: 0;
    }
    QPushButton:hover { color: #666666; }
)";

static QLabel* fieldLabel(const QString& t) {
    QLabel* l = new QLabel(t);
    l->setStyleSheet(
        "color:#888888; font-size:11px; font-weight:600; "
        "letter-spacing:1px; background:transparent; margin-bottom:4px;"
    );
    return l;
}

// Helper: load logo pixmap, scale to given width
static QLabel* makeLogoLabel(int width, QWidget* parent = nullptr) {
    QLabel* lbl = new QLabel(parent);
    QPixmap px(":/logo.png");
    if (!px.isNull()) {
        lbl->setPixmap(px.scaledToWidth(width, Qt::SmoothTransformation));
    } else {
        lbl->setText("QUADRATA");
        lbl->setStyleSheet(
            QString("color:#ffffff; font-size:%1px; font-weight:900; "
                    "letter-spacing:6px; background:transparent;").arg(width / 10)
        );
    }
    lbl->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    return lbl;
}

QString LoginPage::dataPath(const QString& f) const {
    QStringList c = {
        QCoreApplication::applicationDirPath() + "/" + f,
        QDir::currentPath() + "/" + f,
        "C:/Users/Lenovo/Downloads/Project Quadrata/Project Quadrata/Project Quadrata/" + f
    };
    for (auto& p : c) if (QFile::exists(p)) return p;
    return c.last();
}

LoginPage::LoginPage(QWidget* parent) : QWidget(parent) {

    // ── Left branding panel ───────────────────────────────────────────────────
    QWidget* leftPanel = new QWidget(this);
    leftPanel->setStyleSheet("background:transparent;");
    QVBoxLayout* leftVL = new QVBoxLayout(leftPanel);
    leftVL->setContentsMargins(64, 0, 48, 0);
    leftVL->setSpacing(0);

    QLabel* logoLbl = makeLogoLabel(280, leftPanel);
    logoLbl->setAlignment(Qt::AlignLeft);

    QLabel* headline = new QLabel("Good to see\nyou again.", leftPanel);
    headline->setStyleSheet(
        "color:#111111; font-size:38px; font-weight:800; "
        "line-height:1.2; background:transparent;"
    );
    headline->setWordWrap(true);

    QLabel* sub = new QLabel("Book your next journey.", leftPanel);
    sub->setStyleSheet("color:#888888; font-size:14px; background:transparent;");
    sub->setWordWrap(true);

    leftVL->addStretch(2);
    leftVL->addWidget(logoLbl);
    leftVL->addSpacing(44);
    leftVL->addWidget(headline);
    leftVL->addSpacing(16);
    leftVL->addWidget(sub);
    leftVL->addStretch(3);

    // ── Right card panel ──────────────────────────────────────────────────────
    QWidget* rightPanel = new QWidget(this);
    rightPanel->setStyleSheet("background:transparent;");
    QVBoxLayout* rightVL = new QVBoxLayout(rightPanel);
    rightVL->setContentsMargins(48, 0, 64, 0);
    rightVL->setAlignment(Qt::AlignCenter);

    // Card — wider: 500px
    QWidget* card = new QWidget(rightPanel);
    card->setFixedWidth(500);
    card->setStyleSheet(
        "QWidget { background:#ffffff; "
        "border:1px solid #e8e8e8; border-radius:16px; }"
    );
    QGraphicsDropShadowEffect* sh = new QGraphicsDropShadowEffect(card);
    sh->setBlurRadius(40); sh->setColor(QColor(0,0,0,40)); sh->setOffset(0,8);
    card->setGraphicsEffect(sh);

    // Tabs
    m_loginTab  = new QPushButton("LOGIN",   card);
    m_signupTab = new QPushButton("SIGN UP", card);
    m_loginTab->setCursor(Qt::PointingHandCursor);
    m_signupTab->setCursor(Qt::PointingHandCursor);
    m_loginTab->setStyleSheet(S_TAB_ON);
    m_signupTab->setStyleSheet(S_TAB_OFF);

    QHBoxLayout* tabRow = new QHBoxLayout();
    tabRow->setSpacing(0); tabRow->setContentsMargins(0,0,0,0);
    tabRow->addWidget(m_loginTab); tabRow->addWidget(m_signupTab); tabRow->addStretch();

    QFrame* tabDiv = new QFrame(card);
    tabDiv->setFrameShape(QFrame::HLine);
    tabDiv->setStyleSheet("background:#eeeeee; max-height:1px; border:none;");

    m_formStack = new QStackedWidget(card);
    QWidget* loginW  = new QWidget(); buildLoginForm(loginW);
    QWidget* signupW = new QWidget(); buildSignupForm(signupW);
    m_formStack->addWidget(loginW);
    m_formStack->addWidget(signupW);

    QVBoxLayout* cardVL = new QVBoxLayout(card);
    cardVL->setContentsMargins(36, 28, 36, 32);
    cardVL->setSpacing(0);
    cardVL->addLayout(tabRow);
    cardVL->addWidget(tabDiv);
    cardVL->addSpacing(28);
    cardVL->addWidget(m_formStack);

    connect(m_loginTab,  &QPushButton::clicked, this, [this]{ setTab(0); });
    connect(m_signupTab, &QPushButton::clicked, this, [this]{ setTab(1); });

    rightVL->addStretch();
    rightVL->addWidget(card);
    rightVL->addStretch();

    // ── Back link ─────────────────────────────────────────────────────────────
    QPushButton* backBtn = new QPushButton("← Back", this);
    backBtn->setStyleSheet(
        "QPushButton{background:transparent;color:#aaaaaa;border:none;"
        "font-size:11px;padding:6px 0;}"
        "QPushButton:hover{color:#555555;}"
    );
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &LoginPage::backToSplash);

    // ── Vertical divider ──────────────────────────────────────────────────────
    QFrame* vDiv = new QFrame(this);
    vDiv->setFrameShape(QFrame::VLine);
    vDiv->setStyleSheet("background:#eeeeee; max-width:1px; border:none;");

    // ── Root layout ───────────────────────────────────────────────────────────
    QVBoxLayout* rootVL = new QVBoxLayout(this);
    rootVL->setContentsMargins(0,0,0,0); rootVL->setSpacing(0);

    QHBoxLayout* topBar = new QHBoxLayout();
    topBar->setContentsMargins(20,16,20,0);
    topBar->addWidget(backBtn); topBar->addStretch();
    rootVL->addLayout(topBar);

    QHBoxLayout* mainRow = new QHBoxLayout();
    mainRow->setSpacing(0);
    mainRow->addWidget(leftPanel, 1);
    mainRow->addWidget(vDiv);
    mainRow->addWidget(rightPanel, 1);
    rootVL->addLayout(mainRow, 1);
}

void LoginPage::buildLoginForm(QWidget* w) {
    QVBoxLayout* vl = new QVBoxLayout(w);
    vl->setContentsMargins(0,0,0,0); vl->setSpacing(0);

    // ── ID field ──────────────────────────────────────────────────────────────
    m_idLabel = new QLabel("USERNAME", w);
    m_idLabel->setStyleSheet(
        "color:#888888; font-size:11px; font-weight:600; "
        "letter-spacing:1px; background:transparent; margin-bottom:4px;"
    );
    m_idField = new QLineEdit(w);
    m_idField->setPlaceholderText("e.g. Mazen");
    m_idField->setStyleSheet(S_INPUT);
    vl->addWidget(m_idLabel);
    vl->addWidget(m_idField);
    vl->addSpacing(18);

    // ── Password field ────────────────────────────────────────────────────────
    vl->addWidget(fieldLabel("PASSWORD"));
    m_passField = new QLineEdit(w);
    m_passField->setPlaceholderText("Your password");
    m_passField->setEchoMode(QLineEdit::Password);
    m_passField->setStyleSheet(S_INPUT);
    vl->addWidget(m_passField);
    vl->addSpacing(8);

    // ── Error label ───────────────────────────────────────────────────────────
    m_loginError = new QLabel("", w);
    m_loginError->setStyleSheet(
        "color:#cc4444; font-size:12px; background:transparent; padding:4px 0;"
    );
    m_loginError->setAlignment(Qt::AlignCenter);
    m_loginError->hide();
    vl->addWidget(m_loginError);
    vl->addSpacing(14);

    // ── Sign in button ────────────────────────────────────────────────────────
    QPushButton* btn = new QPushButton("Sign in", w);
    btn->setFixedHeight(50); btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(S_BTN_PRIMARY);
    vl->addWidget(btn);
    vl->addSpacing(16);

    // ── Admin toggle — bottom right ───────────────────────────────────────────
    m_adminToggle = new QCheckBox("Admin login", w);
    m_adminToggle->setStyleSheet(R"(
        QCheckBox {
            color: #888888;
            font-size: 11px;
            spacing: 7px;
            background: transparent;
        }
        QCheckBox::indicator {
            width: 15px; height: 15px;
            border-radius: 3px;
            border: 1px solid #cccccc;
            background: #f5f5f5;
        }
        QCheckBox::indicator:checked {
            background: #f0f0f0;
            border: 1px solid #111111;
        }
        QCheckBox::indicator:hover { border: 1px solid #888888; }
    )");
    m_adminToggle->setCursor(Qt::PointingHandCursor);

    QHBoxLayout* bottomRow = new QHBoxLayout();
    bottomRow->addStretch();
    bottomRow->addWidget(m_adminToggle);
    vl->addLayout(bottomRow);

    // Update label when toggle changes
    connect(m_adminToggle, &QCheckBox::toggled, this, [this](bool checked) {
        m_idLabel->setText(checked ? "EMPLOYEE ID" : "USERNAME");
        m_idField->setPlaceholderText(checked ? "e.g. admin" : "e.g. Mazen");
    });
    connect(btn, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(m_passField, &QLineEdit::returnPressed, this, &LoginPage::onLoginClicked);
}

void LoginPage::buildSignupForm(QWidget* w) {
    QScrollArea* scroll = new QScrollArea(w);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background:transparent; border:none;");
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* inner = new QWidget();
    inner->setStyleSheet("background:transparent;");
    QVBoxLayout* vl = new QVBoxLayout(inner);
    vl->setContentsMargins(0,0,4,0); vl->setSpacing(0);

    QLabel* banner = new QLabel("Create a customer account to start booking.", inner);
    banner->setStyleSheet(
        "color:#888888; font-size:11px; background:#f5f5f5; "
        "border-radius:8px; padding:8px 12px; border:1px solid #eeeeee;"
    );
    banner->setWordWrap(true);
    vl->addWidget(banner);
    vl->addSpacing(20);

    auto addField = [&](const QString& label, QLineEdit*& field,
                        const QString& ph, bool pw = false) {
        vl->addWidget(fieldLabel(label));
        field = new QLineEdit(inner);
        field->setPlaceholderText(ph);
        field->setStyleSheet(S_INPUT);
        if (pw) field->setEchoMode(QLineEdit::Password);
        vl->addWidget(field);
        vl->addSpacing(16);
    };

    addField("USERNAME",            m_suId,      "Choose a username");
    addField("FULL NAME",           m_suName,    "Your full name");
    addField("EMAIL ADDRESS",       m_suEmail,   "your@email.com");
    addField("PASSWORD",            m_suPass,    "Choose a password", true);
    addField("AGE",                 m_suAge,     "Your age");
    addField("INITIAL BALANCE ($)", m_suBalance, "e.g. 500");

    m_signupMsg = new QLabel("", inner);
    m_signupMsg->setStyleSheet("font-size:12px; background:transparent; padding:4px 0;");
    m_signupMsg->setAlignment(Qt::AlignCenter);
    m_signupMsg->hide();
    vl->addWidget(m_signupMsg);
    vl->addSpacing(8);

    QPushButton* btn = new QPushButton("Create Account", inner);
    btn->setFixedHeight(50); btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(S_BTN_PRIMARY);
    vl->addWidget(btn);

    scroll->setWidget(inner);
    QVBoxLayout* outerVL = new QVBoxLayout(w);
    outerVL->setContentsMargins(0,0,0,0);
    outerVL->addWidget(scroll);

    connect(btn, &QPushButton::clicked, this, &LoginPage::onSignUpClicked);
}

void LoginPage::setTab(int idx) {
    m_formStack->setCurrentIndex(idx);
    m_loginTab->setStyleSheet(idx == 0 ? S_TAB_ON : S_TAB_OFF);
    m_signupTab->setStyleSheet(idx == 1 ? S_TAB_ON : S_TAB_OFF);
}

void LoginPage::reset() {
    m_idField->clear(); m_passField->clear();
    m_loginError->hide(); m_adminToggle->setChecked(false); setTab(0);
}

void LoginPage::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), QColor(255, 255, 255));  // white
}

void LoginPage::onLoginClicked() {
    m_loginError->hide();
    QString id   = m_idField->text().trimmed();
    QString pass = m_passField->text();
    bool isAdmin = m_adminToggle->isChecked();

    if (id.isEmpty() || pass.isEmpty()) {
        m_loginError->setText("Please fill in all fields."); m_loginError->show(); return;
    }
    QJsonObject root = jsonRead(dataPath("Users.json"));
    if (root.isEmpty()) {
        m_loginError->setText("Cannot open Users.json"); m_loginError->show(); return;
    }

    if (isAdmin) {
        QJsonObject admins = root["Admins"].toObject();
        if (!admins.contains(id)) {
            m_loginError->setText("Employee ID not found."); m_loginError->show(); return;
        }
        if (admins[id].toObject()["Password"].toString() != pass) {
            m_loginError->setText("Incorrect password."); m_loginError->show(); return;
        }
        emit adminLoggedIn(id);
    } else {
        QJsonObject clients = root["Clients"].toObject();
        if (!clients.contains(id)) {
            m_loginError->setText("Username not found."); m_loginError->show(); return;
        }
        if (clients[id].toObject()["Password"].toString() != pass) {
            m_loginError->setText("Incorrect password."); m_loginError->show(); return;
        }
        emit customerLoggedIn(id);
    }
}

void LoginPage::onSignUpClicked() {
    m_signupMsg->hide();
    QString id   = m_suId->text().trimmed();
    QString name = m_suName->text().trimmed();
    QString pass = m_suPass->text();

    if (id.isEmpty() || name.isEmpty() || pass.isEmpty()) {
        m_signupMsg->setStyleSheet("color:#cc4444; font-size:12px; background:transparent;");
        m_signupMsg->setText("Username, name and password are required."); m_signupMsg->show(); return;
    }

    QJsonObject root = jsonRead(dataPath("Users.json"));
    QJsonObject clients = root["Clients"].toObject();
    if (clients.contains(id)) {
        m_signupMsg->setStyleSheet("color:#cc4444; font-size:12px; background:transparent;");
        m_signupMsg->setText("Username already taken."); m_signupMsg->show(); return;
    }

    QJsonObject c;
    c["name"]          = name;
    c["Email"]         = m_suEmail->text().trimmed();
    c["Password"]      = pass;
    c["Balance"]       = m_suBalance->text().toDouble();
    c["Age"]           = m_suAge->text().toInt();
    c["LoyaltyPoints"] = 0;
    c["History"]       = QJsonArray();
    clients[id] = c; root["Clients"] = clients;

    if (!jsonWrite(dataPath("Users.json"), root)) { return; }
    m_signupMsg->setStyleSheet("color:#44aa66; font-size:12px; background:transparent;");
    m_signupMsg->setText("Account created! You can now log in."); m_signupMsg->show();
    m_suId->clear(); m_suName->clear(); m_suEmail->clear();
    m_suPass->clear(); m_suAge->clear(); m_suBalance->clear();
}
