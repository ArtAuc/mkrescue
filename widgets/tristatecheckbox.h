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

    TriStateCheckBox(QString text, QWidget *parent = nullptr){
        setText(text);
        connect(this, SIGNAL(stateChanged(int)), parent, SLOT(SaveCard()));
    }


    QString StateToSql(){
        if (checkState() == Qt::Checked)
            return "1";
        else if(checkState() == Qt::Unchecked)
            return "0";
        else
            return "";
    }

    void SqlToState(QString s){
        if(s == "")
            setCheckState(Qt::PartiallyChecked);
        else if(s == "1")
            setCheckState(Qt::Checked);
        else
            setCheckState(Qt::Unchecked);
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
