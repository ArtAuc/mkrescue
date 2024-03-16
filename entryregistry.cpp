#include "entryregistry.h"

EntryRegistry::EntryRegistry(QWidget *parent)
    : QWidget{parent}
{
}

void EntryRegistry::showEvent(QShowEvent* event) {
    for (int i = 0; i < children().size(); i++) {
        QObject* child = children().at(i);
        if (child->objectName() == "entryTable")
            table = qobject_cast<QTableWidget*>(child);
        else if (child->objectName() == "entryLabel1")
            label1 = qobject_cast<QLabel*>(child);
        else if (child->objectName() == "entryLabel2")
            label2 = qobject_cast<QLabel*>(child);
        else if (child->objectName() == "entryLabel3")
            label3 = qobject_cast<QLabel*>(child);
    }

    resizeEvent(nullptr);
}


void EntryRegistry::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    if(table != nullptr && label1 != nullptr && label2 != nullptr && label3 != nullptr){

        QFont font = table->font();
        float fontSize = std::max(width() * 0.01, (double) 7);
        font.setPointSize(fontSize);
        table->setFont(font);

        for(int i = 0; i < table->columnCount(); i++)
            table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);

        ResizeAuto();

        table->setStyleSheet("QTableWidget::item { padding-bottom: " + QString::number(table->width() / 100) + "px;padding-top: " + QString::number(table->width() / 100) + "px; }");

        table->horizontalHeaderItem(3)->setText("Espèce\nSexe");
        table->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignCenter); // #
        table->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignCenter); // date_entry
        table->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignCenter); // sex

        table->resizeColumnsToContents();
        table->resizeRowsToContents();


        float sumWidth = 0;

        for (int col = 0; col < table->columnCount(); ++col) {
            sumWidth += table->columnWidth(col);
        }

        float factor = 0.95 * table->width() / sumWidth; // Allows to occupy the whole width of the window
        if(factor > 1){
            for (int col = 0; col < table->columnCount(); ++col) {
                table->setColumnWidth(col, static_cast<int>(table->columnWidth(col) * factor));
            }
        }



        label1->setMaximumWidth(width());
    }
}

void EntryRegistry::ResizeAuto()
{
    if(table != nullptr){
    }
}
