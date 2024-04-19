#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QGridLayout>
#include <QScrollArea>

#include "widgets/statwidget.h"
#include "data/saveddata.h"

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

            statVerticalWidget = new QWidget(this);
            statVerticalWidget->setLayout(new QVBoxLayout());
            statVerticalWidget->layout()->addWidget(new StatWidget("currentDogs"));
            statVerticalWidget->layout()->addWidget(new StatWidget("currentMembers"));
            statVerticalWidget->layout()->addWidget(new StatWidget("adoptions"));
            statVerticalWidget->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 white, stop:1 #fafafa);");

            qobject_cast<QHBoxLayout*>(layout())->addWidget(statVerticalWidget);

            alertsWidget = findChild<QWidget*>("homeScrollContents");
            layout()->setSpacing(0);

            setStyleSheet("QWidget#homeScrollContents, QScrollArea {"
                                        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 white, stop:1 #fafafa);"
                                        "border: none;"
                                        "}");

            alertsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        }

        alertDays = 7;
    }

    void resizeEvent(QResizeEvent *event) override{
        QWidget::resizeEvent(event);

        if(alertsWidget != nullptr){
            alertsWidget->layout()->setContentsMargins(0.03 * width(),0,0.03 * width(),0);
            statVerticalWidget->layout()->setContentsMargins(0.03 * width(),0,0.03 * width(),0);
            statVerticalWidget->setMaximumWidth(width() / 3);


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

        for(StatWidget *c : findChildren<StatWidget*>()){
            c->resizeEvent(event);
        }
    }

    void LoadAlerts(QDate lastTimeExport = QDate()){
        for(StatWidget *c : findChildren<StatWidget*>())
            c->UpdateStat();

        QString lastDate = "", lastVaccineDate = "";

        if(lastTimeExport.isNull())
            lastTimeExport = this->lastTimeExport;

        this->lastTimeExport = lastTimeExport;
        lastVetButton = nullptr;
        lastVaccineButton = nullptr;
        lastVetDateReason = "";
        lastVaccineNecessary.clear();

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
                                  ""
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
                                  "AND Dogs.id_dog NOT IN (SELECT LastestDest.id_dog " // Make sur the dog is not out
                                      "FROM ( "
                                      "    SELECT id_dog, MAX(date_prov) AS max_date_prov "
                                      "    FROM ES_Registry "
                                      "    GROUP BY id_dog "
                                      ") AS LastestProv "
                                      "LEFT JOIN ( "
                                      "    SELECT type, date_prov, id_dog, MAX(date) AS max_date "
                                      "    FROM Destinations "
                                      "    GROUP BY date_prov, id_dog "
                                      ") AS LastestDest ON (LastestDest.id_dog = LastestProv.id_dog AND LastestDest.date_prov = LastestProv.max_date_prov) "
                                      "WHERE ( "
                                      "    LastestDest.type = 'Adoption' "
                                      "    OR LastestDest.type = 'Propriétaire' "
                                      "    OR LastestDest.type = 'Mort' "
                                      "    OR LastestDest.type LIKE 'Famille d_accueil' "
                                      "))"
                                  ""
                                  "UNION "
                                  "SELECT :scheduledExportDate AS date, 'Prévoir l''export PDF du registre E/S',  '', strftime('%Y', :lastTimeExport), '' "
                                  ") AS Results ";



            if(alertDays >= 0)
                queryString += "WHERE Results.date BETWEEN DATE('now') AND DATE('now', '+' || " + QString::number(alertDays) + " || ' day') ";            
            else
                queryString += "WHERE Results.date >= DATE('now')";


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

                    QString newVetDateReason = dateString + timeString + query.value(3).toString();
                    if(lastVetDateReason == newVetDateReason){
                        if(lastVetButton != nullptr){
                            // Count number of dogs
                            int dogCount = 2;
                            if(lastVetButton->text().contains("RDV groupé")){
                                QRegularExpressionMatch match = QRegularExpression("RDV groupé (\\d+)").match(lastVetButton->text());
                                if(match.hasMatch()){
                                    dogCount = match.captured(1).toInt() + 1;
                                }
                            }
                            lastVetButton->setText(timeString + "RDV groupé " + QString::number(dogCount) + " chiens (" + query.value(3).toString() + ")");
                        }
                        continue;
                    }

                    lastVetDateReason = newVetDateReason;
                }

                else if(type.startsWith("Prévoir le rappel de vaccin")){
                    if(lastVaccineButton != nullptr &&
                            lastVaccineNecessary.size() < savedData->MaxDogs().toInt() &&
                            lastVaccineDate != "" &&
                            QDate::fromString(lastVaccineDate, "yyyy-MM-dd").daysTo(query.value(0).toDate()) < savedData->MaxDays().toInt()){
                        // Count number of dogs
                        int dogCount = 2;
                        if(lastVaccineButton->text().contains("Prévoir le vaccin groupé")){
                            QRegularExpressionMatch match = QRegularExpression("Prévoir le vaccin groupé \\((\\d+)").match(lastVaccineButton->text());
                            if(match.hasMatch()){
                                dogCount = match.captured(1).toInt() + 1;
                            }
                        }
                        lastVaccineButton->setText("Prévoir le vaccin groupé (" + QString::number(dogCount) + " chiens)");

                        lastVaccineNecessary.append(query.value(4).toString());

                        necessary = lastVaccineNecessary;

                        disconnect(lastVaccineButton, nullptr, nullptr, nullptr);
                        connect(lastVaccineButton, &QPushButton::clicked, this, [this, necessary]() {
                            HandleAlertPress("Prévoir le rappel", necessary);
                        });

                        continue;
                    }
                    color = QColor("#20718e");
                    necessary = {query.value(4).toString()};
                    lastVaccineNecessary = necessary;
                    lastVaccineDate = query.value(0).toString();
                }

                else if(type.startsWith("Prévoir l'export")){
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

                if(type.startsWith("RDV Vétérinaire"))
                    lastVetButton = but;

                else if(type.startsWith("Prévoir le rappel"))
                    lastVaccineButton = but;

                else{
                    lastVetButton = nullptr;
                }

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
                connect(moreButton, SIGNAL(clicked(bool)), this, SLOT(UpAlertDays()));
            }

            resizeEvent(nullptr);
        }
    }

    void SetSavedData(SavedData *savedData){this->savedData = savedData;}

public slots:
    void UpAlertDays(){
        alertDays = -1;
        LoadAlerts();
    }

    void HandleAlertPress(QString type, QStringList necessary){
        if(type.startsWith("Prévoir l'export"))
            emit ExportES(necessary[0]);

        else if (type.startsWith("Prévoir le rappel"))
            emit TriggerEditHome("vaccine", necessary);

        else
            emit TriggerEditHome("vet", necessary);
    }

signals:
    void TriggerEditHome(QString type, QStringList necessary);
    void ExportES(QString year);

private:
    QWidget *alertsWidget = nullptr, *statVerticalWidget = nullptr;
    int alertDays{};
    QSpacerItem *spacer = nullptr;
    QDate lastTimeExport;
    QString lastVetDateReason;
    QPushButton *lastVetButton;
    QPushButton *lastVaccineButton;
    QStringList lastVaccineNecessary;
    QString lastVaccineDate;
    SavedData *savedData;
};

#endif // HOMEPAGE_H
