#ifndef HOVERTOOLBUTTON_H
#define HOVERTOOLBUTTON_H

#include <QToolButton>

class HoverToolButton : public QToolButton {
    Q_OBJECT
public:
    HoverToolButton(QWidget *parent = nullptr) : QToolButton(parent) {
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
        originalSize = iconSize();
        setIconSize(1.2 * originalSize);
    }

    void stopAnimation() {
        setIconSize(originalSize);
    }

private:
    QSize originalSize;
};

#endif // HOVERTOOLBUTTON_H
