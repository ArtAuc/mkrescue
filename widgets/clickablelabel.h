#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) : QLabel(parent, f) {}

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override {
        emit clicked();
        QLabel::mousePressEvent(event);
    }

    void enterEvent(QEnterEvent* event) override {
        setCursor(Qt::PointingHandCursor);
        QLabel::enterEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        unsetCursor();
        QLabel::leaveEvent(event);
    }
};


#endif // CLICKABLELABEL_H
