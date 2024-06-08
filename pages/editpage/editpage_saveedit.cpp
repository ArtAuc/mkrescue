#include "editpage.h"

void EditPage::FillAnimalWidget(QString nameEnd, QString dogName, QString chip, QString sex, QString birthDate, QString description, QWidget *parent){
    if(parent == nullptr)
        parent = this;

    SetField("dogName" + nameEnd, dogName, parent);
    SetField("chip" + nameEnd, chip, parent);
    SetField("sex" + nameEnd, sex, parent);
    SetField("birthDate" + nameEnd, birthDate, parent);
    SetField("description" + nameEnd, description, parent);
}

void EditPage::FillPeopleWidget(QString nameEnd, QString lastName, QString firstName, QString address, QString phone, QString email, QWidget *parent) {
    if(parent == nullptr)
        parent = this;

    SetField("lastName" + nameEnd, lastName, parent);
    SetField("firstName" + nameEnd, firstName, parent);

    QStringList addressList = AddressList(crypto->decryptToString(address));
    SetField("address" + nameEnd, addressList[0], parent);
    SetField("address2" + nameEnd, addressList[1], parent);
    SetField("postalCode" + nameEnd, addressList[2], parent);
    SetField("city" + nameEnd, addressList[3], parent);

    SetField("phone" + nameEnd, phone, parent);
    SetField("email" + nameEnd, email, parent);
}


