#include "editdogwidget.h"

EditDogWidget::EditDogWidget(QWidget *parent) : QWidget(parent){}

EditDogWidget::EditDogWidget(QString nameEnd){
       // Create widgets
       QLabel *chipLabel = new QLabel("N° D'IDENTIFICATION");
       QLineEdit *chipEdit = new QLineEdit;

       QLabel *dogNameLabel = new QLabel("NOM");
       QLineEdit *dogNameEdit = new QLineEdit;

       QLabel *descriptionLabel = new QLabel("DESCRIPTION");
       QLineEdit *descriptionEdit = new QLineEdit;

       QLabel *sexLabel = new QLabel("SEXE");
       QComboBox *sexBox = new QComboBox;
       sexBox->addItem("Mâle");
       sexBox->addItem("Femelle");

       QLabel *birthDateLabel = new QLabel("DATE DE NAISSANCE");
       QDateEdit *birthDateEdit = new QDateEdit;

       chipEdit->setObjectName("chip" + nameEnd);
       chipLabel->setObjectName("chip" + nameEnd + "Label");

       dogNameEdit->setObjectName("dogName" + nameEnd);
       dogNameLabel->setObjectName("dogName" + nameEnd + "Label");

       descriptionEdit->setObjectName("description" + nameEnd);
       descriptionLabel->setObjectName("description" + nameEnd + "Label");

       sexBox->setObjectName("sex" + nameEnd);
       sexLabel->setObjectName("sex" + nameEnd + "Label");

       birthDateEdit->setObjectName("birthDate" + nameEnd);
       birthDateLabel->setObjectName("birthDate" + nameEnd + "Label");

       // Create layouts
       QVBoxLayout *chipEditLayout = new QVBoxLayout;
       chipEditLayout->addWidget(chipLabel);
       chipEditLayout->addWidget(chipEdit);

       QVBoxLayout *dogNameEditLayout = new QVBoxLayout;
       dogNameEditLayout->addWidget(dogNameLabel);
       dogNameEditLayout->addWidget(dogNameEdit);

       QVBoxLayout *descriptionEditLayout = new QVBoxLayout;
       descriptionEditLayout->addWidget(descriptionLabel);
       descriptionEditLayout->addWidget(descriptionEdit);

       QVBoxLayout *sexBoxLayout = new QVBoxLayout;
       sexBoxLayout->addWidget(sexLabel);
       sexBoxLayout->addWidget(sexBox);

       QVBoxLayout *birthDateEditLayout = new QVBoxLayout;
       birthDateEditLayout->addWidget(birthDateLabel);
       birthDateEditLayout->addWidget(birthDateEdit);

       QGridLayout *gridLayout = new QGridLayout(this);
       gridLayout->addLayout(chipEditLayout, 1, 1);
       gridLayout->addLayout(dogNameEditLayout, 1, 0);
       gridLayout->addLayout(descriptionEditLayout, 5, 0, 1, 2);
       gridLayout->addLayout(sexBoxLayout, 3, 0);
       gridLayout->addLayout(birthDateEditLayout, 3, 1);

       setLayout(gridLayout);
}

void EditDogWidget::showEvent(QShowEvent* event){
    /*QWidget::showEvent(event);
    QSqlQuery query;

    QStringList firstNameList, lastNameList, phoneList, emailList, addressList, address2List, postalCodeList, cityList;

    query.exec("SELECT name, chip, description, sex, birth "
               "FROM Dogs;");

    while (query.next()) {
        if (query.value(0).toInt() > 0) {
            QString last_name = query.value(1).toString().isNull() ? "" : query.value(1).toString();
            QString first_name = query.value(2).toString().isNull() ? "" : query.value(2).toString();
            QString phone = query.value(3).toString().isNull() ? "" : query.value(3).toString();
            QString email = query.value(4).toString().isNull() ? "" : query.value(4).toString();
            QString address = query.value(5).toString().isNull() ? "" : query.value(5).toString();

            QStringList addressComponents = address.split("\n");
            if (addressComponents.size() == 3) {
                addressList << addressComponents[0];
                address2List << addressComponents[1];
                QString postalCodeCity = addressComponents[2];

                QStringList postalCodeCityComponents = postalCodeCity.split(" ");
                if (postalCodeCityComponents.size() == 2) {
                    postalCodeList << postalCodeCityComponents[0];
                    cityList << postalCodeCityComponents[1];
                }
            }

            lastNameList << last_name;
            firstNameList << first_name;
            phoneList << phone;
            emailList << email;
        }
    }


    QList<QLineEdit*> lineEdits = this->findChildren<QLineEdit*>();
    foreach (QLineEdit* lineEdit, lineEdits) {
        QString editName = lineEdit->objectName();
        if (!editName.contains("spinbox")) {
            QCompleter* completer = nullptr;
            if (editName.contains("address2"))
                completer = new QCompleter(new QStringListModel(address2List, this), lineEdit);
            else if (editName.contains("firstName"))
                completer = new QCompleter(new QStringListModel(firstNameList, this), lineEdit);
            else if (editName.contains("address"))
                completer = new QCompleter(new QStringListModel(addressList, this), lineEdit);
            else if (editName.contains("postalCode"))
                completer = new QCompleter(new QStringListModel(postalCodeList, this), lineEdit);
            else if (editName.contains("city"))
                completer = new QCompleter(new QStringListModel(cityList, this), lineEdit);
            else if (editName.contains("lastName"))
                completer = new QCompleter(new QStringListModel(lastNameList, this), lineEdit);
            else if (editName.contains("phone"))
                completer = new QCompleter(new QStringListModel(phoneList, this), lineEdit);
            else if (editName.contains("email"))
                completer = new QCompleter(new QStringListModel(emailList, this), lineEdit);

            if (completer) {
                lineEdit->setCompleter(completer);
                completer->setCaseSensitivity(Qt::CaseInsensitive);
                if(editName.contains("lastName") || editName.contains("phone") || editName.contains("email")){
                    connect(completer, SIGNAL(activated(QString)), this, SLOT(FillOtherFields(QString)));
                    connect(completer, SIGNAL(highlighted(QString)), this, SLOT(PreviewOtherFields(QString)));
                    connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(PreviewOtherFields()));
                }
            }
        }
    }*/
}
