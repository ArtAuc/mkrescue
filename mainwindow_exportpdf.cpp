#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::InitExportButtons(){
    connect(ui->entryExportButton, &QPushButton::clicked, this, [=]() {ExportRegistry("entryRegistry");});
    connect(ui->careExportButton, &QPushButton::clicked, this, [=]() {ExportRegistry("careRegistry");});
    connect(ui->membersExportButton, &QPushButton::clicked, this, [=]() {ExportRegistry("members");});

    connect(ui->homePage, &HomePage::ExportES, this, [=](QString year) {ExportRegistry("entryRegistry", year);});
}

void MainWindow::ExportRegistry(QString type, QString year){ // type = "entryRegistry" || "careRegistry" || "members"
    if(year.isEmpty())
        year = ui->yearBox->currentText();

    ui->searchLine->clear();

    if(type.contains("entry"))
        ChangePage(ui->menuTree->topLevelItem(2)->child(0));
    else if(type.contains("care"))
        ChangePage(ui->menuTree->topLevelItem(2)->child(1));
    else if(type.contains("members"))
        ChangePage(ui->menuTree->topLevelItem(2)->child(2));

    ui->yearBox->setCurrentText(year);


    QString suggestedName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Registre ";
    if(type.contains("entry"))
        suggestedName += "ES";
    else if(type.contains("care"))
        suggestedName += "Garderie";
    else if(type.contains("members"))
        suggestedName += "Adherents";

    suggestedName += " " + year;
    suggestedName = QDir::toNativeSeparators(suggestedName) + ".pdf";

    QString fileName = QFileDialog::getSaveFileName(nullptr, "Enregistrement du registre", suggestedName, "Fichiers PDF (*.pdf)");
    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".pdf"))
        fileName += ".pdf";


    Registry *page = findChild<Registry*>(type + "Page");
    QTableWidget* table = page->findChild<QTableWidget*>();
    page->hide();
    setWindowState(Qt::WindowMaximized);

    table->scrollToTop();

    // Hide useless items
    findChild<QToolButton*>(QString(type).remove("Registry") + "AddButton")->hide();
    findChild<QPushButton*>(QString(type).remove("Registry") + "ExportButton")->hide();
    table->hideColumn(table->columnCount() - 1);

    page->setFixedWidth(page->width() / 2);
    page->resizeEvent(nullptr);
    if(type == "entryRegistry")
        ui->entryLabel3->setMaximumWidth(table->columnWidth(7) + table->columnWidth(8)  + table->columnWidth(9) - table->verticalScrollBar()->width() / 2); // "Sortie" label is too large because of hidden scrollbar
    else if (type == "careRegistry")
        ui->careLabel3->setMaximumWidth(table->columnWidth(7) + table->columnWidth(8) - table->verticalScrollBar()->width() / 2); // "Sortie" label is too large because of hidden scrollbar

    table->verticalScrollBar()->hide();
    table->horizontalScrollBar()->hide();
    table->viewport()->setFixedHeight(page->width() / 1.6);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::A4);
    printer.setOutputFileName(fileName);
    printer.setResolution(300);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageMargins(QMarginsF(5, 5, 5, 0));

    QPainter painter;
    painter.begin(&printer);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // First page
    painter.setPen(QPen(Qt::black));
    QFont titleFont("Arial", 30);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    QSizeF pageSize = printer.pageRect(QPrinter::DevicePixel).size();

    // Get shelter's name
    QSqlQuery query;
    HandleErrorExec(&query, "SELECT last_name FROM People WHERE id_people = -2");
    query.next();

    QString txt = query.value(0).toString();
    QRectF rect = painter.boundingRect(QRectF(), Qt::AlignCenter, txt);
    qreal x = (pageSize.width() - rect.width()) / 2;
    painter.drawText(QPointF(x, 0.4 * pageSize.height()), txt);

    txt = "REGISTRE ";
    if(type == "entryRegistry")
        txt += "ENTRÉES ET SORTIES";
    else if(type == "careRegistry")
        txt += "DE GARDERIE";
    else if(type == "members")
        txt += "DES ADHÉRENTS";
    rect = painter.boundingRect(QRectF(), Qt::AlignCenter, txt);
    x = (pageSize.width() - rect.width()) / 2;
    painter.drawText(QPointF(x, pageSize.height() / 2), txt);

    txt = year;
    rect = painter.boundingRect(QRectF(), Qt::AlignCenter, txt);
    x = (pageSize.width() - rect.width()) / 2;
    painter.drawText(QPointF(x, 0.6 * pageSize.height()), txt);

    titleFont.setBold(false);
    titleFont.setPointSize(20);
    painter.setFont(titleFont);
    txt = "Fait le " + QDate::currentDate().toString("dd/MM/yyyy");
    rect = painter.boundingRect(QRectF(), Qt::AlignCenter, txt);
    x = 0.95 * pageSize.width() - rect.width();
    qreal y = 0.98 * pageSize.height() - rect.height();
    painter.drawText(QPointF(x, y), txt);

    // Registry pages
    qreal scaleFactor = printer.pageRect(QPrinter::DevicePixel).width() / qreal(page->width());
    titleFont.setPointSize(10);
    painter.setFont(titleFont);

    int pageNum = 0;
    while(table->rowCount() > 0){
        painter.scale(scaleFactor, scaleFactor);
        printer.newPage();

        int lastVisibleRow = table->rowAt(table->viewport()->height()) - 1;

        if(lastVisibleRow >= 0){
            for (int row = lastVisibleRow + 1; row <= table->rowCount(); ++row) {
                table->hideRow(row);
            }

            table->clearSelection();
            page->render(&painter, QPoint(), QRegion(), RenderFlag::DrawChildren);


            for (int row = 0; row <= lastVisibleRow; ++row)
                table->removeRow(0);

            for(int row = 0; row <= table->rowCount(); ++row)
                table->showRow(row);
        }

        else{
            table->clearSelection();
            page->render(&painter, QPoint(), QRegion(), RenderFlag::DrawChildren);
            table->setRowCount(0);
        }

        // Page numerotation
        pageNum += 1;
        painter.scale(1 / scaleFactor, 1 / scaleFactor);
        txt = QString::number(pageNum);
        rect = painter.boundingRect(QRectF(), Qt::AlignCenter, txt);
        x = 0.98 * pageSize.width() - rect.width();
        y = pageSize.height() - rect.height();
        painter.drawText(QPointF(x, y), txt);
    }


    painter.end();

    page->setMaximumWidth(QWIDGETSIZE_MAX);
    page->setMinimumWidth(0);
    table->viewport()->setMinimumHeight(0);
    table->viewport()->setMaximumHeight(QWIDGETSIZE_MAX);
    page->show();
    table->showColumn(table->columnCount() - 1);
    findChild<QToolButton*>(QString(type).remove("Registry") + "AddButton")->show();
    findChild<QPushButton*>(QString(type).remove("Registry") + "ExportButton")->show();
    table->verticalScrollBar()->show();

    if(type == "entryRegistry")
        LoadEntryRegistry(year, ui->searchLine->text());
    else if(type == "careRegistry")
        LoadCareRegistry(year, ui->searchLine->text());
    else if(type == "members")
        LoadMembers(year, ui->searchLine->text());

    page->resizeEvent(nullptr);

    savedData->SetLastTimeExport(QDate::currentDate());
    savedData->Save();
}

