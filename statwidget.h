#ifndef STATWIDGET_H
#define STATWIDGET_H

#include <QVBoxLayout>
#include <QFrame>

#include "utils.h"

class StatWidget : public QFrame
{
    Q_OBJECT
public:
    explicit StatWidget(QWidget *parent = nullptr) : QFrame(parent){}
    StatWidget(QString type) : StatWidget(nullptr){

        this->type = type;

        setLayout(new QVBoxLayout());
        infoLabel = new QLabel;
        QString hColor;
        if(type == "currentDogs"){
            hColor = "20718e";
            infoLabel->setText("CHIENS AU REFUGE");
        }

        else if(type == "currentMembers"){
            hColor = "2cc09d";
            infoLabel->setText("ADHÉRENTS");
        }

        else if(type == "adoptions"){
            hColor = "a86ba2";
            infoLabel->setText("ADOPTIONS " + QString::number(QDate::currentDate().year()));
        }

        infoLabel->setStyleSheet("color:#" + hColor + ";");
        infoLabel->setAlignment(Qt::AlignCenter);

        layout()->addWidget(infoLabel);


        statLabel = new QLabel("x");
        statLabel->setStyleSheet("color:#333;");
        statLabel->setAlignment(Qt::AlignCenter);

        layout()->addWidget(statLabel);

        setStyleSheet("QFrame {border:1px solid #aaa; border-radius:5px;background-color:white;} "
                      "QLabel {border:none;}");


        UpdateStat();
    }

    void UpdateStat(){
        QSqlQuery query;

        if(type == "currentDogs"){
            query.prepare("SELECT COUNT(*) "
                          "FROM Dogs "
                          "WHERE id_dog NOT IN "
                          "(SELECT id_dog "
                          "FROM Destinations "
                          "WHERE type = 'Entrée au refuge' "
                          "OR type = 'Mort' "
                          "UNION "
                          "SELECT id_dog "
                          "FROM Care_registry) "
                          "AND id_dog IN "
                          "(SELECT id_dog FROM ES_Registry)");
        }

        else if(type == "currentMembers"){
            query.prepare("SELECT COUNT(DISTINCT id_people) "
                          "FROM Members "
                          "WHERE date(Members.date, '+1 year') > date('now')");
        }

        else if(type == "adoptions"){
            query.prepare("SELECT COUNT(*) "
                          "FROM Dogs "
                          "WHERE id_dog IN "
                          "(SELECT id_dog "
                          "FROM Destinations "
                          "WHERE strftime('%Y', Destinations.date) = :year "
                          "AND type = 'Adoption')");

            query.bindValue(":year", QDate::currentDate().year());
        }


        HandleErrorExec(&query);
        query.next();

        statLabel->setText(query.value(0).toString());
    }

    void resizeEvent(QResizeEvent *event) override{
        QFrame::resizeEvent(event);

        if(statLabel != nullptr && infoLabel != nullptr) {
            float parentWidth = qobject_cast<QWidget*>(parent())->width();
            QFont font = infoLabel->font();
            font.setBold(true);
            font.setPointSizeF(0.01 * parentWidth);
            infoLabel->setFont(font);

            font.setPointSizeF(0.02 * parentWidth);
            statLabel->setFont(font);

            setMaximumWidth(parentWidth / 6);
        }

    }

private:
    QString type;
    QLabel *statLabel = nullptr, *infoLabel = nullptr;
};

#endif // STATWIDGET_H
