#ifndef STATWIDGET_H
#define STATWIDGET_H

#include <QVBoxLayout>
#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>
#include <QLabel>
#include <QPushButton>

#include "utils.h"

class GraphWidget : public QWidget {
public:
    GraphWidget(QWidget *parent = nullptr) : QWidget(parent) {setMouseTracking(true);}
    GraphWidget(QString hColor) : GraphWidget(nullptr) {color = QColor("#" + hColor);}

    void setData(const std::vector<QPair<QString, int>>& data) {
        this->data = data;
        update();

        minY = data[0].second;
        maxY = data[0].second;

        for (QPair<QString, int> point : data) {
            minY = std::min(minY, point.second);
            maxY = std::max(maxY, point.second);
        }
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QWidget::paintEvent(event);

        QPainter painter(this);

        painter.setRenderHint(QPainter::Antialiasing, true);

        painter.translate(20, height() - 20);
        painter.scale(1, -1);


        if (!data.empty()) {

            qreal xScale = (width() - 40) / (data.size() - 1);
            qreal yScale;
            if(minY == maxY){
                yScale = 1;
            }
            else
                yScale = (height() - 40) / (maxY - minY);

            // Draw graph
            painter.setPen(QPen(color, 2));
            points.clear();
            QPointF prevPoint;
            for (size_t i = 0; i < data.size(); ++i) {
                QPointF point(i * xScale, (data[i].second - minY) * yScale);
                if(minY == maxY)
                    point.setY((height() - 40) / 2);
                points.push_back(QPointF(point.x(), -point.y()) + QPointF(20, height() - 20));
                painter.drawEllipse(point, 2, 2);
                if (i > 0) {
                    painter.drawLine(prevPoint, point);
                }
                prevPoint = point;
            }
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        QWidget::mouseMoveEvent(event);


        // Check if mouse is over an ellipse
        for (size_t i = 0; i < points.size(); ++i) {
            if (QRectF(points[i].x() - 10, points[i].y() - 10, 20, 20).contains(event->pos())) {
                QToolTip::showText(event->globalPosition().toPoint(), (data[i].first + " : " + QString::number(data[i].second)));
                return;
            }
        }

        QToolTip::hideText();
    }

private:
    std::vector<QPair<QString, int>> data;
    std::vector<QPointF> points;
    int minY;
    int maxY;
    QColor color;
};

class StatWidget : public QFrame
{
    Q_OBJECT
public:
    explicit StatWidget(QWidget *parent = nullptr) : QFrame(parent){}
    StatWidget(QString type) : StatWidget(nullptr){
        this->type = type;

        QGridLayout *layout = new QGridLayout();
        setLayout(layout);
        layout->setSpacing(0);
        layout->setContentsMargins(0,0,0,0);


        statLabel = new QLabel("x");
        statLabel->setStyleSheet("color:#333;");
        statLabel->setAlignment(Qt::AlignCenter);

        layout->addWidget(statLabel, 0, 0, 1, 1);


        infoLabel = new QLabel;

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
            infoLabel->setText("ADOPTIONS");
        }

        else if(type == "pound"){
            hColor = "db759c";
            infoLabel->setText("FOURRIÈRES");
        }

        else if(type == "poundLeft"){
            hColor = "db759c";
            infoLabel->setText("NON RÉCUPÉRÉES");
        }

        else if(type == "abandons"){
            hColor = "a86ba2";
            infoLabel->setText("ABANDONS");
        }

        infoLabel->setStyleSheet("color:#" + hColor + ";padding:6px;");
        infoLabel->setAlignment(Qt::AlignCenter);

        layout->addWidget(infoLabel, 1, 0, 1, 1);

        if(type == "currentDogs" || type == "currentMembers"){
            layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Preferred, QSizePolicy::Expanding), 2, 0, 1, 2);

            statGraph = new GraphWidget(hColor);
            statGraph->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            statGraph->setStyleSheet("background-color:white;");
            layout->addWidget(statGraph, 3, 0, 3, 1);


            // daysInterval buttons
            weekButton = new QPushButton();
            weekButton->setText("Sem.");
            layout->addWidget(weekButton, 3, 1, 1, 1);

            monthButton = new QPushButton();
            monthButton->setText("Mois");
            layout->addWidget(monthButton, 4, 1, 1, 1);

            yearButton = new QPushButton();
            yearButton->setText("An");
            layout->addWidget(yearButton, 5, 1, 1, 1);

            for(QPushButton *b : findChildren<QPushButton*>()){
                connect(b, &QPushButton::clicked, this, [=](){
                    SelectDaysInterval(b);
                });
            }