void EditPage::Edit(QString type, QStringList infos){
    if(cleaning){
        QMessageBox::warning(nullptr, "Mise à jour des données en cours", "Veuillez attendre la fin de la mise à jour des données avant de réessayer.");
        return;
    }

    lastType = type;
    ClearAllPages();
    currentNecessary.clear();
    dealtIdPeople.clear();
    dealtIdDog.clear();

    if(type != "redList" || infos.isEmpty())
        SwitchPage(type + "EditPage");

    removeButton = findChild<QToolButton*>("removeButton");
    removeButton->setVisible(!infos.isEmpty());

    QWidget *currentPage = nullptr;


    if(type == "entry"){
        findChild<QTabWidget*>("entryTabWidget")->setCurrentIndex(0);
        currentPage = findChild<QWidget*>("entryEditPage");

        if(infos.size() > 14){
            currentNecessary.append(infos[0]); // id_ES
            currentNecessary.append(infos[1]); // date_prov


            // Entrée
            SetField("entryDateEdit", infos[1], currentPage);

            QString type_prov = crypto->decryptToString(infos[2]).split("_|_")[0];
            if(type_prov == "Fourrière" || type_prov == "Abandon")
                SetField("entryTypeBox", type_prov, currentPage);

            else
                SetField("entryTypeBox", "Autre", currentPage);

            if(type_prov == "Fourrière" || crypto->decryptToString(infos[2]).split("_|_").size() > 1){
                SetField("otherTypeEdit", crypto->decryptToString(infos[2]).split("_|_")[1], currentPage);
            }


            else{
                FillPeopleWidget("AbandonEdit", infos[3], infos[4], infos[5], infos[6], infos[7], currentPage);
            }

            // Animal
            FillAnimalWidget("EntryAnimalEdit", infos[10], infos[9], infos[8], infos[12], infos[11], currentPage);

            // Sortie

            QStringList destinations = infos[13].split("_-_");
            QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");

            // Sort the destinations using date
            std::sort(destinations.begin(), destinations.end(), DateComparator);


            for (int i = 0; i < destinations.size(); i++){
                QString d = destinations[i];

                if (d != ""){
                    destinationsNumber += 1;
                    AddDestPage();
                    QString iString = QString::number(destinationsNumber);

                    QStringList p = d.split("_|_");

                    SetField("destTypeBox" + iString, p[1], destStacked);
                    SetField("destDateEdit" + iString, p[0], destStacked);
                    FillPeopleWidget("DestEdit" + iString, p[2], p[3], p[4], p[5], p[6], destStacked);
                    SetField("deathCauseEdit" + iString, infos[14], destStacked);
                }
            }
        }
    }

    else if (type == "care"){
        currentPage = findChild<QWidget*>("careEditPage");
        findChild<QTabWidget*>("careTabWidget")->setCurrentIndex(0);
        if(infos.size() > 17 && infos[0].toInt() > 0){
            currentNecessary.append(infos[0]); // id_care
            currentNecessary.append(infos[1]); // entry_date

            // Entrée
            SetField("careEntryDateEdit", infos[1], currentPage);
            FillPeopleWidget("CareEntryEdit", infos[2], infos[3], infos[4], infos[5], infos[6], currentPage);

            // Animal
            FillAnimalWidget("CareAnimalEdit", infos[7], infos[8], infos[9], infos[10], infos[11], currentPage);


            // Sortie
            SetField("careDestDateEdit", infos[12], currentPage);
            FillPeopleWidget("CareDestEdit", infos[13], infos[14], infos[15], infos[16], infos[17], currentPage);
        }

    }

    else if (type == "members"){
        currentPage = findChild<QWidget*>("membersEditPage");
        if(infos.size() > 8 && infos[0].toInt() > 0){
            currentNecessary.append(infos[0]); // id_adhesion
            currentNecessary.append(infos[1]); // date

            SetField("dateMembersEdit", infos[1], currentPage);

            FillPeopleWidget("MembersEdit", infos[2], infos[3], infos[4], infos[5], infos[6], currentPage);
            SetField("typeMembersBox", infos[7], currentPage);
            SetField("amountMembersEdit", infos[8], currentPage);
        }
    }


    else if (type == "redList"){
        if(infos.size() > 0){
            currentNecessary = infos;
            RemoveCurrent();
        }
    }


    else if (type == "lost"){
        currentPage = findChild<QWidget*>("lostEditPage");
        findChild<QTabWidget*>("lostTabWidget")->setCurrentIndex(0);
        if(infos.size() > 13){
            currentNecessary.append(infos[1]); // name
            currentNecessary.append(infos[5]); // date
            currentNecessary.append(infos[13]); // id_people

            // Animal
            SetField("idLostEdit", infos[0], currentPage);
            SetField("nameLostEdit", infos[1], currentPage);
            SetField("speciesLostEdit", infos[2], currentPage);
            SetField("sexLostEdit", infos[3], currentPage);
            SetField("descriptionLostEdit", infos[4], currentPage);
            SetField("lossDateEdit", infos[5], currentPage);
            SetField("lossPlaceEdit", infos[6], currentPage);
            QCheckBox *foundLostBox = currentPage->findChild<QCheckBox*>("foundLostBox");
            foundLostBox->setChecked(infos[7] == "1");
            foundLostBox->setText(QString((infos[7] == "1") ? "" : "Non ") + "Retrouvé");

            // Propriétaire
            FillPeopleWidget("LostOwnerEdit", infos[8], infos[9], infos[12], infos[10], infos[11], currentPage);
        }
    }

    else if (type == "vet"){
        groupedVetIds.clear();
        currentPage = findChild<QWidget*>("vetEditPage");
        findChild<QPushButton*>("groupedPdfButton")->hide();

        disconnect(currentPage->findChild<EditDogWidget*>(), nullptr, nullptr, nullptr);

        for(QWidget *c : currentPage->findChildren<QWidget*>()){
            QString name = c->objectName();
            if (name.contains("VetAnimalEdit") && !name.startsWith("reason"))
                c->show();
        }

        findChild<QPushButton*>("groupedVetButton")->show();

        if(infos.size() > 6){
            // Grouped or not ?
            QSqlQuery query;
            query.prepare("SELECT COUNT(*) "
                          "FROM Vet "
                          "WHERE date = :date "
                          "AND reason = :reason");

            query.bindValue(":date", infos[0]);
            query.bindValue(":reason", infos[6]);
            HandleErrorExec(&query);

            query.next();

            if(query.value(0).toInt() > 1){
                currentNecessary = {infos[0], infos[6]};
                GroupedVet();
                query.prepare("SELECT id_dog "
                              "FROM Vet "
                              "WHERE date = :date "
                              "AND reason = :reason");

                query.bindValue(":date", infos[0]);
                query.bindValue(":reason", infos[6]);

                HandleErrorExec(&query);

                while(query.next()){
                    SetField("dateVetAnimalEdit", infos[0], currentPage);
                    // Reason
                    if(infos[6] == "Vaccin" || infos[6] == "Stérilisation")
                        SetField("reasonVetAnimalBox", infos[6], currentPage);
                    else{
                        SetField("reasonVetAnimalBox", "Autre", currentPage);
                        SetField("reasonVetAnimalEdit", infos[6], currentPage);
                    }

                    AddVetLabel(query.value(0).toString());
                }
            }

            else{
                currentNecessary.append(infos[0]); // date
                currentNecessary.append(infos[6]); // reason
                currentNecessary.append(infos[7]); // id_dog

                SetField("dateVetAnimalEdit", infos[0], currentPage);
                // Reason
                if(infos[6] == "Vaccin" || infos[6] == "Stérilisation")
                    SetField("reasonVetAnimalBox", infos[6], currentPage);
                else{
                    SetField("reasonVetAnimalBox", "Autre", currentPage);
                    SetField("reasonVetAnimalEdit", infos[6], currentPage);
                }

                FillAnimalWidget("VetAnimalEdit", infos[1], infos[2], infos[3], infos[4], infos[5], currentPage);
            }
        }
    }

    else if (type == "adoptionDemand"){
        currentPage = findChild<QWidget*>("adoptionDemandEditPage");
        findChild<QTabWidget*>("adoptionDemandTabWidget")->setCurrentIndex(0);
        if(infos.size() > 10){
            currentNecessary.append(infos[7]); // breed
            currentNecessary.append(infos[10]); // id_people

            // Chien
            SetField("sexAdoptionDemandEdit", infos[5], currentPage);
            SetField("ageAdoptionDemandEdit", infos[6], currentPage);
            SetField("breedAdoptionDemandEdit", infos[7], currentPage);
            SetField("infosAdoptionDemandEdit", infos[9], currentPage);
            QCheckBox *satisfiedAdoptionDemandBox = currentPage->findChild<QCheckBox*>("satisfiedAdoptionDemandBox");
            satisfiedAdoptionDemandBox->setChecked(infos[8] == "1");
            satisfiedAdoptionDemandBox->setText(QString((infos[8] == "1") ? "" : "Non ") + "Satisfaite");

            // Personne
            FillPeopleWidget("AdoptionDemandEdit", infos[0], infos[1], infos[2], infos[3], infos[4], currentPage);
        }
    }

    if(infos.size() > 0){
        AssignIdPeople(currentPage);
        AssignIdDog(currentPage);
    }
}



