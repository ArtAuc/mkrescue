#ifndef EDITPEOPLEWIDGET_H
#define EDITPEOPLEWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QSqlQuery>
#include <QStringListModel>
#include <QCompleter>
#include <QStandardItem>
#include <QAbstractItemView>
#include <QRegularExpression>
#include <QTimer>
#include <QRegularExpressionValidator>

#include "utils.h"

class EditPeopleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditPeopleWidget(QWidget *parent = nullptr);
    EditPeopleWidget(QString nameEnd, bool simplified = false);
    void showEvent(QShowEvent *event) override;
    QString GetOldId(){ return oldId; }
    void SetOldId(QString oldId){ this->oldId = oldId; }

public slots:
    void FillOtherFields(QString s);
    void PreviewOtherFields(QString s = "");

private:
    QString oldId = "-1";
};

#endif // EDITPEOPLEWIDGET_H
