#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTreeWidgetItem>

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

private:
    Ui::MainWindow *ui;
    Database db;
    QList<QToolButton*> modifyButtons;
};
#endif // MAINWINDOW_H
