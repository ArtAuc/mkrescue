#include "editpeoplewidget.h"

EditPeopleWidget::EditPeopleWidget(QWidget *parent)
    : QWidget{parent}
{

}

EditPeopleWidget::EditPeopleWidget(QString nameEnd, bool simplified){
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *firstNameEditLayout = new QVBoxLayout();
    QLabel *firstNameEditLabel = new QLabel("PRÉNOM");
    QLineEdit *firstNameEdit = new QLineEdit();
    firstNameEditLayout->addWidget(firstNameEditLabel);
    firstNameEditLayout->addWidget(firstNameEdit);
    gridLayout->addLayout(firstNameEditLayout, 0, 1);

    QVBoxLayout *lastNameEditLayout = new QVBoxLayout();
    QLabel *lastNameEditLabel = new QLabel("NOM");
    QLineEdit *lastNameEdit = new QLineEdit();
    lastNameEditLayout->addWidget(lastNameEditLabel);
    lastNameEditLayout->addWidget(lastNameEdit);
    gridLayout->addLayout(lastNameEditLayout, 0, 0);

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

    QVBoxLayout *addressEditLayout = new QVBoxLayout();
    QLabel *addressEditLabel = new QLabel("ADRESSE");
    QLineEdit *addressEdit = new QLineEdit();
    addressEditLayout->addWidget(addressEditLabel);
    addressEditLayout->addWidget(addressEdit);
    gridLayout->addLayout(addressEditLayout, 3, 0);

    QVBoxLayout *address2EditLayout = new QVBoxLayout();
    QLabel *address2EditLabel = new QLabel("COMPLÉMENT D'ADRESSE");
    QLineEdit *address2Edit = new QLineEdit();
    address2EditLayout->addWidget(address2EditLabel);
    address2EditLayout->addWidget(address2Edit);
    gridLayout->addLayout(address2EditLayout, 3, 1);

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

    QWidget::setTabOrder(lastNameEdit, firstNameEdit);
    QWidget::setTabOrder(firstNameEdit, phoneEdit);
    QWidget::setTabOrder(phoneEdit, emailEdit);
    QWidget::setTabOrder(emailEdit, addressEdit);
    QWidget::setTabOrder(addressEdit, address2Edit);
    QWidget::setTabOrder(address2Edit, postalCodeEdit);
    QWidget::setTabOrder(postalCodeEdit, cityEdit);

    lastNameEdit->setMaxLength(50);
    firstNameEdit->setMaxLength(255);
    phoneEdit->setMaxLength(20);
    emailEdit->setMaxLength(255);
    addressEdit->setMaxLength(255);
    address2Edit->setMaxLength(255);
    postalCodeEdit->setMaxLength(20);
    cityEdit->setMaxLength(255);

    QRegularExpression disallowUnderscore("\\A[^_|]{0,255}\\z");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(disallowUnderscore, this); // To prevent parsing problems (separators are made of _, |)
    lastNameEdit->setValidator(validator);
    firstNameEdit->setValidator(validator);
    phoneEdit->setValidator(validator);
    emailEdit->setValidator(validator);
    addressEdit->setValidator(validator);
    address2Edit->setValidator(validator);
    QRegularExpression disallowUnderscoreSpace("\\A[^_|\\s]{0,255}\\z");
    validator = new QRegularExpressionValidator(disallowUnderscoreSpace, this);
    postalCodeEdit->setValidator(validator);
    cityEdit->setValidator(validator);

    if(simplified){ // Only show first name, last name and phone
        for(QWidget* c : findChildren<QWidget*>()){
            QString cName = c->objectName();
            if (!(cName.startsWith("firstName") || cName.startsWith("lastName") || cName.startsWith("phone")))
                c->setVisible(false);
        }
    }
}


