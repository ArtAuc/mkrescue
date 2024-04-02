#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::InitExportButtons(){
    connect(ui->entryExportButton, &QPushButton::clicked, this, [=]() {ExportRegistry("entryRegistry");});
    connect(ui->careExportButton, &QPushButton::clicked, this, [=]() {ExportRegistry("careRegistry");});
    connect(ui->membersExportButton, &QPushButton::clicked, this, [=]() {ExportRegistry("members");});
}

void MainWindow::ExportRegistry(QString type){ // type = "entryRegistry" || "careRegistry" || "members"
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
    table->viewport()->setFixedHeight(page->width() / 1.5);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::A4);
    printer.setOutputFileName("output.pdf");
    printer.setResolution(300);
    printer.setPageOrientation(QPageLayout::Landscape);

    QPainter painter;
    painter.begin(&printer);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // First page
    painter.setPen(QPen(Qt::black));
    QFont titleFont("Arial", 30);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    QSizeF pageSize = printer.pageRect(QPrinter::DevicePixel).size();

    QString txt = "Refuge"; //TODO : use shelter's name
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

    txt = ui->yearBox->currentText();
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
        while(lastVisibleRow + 1 > 0 && table->item(lastVisibleRow + 1, 0)->text().isEmpty())
            lastVisibleRow -= 1;

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
        LoadEntryRegistry(ui->yearBox->currentText(), ui->searchLine->text());
    else if(type == "careRegistry")
        LoadCareRegistry(ui->yearBox->currentText(), ui->searchLine->text());
    else if(type == "members")
        LoadMembers(ui->yearBox->currentText(), ui->searchLine->text());

    page->resizeEvent(nullptr);

}
