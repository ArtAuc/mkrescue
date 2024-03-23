#include "destinationpage.h"

DestinationPage::DestinationPage(QWidget* parent) : QWidget(parent) {}

DestinationPage::DestinationPage(QString pageNumber){
    QGridLayout *gridLayout = new QGridLayout(this);

    QVBoxLayout *destDateEditLayout = new QVBoxLayout();
    QLabel *destDateEditLabel = new QLabel("DATE");
    QDateEdit *destDateEdit = new QDateEdit(QDate::currentDate());
    destDateEditLayout->addWidget(destDateEditLabel);
    destDateEditLayout->addWidget(destDateEdit);
    gridLayout->addLayout(destDateEditLayout, 0, 1);

    QVBoxLayout *deathCauseEditLayout = new QVBoxLayout();
    QLabel *deathCauseEditLabel = new QLabel("CAUSE DE LA MORT");
    QLineEdit *deathCauseEdit = new QLineEdit();
    deathCauseEditLayout->addWidget(deathCauseEditLabel);
    deathCauseEditLayout->addWidget(deathCauseEdit);
    gridLayout->addLayout(deathCauseEditLayout, 1, 0, 1, 2);

    EditPeopleWidget* editPeopleWidget = new EditPeopleWidget("DestEdit" + pageNumber);
    gridLayout->addWidget(editPeopleWidget, 2, 0, 1, 2);

    QVBoxLayout *destBoxLayout = new QVBoxLayout();
    QLabel *destBoxLabel = new QLabel("SORTIE N°" + pageNumber);
    QComboBox *destTypeBox = new QComboBox();
    destTypeBox->addItem("");
    destTypeBox->addItem("Adoption");
    destTypeBox->addItem("Propriétaire");
    destTypeBox->addItem("Famille d’accueil");
    destTypeBox->addItem("Entrée au refuge");
    destTypeBox->addItem("Mort");
    destBoxLayout->addWidget(destBoxLabel);
    destBoxLayout->addWidget(destTypeBox);
    gridLayout->addLayout(destBoxLayout, 0, 0);

    destDateEdit->setObjectName("destDateEdit" + pageNumber);
    destDateEditLabel->setObjectName("destDateEditLabel" + pageNumber);

    deathCauseEdit->setObjectName("deathCauseEdit" + pageNumber);
    deathCauseEditLabel->setObjectName("deathCauseEditLabel" + pageNumber);

    destTypeBox->setObjectName("destTypeBox" + pageNumber);
    destBoxLabel->setObjectName("destBoxLabel" + pageNumber);


    QWidget::setTabOrder(destTypeBox, destDateEdit);
    QWidget::setTabOrder(destDateEdit, deathCauseEdit);
}

void DestinationPage::ChangeDestType(QString type){
    for(QWidget* widget : findChildren<QWidget*>()){
        if(widget->objectName().startsWith("firstNameDestEdit") ||
            widget->objectName().startsWith("lastNameDestEdit") ||
            widget->objectName().startsWith("phoneDestEdit") ||
            widget->objectName().startsWith("emailDestEdit")||
            widget->objectName().startsWith("addressDestEdit") ||
            widget->objectName().startsWith("address2DestEdit") ||
            widget->objectName().startsWith("postalCodeDestEdit") ||
            widget->objectName().startsWith("cityDestEdit")){
            widget->setVisible(type == "Adoption" || type == "Famille d’accueil" || type == "Propriétaire");
        }

        else if(widget->objectName().startsWith("destDateEdit"))
            widget->setVisible(type != "");

        else if(widget->objectName().startsWith("deathCauseEdit"))
            widget->setVisible(type == "Mort");
    }
}
