#include "editpage.h"


QStringList EditPage::AddressList(QString address){
    QStringList addressList;

    addressList = address.split(address.contains("\\n") ? "\\n" : "\n");

    if(addressList.size() != 3){
        QMessageBox::critical(nullptr, "Erreur", "Erreur dans le format de l'adresse postale");
        return QStringList({"", "", "", ""});
    }

    else{
        QStringList postalCity = addressList[2].split(" ");
        if(postalCity.size() != 2){
            QMessageBox::critical(nullptr, "Erreur", "Erreur dans le format du code postal");
            return QStringList({"", "", "", ""});
        }

        addressList[2] = postalCity[0];
        addressList.append(postalCity[1]);
    }

    return addressList;
}

void EditPage::SetField(QString name, QString value, QWidget* parent){
    QObject *childObject = parent->findChild<QWidget*>(name);
    CustomDateTimeEdit *dateTimeEdit = qobject_cast<CustomDateTimeEdit*>(childObject);
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(childObject);
    QTextEdit *textEdit = qobject_cast<QTextEdit*>(childObject);
    QComboBox *box = qobject_cast<QComboBox*>(childObject);
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(childObject);

    if(dateTimeEdit){
        if(dateTimeEdit->DateOnly())
            dateTimeEdit->SetDate(QDate::fromString(value, "yyyy-MM-dd"));
        else
            dateTimeEdit->SetDateTime(QDateTime::fromString(value, "yyyy-MM-ddTHH:mm"));
    }
    else if (lineEdit)
        lineEdit->setText(value);
    else if (textEdit)
        textEdit->setText(value);
    else if (box)
        box->setCurrentText(value);
    else if (spinBox)
        spinBox->setValue(value.toDouble());
}

QString EditPage::GetField(QString name, QWidget* parent){
    QObject *childObject = parent->findChild<QWidget*>(name);
    CustomDateTimeEdit *dateTimeEdit = qobject_cast<CustomDateTimeEdit*>(childObject);
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(childObject);
    QTextEdit *textEdit = qobject_cast<QTextEdit*>(childObject);
    QComboBox *box = qobject_cast<QComboBox*>(childObject);
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(childObject);

    if(dateTimeEdit){
        if(dateTimeEdit->DateOnly())
            return dateTimeEdit->GetDate().toString("yyyy-MM-dd");
        else
            return dateTimeEdit->GetDateTime().toString("yyyy-MM-ddTHH:mm");
    }
    else if (lineEdit)
        return lineEdit->text();
    else if (textEdit)
        return textEdit->toPlainText();
    else if (box)
        return box->currentText();
    else if (spinBox)
        return QString::number(spinBox->value());

    return "";
}

bool EditPage::DateComparator(const QString& dest1, const QString& dest2){
    QStringList p1 = dest1.split("_|_");
    QStringList p2 = dest2.split("_|_");

    QDate date1 = QDate::fromString(p1[0], "yyyy-MM-dd");
    QDate date2 = QDate::fromString(p2[0], "yyyy-MM-dd");

    return date1 < date2;

}


