#ifndef EDITPAGE_H
#define EDITPAGE_H

#include "destinationpage.h"
#include "editdogwidget.h"
#include "utils.h"
#include "simplecrypt.h"

class EditPage : public QWidget
{
    Q_OBJECT
public:
    explicit EditPage(QWidget *parent = nullptr);
    QString CreatePersonIfNeeded(QString nameEnd, QWidget *parent = nullptr);
    QString CreateDogIfNeeded(QString nameEnd, QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;
    void AssignIdPeople(QWidget *page);
    void AssignIdDog(QWidget *page);
    void SetCrypto(SimpleCrypt *crypto) {this->crypto = crypto;}
    SimpleCrypt* GetCrypto() {return crypto;}
    void FillAnimalWidget(QString nameEnd, QString dogName, QString chip, QString sex, QString birthDate, QString description, QWidget *parent = nullptr);
    void ClearAllPages();

public slots:
    void AddEntry();
    void AddCare();
    void AddMember();
    void AddRedList();
    void AddLost();
    void AddVet();
    void AddAdoptionDemand();
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
    void RefreshMainWindow();

private:
    void SwitchPage(QString pageName);
    void AddDestPage();
    void UpdateButtons();
    void FillPeopleWidget(QString nameEnd, QString lastName, QString firstName, QString address, QString phone, QString email, QWidget *parent = nullptr);

private:
    QString lastType;
    QStringList currentNecessary = {};
    int destinationsNumber = 0;
    QStringList autofillList;
    QToolButton* removeButton;
    QStringList dealtIdPeople;
    QStringList dealtIdDog;
    SimpleCrypt *crypto;
};

#endif // EDITPAGE_H
