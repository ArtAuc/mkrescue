#ifndef HOVERTOOLBUTTON_H
#define HOVERTOOLBUTTON_H

#include <QApplication>
#include <QMainWindow>
#include <QToolButton>
#include <QIcon>
#include <QPropertyAnimation>
#include <QMouseEvent>

class HoverToolButton : public QToolButton {
    Q_OBJECT
public:
    HoverToolButton(QWidget *parent = nullptr) : QToolButton(parent) {
        animation = new QPropertyAnimation(this, "maximumHeight", this);
        animation->setDuration(200);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
        connect(this, &HoverToolButton::entered, this, &HoverToolButton::startAnimation);
        connect(this, &HoverToolButton::left, this, &HoverToolButton::stopAnimation);
    }

protected:
    void enterEvent(QEnterEvent *event) override {
        Q_UNUSED(event);
        emit entered();
    }

    void leaveEvent(QEvent *event) override {
        Q_UNUSED(event);
        emit left();
    }

signals:
    void entered();
    void left();

private slots:
    void startAnimation() {
        originalHeight = maximumHeight();
        animation->setStartValue(originalHeight);
        animation->setEndValue(0.9 * originalHeight);
        animation->start();
    }

    void stopAnimation() {
        animation->stop();
        setMaximumHeight(originalHeight);
    }

private:
    QPropertyAnimation *animation;
    int originalHeight;
};

#endif // HOVERTOOLBUTTON_H
