#include "dogregistry.h"

QString tableStylesheet = "QTableWidget { gridline-color: #dadcdf; color: #333}"
                          "QTableWidget::item:selected { background-color: #3A4A64; }";

DogRegistry::DogRegistry(QWidget *parent)
    : QWidget{parent}
{

}

void DogRegistry::showEvent(QShowEvent* event) {
    for (int i = 0; i < children().size(); i++) {
        QObject* child = children().at(i);
        if (child->objectName() == type + "Table")
            table = qobject_cast<QTableWidget*>(child);
        else if (child->objectName() == type + "Label1")
            label1 = qobject_cast<QLabel*>(child);
        else if (child->objectName() == type + "Label2")
            label2 = qobject_cast<QLabel*>(child);
        else if (child->objectName() == type + "Label3")
            label3 = qobject_cast<QLabel*>(child);
    }

    if(table != nullptr && label1 != nullptr && label2 != nullptr && label3 != nullptr){
        // Headers
        table->verticalHeader()->setVisible(false);
        for(int i = 0; i < table->columnCount(); i++)
            table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);

        table->horizontalHeaderItem(1)->setText("Date\nd'entrée");
        table->horizontalHeaderItem(3)->setText("Espèce\nSexe");
        table->horizontalHeaderItem(6)->setText("Date de\nnaissance");
        table->horizontalHeaderItem(7)->setText("Date de\nsortie");
        if(type == "entry")
            table->horizontalHeaderItem(9)->setText("Cause de\nla mort");

        table->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: #212834; color: white; font-weight: bold; }");

        // Center-align text for columns
        for(int col = 0; col < table->columnCount(); col++){
            for(int row = 0; row < table->rowCount(); row++)
                table->item(row,col)->setTextAlignment(Qt::AlignCenter);
        }

        // Alternating row color
        int row = 0;
        bool gray = true;
        while(row < table->rowCount()) {
            ColorRow(row, gray);
            row++;

            // For multiple destinations
            while(row < table->rowCount() && table->item(row, 0)->text() == ""){
                ColorRow(row, gray);
                row++;
            }

            gray = !gray;
        }

        // Bold first column #
        if(table->item(0,0) != nullptr){
            QFont font(table->item(0, 0)->font());
            font.setBold(true);
            for(int row = 0; row < table->rowCount(); row++)
                table->item(row, 0)->setFont(font);
        }
    }
}

void DogRegistry::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    if(table != nullptr && label1 != nullptr && label2 != nullptr && label3 != nullptr){
        // Resize modify buttons
        float iconSize = width() * 0.03;
        for (QToolButton* but : table->findChildren<QToolButton*>()){
            but->setIconSize(QSize(iconSize, iconSize));
        }


        QFont font = table->font();
        float fontSize = std::max(width() * 0.007, (double) 7);
        font.setPointSize(fontSize);
        table->setFont(font);
        table->horizontalHeader()->setFont(font);

        table->setStyleSheet(tableStylesheet + "QTableWidget::item { padding-bottom: " + QString::number(table->width() / 100) + "px;padding-top: " + QString::number(table->width() / 100) + "px;}");


        table->resizeColumnsToContents();
        table->resizeRowsToContents();

        float sumWidth = 0;

        for (int col = 0; col < table->columnCount(); ++col) {
            sumWidth += table->columnWidth(col);
        }

        float factor = 0.99 * table->width() / sumWidth; // Allows to occupy the whole width of the window
        if(factor > 1){
            for (int col = 0; col < table->columnCount(); ++col) {
                table->setColumnWidth(col, static_cast<int>(table->columnWidth(col) * factor));
            }
        }

        // Supcategories labels' widths according to table's width
        label1->setMaximumWidth(table->columnWidth(0) + table->columnWidth(1) + table->columnWidth(2));
        label2->setMaximumWidth(table->columnWidth(3) + table->columnWidth(4) + table->columnWidth(5) + table->columnWidth(6));


        type == "care" ?
            label3->setMaximumWidth(table->columnWidth(7) + table->columnWidth(8)) :
            label3->setMaximumWidth(table->columnWidth(7) + table->columnWidth(8) + table->columnWidth(9));



        font.setPointSize(fontSize * 2);
        label1->setFont(font);
        label2->setFont(font);
        label3->setFont(font);

        // Disable labels if window is too narrow
        bool scrollable = sumWidth > width() ;
        label1->setVisible(!scrollable);
        label2->setVisible(!scrollable);
        label3->setVisible(!scrollable);
    }
}

void DogRegistry::ColorRow(int row, bool gray){
    QColor color = gray ? QColor("#e7e9ed") : QColor("#ffffff");
    for (int col = 0; col < table->columnCount() - 1; ++col)
        table->item(row, col)->setBackground(color);
}

