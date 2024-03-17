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
    QString CreatePersonIfNeeded(QStringList infos);
    QString CreateDogIfNeeded(QStringList infos);

public slots:
    void AddEntry();
    void Edit(QString type, QStringList infos);
    void ChangeEntryType(QString type);
    void SaveEdit();
    void QuitEdit();

signals:
    void RefreshMainWindow(QString type);

private:
    void ClearAllPages();
    void SwitchPage(QString pageName);
    QString lastType;
};

#endif // EDITPAGE_H
