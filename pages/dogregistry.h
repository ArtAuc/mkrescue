#ifndef DOGREGISTRY_H
#define DOGREGISTRY_H

#include "registry.h"


class DogRegistry : public Registry
{
    Q_OBJECT
public:
    DogRegistry(QWidget *parent = nullptr) : Registry{parent}{}

    void showEvent(QShowEvent* event) override{
        Registry::showEvent(event);

        for (int i = 0; i < children().size(); i++) {
            QObject* child = children().at(i);
            if (child->objectName() == type + "Label1")
                label1 = qobject_cast<QLabel*>(child);
            else if (child->objectName() == type + "Label2")
                label2 = qobject_cast<QLabel*>(child);
            else if (child->objectName() == type + "Label3")
                label3 = qobject_cast<QLabel*>(child);
        }

        if(table != nullptr && label1 != nullptr && label2 != nullptr && label3 != nullptr){
            if(type == "sponsors"){
                table->horizontalHeaderItem(4)->setText("Montant\npar mois");
                table->horizontalHeaderItem(5)->setText("Date\ndébut");
                table->horizontalHeaderItem(6)->setText("Date\nfin");
            }

            else if(type != "adoptionDemand"){
                table->horizontalHeaderItem(1)->setText("Date\nd'entrée");
                table->horizontalHeaderItem(3)->setText("Espèce\nSexe");
                table->horizontalHeaderItem(6)->setText("Date de\nnaissance");
                table->horizontalHeaderItem(7)->setText("Date de\nsortie");
                if(type == "entry")
                    table->horizontalHeaderItem(9)->setText("Cause de\nla mort");

                // Bold first column #
                if(table->item(0,0) != nullptr){
                    QFont font(table->item(0, 0)->font());
                    font.setBold(true);
                    for(int row = 0; row < table->rowCount(); row++)
                        table->item(row, 0)->setFont(font);
                }
            }
        }
    }

    void resizeEvent(QResizeEvent *event) override{
        Registry::resizeEvent(event);

        if(table != nullptr && label1 != nullptr && label2 != nullptr && label3 != nullptr){
            // Supcategories labels' widths according to table's width
            if(type == "adoptionDemand"){
                label1->setMaximumWidth(table->columnWidth(0) + table->columnWidth(1));
                label2->setMaximumWidth(table->columnWidth(2) + table->columnWidth(3) + table->columnWidth(4));
                label3->setMaximumWidth(table->columnWidth(5));
            }

            else if(type == "sponsors"){
                label1->setMaximumWidth(table->columnWidth(0) + table->columnWidth(1));
                label2->setMaximumWidth(table->columnWidth(2) + table->columnWidth(3));
                label3->setMaximumWidth(table->columnWidth(4) + table->columnWidth(5) + table->columnWidth(6));
            }

            else{
                label1->setMaximumWidth(table->columnWidth(0) + table->columnWidth(1) + table->columnWidth(2));
                label2->setMaximumWidth(table->columnWidth(3) + table->columnWidth(4) + table->columnWidth(5) + table->columnWidth(6));

                if(type == "care")
                    label3->setMaximumWidth(table->columnWidth(7) + table->columnWidth(8));
                else if (type == "entry")
                    label3->setMaximumWidth(table->columnWidth(7) + table->columnWidth(8) + table->columnWidth(9));
            }

            QFont font = table->font();
            float fontSize = font.pointSize();
            font.setPointSize(fontSize * 2);
            label1->setFont(font);
            label2->setFont(font);
            label3->setFont(font);

            float sumWidth = 0;

            for (int col = 0; col < table->columnCount(); ++col) {
                sumWidth += table->columnWidth(col);
            }

            // Disable labels if window is too narrow
            bool scrollable = sumWidth > width() ;
            label1->setVisible(!scrollable);
            label2->setVisible(!scrollable);
            label3->setVisible(!scrollable);
        }
    }


private:
    QLabel* label1 = nullptr, *label2 = nullptr, *label3 = nullptr;
};

#endif // DOGREGISTRY_H
