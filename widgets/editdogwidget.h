#ifndef EDITDOGWIDGET_H
#define EDITDOGWIDGET_H

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QCompleter>
#include <QStringListModel>
#include <QTimer>

#include "utils.h"
#include "widgets/customdatetimeedit.h"

class EditDogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditDogWidget(QWidget *parent = nullptr);
    EditDogWidget(QString nameEnd);
    void showEvent(QShowEvent *event) override;
    QString GetOldId(){ return oldId; }
    void SetOldId(QString oldId){ this->oldId = oldId;}

public slots:
    void ProcessFields(QString s, bool isPreview);
    void LineEditFormat(QString text);

signals:
    void SelectedDog();

private:
    QStringList sexList, birthList, descriptionList;
    QString oldId = "-1";
};

#endif // EDITDOGWIDGET_H
