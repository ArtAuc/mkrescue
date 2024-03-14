#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);

    InitMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitMenu()
{
    QTreeWidget* tree = ui->menuTree;
    connect(tree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(ChangePage(QTreeWidgetItem*)));
}

// Change selected page from stacked widget, based on the selected menu item
void MainWindow::ChangePage(QTreeWidgetItem* item)
{
    QStackedWidget* stacked = ui->stackedWidget;
    QString txt = item->text(0);
    if (txt == "Entrées/Sorties")
        stacked->setCurrentWidget(ui->entryRegistryPage);
    else if (txt == "Garderie")
        stacked->setCurrentWidget(ui->careRegistryPage);
    else if (txt == "Adhérents")
        stacked->setCurrentWidget(ui->membersPage);
    else if (txt == "Liste rouge adoptants")
        stacked->setCurrentWidget(ui->redListPage);
    else if (txt == "Animaux perdus")
        stacked->setCurrentWidget(ui->lostPage);
    else if (txt == "Demandes d'adoption")
        stacked->setCurrentWidget(ui->adoptionDemandPage);

    else{
        ui->menuTree->collapseAll();
        if(txt == "Accueil")
            stacked->setCurrentWidget(ui->homePage);
        else if (txt == "Fiches chiens")
            stacked->setCurrentWidget(ui->dogSheetsPage);
        else if (txt == "Vétérinaire")
            stacked->setCurrentWidget(ui->vetPage);
    }

    if (txt == "Registres" || txt == "Autres")
        item->setExpanded(!item->isExpanded());
}
