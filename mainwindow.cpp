#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);

    InitMenu();
    //ui->entryTable->verticalHeader()->setVisible(false);

    db.ReorderEntryRegistry();
    LoadEntryRegistry();
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
    if (txt == "Entrées/Sorties"){
        stacked->setCurrentWidget(ui->entryRegistryPage);
        LoadEntryRegistry();
    }
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

void MainWindow::LoadEntryRegistry()
{
    QTableWidget* table = ui->entryTable;
    table->clearContents();
    table->setRowCount(0);

    QSqlQuery query = db.GetEntryRegistry("2024");

    while(query.next() && query.value(0).toString() != "")
    {
        int nb = table->rowCount();
        table->insertRow(nb);
        for(int i = 0; i < 20; i++)
            qDebug() << i << query.value(i);
        table->setItem(nb, 0, new QTableWidgetItem(query.value(0).toString())); // id_ES
        table->setItem(nb, 1, new QTableWidgetItem(query.value(1).toDate().toString("dd/MM/yyyy"))); // date_prov
        table->setItem(nb, 2, new QTableWidgetItem(query.value(2).toString() + "\n" + // prov_type
                                                  query.value(3).toString() + " " + query.value(4).toString() + "\n" + //prov_lastname + prov_firstname
                                                  query.value(5).toString() + "\n" + // prov_address
                                                  query.value(6).toString() + "\n" + // prov_phone
                                                  query.value(7).toString())); //prov_email
        table->setItem(nb, 3, new QTableWidgetItem("Chien " + query.value(8).toString())); // sex
        table->setItem(nb, 4, new QTableWidgetItem(query.value(9).toString() + "\n" +
                                                  query.value(10).toString())); // identification
        table->setItem(nb, 5, new QTableWidgetItem(query.value(11).toString())); // description
        table->setItem(nb, 6, new QTableWidgetItem(query.value(12).toDate().toString("dd/MM/yyyy"))); // birth
        table->setItem(nb, 7, new QTableWidgetItem(query.value(13).toDate().toString("dd/MM/yyyy"))); // date_dest
        QStringList destinations = query.value(14).toString().split(";;;");
        QTableWidgetItem* item = new QTableWidgetItem();
        for (QString d : destinations){
            if (d != ""){
                QStringList p = d.split(";-;");
                QString toAppend = p[0] + "\n" + // prov_type
                        p[1] + " " + p[2] + "\n" + //prov_lastname + prov_firstname
                        p[3] + "\n" + // prov_address
                        p[4] + "\n" + // prov_phone
                        p[5]; //prov_email
                item->setText(item->text() + toAppend + "\n\n");
            }
        }

        table->setItem(nb, 8, item);

        table->setItem(nb, 9, new QTableWidgetItem(query.value(15).toString())); // death_cause
    }


}
