#ifndef EDITPAGE_H
#define EDITPAGE_H

#include <QWidget>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QDateEdit>
#include <QLineEdit>
#include <QComboBox>

class EditPage : public QWidget
{
    Q_OBJECT
public:
    explicit EditPage(QWidget *parent = nullptr);

public slots:
    void AddEntry();
    void Edit(QString type, QString infos);
    void ChangeEntryType(QString type);

private:
    void ClearAllPages();
    void SwitchPage(QString pageName);
};

#endif // EDITPAGE_H
