#ifndef STATWIDGET_H
#define STATWIDGET_H

#include <QVBoxLayout>
#include <QFrame>
#include <QPainter>

#include "utils.h"

class GraphWidget : public QWidget {
public:
    GraphWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    GraphWidget(QString hColor) {color = QColor("#" + hColor);}

    void setData(const std::vector<QPointF>& data) {
        this->data = data;
        update();

        minY = data[0].y();
        maxY = data[0].y();

        for (QPointF point : data) {
            minY = std::min(minY, point.y());
            maxY = std::max(maxY, point.y());
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
            qreal yScale = (height() - 40) / (maxY - minY);

            // Draw graph
            painter.setPen(QPen(color, 2));
            QPointF prevPoint;
            for (size_t i = 0; i < data.size(); ++i) {
                QPointF point(i * xScale, (data[i].y() - minY) * yScale);
                if (i > 0) {
                    painter.drawLine(prevPoint, point);
                }
                prevPoint = point;
            }
        }
    }

private:
    std::vector<QPointF> data;
    qreal minY;
    qreal maxY;
    QColor color;
};

class StatWidget : public QFrame
{
    Q_OBJECT
public:
    explicit StatWidget(QWidget *parent = nullptr) : QFrame(parent){}
    StatWidget(QString type) : StatWidget(nullptr){
        this->type = type;

        setLayout(new QVBoxLayout());

        statLabel = new QLabel("x");
        statLabel->setStyleSheet("color:#333;");
        statLabel->setAlignment(Qt::AlignCenter);

        layout()->addWidget(statLabel);


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

        statGraph = new GraphWidget(hColor);
        statGraph->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        statGraph->setStyleSheet("background-color:white;");
        layout()->addWidget(statGraph);

        setStyleSheet("QFrame {border:2px solid #" + hColor + "; border-radius:5px;background-color:white;} "
                      "QLabel {border:none;}");


        UpdateStat();
    }

    void UpdateStat(){
        QSqlQuery query;


        // Charts
        std::vector<QPointF> points;

        int x = 0;
        for(QDate date = QDate::currentDate().addDays(-370); date <= QDate::currentDate(); date = date.addDays(10)){
            if(type == "currentDogs"){
                query.prepare("SELECT COUNT(*) "
                              "FROM Dogs "
                              "WHERE id_dog NOT IN "
                              "(SELECT id_dog "
                              "FROM Destinations "
                              "WHERE Destinations.date <= :date "
                              "AND (type = 'Entrée au refuge' "
                              "OR type = 'Mort') "
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

            points.push_back(QPointF(x, query.value(0).toFloat()));

            x += 1;
        }

        statGraph->setData(points);


        statLabel->setText(QString::number(points[points.size() - 1].y()));
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
};


#endif // STATWIDGET_H
