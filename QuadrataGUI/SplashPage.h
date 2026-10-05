#pragma once
#include <QWidget>

class QPushButton;
class QLabel;
class QPropertyAnimation;
class QGraphicsOpacityEffect;

class SplashPage : public QWidget {
    Q_OBJECT
public:
    explicit SplashPage(QWidget* parent = nullptr);

signals:
    void enterClicked();

protected:
    void paintEvent(QPaintEvent*) override;
    void showEvent(QShowEvent*) override;

private:
    QLabel*                 m_logoLabel;
    QPushButton*            m_enterBtn;
    QLabel*                 m_tagline;
    QGraphicsOpacityEffect* m_btnEffect;
    QPropertyAnimation*     m_btnAnim;
};
