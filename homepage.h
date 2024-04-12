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
            layout()->setContentsMargins(0,0,0,0);

            QWidget *verticalWidget = new QWidget(this);
            verticalWidget->setLayout(new QVBoxLayout());
            verticalWidget->layout()->addWidget(new StatWidget("currentDogs"));
            verticalWidget->layout()->addWidget(new StatWidget("currentMembers"));
            verticalWidget->layout()->addWidget(new StatWidget("adoptions"));
            verticalWidget->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 white, stop:1 #fafafa);");

            qobject_cast<QHBoxLayout*>(layout())->insertWidget(0, verticalWidget);

            alertsWidget = findChild<QWidget*>("homeScrollContents");
            layout()->setSpacing(0);

            alertsWidget->setStyleSheet("QWidget#homeScrollContents, QScrollArea {"
                                        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 white, stop:1 #fafafa);"
                                        "border: none;"
                                        "}");
        }

        alertDays = 7;
    }

    void resizeEvent(QResizeEvent *event) override{
        QWidget::resizeEvent(event);

        if(alertsWidget != nullptr){
            findChild<QScrollArea*>()->setMinimumWidth(0.7 * width());
            alertsWidget->layout()->setContentsMargins(0.03 * width(),0,0.03 * width(),0);
            for(StatWidget *c : findChildren<StatWidget*>())
                c->resizeEvent(event);


            for (QPushButton *but : findChildren<QPushButton*>()){
                QFont font = but->font();
                font.setPointSizeF(0.01 * width());
                but->setFont(font);
                but->setMaximumWidth(0.8 * findChild<QScrollArea*>()->width());
            }

            for (QLabel *lab : alertsWidget->findChildren<QLabel*>()){
                QFont font = lab->font();
                font.setPointSizeF(0.01 * width());
                lab->setFont(font);
                lab->setMaximumWidth(0.8 * findChild<QScrollArea*>()->width());
            }
        }
    }

    void LoadAlerts(QDate lastTimeExport = QDate()){
        for(StatWidget *c : findChildren<StatWidget*>())
            c->UpdateStat();

        QString lastDate = "";

        if(lastTimeExport.isNull())
            lastTimeExport = this->lastTimeExport;

        this->lastTimeExport = lastTimeExport;

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
                                  "WHERE Vet.reason LIKE 'Vaccin%' "
                                  "UNION "
                                  "SELECT :scheduledExportDate AS date, 'Prévoir l''exportation PDF du registre E/S',  '', strftime('%Y', :lastTimeExport), '' "
                                  ") AS Results ";



            if(alertDays >= 0){
                queryString += "WHERE Results.date BETWEEN DATE('now') AND DATE('now', '+' || " + QString::number(alertDays) + " || ' day') ";
            }

            queryString +="ORDER BY Results.date;";


            query.prepare(queryString);

            query.bindValue(":lastTimeExport", lastTimeExport);
            if(lastTimeExport.isValid()){
                if(lastTimeExport.addMonths(6) >= QDate::currentDate())
                    query.bindValue(":scheduledExportDate", lastTimeExport.addMonths(6));
                else // scheduled date has passed
                    query.bindValue(":scheduledExportDate", QDate::currentDate());
            }
            else{
                query.bindValue(":scheduledExportDate", QDate::currentDate());
                lastTimeExport = QDate::currentDate();
            }


            HandleErrorExec(&query);

            while(query.next()){
                QLocale locale = QLocale(QLocale::French);

                QString dateString = locale.toString(query.value(0).toDate());
                dateString = dateString.toUpper();
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

                else if(type.startsWith("Prévoir le rappel de vaccin")){
                    color = QColor("#20718e");
                    necessary = {query.value(4).toString()};
                }

                else if(type.startsWith("Prévoir l'exportation")){
                    color = QColor("#984800");
                    necessary = {QString::number(lastTimeExport.year())};
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


                QPushButton *but = new QPushButton(AutoBreak(timeString + type + QString(query.value(2).toString().isEmpty() ? "" : " : " + query.value(2).toString()) + QString(query.value(3).toString().isEmpty() ? "" : " (" + query.value(3).toString() + ")"), 50));

                connect(but, &QPushButton::clicked, this, [=]() {
                    HandleAlertPress(type, necessary);
                });

                but->setStyleSheet(stylesheet);
                alertsWidget->layout()->addWidget(but);
            }

            spacer = new QSpacerItem(1, 1, QSizePolicy::Preferred, QSizePolicy::Expanding);

            // No alerts found
            if(alertsWidget->findChildren<QPushButton*>().size() == 0){
                alertsWidget->layout()->addItem(spacer);

                QLabel *noneLabel = new QLabel("Aucune notification pour les 7 prochains jours");
                noneLabel->setAlignment(Qt::AlignCenter);
                noneLabel->setStyleSheet("font-weight:bold;border-bottom:1px solid #aaa;color:#333;margin:10px;");
                alertsWidget->layout()->addWidget(noneLabel);
            }

            alertsWidget->layout()->addItem(spacer);



            if(alertDays >= 0){
                QPushButton *moreButton = new QPushButton("Notifications suivantes");
                alertsWidget->layout()->addWidget(moreButton);
                alertsWidget->setLayoutDirection(Qt::RightToLeft);
                connect(moreButton, SIGNAL(clicked(bool)), this, SLOT(UpAlertDays()));
                moreButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
            }


            resizeEvent(nullptr);
        }
    }

public slots:
    void UpAlertDays(){
        alertDays = -1;
        LoadAlerts();
    }

    void HandleAlertPress(QString type, QStringList necessary){
        if(type.startsWith("Prévoir l'exportation"))
            emit ExportES(necessary[0]);

        else
            emit TriggerEditHome("vet", necessary);
    }

signals:
    void TriggerEditHome(QString type, QStringList necessary);
    void ExportES(QString year);

private:
    QWidget *alertsWidget = nullptr;
    int alertDays{};
    QSpacerItem *spacer = nullptr;
    QDate lastTimeExport;
};

#endif // HOMEPAGE_H
