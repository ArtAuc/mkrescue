#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTreeWidgetItem>
#include <QSqlRecord>

#include "database.h"
#include "menutree.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void InitRegistry(QString type);
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void ChangePage(QTreeWidgetItem* item);
    void LoadEntryRegistry(QString year, QString search = "");
    void LoadCareRegistry(QString year, QString search = "");
    void ToggleModifyButtons();
    void Search(QString search);
    void TriggerEdit(QString type, QStringList necessary);
    void RefreshRegistry(QString type);

private:
    Ui::MainWindow *ui;
    Database db;
    QList<QToolButton*> modifyButtons;
    int rowClicked;
};
#endif // MAINWINDOW_H
