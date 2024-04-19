#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSqlRecord>
#include <QPrinter>
#include <QMainWindow>

#include "data/database.h"
#include "widgets/menutree.h"
#include "widgets/dogcard.h"
#include "data/saveddata.h"

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
    void changeEvent(QEvent *event) override {
        if (event->type() == QEvent::ActivationChange || event->type() == QEvent::WindowStateChange)
            resizeEvent(nullptr);

        QMainWindow::changeEvent(event);
    }


public slots:
    void ChangePage(QTreeWidgetItem* item = nullptr);
    void LoadDogCards(QString search = "");
    void LoadEntryRegistry(QString year, QString search = "");
    void LoadCareRegistry(QString year, QString search = "");
    void LoadRedList(QString search = "");
    void LoadMembers(QString year, QString search = "");
    void LoadLost(QString search = "");
    void LoadVet(QString search = "");
    void LoadAdoptionDemand(QString search = "");
    void ToggleModifyButtons();
    void Search(QString search = nullptr);
    void TriggerEdit(QString type, QStringList infos);
    void Clean();
    void SelectDogCard();
    void UnselectDogCard();
    void ToggleFoundBoxText();
    void ToggleSatisfiedBoxText();
    void ToggleReasonEdit();
    void ToggleLock(QByteArray h = NULL, QString email = "", QString appPassword = "");

private:
    void ExportRegistry(QString type, QString year = "");
    QString ClearUselessBreaks(QString s);
    void InitYearRegistry(QString type, QString year);
    void InitExportButtons();
    void InitEditWidgets();

private:
    Ui::MainWindow *ui;
    Database db;
    QList<QToolButton*> modifyButtons;
    int rowClicked;
    SavedData *savedData;
    SimpleCrypt *crypto;
    QMovie *syncMovie = nullptr;
    QRect screenGeometry;
};
#endif // MAINWINDOW_H
