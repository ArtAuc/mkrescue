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

class EditDogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditDogWidget(QWidget *parent = nullptr);
    EditDogWidget(QString nameEnd);
    void showEvent(QShowEvent *event) override;

public slots:
    void FillOtherFields(QString s);
    void PreviewOtherFields(QString s = "");

private:
    QStringList sexList;
};

#endif // EDITDOGWIDGET_H