            SelectDaysInterval(monthButton);
        }

        else{ // Stats without graph
            infoLabel->setText(infoLabel->text() + "\nEN " + QString::number(QDate::currentDate().year()));
        }

        setStyleSheet("QFrame {border:2px solid #" + hColor + "; border-radius:5px;background-color:white;padding:0;} "
                      "QLabel {border:none;}"
                      "QPushButton{"
                      "background:white;"
                      "color:#" + hColor + ";"
                      "border:2px solid #" + hColor + ";"
                      "border-radius:0px;"
                      "border-bottom:none;"
                      "border-right:none;"
                      "margin-right:0;"
                      "padding:0;"
                      "}");



        UpdateStat();
    }

    void SelectDaysInterval(QPushButton *but){
        if(but == weekButton)
            daysInterval = 1;
        else if(but == monthButton)
            daysInterval = 4;
        else if(but == yearButton)
            daysInterval = 37;

        weekButton->setStyleSheet("");
        monthButton->setStyleSheet("");
        yearButton->setStyleSheet("");

        but->setStyleSheet("background-color:#" + hColor + ";color:white");

        UpdateStat();
    }

    void UpdateStat(){
        QSqlQuery query;

        if(type == "currentDogs" || type == "currentMembers"){
            // Charts
            std::vector<QPair<QString, int>> points;

            int x = 0;
            for(QDate date = QDate::currentDate().addDays(-daysInterval * 7); date <= QDate::currentDate(); date = date.addDays(daysInterval)){
                if(type == "currentDogs"){
                    query.prepare("SELECT COUNT(*) "
                                  "FROM Dogs "
                                  "WHERE id_dog NOT IN "
                                  "(SELECT LastestDest.id_dog "
                                  "FROM ( "
                                  "    SELECT id_dog, MAX(date_prov) AS max_date_prov "
                                  "    FROM ES_Registry "
                                  "    WHERE date_prov <= :date "
                                  "    GROUP BY id_dog "
                                  ") AS LastestProv "
                                  "LEFT JOIN ( "
                                  "    SELECT type, date_prov, id_dog, MAX(date) AS max_date "
                                  "    FROM Destinations "
                                  "    WHERE date <= :date "
                                  "    GROUP BY date_prov, id_dog "
                                  ") AS LastestDest ON (LastestDest.id_dog = LastestProv.id_dog AND LastestDest.date_prov = LastestProv.max_date_prov) "
                                  "WHERE ( "
                                  "    LastestDest.type = 'Adoption' "
                                  "    OR LastestDest.type = 'Propriétaire' "
                                  "    OR LastestDest.type = 'Mort' "
                                  "    OR LastestDest.type LIKE 'Famille d_accueil' "
                                  "))"
                                  "AND id_dog IN "
                                  "(SELECT id_dog FROM ES_Registry WHERE date_prov <= :date)");
                }

                else if(type == "currentMembers"){
                    query.prepare("SELECT COUNT(DISTINCT id_people) "
                                  "FROM Members "
                                  "WHERE date(Members.date, '+1 year') > :date "
                                  "AND date <= :date");
                }

                query.bindValue(":date", date);

                HandleErrorExec(&query);
                query.next();

                points.push_back(QPair<QString, int>(date.toString("dd/MM/yyyy"), query.value(0).toInt()));

                x += 1;
            }

            statGraph->setData(points);

            statLabel->setText(QString::number(points[points.size() - 1].second));
        }

        else{ // Stats without charts
            if(type == "adoptions"){
                query.prepare("SELECT COUNT(*) "
                              "FROM Dogs "
                              "WHERE id_dog IN "
                              "(SELECT id_dog "
                              "FROM Destinations "
                              "WHERE strftime('%Y', Destinations.date) = :currentYear "
                              "AND type = 'Adoption')");

                query.bindValue(":currentYear", QString::number(QDate::currentDate().year()));

                HandleErrorExec(&query);

                query.next();
                statLabel->setText(query.value(0).toString());
            }

            else if (type.startsWith("pound")){
                int count = 0;

                if(type == "pound")
                    query.prepare("SELECT type_prov "
                                  "FROM ES_Registry "
                                  "WHERE strftime('%Y', date_prov) = :currentYear");

                else if (type == "poundLeft")
                    query.prepare("SELECT ES_Registry.type_prov, ES_Registry.date_prov, ES_Registry.id_dog "
                                  "FROM ES_Registry "
                                  "LEFT JOIN Destinations ON Destinations.id_dog = ES_Registry.id_dog "
                                  "WHERE strftime('%Y', ES_Registry.date_prov) = :currentYear "
                                  "AND Destinations.type != 'Propriétaire' "
                                  "AND ES_Registry.date_prov = ("
                                  "SELECT MAX(date_prov) "
                                  "FROM ES_Registry AS esr "
                                  "WHERE esr.id_dog = ES_Registry.id_dog "
                                  "GROUP BY esr.id_dog)");

                query.bindValue(":currentYear", QString::number(QDate::currentDate().year()));

                HandleErrorExec(&query);

                while(query.next()){
                    QString type_prov = crypto->decryptToString(query.value(0).toString());
                    if(type_prov.startsWith("Fourrière")){
                        count++;
                    }
                }

                statLabel->setText(QString::number(count));
            }

            else if(type == "abandons"){
                int count = 0;

                query.prepare("SELECT type_prov "
                              "FROM ES_Registry "
                              "WHERE strftime('%Y', date_prov) = :currentYear");

                query.bindValue(":currentYear", QString::number(QDate::currentDate().year()));

                HandleErrorExec(&query);

                while(query.next()){
                    QString type_prov = crypto->decryptToString(query.value(0).toString());
                    if(type_prov.startsWith("Abandon")){
                        count++;
                    }
                }

                statLabel->setText(QString::number(count));
            }
        }
    }

    void resizeEvent(QResizeEvent *event) override{
        QFrame::resizeEvent(event);

        if(statLabel != nullptr && infoLabel != nullptr) {
            QWidget *parent  = qobject_cast<QWidget*>(this->parent());
            QFont font = infoLabel->font();
            font.setBold(true);
            font.setPointSizeF(0.03 * parent->width());
            infoLabel->setFont(font);

            font.setPointSizeF(0.06 * parent->width());
            statLabel->setFont(font);

            setMaximumHeight(parent->height() / 4);

            if(statGraph != nullptr)
                statGraph->setMaximumWidth(0.7 * width());
        }

    }

public:

private:
    QString type;
    QLabel *statLabel = nullptr, *infoLabel = nullptr;
    GraphWidget *statGraph = nullptr;
    int daysInterval = 10;
    QPushButton *weekButton, *monthButton, *yearButton;
    QString hColor;
};

#endif // STATWIDGET_H
