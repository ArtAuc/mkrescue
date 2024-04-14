#ifndef EDITPAGE_H
#define EDITPAGE_H

#include <QStackedWidget>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QCheckBox>
#include <QPushButton>
#include <QGraphicsColorizeEffect>

#include "widgets/destinationpage.h"
#include "widgets/editdogwidget.h"
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
    static QString GetField(QString name, QWidget* parent);
    static void SetField(QString name, QString value, QWidget* parent);
    static QStringList AddressList(QString address);
    static bool DateComparator(const QString& dest1, const QString& dest2);

public slots:
    void Edit(QString type, QStringList infos);
    void ChangeEntryType(QString type);
    void SaveEdit();
    void QuitEdit();
    void PrevDestPage();
    void NextDestPage();
    void UpdateDestinationPages(QString type = "");
    void SameDestCare();
    void RemoveCurrent();
    void FinishedCleaning(){cleaning = false;}

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

    bool cleaning = false;
};

#endif // EDITPAGE_H
