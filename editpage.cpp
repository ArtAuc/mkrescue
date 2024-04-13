#include "editpage.h"

EditPage::EditPage(QWidget *parent)
    : QWidget{parent}
{

}

void EditPage::SwitchPage(QString pageName){
    resizeEvent(nullptr);
    qobject_cast<QStackedWidget*>(parent())->setCurrentWidget(this);
    findChild<QStackedWidget*>("editStacked")->setCurrentWidget(findChild<QWidget*>(pageName));
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
            message += "Registre E/S : " + crypto->decryptToString(query.value(0).toString()).split("_|_")[0] + " (" + query.value(1).toString() + ")\n";
        }

        // Care_registry
        query.prepare("SELECT Dogs.name "
                                  "FROM Care_registry "
                                  "JOIN Dogs ON Care_registry.id_dog = Dogs.id_dog "
                                  "WHERE id_people_prov = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        while (message.count("\n") < 10 && query.next()) {
            message += "Registre Garderie : " + query.value(0).toString() + "\n";
        }

        // Destinations
        query.prepare("SELECT Destinations.type, Dogs.name "
                                  "FROM Destinations "
                                  "JOIN Dogs ON Destinations.id_dog = Dogs.id_dog "
                                  "WHERE id_people = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        while (message.count("\n") < 10 && query.next()) {
            message += "Registre E/S : " + query.value(0).toString() + " (" + query.value(1).toString() + ")\n";
        }

        // Red_list
        query.prepare("SELECT * "
                             "FROM Red_list "
                             "WHERE id_people = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        if (message.count("\n") < 10 && query.next()) {
            message += "Liste rouge\n";
        }

        // Members
        query.prepare("SELECT strftime('%Y', date) "
                             "FROM Members "
                             "WHERE id_people = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        while (message.count("\n") < 10 && query.next()) {
            message += "Adhérents (" + query.value(0).toString() + ")\n";
        }

        // Lost
        query.prepare("SELECT name "
                          "FROM Lost "
                          "WHERE id_people = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        while (message.count("\n") < 10 && query.next()) {
            message += "Animaux perdus : " + query.value(0).toString() + "\n";
        }

        // Adoption_demand
        query.prepare("SELECT breed "
                                    "FROM Adoption_demand "
                                    "WHERE id_people = :old_id_people");
        query.bindValue(":old_id_people", old_id_people);
        HandleErrorExec(&query);
        while (message.count("\n") < 10 && query.next()) {
            message += "Demande d'adoption(" + query.value(0).toString() + ")\n";
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

void EditPage::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    QList<QWidget*> children = findChild<QWidget*>(lastType + "EditPage")->findChildren<QWidget*>();

    if(children.size() > 0){
        QFont font = children[0]->font();
        float fontSize = std::max(width() * 0.007, (double) 9);
        font.setPointSize(fontSize);

        for(QWidget* c : children){
            if(!c->objectName().contains("spinbox") && (qobject_cast<QLineEdit*>(c) || qobject_cast<QTextEdit*>(c) || qobject_cast<QDateTimeEdit*>(c) || qobject_cast<QComboBox*>(c) || qobject_cast<QDoubleSpinBox*>(c) || qobject_cast<QCheckBox*>(c))){
                c->setFont(font);
                c->setStyleSheet("padding:" + QString::number(fontSize) + "px;");
            }

            else if (qobject_cast<QLabel*>(c))
                c->setFont(font);
        }

        font.setPointSize(fontSize * 1.2);

        for(QPushButton *b : findChildren<QPushButton*>())
        {
            b->setFont(font);
            b->setStyleSheet("padding:" + QString::number(fontSize) + "px;");
        }
    }



    // Margins besides the form
    QMargins margins = layout()->contentsMargins();
    margins.setLeft(0.2 * width());
    margins.setRight(0.2 * width());
    layout()->setContentsMargins(margins);

    // Tab header
    if(lastType == "entry" || lastType == "care" || lastType == "lost" || lastType == "adoptionDemand"){
        QTabBar* bar = findChild<QWidget*>(lastType + "EditPage")->findChild<QTabBar*>();
        QFont font = bar->font();
        font.setPointSize(width() * 0.01);
        bar->setFont(font);
    }

}

