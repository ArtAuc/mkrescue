#include "editpage.h"


void EditPage::Edit(QString type, QStringList infos){
    lastType = type;
    ClearAllPages();
    currentNecessary.clear();
    dealtIdPeople.clear();

    if((type != "redList" && type != "members") || infos.isEmpty())
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

            QString type_prov = infos[2].split("_|_")[0];
            SetField("entryTypeBox", type_prov, currentPage);

            if(type_prov == "Fourrière"){
                SetField("poundPlaceEdit", infos[2].split("_|_")[1], currentPage);
            }

            else{
                SetField("lastNameAbandonEdit", infos[3], currentPage);
                SetField("firstNameAbandonEdit", infos[4], currentPage);

                QStringList addressList = AddressList(infos[5]);
                SetField("addressAbandonEdit", addressList[0], currentPage);
                SetField("address2AbandonEdit", addressList[1], currentPage);
                SetField("postalCodeAbandonEdit", addressList[2], currentPage);
                SetField("cityAbandonEdit", addressList[3], currentPage);

                SetField("phoneAbandonEdit", infos[6], currentPage);
                SetField("emailAbandonEdit", infos[7], currentPage);
            }

            // Animal
            SetField("dogNameEntryAnimalEdit", infos[10], currentPage);
            SetField("chipEntryAnimalEdit", infos[9], currentPage);
            SetField("sexEntryAnimalEdit", infos[8], currentPage);
            SetField("birthDateEntryAnimalEdit", infos[12], currentPage);
            SetField("descriptionEntryAnimalEdit", infos[11], currentPage);


            // Sortie

            QStringList destinations = infos[13].split("_-_");
            QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");

            // Sort the destinations using date
            std::sort(destinations.begin(), destinations.end(), dateComparator);


            for (int i = 0; i < destinations.size(); i++){
                QString d = destinations[i];

                if (d != ""){
                    destinationsNumber += 1;
                    AddDestPage();
                    QString iString = QString::number(destinationsNumber);

                    QStringList p = d.split("_|_");

                    SetField("destTypeBox" + iString, p[1], destStacked);
                    SetField("destDateEdit" + iString, p[0], destStacked);
                    SetField("lastNameDestEdit" + iString, p[2], destStacked);
                    SetField("firstNameDestEdit" + iString, p[3], destStacked);
                    QStringList addressList = AddressList(p[4]);
                    SetField("addressDestEdit" + iString, addressList[0], destStacked);
                    SetField("address2DestEdit" + iString, addressList[1], destStacked);
                    SetField("postalCodeDestEdit" + iString, addressList[2], destStacked);
                    SetField("cityDestEdit" + iString, addressList[3], destStacked);

                    SetField("phoneDestEdit" + iString, p[5], destStacked);
                    SetField("emailDestEdit" + iString, p[6], destStacked);
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

            SetField("lastNameCareEntryEdit", infos[2], currentPage);
            SetField("firstNameCareEntryEdit", infos[3], currentPage);

            QStringList addressList = AddressList(infos[4]);
            SetField("addressCareEntryEdit", addressList[0], currentPage);
            SetField("address2CareEntryEdit", addressList[1], currentPage);
            SetField("postalCodeCareEntryEdit", addressList[2], currentPage);
            SetField("cityCareEntryEdit", addressList[3], currentPage);

            SetField("phoneCareEntryEdit", infos[5], currentPage);
            SetField("emailCareEntryEdit", infos[6], currentPage);

            // Animal
            SetField("dogNameCareAnimalEdit", infos[7], currentPage);
            SetField("chipCareAnimalEdit", infos[8], currentPage);
            SetField("sexCareAnimalEdit", infos[9], currentPage);
            SetField("birthDateCareAnimalEdit", infos[10], currentPage);
            SetField("descriptionCareAnimalEdit", infos[11], currentPage);


            // Sortie
            SetField("careDestDateEdit", infos[12], currentPage);
            SetField("lastNameCareDestEdit", infos[13], currentPage);
            SetField("firstNameCareDestEdit", infos[14], currentPage);

            addressList = AddressList(infos[15]);

            SetField("addressCareDestEdit", addressList[0], currentPage);
            SetField("address2CareDestEdit", addressList[1], currentPage);
            SetField("postalCodeCareDestEdit", addressList[2], currentPage);
            SetField("cityCareDestEdit", addressList[3], currentPage);

            SetField("phoneCareDestEdit", infos[16], currentPage);
            SetField("emailCareDestEdit", infos[17], currentPage);
        }

    }

    else if (type == "redList" || type == "members"){
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
            SetField("lastNameLostOwnerEdit", infos[8], currentPage);
            SetField("firstNameLostOwnerEdit", infos[9], currentPage);
            SetField("phoneLostOwnerEdit", infos[10], currentPage);
            SetField("emailLostOwnerEdit", infos[11], currentPage);

            QStringList addressList = AddressList(infos[12]);

            SetField("addressLostOwnerEdit", addressList[0], currentPage);
            SetField("address2LostOwnerEdit", addressList[1], currentPage);
            SetField("postalCodeLostOwnerEdit", addressList[2], currentPage);
            SetField("cityLostOwnerEdit", addressList[3], currentPage);
        }
    }

    AssignIdPeople(currentPage);
}