// Returns id_people newly created, or already existent
QString EditPage::CreatePersonIfNeeded(QString nameEnd, QWidget *parent) {
    if(parent == nullptr)
        parent = this;


    QString last_name = GetField("lastName" + nameEnd, parent);
    QString first_name = GetField("firstName" + nameEnd, parent);
    QString phone = GetField("phone" + nameEnd, parent);
    QString email = GetField("email" + nameEnd, parent);
    QString address = GetField("address" + nameEnd, parent) + "\n" +
                      GetField("address2" + nameEnd, parent) + "\n" +
                      GetField("postalCode" + nameEnd, parent) + " " +
                      GetField("city" + nameEnd, parent);
    QString old_id_people = parent->findChild<EditPeopleWidget*>(nameEnd)->GetOldId();


    QSqlQuery query;

    // If already modified, do not ask again
    if(dealtIdPeople.contains(old_id_people))
        return old_id_people;

    // Keep this so the user is not asked if no changes are made
    query.prepare("SELECT id_people FROM People "
                   "WHERE last_name = :last_name "
                   "AND first_name = :first_name "
                   "AND phone = :phone "
                   "AND email = :email "
                   "AND address = :address");

    query.bindValue(":last_name", last_name);
    query.bindValue(":first_name", first_name);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);
    query.bindValue(":address", crypto->encryptToString(address));
    HandleErrorExec(&query);

    if(query.next())
        return query.value(0).toString();

    query.clear();

    if (old_id_people.toInt() > 0) {
        QString message;

        // ES_registry
        query.prepare("SELECT ES_registry.type_prov, Dogs.name "
                                "FROM ES_registry "
                                "JOIN Dogs ON ES_registry.id_dog = Dogs.id_dog "
                                "WHERE id_people_prov = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        while (message.count("\n") < 10 && query.next()) {
            message += "- Registre E/S : " + crypto->decryptToString(query.value(0).toString()).split("_|_")[0] + " (" + query.value(1).toString() + ")\n";
        }

        // Care_registry
        query.prepare("SELECT Dogs.name "
                                  "FROM Care_registry "
                                  "JOIN Dogs ON Care_registry.id_dog = Dogs.id_dog "
                                  "WHERE id_people_prov = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        while (message.count("\n") < 10 && query.next()) {
            message += "- Registre Garderie : " + query.value(0).toString() + "\n";
        }

        // Destinations
        query.prepare("SELECT Destinations.type, Dogs.name "
                                  "FROM Destinations "
                                  "JOIN Dogs ON Destinations.id_dog = Dogs.id_dog "
                                  "WHERE id_people = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        while (message.count("\n") < 10 && query.next()) {
            message += "- Registre E/S : " + query.value(0).toString() + " (" + query.value(1).toString() + ")\n";
        }

        // Red_list
        query.prepare("SELECT * "
                             "FROM Red_list "
                             "WHERE id_people = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        if (message.count("\n") < 10 && query.next()) {
            message += "- Liste rouge\n";
        }

        // Members
        query.prepare("SELECT strftime('%Y', date) "
                             "FROM Members "
                             "WHERE id_people = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        while (message.count("\n") < 10 && query.next()) {
            message += "- Adhérents (" + query.value(0).toString() + ")\n";
        }

        // Lost
        query.prepare("SELECT name "
                          "FROM Lost "
                          "WHERE id_people = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        while (message.count("\n") < 10 && query.next()) {
            message += "- Animaux perdus : " + query.value(0).toString() + "\n";
        }

        // Adoption_demand
        query.prepare("SELECT breed "
                                    "FROM Adoption_demand "
                                    "WHERE id_people = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        while (message.count("\n") < 10 && query.next()) {
            message += "- Demande d'adoption(" + query.value(0).toString() + ")\n";
        }

        if(message.count("\n") == 10)
            message += "...";

        QMessageBox::StandardButton reply = QMessageBox::No;
        if(message.count("\n") > 1) {
            reply = QMessageBox::warning(nullptr, "Modification sur les informations de la personne", "Voulez-vous aussi modifier les informations de " + last_name + " " + first_name +
                    " dans : \n" + message,
                    QMessageBox::Yes | QMessageBox::No);

        }

        if(reply == QMessageBox::Yes || message.count("\n") <= 1){
            query.prepare("UPDATE People "
                          "SET last_name = :last_name, "
                          "first_name = :first_name, "
                          "address = :address, "
                          "phone = :phone, "
                          "email = :email "
                          "WHERE id_people = :id");

            query.bindValue(":id", old_id_people);
            query.bindValue(":last_name", last_name);
            query.bindValue(":first_name", first_name);
            query.bindValue(":address", crypto->encryptToString(address));
            query.bindValue(":phone", phone);
            query.bindValue(":email", email);

            HandleErrorExec(&query);

            dealtIdPeople.append(old_id_people);

            return old_id_people;
        }
    }

    HandleErrorExec(&query, "SELECT MAX(id_people) + 1 FROM People WHERE id_people > 0;");
    query.next();
    QString newId = query.value(0).toString();
    if (newId.isEmpty())
        newId = "1";

    query.prepare("INSERT INTO People (id_people, last_name, first_name, address, phone, email) "
                  "VALUES (:id, :last_name, :first_name, :address, :phone, :email)");

    query.bindValue(":id", newId);
    query.bindValue(":last_name", last_name);
    query.bindValue(":first_name", first_name);
    query.bindValue(":address", crypto->encryptToString(address));
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);
    HandleErrorExec(&query);


    return newId;
}


