#include "SplashPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QLinearGradient>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>

SplashPage::SplashPage(QWidget* parent) : QWidget(parent) {

    // ── Logo image ────────────────────────────────────────────────────────────
    m_logoLabel = new QLabel(this);
    QPixmap logo(":/logo.png");
    if (!logo.isNull()) {
        // Scale to a good splash size — 480px wide
        m_logoLabel->setPixmap(logo.scaledToWidth(480, Qt::SmoothTransformation));
    } else {
        m_logoLabel->setText("QUADRATA");
        m_logoLabel->setStyleSheet(
            "color: #111111; font-size: 52px; font-weight: 900; "
            "letter-spacing: 12px; background: transparent;"
        );
    }
    m_logoLabel->setAlignment(Qt::AlignCenter);

    // ── Tagline ───────────────────────────────────────────────────────────────
    m_tagline = new QLabel("Travel · Explore · Discover", this);
    m_tagline->setAlignment(Qt::AlignCenter);
    m_tagline->setStyleSheet(
        "color: #888888;"
        "font-size: 11px;"
        "letter-spacing: 5px;"
        "background: transparent;"
    );

    // ── Enter button ──────────────────────────────────────────────────────────
    m_enterBtn = new QPushButton("Get Started", this);
    m_enterBtn->setFixedSize(180, 46);
    m_enterBtn->setCursor(Qt::PointingHandCursor);
    m_enterBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #111111;
            color: #ffffff;
            border: none;
            border-radius: 23px;
            font-size: 13px;
            font-weight: 700;
            letter-spacing: 2px;
        }
        QPushButton:hover  { background-color: #333333; }
        QPushButton:pressed{ background-color: #000000; }
    )");

    // Fade-in
    m_btnEffect = new QGraphicsOpacityEffect(m_enterBtn);
    m_enterBtn->setGraphicsEffect(m_btnEffect);
    m_btnEffect->setOpacity(0.0);

    m_btnAnim = new QPropertyAnimation(m_btnEffect, "opacity", this);
    m_btnAnim->setDuration(1000);
    m_btnAnim->setStartValue(0.0);
    m_btnAnim->setEndValue(1.0);
    m_btnAnim->setEasingCurve(QEasingCurve::InOutQuad);

    // ── Layout ────────────────────────────────────────────────────────────────
    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->setAlignment(Qt::AlignCenter);
    vl->setSpacing(0);
    vl->addStretch(2);
    vl->addWidget(m_logoLabel, 0, Qt::AlignCenter);
    vl->addSpacing(16);
    vl->addWidget(m_tagline,   0, Qt::AlignCenter);
    vl->addSpacing(48);
    vl->addWidget(m_enterBtn,  0, Qt::AlignCenter);
    vl->addStretch(3);

    connect(m_enterBtn, &QPushButton::clicked, this, &SplashPage::enterClicked);
}

void SplashPage::showEvent(QShowEvent* e) {
    QWidget::showEvent(e);
    m_btnEffect->setOpacity(0.0);
    QTimer::singleShot(400, this, [this]() { m_btnAnim->start(); });
}

void SplashPage::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), QColor(255, 255, 255));  // white
}