void EditPage::SaveEdit()
{
    QSqlQuery query;
    if(lastType == "entry"){
        QWidget *entryEditPage = findChild<QWidget*>("entryEditPage");
        // Entrée
        QString date_prov = GetField("entryDateEdit", entryEditPage);
        QString type_prov = GetField("entryTypeBox", entryEditPage);
        QString id_people_prov = "-1";
        QString dogName =  GetField("dogNameEntryAnimalEdit", entryEditPage);
        if(GetField("entryTypeBox", entryEditPage) == "Abandon"){
            id_people_prov = CreatePersonIfNeeded(GetField("lastNameAbandonEdit", entryEditPage),
                                    GetField("firstNameAbandonEdit", entryEditPage),
                                    GetField("phoneAbandonEdit", entryEditPage),
                                    GetField("emailAbandonEdit", entryEditPage),
                                    GetField("addressAbandonEdit", entryEditPage) + "\n" +
                                    GetField("address2AbandonEdit", entryEditPage) + "\n" +
                                    GetField("postalCodeAbandonEdit", entryEditPage) + " " +
                                    GetField("cityAbandonEdit", entryEditPage),
                                    entryEditPage->findChild<EditPeopleWidget*>("AbandonEdit")->GetOldId());
        }

        else{ // Fourrière
            type_prov += "_|_" + GetField("poundPlaceEdit", entryEditPage);
        }

        // Animal
        QString id_dog = CreateDogIfNeeded(QStringList({
                                                           dogName,
                                                           GetField("chipEntryAnimalEdit", entryEditPage),
                                                           GetField("sexEntryAnimalEdit", entryEditPage),
                                                           GetField("descriptionEntryAnimalEdit", entryEditPage),
                                                           GetField("birthDateEntryAnimalEdit", entryEditPage)
                                                       }));
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
                    id_peoples.append(CreatePersonIfNeeded(GetField("lastNameDestEdit" + iString, destStacked),
                                                        GetField("firstNameDestEdit" + iString, destStacked),
                                                        GetField("phoneDestEdit" + iString, destStacked),
                                                        GetField("emailDestEdit" + iString, destStacked),
                                                        GetField("addressDestEdit" + iString, destStacked) + "\n" +
                                                        GetField("address2DestEdit" + iString, destStacked) + "\n" +
                                                        GetField("postalCodeDestEdit" + iString, destStacked) + " " +
                                                        GetField("cityDestEdit" + iString, destStacked),
                                                        destStacked->findChild<EditPeopleWidget*>("DestEdit" + iString)->GetOldId()));
                }
            }
        }

        if(death_causes.isEmpty())
            death_causes.append("");


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
            query.bindValue(":type_prov", type_prov);
            query.bindValue(":death_cause", death_causes[0]);
            query.bindValue(":id_ES", currentNecessary[0]);
            query.bindValue(":current_date_prov", currentNecessary[1]);
        }
        else { // Creating
            queryString = "INSERT INTO ES_Registry (id_dog, type_prov, date_prov, id_people_prov, death_cause) "
                          "VALUES (:id_dog, :type_prov, :date_prov, :id_people_prov, :death_cause)";

            query.prepare(queryString);
            query.bindValue(":id_dog", id_dog);
            query.bindValue(":type_prov", type_prov);
            query.bindValue(":date_prov", date_prov);
            query.bindValue(":id_people_prov", id_people_prov);
            query.bindValue(":death_cause", death_causes[0]);
        }

        HandleErrorExec(&query);


        query.prepare("DELETE FROM Destinations WHERE id_dog = :id_dog");
        query.bindValue(":id_dog", id_dog);
        HandleErrorExec(&query);



        for(int i = 0; i < id_peoples.count(); i++){
            query.prepare("INSERT INTO Destinations (id_dog, id_people, date, type) "
                          "VALUES (:id_dog, :id_people, :date, :type)");

            query.bindValue(":id_dog", id_dog);
            query.bindValue(":id_people", id_peoples[i]);
            query.bindValue(":date", dates[i]);
            query.bindValue(":type", types[i]);

            HandleErrorExec(&query);
        }

    }

    else if(lastType == "redList"){
        QWidget *redListEditPage = findChild<QWidget*>("redListEditPage");
        QString id_people = CreatePersonIfNeeded(GetField("lastNameRedListEdit", redListEditPage),
                                GetField("firstNameRedListEdit", redListEditPage),
                                GetField("phoneRedListEdit", redListEditPage),
                                GetField("emailRedListEdit", redListEditPage),
                                GetField("addressRedListEdit", redListEditPage) + "\n" +
                                GetField("address2RedListEdit", redListEditPage) + "\n" +
                                GetField("postalCodeRedListEdit", redListEditPage) + " " +
                                GetField("cityRedListEdit", redListEditPage),
                                redListEditPage->findChild<EditPeopleWidget*>()->GetOldId());
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
        QString id_people_prov = CreatePersonIfNeeded(GetField("lastNameCareEntryEdit", careEditPage),
                                GetField("firstNameCareEntryEdit", careEditPage),
                                GetField("phoneCareEntryEdit", careEditPage),
                                GetField("emailCareEntryEdit", careEditPage),
                                GetField("addressCareEntryEdit", careEditPage) + "\n" +
                                GetField("address2CareEntryEdit", careEditPage) + "\n" +
                                GetField("postalCodeCareEntryEdit", careEditPage) + " " +
                                GetField("cityCareEntryEdit", careEditPage),
                                careEditPage->findChild<EditPeopleWidget*>("CareEntryEdit")->GetOldId());

        // Animal
        QString id_dog = CreateDogIfNeeded(QStringList({
                                                           GetField("dogNameCareAnimalEdit", careEditPage),
                                                           GetField("chipCareAnimalEdit", careEditPage),
                                                           GetField("sexCareAnimalEdit", careEditPage),
                                                           GetField("descriptionCareAnimalEdit", careEditPage),
                                                           GetField("birthDateCareAnimalEdit", careEditPage)
                                                       }));

        // Sortie
        QString exit_date = GetField("careDestDateEdit", careEditPage);

        QString id_people_dest = CreatePersonIfNeeded(GetField("lastNameCareDestEdit", careEditPage),
                                    GetField("firstNameCareDestEdit", careEditPage),
                                    GetField("phoneCareDestEdit", careEditPage),
                                    GetField("emailCareDestEdit", careEditPage),
                                    GetField("addressCareDestEdit", careEditPage) + "\n" +
                                    GetField("address2CareDestEdit", careEditPage) + "\n" +
                                    GetField("postalCodeCareDestEdit", careEditPage) + " " +
                                    GetField("cityCareDestEdit", careEditPage),
                                    careEditPage->findChild<EditPeopleWidget*>("CareDestEdit")->GetOldId());

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
        QString id_people = CreatePersonIfNeeded(GetField("lastNameMembersEdit", membersEditPage),
                                GetField("firstNameMembersEdit", membersEditPage),
                                GetField("phoneMembersEdit", membersEditPage),
                                GetField("emailMembersEdit", membersEditPage),
                                GetField("addressMembersEdit", membersEditPage) + "\n" +
                                GetField("address2MembersEdit", membersEditPage) + "\n" +
                                GetField("postalCodeMembersEdit", membersEditPage) + " " +
                                GetField("cityMembersEdit", membersEditPage),
                                membersEditPage->findChild<EditPeopleWidget*>("MembersEdit")->GetOldId());
        QString type = GetField("typeMembersBox", membersEditPage);
        QString amount = GetField("amountMembersEdit", membersEditPage);
        QString date = GetField("dateMembersEdit", membersEditPage);

        query.prepare("INSERT INTO Members (id_people, date, amount, type) "
                      "VALUES (:id, :date, :amount, :type);");
        query.bindValue(":id", id_people);
        query.bindValue(":date", date);
        query.bindValue(":amount", amount);
        query.bindValue(":type", type);
        HandleErrorExec(&query);

    }

    else if(lastType == "lost"){
        QWidget *lostEditPage = findChild<QWidget*>("lostEditPage");
        QString id_people = CreatePersonIfNeeded(GetField("lastNameLostOwnerEdit", lostEditPage),
                                GetField("firstNameLostOwnerEdit", lostEditPage),
                                GetField("phoneLostOwnerEdit", lostEditPage),
                                GetField("emailLostOwnerEdit", lostEditPage),
                                GetField("addressLostOwnerEdit", lostEditPage) + "\n" +
                                GetField("address2LostOwnerEdit", lostEditPage) + "\n" +
                                GetField("postalCodeLostOwnerEdit", lostEditPage) + " " +
                                GetField("cityLostOwnerEdit", lostEditPage),
                                lostEditPage->findChild<EditPeopleWidget*>("LostOwnerEdit")->GetOldId());
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
                                  "place = :place "
                                  "WHERE name = :name_nec "
                                  "AND date = :date_nec "
                                  "AND id_people = :id_people;";

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
            query.bindValue(":name_nec", currentNecessary[0]);
            query.bindValue(":date_nec", currentNecessary[1]);
            query.bindValue(":id_people", currentNecessary[2]);
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


    QuitEdit();
}
