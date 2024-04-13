#ifndef STATWIDGET_H
#define STATWIDGET_H

#include <QVBoxLayout>
#include <QFrame>
#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>

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
            if(minY == maxY)
                yScale = 1;
            else
                yScale = (height() - 40) / (maxY - minY);

            // Draw graph
            painter.setPen(QPen(color, 2));
            points.clear();
            QPointF prevPoint;
            for (size_t i = 0; i < data.size(); ++i) {
                QPointF point(i * xScale, (data[i].second - minY) * yScale);
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
        layout->setContentsMargins(0,6,0,0);

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
            infoLabel->setText("ADOPTIONS " + QString::number(QDate::currentDate().year()));
        }

        infoLabel->setStyleSheet("color:#" + hColor + ";");
        infoLabel->setAlignment(Qt::AlignCenter);

        layout->addWidget(infoLabel, 1, 0, 1, 1);

        statGraph = new GraphWidget(hColor);
        statGraph->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        statGraph->setStyleSheet("background-color:white;");
        layout->addWidget(statGraph, 2, 0, 3, 1);


        // daysInterval buttons
        weekButton = new QPushButton();
        weekButton->setText("Sem.");
        layout->addWidget(weekButton, 2, 1, 1, 1);
        connect(weekButton, &QPushButton::clicked, this, [=](){
            SelectDaysInterval(weekButton);
        });

        monthButton = new QPushButton();
        monthButton->setText("Mois");
        connect(monthButton, &QPushButton::clicked, this, [=](){
            SelectDaysInterval(monthButton);
        });
        layout->addWidget(monthButton, 3, 1, 1, 1);

        yearButton = new QPushButton();
        yearButton->setText("An");
        layout->addWidget(yearButton, 4, 1, 1, 1);
        connect(yearButton, &QPushButton::clicked, this, [=](){
            SelectDaysInterval(yearButton);
        });


        SelectDaysInterval(monthButton);

        setStyleSheet("QFrame {border:2px solid #" + hColor + "; border-radius:5px;background-color:white;padding-right:0;} "
                      "QLabel {border:none;}"
                      "QPushButton{"
                      "background:white;"
                      "color:#" + hColor + ";"
                      "border:2px solid #" + hColor + ";"
                      "border-radius:0px;"
                      "border-bottom:none;"
                      "border-right:none;"
                      "margin-right:0;"
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


        // Charts
        std::vector<QPair<QString, int>> points;

        int x = 0;
        for(QDate date = QDate::currentDate().addDays(-daysInterval * 7); date <= QDate::currentDate(); date = date.addDays(daysInterval)){
            if(type == "currentDogs"){
                query.prepare("SELECT COUNT(*) "
                              "FROM Dogs "
                              "WHERE id_dog NOT IN "
                              "(SELECT id_dog "
                              "FROM Destinations "
                              "WHERE Destinations.date <= :date "
                              "AND (type = 'Adoption' "
                              "OR type = 'Propriétaire' "
                              "OR type = 'Mort' "
                              "OR type LIKE 'Famille d_accueil') "
                              "UNION "
                              "SELECT id_dog "
                              "FROM Care_registry) "
                              "AND id_dog IN "
                              "(SELECT id_dog FROM ES_Registry WHERE date_prov <= :date)");
            }

            else if(type == "currentMembers"){
                query.prepare("SELECT COUNT(DISTINCT id_people) "
                              "FROM Members "
                              "WHERE date(Members.date, '+1 year') > date('now') "
                              "AND date <= :date");
            }

            else if(type == "adoptions"){
                query.prepare("SELECT COUNT(*) "
                              "FROM Dogs "
                              "WHERE id_dog IN "
                              "(SELECT id_dog "
                              "FROM Destinations "
                              "WHERE strftime('%Y', Destinations.date) = :currentYear "
                              "AND type = 'Adoption'"
                              "AND Destinations.date <= :date)");

                query.bindValue(":currentYear", QString::number(QDate::currentDate().year()));
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
        }

    }

private:
    QString type;
    QLabel *statLabel = nullptr, *infoLabel = nullptr;
    GraphWidget *statGraph = nullptr;
    int daysInterval = 10;
    QPushButton *weekButton, *monthButton, *yearButton;
    QString hColor;
};


#endif // STATWIDGET_H
