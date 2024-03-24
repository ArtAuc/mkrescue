#ifndef EDITDOGWIDGET_H
#define EDITDOGWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QDateEdit>
#include <QVBoxLayout>
#include <QSqlQuery>

class EditDogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditDogWidget(QWidget *parent = nullptr);
    EditDogWidget(QString nameEnd);
    void showEvent(QShowEvent *event) override;
signals:

};

#endif // EDITDOGWIDGET_H
