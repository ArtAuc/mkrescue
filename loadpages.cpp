#include "mainwindow.h"
#include "ui_mainwindow.h"


void MainWindow::LoadDogCards(QString search){
    qDeleteAll(ui->dogCardsPage->findChildren<DogCard*>());

    QString type = "current";
    if(ui->outCheckbox->checkState() == Qt::Checked)
        type += "|out";

    if(ui->careCheckbox->checkState() == Qt::Checked)
        type += "|care";

    int row = 0;
    int col = 0;

    QSqlQuery query = db.GetDogs(type, search);

    QGridLayout *layout = qobject_cast<QGridLayout*>(ui->dogCardsContent->layout());
    while(query.next() && query.value(0).toString() != "")
    {
        DogCard *dogCard = new DogCard(this,
                                       query.value(0).toString(),
                                       query.value(1).toString(),
                                       query.value(2).toString(),
                                       query.value(3).toString(),
                                       query.value(4).toString(),
                                       query.value(5).toString());

        layout->addWidget(dogCard, row, col);

        col += 1;
        if(col > 2){
            col = 0;
            row += 1;
        }
    }

    resizeEvent(nullptr);
}

void MainWindow::LoadEntryRegistry(QString year, QString search)
{
    InitYearRegistry("entry", year);

    QTableWidget* table = ui->entryTable;
    table->clearContents();
    table->setRowCount(0);

    connect(ui->yearBox, SIGNAL(currentTextChanged(QString)), this, SLOT(LoadEntryRegistry(QString)));


    QSqlQuery query = db.GetEntryRegistry(year, search);

    while(query.next() && query.value(0).toString() != "")
    {
        int nb = table->rowCount();
        table->insertRow(nb);
        table->setItem(nb, 0, new QTableWidgetItem(query.value(0).toString())); // id_ES
        table->setItem(nb, 1, new QTableWidgetItem(query.value(1).toDate().toString("dd/MM/yyyy"))); // date_prov
        QStringList prov_type = query.value(2).toString().split("___");
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
        QStringList destinations = query.value(13).toString().split("_-_");
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

                QStringList p = d.split("___");
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

        QStringList necessary = {query.value(0).toString(), query.value(1).toString()};

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
    InitYearRegistry("care", year);
    db.ReorderCareRegistry();

    QTableWidget* table = ui->careTable;
    table->clearContents();
    table->setRowCount(0);

    connect(ui->yearBox, SIGNAL(currentTextChanged(QString)), this, SLOT(LoadCareRegistry(QString)));

    QSqlQuery query = db.GetCareRegistry(year, search);

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

        QStringList necessary = {query.value(0).toString(), query.value(1).toString()};

        connect(modifyButton, &QToolButton::clicked, this, [=](){
            TriggerEdit("care", necessary);
        });

        modifyButtons.append(modifyButton);
    }

    ui->careRegistryPage->showEvent(nullptr);
    ui->careRegistryPage->resizeEvent(nullptr);

}

void MainWindow::LoadMembers(QString year, QString search)
{
    InitYearRegistry("members", year);
    db.ReorderMembers();

    QTableWidget* table = ui->membersTable;
    table->clearContents();
    table->setRowCount(0);

    connect(ui->yearBox, SIGNAL(currentTextChanged(QString)), this, SLOT(LoadMembers(QString)));

    QSqlQuery query = db.GetMembers(year, search);

    while(query.next() && query.value(0).toString() != "")
    {
        int nb = table->rowCount();
        table->insertRow(nb);
        table->setItem(nb, 0, new QTableWidgetItem(query.value(0).toString())); // id_adhesion
        table->setItem(nb, 1, new QTableWidgetItem(query.value(1).toDate().toString("dd/MM/yyyy"))); // date
        table->setItem(nb, 2, new QTableWidgetItem(ClearUselessBreaks(query.value(2).toString() + "\n" + query.value(3).toString()))); //lastname + firstname
        table->setItem(nb, 3, new QTableWidgetItem(ClearUselessBreaks(query.value(4).toString().replace("\\n", "\n")))); // address
        table->setItem(nb, 4, new QTableWidgetItem(query.value(5).toString())); // phone
        table->setItem(nb, 5, new QTableWidgetItem(query.value(6).toString())); // email
        QDate endDate = query.value(1).toDate().addYears(1);
        table->setItem(nb, 6, new QTableWidgetItem(endDate.toString("dd/MM/yyyy"))); // end date
        table->setItem(nb, 7, new QTableWidgetItem(query.value(7).toString() + "\n" +  // type
                                                   query.value(8).toString() + "€")); // amount

        // Modify icon
        QToolButton* deleteButton = new QToolButton(table);
        deleteButton->setIcon(QIcon("media/trash.svg"));
        deleteButton->setStyleSheet("background-color:rgba(0,0,0,0);border-style:none;text-align: center;");

        table->setItem(nb, 8, new QTableWidgetItem(""));
        table->item(nb, 8)->setBackground(QColor("#984800"));
        table->setCellWidget(nb, 8, deleteButton);

        QStringList necessary = {query.value(0).toString(), query.value(1).toString()};

        connect(deleteButton, &QToolButton::clicked, this, [=](){
            TriggerEdit("members", necessary);
        });

        modifyButtons.append(deleteButton);
    }

    ui->membersPage->showEvent(nullptr);
    ui->membersPage->resizeEvent(nullptr);

}


void MainWindow::LoadRedList(QString search){
    QTableWidget* table = ui->redListTable;
    table->clearContents();
    table->setRowCount(0);

    modifyButtons.clear();

    QSqlQuery query = db.GetRedList(search);

    while(query.next()){
        int nb = table->rowCount();
        table->insertRow(nb);
        table->setItem(nb, 0, new QTableWidgetItem(query.value(0).toString() + " " + query.value(1).toString())); // last_name + first_name
        table->setItem(nb, 1, new QTableWidgetItem(query.value(2).toString())); // phone

        QStringList reasons = query.value(3).toString().split("___");
        QString reasonsString = "";

        for (QString r : reasons){
            reasonsString += r;
            reasonsString += "\n";
        }
        table->setItem(nb, 2, new QTableWidgetItem(ClearUselessBreaks(reasonsString)));

        // Delete icon
        table->setItem(nb, 3, new QTableWidgetItem(""));
        QToolButton* deleteButton = new QToolButton(table);
        deleteButton->setIcon(QIcon("media/trash.svg"));
        deleteButton->setStyleSheet("background-color:rgba(0,0,0,0);border-style:none;text-align: center;");

        table->item(nb, 3)->setBackground(QColor("#984800"));
        table->setCellWidget(nb, 3, deleteButton);

        QStringList necessary = {query.value(4).toString()};

        connect(deleteButton, &QToolButton::clicked, this, [=](){
            TriggerEdit("redList", necessary);
        });

        modifyButtons.append(deleteButton);

    }

    ui->redListPage->showEvent(nullptr);
    ui->redListPage->resizeEvent(nullptr);
}