void EditPage::SameDestCare(){
    QWidget *careEditPage = findChild<QWidget*>("careEditPage");
    SetField("firstNameCareDestEdit", GetField("firstNameCareEntryEdit", careEditPage), careEditPage);
    SetField("lastNameCareDestEdit", GetField("lastNameCareEntryEdit", careEditPage), careEditPage);
    SetField("phoneCareDestEdit", GetField("phoneCareEntryEdit", careEditPage), careEditPage);
    SetField("emailCareDestEdit", GetField("emailCareEntryEdit", careEditPage), careEditPage);
    SetField("addressCareDestEdit", GetField("addressCareEntryEdit", careEditPage), careEditPage);
    SetField("address2CareDestEdit", GetField("address2CareEntryEdit", careEditPage), careEditPage);
    SetField("postalCodeCareDestEdit", GetField("postalCodeCareEntryEdit", careEditPage), careEditPage);
    SetField("cityCareDestEdit", GetField("cityCareEntryEdit", careEditPage), careEditPage);

}

void EditPage::QuitEdit()
{
    QStackedWidget* stackedWidget = qobject_cast<QStackedWidget*>(parent());
    if(lastType == "entry" || lastType == "care")
        stackedWidget->setCurrentWidget(stackedWidget->findChild<QWidget*>(lastType + "RegistryPage"));

    else
        stackedWidget->setCurrentWidget(stackedWidget->findChild<QWidget*>(lastType + "Page"));

    emit RefreshMainWindow();
}


void EditPage::RemoveCurrent(){
    QMessageBox::StandardButton reply;

    QString message;

    if(lastType == "entry")
        message = "Voulez-vous supprimer cette entrée du registre E/S?";
    else if (lastType == "care")
        message = "Voulez-vous supprimer cette entrée du registre de Garderie ?";
    else if (lastType == "redList")
        message = "Voulez-vous supprimer cette personne de la liste rouge ?";
    else if (lastType == "members")
        message = "Voulez-vous supprimer cette personne du registre des adhérents ?";
    else if (lastType == "lost")
        message = "Voulez-vous supprimer cette entrée des animaux perdus ?";
    else if (lastType == "vet")
        message = "Voulez-vous supprimer ce RDV Vétérinaire ?";
    else if (lastType == "adoptionDemand")
        message = "Voulez-vous supprimer cette demande d'adoption ?";

    reply = QMessageBox::question(nullptr, "Confirmation de suppression", message,
                                              QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes){
        QSqlQuery query;
        if(lastType == "entry"){
            query.prepare("DELETE FROM ES_Registry "
                          "WHERE id_ES = :id "
                          "AND date_prov = :date;");
            query.bindValue(":id", currentNecessary[0]);
            query.bindValue(":date", currentNecessary[1]);
            HandleErrorExec(&query);

            query.prepare("DELETE FROM Destinations "
                          "WHERE id_dog = :id "
                          "AND date_prov = :date;");
            query.bindValue(":id", currentNecessary[0]);
            query.bindValue(":date", currentNecessary[1]);
            HandleErrorExec(&query);
        }

        else if(lastType == "care"){
            query.prepare("DELETE FROM Care_registry "
                          "WHERE id_care = :id "
                          "AND entry_date = :date;");
            query.bindValue(":id", currentNecessary[0]);
            query.bindValue(":date", currentNecessary[1]);
        }

        else if(lastType == "redList"){
            query.prepare("DELETE FROM Red_list "
                       "WHERE id_people = :id;");
            query.bindValue(":id", currentNecessary[0]);
        }

        else if(lastType == "members"){
            query.prepare("DELETE FROM Members "
                          "WHERE id_adhesion = :id "
                          "AND date = :date;");
            query.bindValue(":id", currentNecessary[0]);
            query.bindValue(":date", currentNecessary[1]);
        }

        else if(lastType == "lost"){
            query.prepare("DELETE FROM Lost "
                          "WHERE name = :name "
                          "AND date = :date "
                          "AND id_people = :id;");
            query.bindValue(":name", currentNecessary[0]);
            query.bindValue(":date", currentNecessary[1]);
            query.bindValue(":id", currentNecessary[2]);
        }

        else if(lastType == "vet"){
            query.prepare("DELETE FROM Vet "
                          "WHERE date = :date "
                          "AND id_dog = :id_dog;");
            query.bindValue(":date", currentNecessary[0]);
            query.bindValue(":id_dog", currentNecessary[1]);
        }

        else if(lastType == "adoptionDemand"){
            query.prepare("DELETE FROM Adoption_demand "
                          "WHERE breed = :breed "
                          "AND id_people = :id_people;");
            query.bindValue(":breed", currentNecessary[0]);
            query.bindValue(":id_people", currentNecessary[1]);
        }

        HandleErrorExec(&query);

        QuitEdit();
    }
}

