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
    void ChangeDestType(QString type);
    void SaveEdit();
    void QuitEdit();
    void SetField(QString name, QString value);
    QString GetField(QString name);

signals:
    void RefreshMainWindow(QString type);

private:
    void ClearAllPages();
    void SwitchPage(QString pageName);
    QString lastType;
    int currentId = -1;
};

#endif // EDITPAGE_H