void MainWindow::ExportGroupedVet(){
    QStringList groupedVetIds = ui->editPage->GroupedVetIds();

    QDateTime dateTime = ui->dateVetAnimalEdit->GetDateTime();
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Exporter un RDV vétérinaire groupé", "RDV_groupe_" + dateTime.toString("dd_MM_yy_hh_mm"), "Fichier PDF (*.pdf)");
    if(fileName.isEmpty()) {
        QMessageBox::critical(nullptr, "Erreur d'export du RDV", "Le nom de fichier sélectionné est incorrect.");
        return;
    }

    if (!fileName.endsWith(".pdf"))
        fileName += ".pdf";

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::A4);
    printer.setOutputFileName(fileName);
    printer.setResolution(300);
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setPageMargins(QMarginsF(5, 5, 5, 5));

    QPainter painter;
    if (!painter.begin(&printer)) {
        QMessageBox::critical(nullptr, "Erreur", "L'impression PDF a échoué.");
        return;
    }

    painter.setPen(Qt::black);
    QFont titleFont("Arial", 20);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    QSizeF pageSize = printer.pageRect(QPrinter::DevicePixel).size();

    // Get shelter's name
    QSqlQuery shelterQuery;
    HandleErrorExec(&shelterQuery, "SELECT last_name FROM People WHERE id_people = -2");
    shelterQuery.next();
    QString shelterName = shelterQuery.value(0).toString();

    QString reason = ui->reasonVetAnimalBox->currentText() == "Autre" ? ui->reasonVetAnimalEdit->text() : ui->reasonVetAnimalBox->currentText();
    QString titleText = "RDV du " + dateTime.toString("dd/MM/yyyy h:mm") + " - " + reason + " - " + shelterName;
    QRectF titleRect = painter.boundingRect(QRectF(), Qt::AlignCenter, titleText);
    qreal titleX = (pageSize.width() - titleRect.width()) / 2;
    painter.drawText(QPointF(titleX, 0.1 * pageSize.height()), titleText);
    painter.setFont(QFont("Arial", 10));

    qreal lastY = 0.15 * pageSize.height();
    for(const QString& id_dog : groupedVetIds) {
        QSqlQuery dogQuery;
        QString dogQueryStr = "SELECT name, chip, birth, sex, description, sterilized FROM Dogs WHERE id_dog = " + id_dog;
        HandleErrorExec(&dogQuery, dogQueryStr);

        if (dogQuery.next()) {
            QString dogName = dogQuery.value(0).toString();
            QString dogChip = dogQuery.value(1).toString();
            QString birth = dogQuery.value(2).toDate().toString("dd/MM/yyyy");
            QString dogSex = dogQuery.value(3).toString();
            QString dogDescription = dogQuery.value(4).toString();
            QString sterilized = dogQuery.value(5).toString();
            if(!sterilized.isEmpty()){
                if(sterilized == "1")
                    sterilized = " stérilisé";
                else
                    sterilized = " non stérilisé";

                if(dogSex == "Femelle")
                    sterilized += "e";
            }


            QString ageString = GetAge(QDate::fromString(birth, "dd/MM/yyyy"));
            QString dogDetails = QString("%1 - %5%7 - %6\n"
                                         "Né le %2 (%3)\n"
                                         "Identification : %4")
                                    .arg(dogName)
                                    .arg(birth)
                                    .arg(ageString)
                                    .arg(dogChip)
                                    .arg(dogSex)
                                    .arg(dogDescription)
                                    .arg(sterilized);

            QSqlQuery vaccineQuery;
            vaccineQuery.prepare("SELECT Vet.date "
                                 "FROM Vet "
                                 "WHERE id_dog = :id "
                                 "AND reason LIKE 'Vaccin%' "
                                 "AND Vet.date < DATE('now') "
                                 "ORDER BY Vet.date DESC");

            vaccineQuery.bindValue(":id", id_dog);
            HandleErrorExec(&vaccineQuery);
            if(vaccineQuery.next()){
                dogDetails += "\nDernier vaccin : " + vaccineQuery.value(0).toDate().toString("dd/MM/yyyy");
            }

            QStringList dogDetailsLines = dogDetails.split("\n");
            for (int i = 0; i < dogDetailsLines.size(); ++i) {
                if (i == 0) {
                    QFont boldFont = painter.font();
                    boldFont.setBold(true);
                    painter.setFont(boldFont);
                }
                else {
                    QFont normalFont = painter.font();
                    normalFont.setBold(false);
                    painter.setFont(normalFont);
                }


                lastY = lastY + painter.fontMetrics().height() + 10;
                painter.drawText(QPointF(0.1 * pageSize.width(), lastY), dogDetailsLines[i]);
            }

            lastY += 30;
            if(lastY > 0.8 * pageSize.height()){
                lastY = 0.15 * pageSize.height();
                painter.translate(pageSize.width() / 2, 0);
            }
        }
    }
}
