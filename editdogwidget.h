#ifndef EDITDOGWIDGET_H
#define EDITDOGWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QDateEdit>
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QStringListModel>
#include <QCompleter>
#include <QTimer>

#include "utils.h"

class EditDogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditDogWidget(QWidget *parent = nullptr);
    EditDogWidget(QString nameEnd);
    void showEvent(QShowEvent *event) override;
    QString GetOldId(){ return oldId; }
    void SetOldId(QString oldId){ this->oldId = oldId; }

public slots:
    void FillOtherFields(QString s);
    void PreviewOtherFields(QString s = "");

private:
    QStringList sexList, birthList;
    QString oldId = "-1";
};

#endif // EDITDOGWIDGET_H
