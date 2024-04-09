#include "registry.h"

Registry::Registry(QWidget *parent)
    : QWidget{parent}
{
}

void Registry::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);

    layout()->setSpacing(0);

    table = findChild<QTableWidget*>(type + "Table");
    addButton = findChild<QToolButton*>(type + "AddButton");

    if(table != nullptr && addButton != nullptr){
        // Headers
        table->verticalHeader()->setVisible(false);
        for(int i = 0; i < table->columnCount(); i++)
            table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);

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

        addButton->setIcon(QIcon("media/add.svg"));
        addButton->setStyleSheet("background-color:#5596ae;"
                                 "border:none;");

        if(type == "members"){
            table->horizontalHeaderItem(1)->setText("Date\nd'adhésion");
            table->horizontalHeaderItem(2)->setText("Nom\nPrénom");
            table->horizontalHeaderItem(6)->setText("Valable\njusqu'au");
            table->horizontalHeaderItem(7)->setText("Nature\nMontant");
        }
        else if(type == "lost"){
            table->horizontalHeaderItem(1)->setText("Espèce\nSexe");
            table->horizontalHeaderItem(3)->setText("Date de\nperte");
            table->horizontalHeaderItem(4)->setText("Lieu de\nperte");
        }

        else if(type == "vet"){
            table->horizontalHeaderItem(0)->setText("Date\n du RDV");
        }

        horizontalPlacehold = findChild<QWidget*>(type + "HorizontalPlacehold");

        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setFrameStyle(QFrame::NoFrame);
    }

}

void Registry::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    if(table != nullptr && addButton != nullptr){
        // Resize modify buttons
        float iconSize = width() * 0.02;
        for (QToolButton* but : table->findChildren<QToolButton*>()){
            but->setIconSize(QSize(iconSize, iconSize));
            but->setMinimumHeight(0.8 * iconSize);
        }


        QFont font = table->font();
        float fontSize = width() * 0.007;
        font.setPointSize(fontSize);

        for(QPushButton* but : findChildren<QPushButton*>()){
            but->setFont(font);;
        }

        table->setFont(font);
        table->horizontalHeader()->setFont(font);

        table->setStyleSheet("QTableWidget::item { padding-bottom: " + QString::number(table->width() / 100) + "px;padding-top: " + QString::number(table->width() / 100) + "px;}");


        table->resizeColumnsToContents();
        table->resizeRowsToContents();

        float sumWidth = 0;

        for (int col = 0; col < table->columnCount(); ++col) {
            sumWidth += table->columnWidth(col);
        }


        if(table->verticalScrollBar()->isVisible())
            sumWidth += table->verticalScrollBar()->width();


        float factor = table->width() / sumWidth; // Allows to occupy the whole width of the window
        if(factor > 1)
            for (int col = 0; col < table->columnCount(); ++col)
                table->setColumnWidth(col, static_cast<int>(table->columnWidth(col) * factor));

        sumWidth = 0;

        for (int col = 0; col < table->columnCount() - 1; ++col) {
            sumWidth += table->columnWidth(col);
        }

        if(horizontalPlacehold != nullptr)
            horizontalPlacehold->setMaximumWidth(sumWidth);

        addButton->setFixedSize(QSize(std::max(table->columnWidth(table->columnCount() - 1), int(0.03 * width())), fontSize * 4.5));
        addButton->setIconSize(0.7 * QSize(iconSize, iconSize));
    }
}

void Registry::ColorRow(int row, bool gray){
    QColor color = gray ? QColor("#e7e9ed") : QColor("#ffffff");
    for (int col = 0; col < table->columnCount() - 1; ++col)
        table->item(row, col)->setBackground(color);
}
