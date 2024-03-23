#ifndef EDITPAGE_H
#define EDITPAGE_H

#include <QWidget>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QDateEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QLayout>
#include <QSpinBox>
#include <QLabel>
#include <QToolButton>
#include <QGraphicsColorizeEffect>
#include <QSqlError>

#include "destinationpage.h"

class EditPage : public QWidget
{
    Q_OBJECT
public:
    explicit EditPage(QWidget *parent = nullptr);
    QString CreatePersonIfNeeded(QStringList infos);
    QString CreateDogIfNeeded(QStringList infos);
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void AddEntry();
    void Edit(QString type, QStringList infos);
    void ChangeEntryType(QString type);
    void SaveEdit();
    void QuitEdit();
    void SetField(QString name, QString value, QWidget* parent = nullptr);
    QString GetField(QString name, QWidget* parent = nullptr);
    void PrevDestPage();
    void NextDestPage();
    void UpdateDestinationPages(QString type = "");
    void LoadAutofill();

signals:
    void RefreshMainWindow(QString type);
    void AbandonRedList(QString id_people, QString reason);

private:
    void ClearAllPages();
    void SwitchPage(QString pageName);
    void AddDestPage();
    QString lastType;
    int currentId = -1;
    int destinationsNumber = 0;
    QStringList autofillList;
};

#endif // EDITPAGE_H
