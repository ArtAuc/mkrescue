#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    ui->stackedWidget->setCurrentWidget(ui->entryRegistryPage);

    connect(ui->menuButton, SIGNAL(clicked(bool)), ui->menuTree, SLOT(Toggle()));
    connect(ui->menuButton, SIGNAL(clicked(bool)), this, SLOT(ToggleModifyButtons()));
    connect(ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(Search(QString)));
    connect(ui->addEntryButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(AddEntry()));
    connect(ui->entryTypeBox, SIGNAL(currentTextChanged(QString)), ui->editPage, SLOT(ChangeEntryType(QString)));
    connect(ui->submitButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(SaveEdit()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(QuitEdit()));
    connect(ui->editPage, SIGNAL(RefreshMainWindow(QString)), this, SLOT(RefreshRegistry(QString)));

    InitRegistry("Entry");
    ui->entryRegistryPage->SetType("entry");
    ui->entryLabelLayout->setAlignment(Qt::AlignLeft);

    InitRegistry("Care");
    ui->careRegistryPage->SetType("care");
    ui->careLabelLayout->setAlignment(Qt::AlignLeft);


    LoadEntryRegistry("2024");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitRegistry(QString type)
{
}

// Change selected page from stacked widget, based on the selected menu item
void MainWindow::ChangePage(QTreeWidgetItem* item)
{
    QStackedWidget* stacked = ui->stackedWidget;
    QString txt = item->text(0);

    QComboBox* box = ui->yearBox;
    QObject::disconnect(box, nullptr, this, nullptr);

    if (txt == " Entrées/Sorties"){
        stacked->setCurrentWidget(ui->entryRegistryPage);
        LoadEntryRegistry(QString::number(QDate::currentDate().year()));
    }
    else if (txt == " Garderie"){
        stacked->setCurrentWidget(ui->careRegistryPage);
        LoadCareRegistry(QString::number(QDate::currentDate().year()));
    }
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

void MainWindow::LoadEntryRegistry(QString year, QString search)
{
    db.ReorderEntryRegistry();

    // Init yearBox
    std::vector<QString> years = db.GetRegistryYears("entry");

    QComboBox* box = ui->yearBox;
    QObject::disconnect(box, nullptr, this, nullptr);
    box->clear();

    for(QString y : years){
        if(y != QString::number(QDate::currentDate().year()))
            box->addItem(y);
    }

    box->addItem(QString::number(QDate::currentDate().year()));

    box->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    box->adjustSize();


    QTableWidget* table = ui->entryTable;
    table->clearContents();
    table->setRowCount(0);

    ui->yearBox->setCurrentIndex(ui->yearBox->findText(year));

    connect(box, SIGNAL(currentTextChanged(QString)), this, SLOT(LoadEntryRegistry(QString)));


    QSqlQuery query = db.GetEntryRegistry(year, search);

    modifyButtons.clear();

    while(query.next() && query.value(0).toString() != "")
    {
        int nb = table->rowCount();
        table->insertRow(nb);
        table->setItem(nb, 0, new QTableWidgetItem(query.value(0).toString())); // id_ES
        table->setItem(nb, 1, new QTableWidgetItem(query.value(1).toDate().toString("dd/MM/yyyy"))); // date_prov
        QStringList prov_type = query.value(2).toString().split(";-;");
        if(prov_type[0] == "Fourrière"){
            table->setItem(nb, 2, new QTableWidgetItem(ClearUselessBreaks(prov_type[0] + "\n" + // prov_type
                                                      prov_type[1]))); //place

        }
        else if (prov_type[0] == "Abandon")
            table->setItem(nb, 2, new QTableWidgetItem(ClearUselessBreaks(prov_type[0] + "\n" + // prov_type
                                                      query.value(3).toString() + " " + query.value(4).toString() + "\n" + //prov_lastname + prov_firstname
                                                      query.value(5).toString().replace("\\n", "\n") + "\n" + // prov_address
                                                      query.value(6).toString() + "\n" + // prov_phone
                                                      query.value(7).toString()))); //prov_email

        table->setItem(nb, 3, new QTableWidgetItem("Chien\n" + query.value(8).toString())); // sex
        table->setItem(nb, 4, new QTableWidgetItem(ClearUselessBreaks(query.value(9).toString() + "\n" +
                                                  query.value(10).toString()))); // identification
        table->setItem(nb, 5, new QTableWidgetItem(query.value(11).toString())); // description
        table->setItem(nb, 6, new QTableWidgetItem(query.value(12).toDate().toString("dd/MM/yyyy"))); // birth
        QStringList destinations = query.value(13).toString().split(";;;");

        QString destString = "";
        QString dateString = "";

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
                dateString = QDate::fromString(p[0], "yyyy-MM-dd").toString("dd/MM/yyyy");

                destString = p[1] + "\n" + // dest_type
                        p[2] + " " + p[3] + "\n" + //dest_lastname + dest_firstname
                        p[4].replace("\\n", "\n") + "\n" + // dest_address
                        p[5] + "\n" + // dest_phone
                        p[6]; //dest_email
            }

            table->setItem(nb + i, 7, new QTableWidgetItem(dateString)); // date_dest
            table->setItem(nb + i, 8, new QTableWidgetItem(ClearUselessBreaks(destString)));
        }

        table->setItem(nb, 9, new QTableWidgetItem(query.value(14).toString())); // death_cause

        table->setItem(nb, 10, new QTableWidgetItem(""));

        // Modify icon
        QToolButton* modifyButton = new QToolButton(table);
        modifyButton->setIcon(QIcon("media/modify.svg"));
        modifyButton->setStyleSheet("background-color:rgba(0,0,0,0);border-style:none;text-align: center;");

        table->item(nb, 10)->setBackground(QColor("#749674"));
        table->setCellWidget(nb, 10, modifyButton);

        QStringList necessary = {{query.value(0).toString(), query.value(1).toString()}};

        connect(modifyButton, &QToolButton::clicked, this, [=](){
            TriggerEdit("entry", necessary);
        });

        modifyButtons.append(modifyButton);
    }

    ui->entryRegistryPage->showEvent(nullptr);
    ui->entryRegistryPage->resizeEvent(nullptr);
}

