#ifndef EDITPEOPLEWIDGET_H
#define EDITPEOPLEWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>


class EditPeopleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditPeopleWidget(QWidget *parent = nullptr);
    EditPeopleWidget(QString nameEnd);

signals:

};

#endif // EDITPEOPLEWIDGET_H
