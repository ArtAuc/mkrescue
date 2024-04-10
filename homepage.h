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
            QGridLayout* layout = qobject_cast<QGridLayout*>(this->layout());
            layout->setContentsMargins(0,0,0,0);

            alertsLabel = new QLabel("NOTIFICATIONS");
            alertsLabel->setStyleSheet("color:#333; padding:20px; background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fafafa, stop:1 white);font-weight:bold;border-bottom:1px solid #aaa;border-left:1px solid #aaa;");
            alertsLabel->setAlignment(Qt::AlignCenter);
            layout->addWidget(alertsLabel, 1, 1, 1, 1);


            QWidget *horizontalWidget = new QWidget(this);
            horizontalWidget->setLayout(new QHBoxLayout());
            horizontalWidget->layout()->addWidget(new StatWidget("currentDogs"));
            horizontalWidget->layout()->addWidget(new StatWidget("currentMembers"));
            horizontalWidget->layout()->addWidget(new StatWidget("currentMembers"));
            horizontalWidget->layout()->addWidget(new StatWidget("currentMembers"));
            horizontalWidget->layout()->addWidget(new StatWidget("currentMembers"));
            horizontalWidget->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 white, stop:1 #fafafa);");

            layout->addWidget(horizontalWidget, 0, 0, 1, 2);

            alertsWidget = findChild<QWidget*>("homeScrollContents");
            layout->setSpacing(0);

            alertsWidget->setStyleSheet("QWidget#homeScrollContents, QScrollArea{background-color:white;border:none;border-left:1px solid #aaa;}");
        }

        alertDays = 0;
        UpAlertDays();
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
                but->setMaximumWidth(0.8 * findChild<QScrollArea*>()->width());
            }

            for (QLabel *lab : alertsWidget->findChildren<QLabel*>()){
                lab->setFont(font);
                lab->setMaximumWidth(0.8 * findChild<QScrollArea*>()->width());
            }
        }
    }

    void LoadAlerts(){
        if(alertsWidget != nullptr){
            qDeleteAll(alertsWidget->findChildren<QPushButton*>());
            qDeleteAll(alertsWidget->findChildren<QLabel*>());

            alertsWidget->layout()->removeItem(spacer);
            delete spacer;

            QSqlQuery query;

            // Vet main appointments
            QString queryString = "SELECT * FROM ("
                                  "SELECT Vet.date, 'RDV Vétérinaire', Dogs.name, Vet.reason, Vet.id_dog "
                                  "FROM Vet "
                                  "JOIN Dogs ON Vet.id_dog = Dogs.id_dog "
                                  "UNION "
                                  "SELECT DATE(Vet.date, '+1 year'), 'Prévoir le rappel de vaccin', Dogs.name, '', Vet.id_dog "
                                  "FROM Vet "
                                  "JOIN Dogs ON Vet.id_dog = Dogs.id_dog "
                                  "JOIN ("
                                  "    SELECT id_dog, MAX(date) AS max_date "
                                  "    FROM Vet "
                                  "    WHERE reason = 'Vaccin' "
                                  "    GROUP BY id_dog "
                                  ") AS LatestVet ON Vet.id_dog = LatestVet.id_dog AND Vet.date = LatestVet.max_date "
                                  "WHERE Vet.reason LIKE 'Vaccin%'"
                                  ") AS Results "
                                  "WHERE Results.date BETWEEN DATE('now') AND DATE('now', '+' || :alertDays || ' day') "
                                  "ORDER BY Results.date";

            query.prepare(queryString);
            query.bindValue(":alertDays", std::abs(alertDays));

            HandleErrorExec(&query);
            while(query.next()){
                QString dateString = query.value(0).toDate().toString("dd/MM/yyyy");
                QString timeString;
                QString type = query.value(1).toString();
                QString stylesheet = "QPushButton{padding-top:20px;padding-bottom:20px;margin-bottom:5px;";
                QStringList necessary;

                if(query.value(0).toDate() == QDate::currentDate()){
                    stylesheet += "border:2px solid #634049;";
                    dateString = "AUJOURD'HUI - " + dateString;
                }

                else if(QDate::currentDate().daysTo(query.value(0).toDate()) == 1)
                    dateString = "DEMAIN - " + dateString;

                QColor color;
                if(type == "RDV Vétérinaire"){
                    color = QColor("#2cc09d");
                    timeString = query.value(0).toDateTime().toString("h:mm") + " - ";
                    necessary = {query.value(0).toString(), query.value(4).toString()};
                }

                else if(type == "Prévoir le rappel de vaccin"){
                    color = QColor("#20718e");
                    necessary = {query.value(4).toString()};
                }

                if(lastDate != dateString){
                    QLabel *dateLabel = new QLabel(dateString);
                    dateLabel->setAlignment(Qt::AlignCenter);
                    dateLabel->setStyleSheet("font-weight:bold;border-bottom:1px solid #aaa;color:#333;margin:10px;");
                    alertsWidget->layout()->addWidget(dateLabel);
                }

                lastDate = dateString;

                stylesheet += "background-color:" + color.name() + ";}"
                              "QPushButton:hover{background-color:" + color.lighter(110).name() + ";}";

                QPushButton *but = new QPushButton(timeString + type + " : " + query.value(2).toString() + QString(query.value(3).toString().isEmpty() ? "" : " (" + query.value(3).toString() + ")"));

                connect(but, &QPushButton::clicked, this, [=]() {
                    TriggerEditSlot("vet", necessary);
                });
                but->setStyleSheet(stylesheet);
                alertsWidget->layout()->addWidget(but);
            }

            spacer = new QSpacerItem(1, 1, QSizePolicy::Preferred, QSizePolicy::Expanding);
            alertsWidget->layout()->addItem(spacer);

            QPushButton *moreButton = new QPushButton("Notifications suivantes");
            alertsWidget->layout()->addWidget(moreButton);
            alertsWidget->setLayoutDirection(Qt::RightToLeft);
            connect(moreButton, SIGNAL(clicked(bool)), this, SLOT(UpAlertDays()));
            moreButton->setVisible(alertDays >= 0);
            moreButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

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

        if(alertDays > 1)
            alertsLabel->setText(QString("NOTIFICATIONS (%1 prochains jours)").arg(alertDays));

        LoadAlerts();
    }

    void TriggerEditSlot(QString type, QStringList necessary){
        emit TriggerEditHome(type, necessary);
    }

signals:
    void TriggerEditHome(QString type, QStringList necessary);

private:
    QLabel *alertsLabel;
    QWidget *alertsWidget = nullptr;
    int alertDays{};
    QSpacerItem *spacer = nullptr;
    QString lastDate;
};

#endif // HOMEPAGE_H