// Returns id_dog newly created, or already existent
QString EditPage::CreateDogIfNeeded(QString nameEnd, QWidget *parent) {
    if(parent == nullptr)
        parent = this;

    QString name = GetField("dogName" + nameEnd, parent);
    QString chip = GetField("chip" + nameEnd, parent);
    QString sex = GetField("sex" + nameEnd, parent);
    QString description = GetField("description" + nameEnd, parent);
    QString birth = GetField("birthDate" + nameEnd, parent);
    QString old_id_dog = parent->findChild<EditDogWidget*>(nameEnd)->GetOldId();

    if(name.isEmpty() && chip.isEmpty()){ // Empty fields
        QMessageBox::critical(nullptr, "Erreur", "Veuillez entrer un nom ou une identification pour le chien");
        return "-2";
    }

    QSqlQuery query;

    // If already modified, do not ask again
    if(dealtIdDog.contains(old_id_dog))
        return old_id_dog;

    // Keep this so the user is not asked if no changes are made
    query.prepare("SELECT id_dog FROM Dogs "
                  "WHERE chip = :chip "
                  "AND name = :name "
                  "AND sex = :sex "
                  "AND description = :description "
                  "AND birth = :birth");

    query.bindValue(":chip", chip);
    query.bindValue(":name", name);
    query.bindValue(":sex", sex);
    query.bindValue(":description", description);
    query.bindValue(":birth", birth);

    HandleErrorExec(&query);
    if (query.next())
        return query.value(0).toString();
    query.clear();

    if(old_id_dog.toInt() > 0){
        // If does not correspond exactly, ask if the user wants to overwrite
        QString message;

        // ES_registry
        query.prepare("SELECT ES_registry.type_prov "
                      "FROM ES_registry "
                      "WHERE ES_registry.id_dog = :old_id_dog;");
        query.bindValue(":old_id_dog", old_id_dog);

        HandleErrorExec(&query);

        while(message.count("\n") < 10 && query.next()){
            message += "Registre E/S : " + crypto->decryptToString(query.value(0).toString()).split("_|_")[0] + "\n";
        }

        // Care_registry
        query.prepare("SELECT * "
                      "FROM Care_registry "
                      "WHERE Care_registry.id_dog = :old_id_dog");
        query.bindValue(":old_id_dog", old_id_dog);
        HandleErrorExec(&query);

        if(message.count("\n") < 10 && query.next())
            message += "Registre Garderie\n";

        // Vet
        query.prepare("SELECT * "
                      "FROM Vet "
                      "WHERE Vet.id_dog = :old_id_dog");
        query.bindValue(":old_id_dog", old_id_dog);
        HandleErrorExec(&query);

        if(message.count("\n") < 10 && query.next())
            message += "RDV Vétérinaire\n";

        QMessageBox::StandardButton reply = QMessageBox::No;
        if(message.count("\n") > 1) {
            reply = QMessageBox::warning(nullptr, "Modification sur les informations du chien", "Voulez-vous aussi modifier les informations de " + name +
                    " dans : \n" + message,
                    QMessageBox::Yes | QMessageBox::No);

        }

        if(reply == QMessageBox::Yes || message.count("\n") <= 1){
            query.prepare("UPDATE Dogs "
                          "SET name = :name, "
                          "sex = :sex, "
                          "chip = :chip, "
                          "description = :description, "
                          "birth = :birth "
                          "WHERE id_dog = :id");

            query.bindValue(":id", old_id_dog);
            query.bindValue(":name", name);
            query.bindValue(":sex", sex);
            query.bindValue(":chip", chip);
            query.bindValue(":description", description);
            query.bindValue(":birth", birth);

            HandleErrorExec(&query);

            dealtIdDog.append(old_id_dog);

            return old_id_dog;
        }
    }

    HandleErrorExec(&query, "SELECT MAX(id_dog) + 1 FROM Dogs;");
    query.next();
    QString newId = query.value(0).toString();
    if (newId.isEmpty())
        newId = "1";

    query.prepare("INSERT INTO Dogs (id_dog, name, sex, chip, description, birth) "
                  "VALUES (:id, :name, :sex, :chip, :description, :birth)");

    query.bindValue(":id", newId);
    query.bindValue(":name", name);
    query.bindValue(":sex", sex);
    query.bindValue(":chip", chip);
    query.bindValue(":description", description);
    query.bindValue(":birth", birth);
    HandleErrorExec(&query);

    return newId;
}
