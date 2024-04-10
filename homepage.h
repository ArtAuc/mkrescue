#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QGridLayout>
#include <QScrollArea>

#include "statwidget.h"

class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr) : QWidget(parent) {
    }

    void showEvent(QShowEvent *event) override{
        QWidget::showEvent(event);

        if(alertsWidget == nullptr){
            QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(this->layout());
            layout->setContentsMargins(0,0,0,0);

            alertsLabel = new QLabel("NOTIFICATIONS");
            alertsLabel->setStyleSheet("color:#333; padding:20px; background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fafafa, stop:1 white);font-weight:bold;border-bottom:1px solid #aaa");
            alertsLabel->setAlignment(Qt::AlignCenter);
            layout->insertWidget(0, alertsLabel);


            QWidget *horizontalWidget = new QWidget(this);
            horizontalWidget->setLayout(new QHBoxLayout());
            horizontalWidget->layout()->addWidget(new StatWidget("currentDogs"));
            horizontalWidget->layout()->addWidget(new StatWidget("currentMembers"));
            horizontalWidget->layout()->addWidget(new StatWidget("currentMembers"));
            horizontalWidget->layout()->addWidget(new StatWidget("currentMembers"));
            horizontalWidget->layout()->addWidget(new StatWidget("currentMembers"));
            horizontalWidget->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 white, stop:1 #fafafa);");

            layout->insertWidget(0, horizontalWidget);

            alertsWidget = findChild<QWidget*>("homeScrollContents");
            layout->setSpacing(0);

            UpAlertDays();

            alertsWidget->setStyleSheet("QWidget#homeScrollContents, QScrollArea{background-color:white;border:none;}");
        }
    }

    void resizeEvent(QResizeEvent *event) override{
        QWidget::resizeEvent(event);

        if(alertsWidget != nullptr){

            alertsWidget->layout()->setContentsMargins(0.03 * width(),0,0.03 * width(),0);
            for(StatWidget *c : findChildren<StatWidget*>())
                c->resizeEvent(event);

            QFont font = alertsLabel->font();
            font.setPointSizeF(0.01 * width());
            alertsLabel->setFont(font);

            for (QPushButton *but : findChildren<QPushButton*>()){
                but->setFont(font);
            }
        }
    }

    void LoadAlerts(){
        if(alertsWidget != nullptr){
            qDeleteAll(alertsWidget->findChildren<QPushButton*>());

            alertsWidget->layout()->removeItem(spacer);
            delete spacer;

            QSqlQuery query;

            // Vet main appointments
            QString queryString = "SELECT * FROM ("
                                  "SELECT Vet.date, 'RDV Vétérinaire', Dogs.name, Vet.reason "
                                  "FROM Vet "
                                  "JOIN Dogs ON Vet.id_dog = Dogs.id_dog "
                                  "UNION "
                                  "SELECT DATE(Vet.date, '+1 year'), 'Prévoir le rappel de vaccin', Dogs.name, '' "
                                  "FROM Vet "
                                  "JOIN Dogs ON Vet.id_dog = Dogs.id_dog "
                                  "JOIN ("
                                  "    SELECT id_dog, MAX(date) AS max_date "
                                  "    FROM Vet "
                                  "    WHERE reason = 'Vaccin' "
                                  "    GROUP BY id_dog "
                                  ") AS LatestVet ON Vet.id_dog = LatestVet.id_dog AND Vet.date = LatestVet.max_date "
                                  "WHERE Vet.reason = 'Vaccin'"
                                  ") AS Results "
                                  "WHERE Results.date BETWEEN DATE('now') AND DATE('now', '+' || :alertDays || ' day') "
                                  "ORDER BY Results.date";

            query.prepare(queryString);
            query.bindValue(":alertDays", std::abs(alertDays));

            HandleErrorExec(&query);
            while(query.next()){
                QString dateString = query.value(0).toDate().toString("dd/MM/yy");
                QString type = query.value(1).toString();
                QString stylesheet = "QPushButton{padding-top:20px;padding-bottom:20px;margin-top:20px;";
                if(query.value(0).toDate() == QDate::currentDate()){
                    stylesheet += "border:2px solid red;";
                    dateString = "Aujourd'hui";
                }

                else if(QDate::currentDate().daysTo(query.value(0).toDate()) == 1)
                    dateString = "Demain";

                QPushButton *but = new QPushButton(dateString + " - " + type + " : " + query.value(2).toString() + QString(query.value(3).toString().isEmpty() ? "" : " (" + query.value(3).toString() + ")"));

                QColor color;
                if(type == "RDV Vétérinaire")
                    color = QColor("#2cc09d");

                stylesheet += "background-color:" + color.name() + ";}"
                              "QPushButton:hover{background-color:" + color.lighter(110).name() + ";}";

                but->setStyleSheet(stylesheet);
                alertsWidget->layout()->addWidget(but);
            }

            spacer = new QSpacerItem(1, 1, QSizePolicy::Preferred, QSizePolicy::Expanding);
            alertsWidget->layout()->addItem(spacer);

            QPushButton *moreButton = new QPushButton("Notifications suivantes");
            alertsWidget->layout()->addWidget(moreButton);
            connect(moreButton, SIGNAL(clicked(bool)), this, SLOT(UpAlertDays()));
            moreButton->setVisible(alertDays >= 0);

            resizeEvent(nullptr);
        }
    }

public slots:
    void UpAlertDays(){
        QSqlQuery query;
        query.prepare("SELECT MIN(Results.date) FROM ("
                      "SELECT Vet.date "
                      "FROM Vet "
                      "UNION "
                      "SELECT DATE(Vet.date, '+1 year') "
                      "FROM Vet "
                      "JOIN Dogs ON Vet.id_dog = Dogs.id_dog "
                      "JOIN ("
                      "    SELECT id_dog, MAX(date) AS max_date "
                      "    FROM Vet "
                      "    WHERE reason = 'Vaccin' "
                      "    GROUP BY id_dog "
                      ") AS LatestVet ON Vet.id_dog = LatestVet.id_dog AND Vet.date = LatestVet.max_date "
                      "WHERE Vet.reason = 'Vaccin') AS Results "
                      "WHERE Results.date > DATE('now', '+' || :alertDays || ' day')");

        query.bindValue(":alertDays", std::abs(alertDays));
        HandleErrorExec(&query);


        if(query.next() && query.value(0).toString() != ""){
            alertDays = QDate::currentDate().daysTo(query.value(0).toDate()) + 7;
        }

        else{
            alertDays = -std::abs(alertDays);
        }

        LoadAlerts();
    }


private:
    QLabel *alertsLabel;
    QWidget *alertsWidget = nullptr;
    int alertDays{};
    QSpacerItem *spacer = nullptr;
};

#endif // HOMEPAGE_H
