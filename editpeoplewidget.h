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

class EditPeopleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditPeopleWidget(QWidget *parent = nullptr);
    EditPeopleWidget(QString nameEnd);
    void showEvent(QShowEvent *event) override;

public slots:
    void FillOtherFields(QString s);
    void PreviewOtherFields(QString s = "");
};

#endif // EDITPEOPLEWIDGET_H
