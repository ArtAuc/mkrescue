#include "destinationpage.h"

DestinationPage::DestinationPage(QWidget* parent) : QWidget(parent) {
       QGridLayout *gridLayout = new QGridLayout(this);

       QVBoxLayout *firstNameDestEditLayout = new QVBoxLayout();
       QLabel *firstNameDestEditLabel = new QLabel("PRÉNOM");
       QLineEdit *firstNameDestEdit = new QLineEdit("Prénom");
       firstNameDestEditLayout->addWidget(firstNameDestEditLabel);
       firstNameDestEditLayout->addWidget(firstNameDestEdit);
       gridLayout->addLayout(firstNameDestEditLayout, 1, 0);

       QVBoxLayout *addressDestEditLayout = new QVBoxLayout();
       QLabel *addressDestEditLabel = new QLabel("ADRESSE");
       QLineEdit *addressDestEdit = new QLineEdit("Adresse");
       addressDestEditLayout->addWidget(addressDestEditLabel);
       addressDestEditLayout->addWidget(addressDestEdit);
       gridLayout->addLayout(addressDestEditLayout, 4, 0);

       QVBoxLayout *destDateEditLayout = new QVBoxLayout();
       QLabel *destDateEditLabel = new QLabel("DATE");
       QDateEdit *destDateEdit = new QDateEdit();
       destDateEditLayout->addWidget(destDateEditLabel);
       destDateEditLayout->addWidget(destDateEdit);
       gridLayout->addLayout(destDateEditLayout, 0, 1);

       QVBoxLayout *postalCodeDestEditLayout = new QVBoxLayout();
       QLabel *postalCodeDestEditLabel = new QLabel("CODE POSTAL");
       QLineEdit *postalCodeDestEdit = new QLineEdit("Code postal");
       postalCodeDestEditLayout->addWidget(postalCodeDestEditLabel);
       postalCodeDestEditLayout->addWidget(postalCodeDestEdit);
       gridLayout->addLayout(postalCodeDestEditLayout, 5, 1);

       QVBoxLayout *cityDestEditLayout = new QVBoxLayout();
       QLabel *cityDestEditLabel = new QLabel("VILLE");
       QLineEdit *cityDestEdit = new QLineEdit("Ville");
       cityDestEditLayout->addWidget(cityDestEditLabel);
       cityDestEditLayout->addWidget(cityDestEdit);
       gridLayout->addLayout(cityDestEditLayout, 5, 0);

       QVBoxLayout *lastNameDestEditLayout = new QVBoxLayout();
       QLabel *lastNameDestEditLabel = new QLabel("NOM");
       QLineEdit *lastNameDestEdit = new QLineEdit("Nom");
       lastNameDestEditLayout->addWidget(lastNameDestEditLabel);
       lastNameDestEditLayout->addWidget(lastNameDestEdit);
       gridLayout->addLayout(lastNameDestEditLayout, 1, 1);

       QVBoxLayout *deathCauseEditLayout = new QVBoxLayout();
       QLabel *deathCauseEditLabel = new QLabel("CAUSE DE LA MORT");
       QLineEdit *deathCauseEdit = new QLineEdit();
       deathCauseEditLayout->addWidget(deathCauseEditLabel);
       deathCauseEditLayout->addWidget(deathCauseEdit);
       gridLayout->addLayout(deathCauseEditLayout, 6, 0);

       QVBoxLayout *phoneDestEditLayout = new QVBoxLayout();
       QLabel *phoneDestEditLabel = new QLabel("TÉLÉPHONE");
       QLineEdit *phoneDestEdit = new QLineEdit("Téléphone");
       phoneDestEditLayout->addWidget(phoneDestEditLabel);
       phoneDestEditLayout->addWidget(phoneDestEdit);
       gridLayout->addLayout(phoneDestEditLayout, 2, 0, 1, 2);

       QVBoxLayout *emailDestEditLayout = new QVBoxLayout();
       QLabel *emailDestEditLabel = new QLabel("EMAIL");
       QLineEdit *emailDestEdit = new QLineEdit("Mail");
       emailDestEditLayout->addWidget(emailDestEditLabel);
       emailDestEditLayout->addWidget(emailDestEdit);
       gridLayout->addLayout(emailDestEditLayout, 3, 0, 1, 2);

       QVBoxLayout *destBoxLayout = new QVBoxLayout();
       QLabel *destBoxLabel = new QLabel("SORTIE N°");
       QComboBox *destTypeBox = new QComboBox();
       destTypeBox->addItem("");
       destTypeBox->addItem("Adoption");
       destTypeBox->addItem("Propriétaire");
       destTypeBox->addItem("Famille d'accueil");
       destTypeBox->addItem("Entrée au refuge");
       destTypeBox->addItem("Mort");
       destBoxLayout->addWidget(destBoxLabel);
       destBoxLayout->addWidget(destTypeBox);
       gridLayout->addLayout(destBoxLayout, 0, 0);

       QVBoxLayout *address2DestEditLayout = new QVBoxLayout();
       QLabel *address2DestEditLabel = new QLabel("COMPLÉMENT D'ADRESSE");
       QLineEdit *address2DestEdit = new QLineEdit("Adresse 2");
       address2DestEditLayout->addWidget(address2DestEditLabel);
       address2DestEditLayout->addWidget(address2DestEdit);
       gridLayout->addLayout(address2DestEditLayout, 4, 1);

       firstNameDestEdit->setObjectName("firstNameDestEdit");
       firstNameDestEditLabel->setObjectName("firstNameDestEditLabel");

       addressDestEdit->setObjectName("addressDestEdit");
       addressDestEditLabel->setObjectName("addressDestEditLabel");

       destDateEdit->setObjectName("destDateEdit");
       destDateEditLabel->setObjectName("destDateEditLabel");

       postalCodeDestEdit->setObjectName("postalCodeDestEdit");
       postalCodeDestEditLabel->setObjectName("postalCodeDestEditLabel");

       cityDestEdit->setObjectName("cityDestEdit");
       cityDestEditLabel->setObjectName("cityDestEditLabel");

       lastNameDestEdit->setObjectName("lastNameDestEdit");
       lastNameDestEditLabel->setObjectName("lastNameDestEditLabel");

       deathCauseEdit->setObjectName("deathCauseEdit");
       deathCauseEditLabel->setObjectName("deathCauseEditLabel");

       phoneDestEdit->setObjectName("phoneDestEdit");
       phoneDestEditLabel->setObjectName("phoneDestEditLabel");

       emailDestEdit->setObjectName("emailDestEdit");
       emailDestEditLabel->setObjectName("emailDestEditLabel");

       destTypeBox->setObjectName("destTypeBox");
       destBoxLabel->setObjectName("destBoxLabel");

       address2DestEdit->setObjectName("address2DestEdit");
       address2DestEditLabel->setObjectName("address2DestEditLabel");

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
            widget->setVisible(type == "Adoption" || type == "Famille d'accueil" || type == "Propriétaire");
        }

        else if(widget->objectName().startsWith("destDateEdit"))
            widget->setVisible(type != "");

        else if(widget->objectName().startsWith("deathCauseEdit"))
            widget->setVisible(type == "Mort");
    }
}