void EditPage::AssignIdPeople(QWidget *currentPage){
    if(currentPage == nullptr)
        return;

    QSqlQuery query;
    for(EditPeopleWidget *editPeopleWidget : currentPage->findChildren<EditPeopleWidget*>()){
        editPeopleWidget->SetCrypto(crypto);

        query.prepare("SELECT id_people "
                      "FROM People "
                      "WHERE last_name = :last_name "
                      "AND first_name = :first_name "
                      "AND address = :address "
                      "AND phone = :phone "
                      "AND email = :email;");


        QString lastName = GetField("lastName" + editPeopleWidget->objectName(), editPeopleWidget);
        QString firstName = GetField("firstName" + editPeopleWidget->objectName(), editPeopleWidget);
        QString address = GetField("address" + editPeopleWidget->objectName(), editPeopleWidget) + "\n" +
                          GetField("address2" + editPeopleWidget->objectName(), editPeopleWidget) + "\n" +
                          GetField("postalCode" + editPeopleWidget->objectName(), editPeopleWidget) + " " + GetField("city" + editPeopleWidget->objectName(), editPeopleWidget);
        QString phone = GetField("phone" + editPeopleWidget->objectName(), editPeopleWidget);
        QString email = GetField("email" + editPeopleWidget->objectName(), editPeopleWidget);

        query.bindValue(":last_name", lastName);
        query.bindValue(":first_name", firstName);
        query.bindValue(":address", crypto->encryptToString(address));
        query.bindValue(":phone", phone);
        query.bindValue(":email", email);

        HandleErrorExec(&query);

        query.next();
        editPeopleWidget->SetOldId(query.value(0).toString());
    }
}

void EditPage::AssignIdDog(QWidget *currentPage) {
    if (currentPage == nullptr)
        return;

    QSqlQuery query;
    for (EditDogWidget *editDogWidget : currentPage->findChildren<EditDogWidget*>()) {
        query.prepare("SELECT id_dog "
                      "FROM Dogs "
                      "WHERE name = :name "
                      "AND sex = :sex "
                      "AND chip = :chip "
                      "AND description = :description "
                      "AND birth = :birth;");

        QString name = GetField("dogName" + editDogWidget->objectName(), editDogWidget);
        QString sex = GetField("sex" + editDogWidget->objectName(), editDogWidget);
        QString chip = GetField("chip" + editDogWidget->objectName(), editDogWidget);
        QString description = GetField("description" + editDogWidget->objectName(), editDogWidget);
        QString birth = GetField("birthDate" + editDogWidget->objectName(), editDogWidget);

        query.bindValue(":name", name);
        query.bindValue(":sex", sex);
        query.bindValue(":chip", chip);
        query.bindValue(":description", description);
        query.bindValue(":birth", birth);

        HandleErrorExec(&query);

        if(query.next()) // If not the case, probably creating a new dog
            editDogWidget->SetOldId(query.value(0).toString());
    }
}
