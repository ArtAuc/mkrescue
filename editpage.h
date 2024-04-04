#ifndef EDITPAGE_H
#define EDITPAGE_H

#include "destinationpage.h"
#include "editdogwidget.h"
#include "utils.h"

class EditPage : public QWidget
{
    Q_OBJECT
public:
    explicit EditPage(QWidget *parent = nullptr);
    QString CreatePersonIfNeeded(QString last_name, QString first_name, QString phone, QString email, QString address, QString old_id_people);
    QString CreateDogIfNeeded(QStringList infos);
    void resizeEvent(QResizeEvent *event) override;
    void AssignIdPeople(QWidget *page);

public slots:
    void AddEntry();
    void AddCare();
    void AddMember();
    void AddRedList();
    void AddLost();
    void Edit(QString type, QStringList infos);
    void ChangeEntryType(QString type);
    void SaveEdit();
    void QuitEdit();
    void PrevDestPage();
    void NextDestPage();
    void UpdateDestinationPages(QString type = "");
    void SameDestCare();
    void RemoveCurrent();

signals:
    void RefreshMainWindow(QString type);

private:
    void ClearAllPages();
    void SwitchPage(QString pageName);
    void AddDestPage();
    void UpdateButtons();

private:
    QString lastType;
    QStringList currentNecessary = {};
    int destinationsNumber = 0;
    QStringList autofillList;
    QToolButton* removeButton;
    QStringList dealtIdPeople;
};

#endif // EDITPAGE_H