void EditPeopleWidget::showEvent(QShowEvent* event){
    QSqlQuery query;

    QStringList firstNameList, lastNameList, phoneList, emailList, addressList, address2List, postalCodeList, cityList;

    query.exec("SELECT id_people, last_name, first_name, phone, email, address "
               "FROM People "
               "ORDER BY id_people DESC;");

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

            lastNameList << last_name + (first_name.isEmpty() ? "" : "|") + first_name + (phone.isEmpty() ? "" : "|") + phone;
            firstNameList << first_name + (last_name.isEmpty() ? "" : "|") + last_name + (phone.isEmpty() ? "" : "|") + phone;
            phoneList << phone + (last_name.isEmpty() ? "" : "|") + last_name + (first_name.isEmpty() ? "" : "|") + first_name;
            emailList << email + (last_name.isEmpty() ? "" : "|") + last_name + (first_name.isEmpty() ? "" : "|") + first_name;
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
                completer->setCaseSensitivity(Qt::CaseInsensitive);
                lineEdit->setCompleter(completer);
                if(editName.contains("lastName") || editName.contains("firstName") || editName.contains("phone") || editName.contains("email")){
                    QObject::disconnect(completer, SIGNAL(activated(QString)), nullptr, nullptr);
                    QObject::disconnect(completer, SIGNAL(highlighted(QString)), nullptr, nullptr);
                    connect(completer, SIGNAL(activated(QString)), this, SLOT(FillOtherFields(QString)));
                    connect(completer, SIGNAL(activated(QString)), this, SLOT(FillOtherFields(QString)));
                    connect(completer, SIGNAL(highlighted(QString)), this, SLOT(PreviewOtherFields(QString)));
                    connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(PreviewOtherFields()));
                }
            }
        }
    }
}

void EditPeopleWidget::FillOtherFields(QString s){
    QCompleter* completer = qobject_cast<QCompleter*>(QObject::sender());
    QAbstractItemModel *model = nullptr;
    if(completer)
        model = completer->model();

    int row = -1;

    if (model) {
        int rowCount = model->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            QModelIndex index = model->index(i, 0);
            QVariant data = model->data(index, Qt::DisplayRole);
            if (data.isValid() && data.toString() == s){
                row = i;
                break;
            }
        }
    }

    QList<QLineEdit*> lineEdits = this->findChildren<QLineEdit*>();
    if(row >= 0){
        foreach (QLineEdit* lineEdit, lineEdits) {
            if (!lineEdit->objectName().contains("spinbox")) {
                QStringList list;
                QAbstractItemModel *model = lineEdit->completer()->model();

                if (model) {
                    int rowCount = model->rowCount();
                    for (int i = 0; i < rowCount; ++i) {
                        QModelIndex index = model->index(i, 0);
                        QVariant data = model->data(index, Qt::DisplayRole);
                        if (data.isValid())
                            list << data.toString();
                    }
                }

                QTimer::singleShot(0, [=]() {
                    lineEdit->setText(list[row].split("|")[0]);
                });
            }
        }
    }
}


void EditPeopleWidget::PreviewOtherFields(QString s){
    QCompleter* completer = qobject_cast<QCompleter*>(QObject::sender());
    QAbstractItemModel *model = nullptr;
    if(completer)
        model = completer->model();

    int row = -1;

    if (model) {
        int rowCount = model->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            QModelIndex index = model->index(i, 0);
            QVariant data = model->data(index, Qt::DisplayRole);
            if (data.isValid() && data.toString() == s){
                row = i;
                break;
            }
        }
    }

    QList<QLineEdit*> lineEdits = this->findChildren<QLineEdit*>();
    if(row >= 0){
        foreach (QLineEdit* lineEdit, lineEdits) {
            if (!lineEdit->objectName().contains("spinbox")) {
                QStringList list;
                QAbstractItemModel *model = lineEdit->completer()->model();

                if (model) {
                    int rowCount = model->rowCount();
                    for (int i = 0; i < rowCount; ++i) {
                        QModelIndex index = model->index(i, 0);
                        QVariant data = model->data(index, Qt::DisplayRole);
                        if (data.isValid())
                            list << data.toString();
                    }
                }

                lineEdit->setPlaceholderText(list[row].split("|")[0]);
            }
        }
    }

    else{
        foreach (QLineEdit* lineEdit, lineEdits)
            lineEdit->setPlaceholderText("");
    }
}
