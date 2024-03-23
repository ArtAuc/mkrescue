#include "editpeoplewidget.h"

EditPeopleWidget::EditPeopleWidget(QWidget *parent)
    : QWidget{parent}
{

}

EditPeopleWidget::EditPeopleWidget(QString nameEnd){
    QGridLayout *gridLayout = new QGridLayout(this);

    QVBoxLayout *firstNameEditLayout = new QVBoxLayout();
    QLabel *firstNameEditLabel = new QLabel("PRÉNOM");
    QLineEdit *firstNameEdit = new QLineEdit();
    firstNameEditLayout->addWidget(firstNameEditLabel);
    firstNameEditLayout->addWidget(firstNameEdit);
    gridLayout->addLayout(firstNameEditLayout, 0, 0);

    QVBoxLayout *addressEditLayout = new QVBoxLayout();
    QLabel *addressEditLabel = new QLabel("ADRESSE");
    QLineEdit *addressEdit = new QLineEdit();
    addressEditLayout->addWidget(addressEditLabel);
    addressEditLayout->addWidget(addressEdit);
    gridLayout->addLayout(addressEditLayout, 3, 0);

    QVBoxLayout *postalCodeEditLayout = new QVBoxLayout();
    QLabel *postalCodeEditLabel = new QLabel("CODE POSTAL");
    QLineEdit *postalCodeEdit = new QLineEdit();
    postalCodeEditLayout->addWidget(postalCodeEditLabel);
    postalCodeEditLayout->addWidget(postalCodeEdit);
    gridLayout->addLayout(postalCodeEditLayout, 4, 0);

    QVBoxLayout *cityEditLayout = new QVBoxLayout();
    QLabel *cityEditLabel = new QLabel("VILLE");
    QLineEdit *cityEdit = new QLineEdit();
    cityEditLayout->addWidget(cityEditLabel);
    cityEditLayout->addWidget(cityEdit);
    gridLayout->addLayout(cityEditLayout, 4, 1);

    QVBoxLayout *lastNameEditLayout = new QVBoxLayout();
    QLabel *lastNameEditLabel = new QLabel("NOM");
    QLineEdit *lastNameEdit = new QLineEdit();
    lastNameEditLayout->addWidget(lastNameEditLabel);
    lastNameEditLayout->addWidget(lastNameEdit);
    gridLayout->addLayout(lastNameEditLayout, 0, 1);

    QVBoxLayout *phoneEditLayout = new QVBoxLayout();
    QLabel *phoneEditLabel = new QLabel("TÉLÉPHONE");
    QLineEdit *phoneEdit = new QLineEdit();
    phoneEditLayout->addWidget(phoneEditLabel);
    phoneEditLayout->addWidget(phoneEdit);
    gridLayout->addLayout(phoneEditLayout, 1, 0, 1, 2);

    QVBoxLayout *emailEditLayout = new QVBoxLayout();
    QLabel *emailEditLabel = new QLabel("EMAIL");
    QLineEdit *emailEdit = new QLineEdit();
    emailEditLayout->addWidget(emailEditLabel);
    emailEditLayout->addWidget(emailEdit);
    gridLayout->addLayout(emailEditLayout, 2, 0, 1, 2);

    QVBoxLayout *address2EditLayout = new QVBoxLayout();
    QLabel *address2EditLabel = new QLabel("COMPLÉMENT D'ADRESSE");
    QLineEdit *address2Edit = new QLineEdit();
    address2EditLayout->addWidget(address2EditLabel);
    address2EditLayout->addWidget(address2Edit);
    gridLayout->addLayout(address2EditLayout, 3, 1);

    firstNameEdit->setObjectName("firstName" + nameEnd);
    firstNameEditLabel->setObjectName("firstName" + nameEnd + "Label");

    addressEdit->setObjectName("address" + nameEnd);
    addressEditLabel->setObjectName("address" + nameEnd + "Label");

    postalCodeEdit->setObjectName("postalCode" + nameEnd);
    postalCodeEditLabel->setObjectName("postalCode" + nameEnd + "Label");

    cityEdit->setObjectName("city" + nameEnd);
    cityEditLabel->setObjectName("city" + nameEnd + "Label");

    lastNameEdit->setObjectName("lastName" + nameEnd);
    lastNameEditLabel->setObjectName("lastName" + nameEnd + "Label");

    phoneEdit->setObjectName("phone" + nameEnd);
    phoneEditLabel->setObjectName("phone" + nameEnd + "Label");

    emailEdit->setObjectName("email" + nameEnd);
    emailEditLabel->setObjectName("email" + nameEnd + "Label");

    address2Edit->setObjectName("address2" + nameEnd);
    address2EditLabel->setObjectName("address2" + nameEnd + "Label");

    QWidget::setTabOrder(firstNameEdit, lastNameEdit);
    QWidget::setTabOrder(lastNameEdit, phoneEdit);
    QWidget::setTabOrder(phoneEdit, emailEdit);
    QWidget::setTabOrder(emailEdit, addressEdit);
    QWidget::setTabOrder(addressEdit, address2Edit);
    QWidget::setTabOrder(address2Edit, cityEdit);
    QWidget::setTabOrder(postalCodeEdit, cityEdit);

    setStyleSheet("padding:0;");
}

