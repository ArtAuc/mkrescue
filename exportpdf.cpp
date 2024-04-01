#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::InitExportButtons(){
    connect(ui->exportEntryButton, SIGNAL(clicked()), this, SLOT(ExportEntryRegistry()));
}

void MainWindow::ExportEntryRegistry() {
    Registry *entryRegistryPage = ui->entryRegistryPage;
    QTableWidget* table = ui->entryTable;
    entryRegistryPage->hide();

    table->scrollToTop();

    // Hide useless items
    ui->entryAddButton->hide();
    table->hideColumn(10);
    ui->exportEntryButton->hide();

    table->setFont(QFont("Sans Serif", 7));
    table->resizeColumnsToContents();
    float sumWidth = 0;

    for (int col = 0; col < table->columnCount(); ++col) {
        sumWidth += table->columnWidth(col);
    }

    entryRegistryPage->setMinimumWidth(sumWidth);
    entryRegistryPage->setMaximumWidth(entryRegistryPage->height() / 1.4);
    entryRegistryPage->resizeEvent(nullptr);
    ui->entryLabel3->setMaximumWidth(table->columnWidth(7) + table->columnWidth(8)  + table->columnWidth(9) - table->verticalScrollBar()->width() / 2); // "Sortie" label is too large because of hidden scrollbar
    table->verticalScrollBar()->hide();
    table->horizontalScrollBar()->hide();

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::A4);
    printer.setOutputFileName("output.pdf");
    printer.setResolution(300);

    QPainter painter;
    painter.begin(&printer);

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

    txt = "REGISTRE ENTRÉES ET SORTIES";
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
    qreal scaleFactor = printer.pageRect(QPrinter::DevicePixel).width() / qreal(entryRegistryPage->width());

    painter.scale(scaleFactor, scaleFactor);

    while(table->rowCount() > 0){
        // TODO : page number
        printer.newPage();

        int lastVisibleRow = table->rowAt(table->verticalScrollBar()->value() + table->viewport()->height()) - 1;
        while(lastVisibleRow + 1 > 0 && table->item(lastVisibleRow + 1, 0)->text().isEmpty())
            lastVisibleRow -= 1;

        if(lastVisibleRow >= 0){
            for (int row = lastVisibleRow + 1; row <= table->rowCount(); ++row) {
                table->hideRow(row);
            }

            entryRegistryPage->render(&painter, QPoint(), QRegion(), RenderFlag::DrawChildren);


            for (int row = 0; row <= lastVisibleRow; ++row)
                table->removeRow(0);

            for(int row = 0; row <= table->rowCount(); ++row)
                table->showRow(row);
        }

        else{
            entryRegistryPage->render(&painter, QPoint(), QRegion(), RenderFlag::DrawChildren);
            table->setRowCount(0);
        }

    }


    painter.end();

    entryRegistryPage->setMaximumWidth(QWIDGETSIZE_MAX);
    entryRegistryPage->setMinimumWidth(0);
    entryRegistryPage->show();
    ui->entryAddButton->show();
    table->showColumn(10);
    ui->exportEntryButton->show();
    table->verticalScrollBar()->show();


    LoadEntryRegistry(ui->yearBox->currentText(), ui->searchLine->text());
    entryRegistryPage->resizeEvent(nullptr);
}

