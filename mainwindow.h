#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTreeWidgetItem>
#include <QSqlRecord>
#include <QMessageBox>

#include "database.h"
#include "menutree.h"
#include "dogcard.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void ChangePage(QTreeWidgetItem* item);
    void LoadDogCards(QString search = "");
    void LoadEntryRegistry(QString year, QString search = "");
    void LoadCareRegistry(QString year, QString search = "");
    void LoadRedList(QString search = "");
    void LoadMembers(QString year, QString search = "");
    void ToggleModifyButtons();
    void Search(QString search);
    void TriggerEdit(QString type, QStringList necessary);
    void RefreshPage(QString type);
    void Clean();
    void SelectDogCard();

private:
    QString ClearUselessBreaks(QString s);
    void InitYearRegistry(QString type, QString year);

private:
    Ui::MainWindow *ui;
    Database db;
    QList<QToolButton*> modifyButtons;
    int rowClicked;
};
#endif // MAINWINDOW_H
