#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    ui->stackedWidget->setCurrentWidget(ui->entryRegistryPage);
    ui->entryLabelLayout->setAlignment(Qt::AlignLeft);

    connect(ui->menuButton, SIGNAL(clicked(bool)), ui->menuTree, SLOT(Toggle()));
    connect(ui->menuButton, SIGNAL(clicked(bool)), this, SLOT(ToggleModifyButtons()));

    InitEntryRegistry();
    LoadEntryRegistry("2024");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitEntryRegistry()
{
    ui->entryTable->verticalHeader()->setVisible(false);
    db.ReorderEntryRegistry();

    std::vector<QString> years = db.GetRegistryYears("Entry");


    for(QString y : years){
        if(y != QString::number(QDate::currentDate().year()))
            ui->yearEntryBox->addItem(y);
    }

    ui->yearEntryBox->addItem(QString::number(QDate::currentDate().year()));

    connect(ui->yearEntryBox, SIGNAL(currentTextChanged(QString)), this, SLOT(LoadEntryRegistry(QString)));
}

// Change selected page from stacked widget, based on the selected menu item
void MainWindow::ChangePage(QTreeWidgetItem* item)
{
    QStackedWidget* stacked = ui->stackedWidget;
    QString txt = item->text(0);

    if (txt == " Entrées/Sorties"){
        stacked->setCurrentWidget(ui->entryRegistryPage);
        ui->yearEntryBox->setCurrentIndex(ui->yearEntryBox->count() - 1);
        LoadEntryRegistry(QString::number(QDate::currentDate().year()));
    }
    else if (txt == " Garderie")
        stacked->setCurrentWidget(ui->careRegistryPage);
    else if (txt == " Adhérents")
        stacked->setCurrentWidget(ui->membersPage);
    else if (txt == " Liste rouge adoptants")
        stacked->setCurrentWidget(ui->redListPage);
    else if (txt == " Animaux perdus")
        stacked->setCurrentWidget(ui->lostPage);
    else if (txt == " Demandes d'adoption")
        stacked->setCurrentWidget(ui->adoptionDemandPage);

    else{
        ui->menuTree->collapseAllExcept(txt);
        if(txt == " Accueil")
            stacked->setCurrentWidget(ui->homePage);
        else if (txt == " Fiches chiens")
            stacked->setCurrentWidget(ui->dogSheetsPage);
        else if (txt == " Vétérinaire")
            stacked->setCurrentWidget(ui->vetPage);
    }

    if (txt == "  Registres"  || txt == " Autres")
        item->setExpanded(!item->isExpanded());

    else
        ui->titleLabel->setText(txt.trimmed());
}

void MainWindow::LoadEntryRegistry(QString year)
{
    QTableWidget* table = ui->entryTable;
    table->clearContents();
    table->setRowCount(0);

    ui->yearEntryBox->setCurrentIndex(ui->yearEntryBox->findText(year));

    QSqlQuery query = db.GetEntryRegistry(year);

    modifyButtons.clear();

    while(query.next() && query.value(0).toString() != "")
    {
        int nb = table->rowCount();
        table->insertRow(nb);
        table->setItem(nb, 0, new QTableWidgetItem(query.value(0).toString())); // id_ES
        table->setItem(nb, 1, new QTableWidgetItem(query.value(1).toDate().toString("dd/MM/yyyy"))); // date_prov
        table->setItem(nb, 2, new QTableWidgetItem(query.value(2).toString() + "\n" + // prov_type
                                                  query.value(3).toString() + " " + query.value(4).toString() + "\n" + //prov_lastname + prov_firstname
                                                  query.value(5).toString() + "\n" + // prov_address
                                                  query.value(6).toString() + "\n" + // prov_phone
                                                  query.value(7).toString())); //prov_email
        table->setItem(nb, 3, new QTableWidgetItem("Chien\n" + query.value(8).toString())); // sex
        table->setItem(nb, 4, new QTableWidgetItem(query.value(9).toString() + "\n" +
                                                  query.value(10).toString())); // identification
        table->setItem(nb, 5, new QTableWidgetItem(query.value(11).toString())); // description
        table->setItem(nb, 6, new QTableWidgetItem(query.value(12).toDate().toString("dd/MM/yyyy"))); // birth
        QStringList destinations = query.value(13).toString().split(";;;");

        for (int i = 0; i < destinations.size(); i++){
            QString d = destinations[i];
            if (d != ""){
                if(i > 0){
                    table->insertRow(nb + i);
                    for (int col = 0; col < table->columnCount(); ++col) {
                        QTableWidgetItem *emptyItem = new QTableWidgetItem();
                        table->setItem(nb + i, col, emptyItem);
                    }
                }

                QStringList p = d.split(";-;");
                table->setItem(nb + i, 7, new QTableWidgetItem(QDate::fromString(p[0], "yyyy-MM-dd").toString("dd/MM/yyyy"))); // date_dest

                QString destString = p[1] + "\n" + // dest_type
                        p[2] + " " + p[3] + "\n" + //dest_lastname + dest_firstname
                        p[4] + "\n" + // dest_address
                        p[5] + "\n" + // dest_phone
                        p[6]; //dest_email
                table->setItem(nb + i, 8, new QTableWidgetItem(destString));
            }
        }

        table->setItem(nb, 9, new QTableWidgetItem(query.value(14).toString())); // death_cause

        table->setItem(nb, 10, new QTableWidgetItem(""));

        // Modify icon
        QToolButton* modifyButton = new QToolButton(table);
        modifyButton->setIcon(QIcon("media/modify.svg"));
        modifyButton->setStyleSheet("background-color:rgba(0,0,0,0);border-style:none;text-align: center;");

        table->item(nb, 10)->setBackground(QColor("#749674"));
        table->setCellWidget(nb, 10, modifyButton);

        connect(modifyButton, SIGNAL(clicked(bool)), this, SLOT(close()));

        modifyButtons.append(modifyButton);
    }

    ui->entryRegistryPage->showEvent(nullptr);
    ui->entryRegistryPage->resizeEvent(nullptr);
}

void MainWindow::resizeEvent(QResizeEvent *event){
    QFont font = ui->titleLabel->font();
    font.setPointSize(0.02 * ui->stackedWidget->width());
    ui->titleLabel->setFont(font);
}

void MainWindow::ToggleModifyButtons()
{
    for(QToolButton* but : modifyButtons){
        but->setIcon(QIcon());
        QTimer::singleShot(300, [but](){but->setIcon(QIcon("media/modify.svg"));});
    }

}
