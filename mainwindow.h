#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTreeWidgetItem>

#include "database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void InitEntryRegistry();

public slots:
    void ChangePage(QTreeWidgetItem* item);
    void LoadEntryRegistry(QString year);

private:
    Ui::MainWindow *ui;
    Database db;
};
#endif // MAINWINDOW_H
