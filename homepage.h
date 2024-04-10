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
        setLayout(new QVBoxLayout(this));
        layout()->setContentsMargins(0,0,0,0);

        QWidget *horizontalWidget = new QWidget(this);
        horizontalWidget->setLayout(new QHBoxLayout());
        horizontalWidget->layout()->addWidget(new StatWidget("currentDogs"));
        horizontalWidget->layout()->addWidget(new StatWidget("currentMembers"));
        horizontalWidget->layout()->addWidget(new StatWidget("currentMembers"));
        horizontalWidget->layout()->addWidget(new StatWidget("currentMembers"));
        horizontalWidget->layout()->addWidget(new StatWidget("currentMembers"));
        horizontalWidget->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 white, stop:1 #efefef);");

        layout()->addWidget(horizontalWidget);
        layout()->addWidget(new QScrollArea);
        layout()->setSpacing(0);

        setLayout(layout());
    }

    void showEvent(QShowEvent *event) override{
        QWidget::showEvent(event);

        for(StatWidget *c : findChildren<StatWidget*>())
            c->UpdateStat();
    }

    void resizeEvent(QResizeEvent *event) override{
        QWidget::resizeEvent(event);
        for(StatWidget *c : findChildren<StatWidget*>())
            c->resizeEvent(event);
    }

signals:

};

#endif // HOMEPAGE_H
