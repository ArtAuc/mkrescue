#ifndef REGISTRY_H
#define REGISTRY_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QHeaderView>
#include <QScrollBar>
#include <QToolButton>
#include <QPushButton>
#include <QLayout>
#include "utils.h"


class Registry : public QWidget
{
    Q_OBJECT
public:
    explicit Registry(QWidget *parent = nullptr);
    void ColorRow(int row, bool gray);
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void SetType(QString type){this->type = type;}
    float SumWidth();

protected:
    QTableWidget* table;
    QToolButton* addButton = nullptr;
    QString type;
    QWidget *horizontalPlacehold = nullptr;
};

#endif // REGISTRY_H