void EditPage::SaveEdit()
{
    if(cleaning){
        QMessageBox::warning(nullptr, "Mise à jour des données en cours", "Veuillez attendre la fin de la mise à jour des données avant de réessayer.");
        return;
    }

    QSqlQuery query;
    if(lastType == "entry"){
        QWidget *entryEditPage = findChild<QWidget*>("entryEditPage");
        // Entrée
        QString date_prov = GetField("entryDateEdit", entryEditPage);
        QString type_prov = GetField("entryTypeBox", entryEditPage);
        QString id_people_prov = "-1";
        if(type_prov == "Abandon")
            id_people_prov = CreatePersonIfNeeded("AbandonEdit", entryEditPage);

        else if (type_prov == "Fourrière"){
            type_prov += "_|_" + GetField("otherTypeEdit", entryEditPage);
        }

        else // Autre
            type_prov = "_|_" + GetField("otherTypeEdit", entryEditPage);


        // Animal
        QString id_dog = CreateDogIfNeeded("EntryAnimalEdit", entryEditPage);
        if(id_dog == "-2")
            return;
        // Sortie
        QStringList death_causes, id_peoples, dates, types;

        QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
        for(int i = 1; i < destStacked->count() + 1; i++){
            QString iString = QString::number(i);
            QString type = GetField("destTypeBox" + iString, destStacked);
            if(type != ""){
                types.append(type);
                dates.append(GetField("destDateEdit" + iString, destStacked));

                if(type == "Mort"){
                    death_causes.append(GetField("deathCauseEdit" + iString, destStacked));
                    id_peoples.append("-1");
                }

                else if(type == "Entrée au refuge"){
                    id_peoples.append("-2");
                }

                else{
                    id_peoples.append(CreatePersonIfNeeded("DestEdit" + iString, destStacked));
                }
            }
        }

        if(death_causes.isEmpty())
            death_causes.append("");

        if(id_dog != entryEditPage->findChild<EditDogWidget*>("EntryAnimalEdit")->GetOldId()){// Verify that there is not an entry with this dog the same day
            query.prepare("SELECT * "
                          "FROM ES_registry "
                          "WHERE id_dog = :id_dog "
                          "AND date_prov = :date_prov");
            query.bindValue(":id_dog", id_dog);
            query.bindValue(":date_prov", date_prov);
            HandleErrorExec(&query);
            if(query.next()){
                QMessageBox::critical(nullptr, "Erreur", "Impossible de créer deux entrées avec le même chien arrivant le même jour");
                return;
            }
        }


        QString queryString;
        if (!currentNecessary.isEmpty()) { // Modifying
            queryString = "UPDATE ES_Registry "
                          "SET id_dog = :id_dog, "
                          "date_prov = :date_prov, "
                          "id_people_prov = :id_people_prov, "
                          "type_prov = :type_prov, "
                          "death_cause = :death_cause "
                          "WHERE id_ES = :id_ES "
                          "AND date_prov = :current_date_prov";

            query.prepare(queryString);
            query.bindValue(":id_dog", id_dog);
            query.bindValue(":date_prov", date_prov);
            query.bindValue(":id_people_prov", id_people_prov);
            query.bindValue(":type_prov", crypto->encryptToString(type_prov));
            query.bindValue(":death_cause", death_causes[0]);
            query.bindValue(":id_ES", currentNecessary[0]);
            query.bindValue(":current_date_prov", currentNecessary[1]);
        }
        else { // Creating
            queryString = "INSERT INTO ES_Registry (id_dog, type_prov, date_prov, id_people_prov, death_cause) "
                          "VALUES (:id_dog, :type_prov, :date_prov, :id_people_prov, :death_cause)";

            query.prepare(queryString);
            query.bindValue(":id_dog", id_dog);
            query.bindValue(":type_prov", crypto->encryptToString(type_prov));
            query.bindValue(":date_prov", date_prov);
            query.bindValue(":id_people_prov", id_people_prov);
            query.bindValue(":death_cause", death_causes[0]);
        }

        HandleErrorExec(&query);

        if(currentNecessary.size() > 1){
            query.prepare("DELETE FROM Destinations WHERE id_dog = :id_dog AND date_prov = :date");
            query.bindValue(":id_dog", entryEditPage->findChild<EditDogWidget*>("EntryAnimalEdit")->GetOldId());
            query.bindValue(":date", currentNecessary[1]);
            HandleErrorExec(&query);
        }

        for(int i = 0; i < id_peoples.count(); i++){
            query.prepare("INSERT INTO Destinations (id_dog, id_people, date, type, date_prov) "
                          "VALUES (:id_dog, :id_people, :date, :type, :date_prov)");

            query.bindValue(":id_dog", id_dog);
            query.bindValue(":id_people", id_peoples[i]);
            query.bindValue(":date", dates[i]);
            query.bindValue(":type", types[i]);
            query.bindValue(":date_prov", date_prov);

            HandleErrorExec(&query);
        }

    }

    else if(lastType == "redList"){
        QWidget *redListEditPage = findChild<QWidget*>("redListEditPage");
        QString id_people = CreatePersonIfNeeded("RedListEdit", redListEditPage);
        QString reason = GetField("reasonRedListEdit", redListEditPage);

        query.prepare("INSERT INTO Red_list (id_people, reason) "
                      "VALUES (:id, :reason);");
        query.bindValue(":id", id_people);
        query.bindValue(":reason", reason);
        HandleErrorExec(&query);

    }

    else if(lastType == "care"){
        QWidget *careEditPage = findChild<QWidget*>("careEditPage");
        // Entrée
        QString entry_date = GetField("careEntryDateEdit", careEditPage);
        QString id_people_prov = CreatePersonIfNeeded("CareEntryEdit", careEditPage);

        // Animal
        QString id_dog = CreateDogIfNeeded("CareAnimalEdit", careEditPage);
        if(id_dog == "-2")
            return;

        // Sortie
        QString exit_date = GetField("careDestDateEdit", careEditPage);

        QString id_people_dest = CreatePersonIfNeeded("CareDestEdit", careEditPage);

        QString queryString;
        if (!currentNecessary.isEmpty()) { // Modifying
            queryString = "UPDATE Care_registry "
                          "SET id_dog = :id_dog, "
                          "entry_date = :entry_date, "
                          "id_people_prov = :id_people_prov, "
                          "exit_date = :exit_date, "
                          "id_people_dest = :id_people_dest "
                          "WHERE id_care = :id_care "
                          "AND entry_date = :current_entry_date";

            query.prepare(queryString);
            query.bindValue(":id_dog", id_dog);
            query.bindValue(":entry_date", entry_date);
            query.bindValue(":id_people_prov", id_people_prov);
            query.bindValue(":exit_date", exit_date);
            query.bindValue(":id_people_dest", id_people_dest);
            query.bindValue(":id_care", currentNecessary[0]);
            query.bindValue(":current_entry_date", currentNecessary[1]);
        }
        else { // Creating
            query.prepare("INSERT INTO Care_registry (id_dog, entry_date, id_people_prov, exit_date, id_people_dest) "
                          "VALUES (:id_dog, :entry_date, :id_people_prov, :exit_date, :id_people_dest)");

            query.bindValue(":id_dog", id_dog);
            query.bindValue(":entry_date", entry_date);
            query.bindValue(":id_people_prov", id_people_prov);
            query.bindValue(":exit_date", exit_date);
            query.bindValue(":id_people_dest", id_people_dest);
        }

        HandleErrorExec(&query);
    }

    else if(lastType == "members"){
        QWidget *membersEditPage = findChild<QWidget*>("membersEditPage");
        QString id_people = CreatePersonIfNeeded("MembersEdit", membersEditPage);
        QString type = GetField("typeMembersBox", membersEditPage);
        QString amount = GetField("amountMembersEdit", membersEditPage);
        QString date = GetField("dateMembersEdit", membersEditPage);


        if (!currentNecessary.isEmpty()) { // Modifying
            QString queryString;
            queryString = "UPDATE Members "
                          "SET date = :date, "
                          "amount = :amount, "
                          "type = :type, "
                          "id_people = :id_people "
                          "WHERE id_adhesion = :id_adhesion "
                          "AND date = :current_date";

            query.prepare(queryString);
            query.bindValue(":date", date);
            query.bindValue(":amount", amount);
            query.bindValue(":type", type);
            query.bindValue(":id_people", id_people);
            query.bindValue(":id_adhesion", currentNecessary[0]);
            query.bindValue(":current_date", currentNecessary[1]);
        }
        else { // Creating
            query.prepare("INSERT INTO Members (id_people, date, amount, type) "
                          "VALUES (:id_people, :date, :amount, :type)");

            query.bindValue(":id_people", id_people);
            query.bindValue(":date", date);
            query.bindValue(":amount", amount);
            query.bindValue(":type", type);
        }

        HandleErrorExec(&query);

    }

    else if(lastType == "lost"){
        QWidget *lostEditPage = findChild<QWidget*>("lostEditPage");
        QString id_people = CreatePersonIfNeeded("LostOwnerEdit", lostEditPage);
        QString species = GetField("speciesLostEdit", lostEditPage);
        QString name = GetField("nameLostEdit", lostEditPage);
        QString found = lostEditPage->findChild<QCheckBox*>("foundLostBox")->isChecked() ? "1" : "0";
        QString description = GetField("descriptionLostEdit", lostEditPage);
        QString identification = GetField("idLostEdit", lostEditPage);
        QString sex = GetField("sexLostEdit", lostEditPage);
        QString date = GetField("lossDateEdit", lostEditPage);
        QString place = GetField("lossPlaceEdit", lostEditPage);

        if (!currentNecessary.isEmpty()) { // Modifying
            QString queryString = "UPDATE Lost "
                                  "SET species = :species, "
                                  "name = :name, "
                                  "found = :found, "
                                  "description = :description, "
                                  "identification = :identification, "
                                  "sex = :sex, "
                                  "date = :date, "
                                  "place = :place, "
                                  "id_people = :id_people "
                                  "WHERE name = :name_nec "
                                  "AND date = :date_nec "
                                  "AND id_people = :id_nec;";

            QSqlQuery query;
            query.prepare(queryString);
            query.bindValue(":species", species);
            query.bindValue(":name", name);
            query.bindValue(":found", found);
            query.bindValue(":description", description);
            query.bindValue(":identification", identification);
            query.bindValue(":sex", sex);
            query.bindValue(":date", date);
            query.bindValue(":place", place);
            query.bindValue(":id_people", id_people);
            query.bindValue(":name_nec", currentNecessary[0]);
            query.bindValue(":date_nec", currentNecessary[1]);
            query.bindValue(":id_nec", currentNecessary[2]);
            HandleErrorExec(&query);
        }
        else { // Creating
            QSqlQuery query;
            query.prepare("INSERT INTO Lost (id_people, species, name, found, description, identification, sex, date, place) "
                          "VALUES (:id_people, :species, :name, :found, :description, :identification, :sex, :date, :place)");

            query.bindValue(":id_people", id_people);
            query.bindValue(":species", species);
            query.bindValue(":name", name);
            query.bindValue(":found", found);
            query.bindValue(":description", description);
            query.bindValue(":identification", identification);
            query.bindValue(":sex", sex);
            query.bindValue(":date", date);
            query.bindValue(":place", place);
            HandleErrorExec(&query);
        }
    }

    else if(lastType == "vet"){
        QWidget *vetEditPage = findChild<QWidget*>("vetEditPage");

        QString date = vetEditPage->findChild<CustomDateTimeEdit*>("dateVetAnimalEdit")->GetDateTime().toString("yyyy-MM-ddTHH:mm");
        QString reason = vetEditPage->findChild<QComboBox*>("reasonVetAnimalBox")->currentText();
        if(reason == "Autre")
            reason = vetEditPage->findChild<QLineEdit*>("reasonVetAnimalEdit")->text();

        if(groupedVetIds.isEmpty()){
            QString id_dog = CreateDogIfNeeded("VetAnimalEdit", vetEditPage);
            if(id_dog == "-2")
                return;

            if (!currentNecessary.isEmpty()) { // Modifying
                QString queryString = "UPDATE Vet "
                                      "SET id_dog = :id, "
                                      "date = :date, "
                                      "reason = :reason "
                                      "WHERE date = :date_nec "
                                      "AND id_dog = :id_nec;";

                QSqlQuery query;
                query.prepare(queryString);
                query.bindValue(":id", id_dog);
                query.bindValue(":date", date);
                query.bindValue(":reason", reason);
                query.bindValue(":date_nec", currentNecessary[0]);
                query.bindValue(":id_nec", currentNecessary[2]);
                HandleErrorExec(&query);
            }
            else { // Creating
                QSqlQuery query;
                query.prepare("INSERT INTO Vet (id_dog, date, reason) "
                              "VALUES (:id_dog, :date, :reason)");

                query.bindValue(":id_dog", id_dog);
                query.bindValue(":date", date);
                query.bindValue(":reason", reason);
                HandleErrorExec(&query);
            }
        }

        else{ // Grouped vet
            if(!currentNecessary.isEmpty()){
                QSqlQuery query;
                query.prepare("DELETE FROM Vet "
                              "WHERE date = :date_nec "
                              "AND reason = :reason_nec");
                query.bindValue(":date_nec", currentNecessary[0]);
                query.bindValue(":reason_nec", currentNecessary[1]);

                HandleErrorExec(&query);
            }

            for(QString id_dog : groupedVetIds){
                query.prepare("INSERT INTO Vet (id_dog, date, reason) "
                              "VALUES (:id_dog, :date, :reason)");

                query.bindValue(":id_dog", id_dog);
                query.bindValue(":date", date);
                query.bindValue(":reason", reason);
                HandleErrorExec(&query);
            }
        }
    }

    else if(lastType == "adoptionDemand"){
        QWidget *adoptionDemandEditPage = findChild<QWidget*>("adoptionDemandEditPage");
        QString id_people = CreatePersonIfNeeded("AdoptionDemandEdit", adoptionDemandEditPage);
        QString age = GetField("ageAdoptionDemandEdit", adoptionDemandEditPage);
        QString satisfied = adoptionDemandEditPage->findChild<QCheckBox*>("satisfiedAdoptionDemandBox")->isChecked() ? "1" : "0";
        QString breed = GetField("breedAdoptionDemandEdit", adoptionDemandEditPage);
        QString sex = GetField("sexAdoptionDemandEdit", adoptionDemandEditPage);
        QString infos = GetField("infosAdoptionDemandEdit", adoptionDemandEditPage);

        if (!currentNecessary.isEmpty()) { // Modifying
            QString queryString = "UPDATE Adoption_demand "
                                  "SET sex = :sex, "
                                  "age = :age, "
                                  "breed = :breed, "
                                  "infos = :infos, "
                                  "id_people = :id_people, "
                                  "satisfied = :satisfied "
                                  "WHERE breed = :breed_nec "
                                  "AND id_people = :id_nec;";

            QSqlQuery query;
            query.prepare(queryString);
            query.bindValue(":sex", sex);
            query.bindValue(":age", age);
            query.bindValue(":breed", breed);
            query.bindValue(":infos", infos);
            query.bindValue(":satisfied", satisfied);
            query.bindValue(":id_people", id_people);
            query.bindValue(":breed_nec", currentNecessary[0]);
            query.bindValue(":id_nec", currentNecessary[1]);
            HandleErrorExec(&query);
        }
        else { // Creating
            QSqlQuery query;
            query.prepare("INSERT INTO Adoption_demand (id_people, sex, breed, age, satisfied, infos) "
                          "VALUES (:id_people, :sex, :breed, :age, :satisfied, :infos)");

            query.bindValue(":id_people", id_people);
            query.bindValue(":sex", sex);
            query.bindValue(":breed", breed);
            query.bindValue(":age", age);
            query.bindValue(":satisfied", satisfied);
            query.bindValue(":infos", infos);
            HandleErrorExec(&query);
        }
    }

    QuitEdit();
}