void MainWindow::LoadCareRegistry(QString year, QString search)
{   
    db.ReorderCareRegistry();

    // Init yearBox
    std::vector<QString> years = db.GetRegistryYears("care");

    QComboBox* box = ui->yearBox;
    QObject::disconnect(box, nullptr, this, nullptr);
    box->clear();

    for(QString y : years){
        if(y != QString::number(QDate::currentDate().year()))
            box->addItem(y);
    }

    box->addItem(QString::number(QDate::currentDate().year()));

    box->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    box->adjustSize();


    QTableWidget* table = ui->careTable;
    table->clearContents();
    table->setRowCount(0);

    ui->yearBox->setCurrentIndex(ui->yearBox->findText(year));
    connect(box, SIGNAL(currentTextChanged(QString)), this, SLOT(LoadCareRegistry(QString)));

    QSqlQuery query = db.GetCareRegistry(year, search);

    modifyButtons.clear();


    while(query.next() && query.value(0).toString() != "")
    {
        int nb = table->rowCount();
        table->insertRow(nb);
        table->setItem(nb, 0, new QTableWidgetItem(query.value(0).toString())); // id_care
        table->setItem(nb, 1, new QTableWidgetItem(query.value(1).toDate().toString("dd/MM/yyyy"))); // entry_date
        table->setItem(nb, 2, new QTableWidgetItem(ClearUselessBreaks(query.value(2).toString() + " " + query.value(3).toString() + "\n" + //prov_lastname + prov_firstname
                                                  query.value(4).toString().replace("\\n", "\n") + "\n" + // prov_address
                                                  query.value(5).toString() + "\n" + // prov_phone
                                                  query.value(6).toString()))); //prov_email
        table->setItem(nb, 3, new QTableWidgetItem("Chien\n" + query.value(7).toString())); // sex
        table->setItem(nb, 4, new QTableWidgetItem(ClearUselessBreaks(query.value(8).toString() + "\n" +
                                                  query.value(9).toString()))); // identification
        table->setItem(nb, 5, new QTableWidgetItem(query.value(10).toString())); // description
        table->setItem(nb, 6, new QTableWidgetItem(query.value(11).toDate().toString("dd/MM/yyyy"))); // birth
        table->setItem(nb, 7, new QTableWidgetItem(query.value(12).toDate().toString("dd/MM/yyyy"))); // exit_date
        table->setItem(nb, 8, new QTableWidgetItem(ClearUselessBreaks(query.value(13).toString() + " " + query.value(14).toString() + "\n" + //dest_lastname + dest_firstname
                                                  query.value(15).toString().replace("\\n", "\n") + "\n" + // dest_address
                                                  query.value(16).toString() + "\n" + // dest_phone
                                                  query.value(17).toString()))); //dest_email

        table->setItem(nb, 9, new QTableWidgetItem(""));

        // Modify icon
        QToolButton* modifyButton = new QToolButton(table);
        modifyButton->setIcon(QIcon("media/modify.svg"));
        modifyButton->setStyleSheet("background-color:rgba(0,0,0,0);border-style:none;text-align: center;");

        table->item(nb, 9)->setBackground(QColor("#749674"));
        table->setCellWidget(nb, 9, modifyButton);

        //connect(modifyButton, SIGNAL(clicked(bool)), this, [=](){TriggerEdit("care", QList())});

        modifyButtons.append(modifyButton);
    }

    ui->careRegistryPage->showEvent(nullptr);
    ui->careRegistryPage->resizeEvent(nullptr);

}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->menuTree->SetInitialWidth(width() * 0.15);

    QFont font = ui->titleLabel->font();
    font.setPointSize(0.02 * ui->stackedWidget->width());
    ui->titleLabel->setFont(font);

    QFont comboboxFont = ui->yearBox->font();
    comboboxFont.setPointSizeF(0.015 * ui->stackedWidget->width());
    ui->yearBox->setFont(comboboxFont);

    QSize textSize = ui->yearBox->fontMetrics().size(Qt::TextSingleLine, ui->yearBox->currentText());
    ui->yearBox->setFixedSize(textSize + QSize(35, 35));
}

