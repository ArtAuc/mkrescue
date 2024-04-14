#ifndef EDITPEOPLEWIDGET_H
#define EDITPEOPLEWIDGET_H

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QStringListModel>
#include <QCompleter>
#include <QDateEdit>
#include <QComboBox>
#include <QTimer>

#include "simplecrypt.h"
#include "utils.h"

class EditPeopleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditPeopleWidget(QWidget *parent = nullptr);
    EditPeopleWidget(QString nameEnd);
    void showEvent(QShowEvent *event) override;
    QString GetOldId(){ return oldId; }
    void SetOldId(QString oldId){ this->oldId = oldId; }
    void SetCrypto(SimpleCrypt *crypto){this->crypto = crypto;}

public slots:
    void ProcessFields(QString s, bool isReview);
    void LineEditFormat(QString text);

private:
    QString oldId = "-1";
    SimpleCrypt *crypto;
    QStringList cityList, postalCodeList;
};

#endif // EDITPEOPLEWIDGET_H
