#ifndef DOGREGISTRY_H
#define DOGREGISTRY_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QHeaderView>
#include <QScrollBar>
#include <QToolButton>


class DogRegistry : public QWidget
{
    Q_OBJECT
public:
    explicit DogRegistry(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;
    void showEvent( QShowEvent* event ) override;
    void SetType(QString type){this->type = type;}

private:
    void ColorRow(int row, bool gray);

private:
    QTableWidget* table = nullptr;
    QLabel* label1 = nullptr, *label2 = nullptr, *label3 = nullptr;
    QString type;
};

#endif // DOGREGISTRY_H