void MainWindow::ToggleModifyButtons()
{
    for(QToolButton* but : modifyButtons){
        but->setIcon(QIcon());
        QTimer::singleShot(310, [but](){but->setIcon(QIcon("media/modify.svg"));});
    }

}

void MainWindow::Search(QString search){
    QString pageName = ui->stackedWidget->currentWidget()->objectName();
    if(pageName == "entryRegistryPage")
        LoadEntryRegistry(ui->yearBox->currentText(), search);

    else if(pageName == "careRegistryPage")
        LoadCareRegistry(ui->yearBox->currentText(), search);
}

void MainWindow::TriggerEdit(QString type, QStringList necessary){
    QSqlQuery query;

    if(type == "entry")
        query.exec("SELECT ES_registry.id_ES, "
                    "ES_registry.date_prov, "
                    "ES_registry.type_prov, "
                    "People_prov.last_name, "
                    "People_prov.first_name, "
                    "People_prov.address, "
                    "People_prov.phone, "
                    "People_prov.email, "
                    "Dogs.sex, "
                    "Dogs.chip, "
                    "Dogs.name, "
                    "Dogs.description, "
                    "Dogs.birth, "
                    "GROUP_CONCAT(Destinations.date || ';-;' || Destinations.type || ';-;' || People_dest.last_name || ';-;' || People_dest.first_name || ';-;' || People_dest.address || ';-;' || People_dest.phone || ';-;' || People_dest.email, ';;;'), "
                    "ES_registry.death_cause "
                    "FROM ES_registry "
                    "JOIN People AS People_prov ON ES_registry.id_people_prov = People_prov.id_people "
                    "JOIN Dogs ON ES_registry.id_dog = Dogs.id_dog "
                    "LEFT JOIN Destinations ON Dogs.id_dog = Destinations.id_dog "
                    "LEFT JOIN People AS People_dest ON Destinations.id_people = People_dest.id_people "
                    "WHERE id_ES = " + necessary[0] + " AND date_prov = '" + necessary[1] + "'"
                    "GROUP BY Dogs.id_dog ");

    query.next();

    QStringList infos;
    for(int i = 0; i < query.record().count(); i++)
        infos.append(query.value(i).toString());


    ui->editPage->Edit(type, infos);
}

void MainWindow::RefreshRegistry(QString type){
    if(type == "entry")
        LoadEntryRegistry(ui->yearBox->currentText());

    else if(type == "care")
        LoadCareRegistry(ui->yearBox->currentText());
}

QString MainWindow::ClearUselessBreaks(QString s){
    while(s.contains("\n\n"))
        s = s.replace("\n\n", "\n");
    s = s.trimmed();
    return s;
}
