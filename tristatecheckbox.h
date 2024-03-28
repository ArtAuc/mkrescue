#ifndef TRISTATECHECKBOX_H
#define TRISTATECHECKBOX_H

#include <QCheckBox>
#include <QMouseEvent>

class TriStateCheckBox : public QCheckBox {
    Q_OBJECT

public:
    explicit TriStateCheckBox(QWidget *parent = nullptr) : QCheckBox(parent) {
        setTristate(true);
        setCheckState(Qt::PartiallyChecked);
    }

    TriStateCheckBox(QString text, QWidget *parent = nullptr) : TriStateCheckBox(parent) {
        setText(text);
    }



protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (isEnabled()) {
            if (event->button() == Qt::LeftButton) {
                if (checkState() == Qt::Unchecked)
                    setCheckState(Qt::PartiallyChecked);
                else if (checkState() == Qt::PartiallyChecked)
                    setCheckState(Qt::Checked);
                else
                    setCheckState(Qt::Unchecked);
                emit stateChanged(checkState());
                event->accept();
                return;
            }
        }
        QCheckBox::mousePressEvent(event);
    }

signals:
    void stateChanged(int state);
};

#endif // TRISTATECHECKBOX_H
