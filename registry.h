#ifndef REGISTRY_H
#define REGISTRY_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QHeaderView>
#include <QScrollBar>
#include <QToolButton>


class Registry : public QWidget
{
    Q_OBJECT
public:
    explicit Registry(QWidget *parent = nullptr);
    void ColorRow(int row, bool gray);
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

protected:
    QTableWidget* table;
    QToolButton* addButton = nullptr;
};

#endif // REGISTRY_H
