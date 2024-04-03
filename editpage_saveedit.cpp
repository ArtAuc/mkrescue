#include "editpage.h"


void EditPage::Edit(QString type, QStringList infos){
    lastType = type;
    ClearAllPages();
    currentNecessary.clear();

    if((type != "redList" && type != "members") || infos.isEmpty())
        SwitchPage(type + "EditPage");

    removeButton = findChild<QToolButton*>("removeButton");
    removeButton->setVisible(!infos.isEmpty());

    if(type == "entry"){
        findChild<QTabWidget*>("entryTabWidget")->setCurrentIndex(0);

        if(infos.size() > 14){
            currentNecessary.append(infos[0]); // id_ES
            currentNecessary.append(infos[1]); // date_prov


            // Entrée
            SetField("entryDateEdit", infos[1]);

            QString type_prov = infos[2].split("___")[0];
            SetField("entryTypeBox", type_prov);

            if(type_prov == "Fourrière"){
                SetField("poundPlaceEdit", infos[2].split("___")[1]);
            }

            else{
                SetField("lastNameAbandonEdit", infos[3]);
                SetField("firstNameAbandonEdit", infos[4]);

                QStringList addressList = infos[5].split(infos[5].contains("\\n") ? "\\n" : "\n");
                if(addressList.size() < 3)
                    qDebug() << "Wrong address format";

                else{
                    SetField("addressAbandonEdit", addressList[0]);
                    SetField("address2AbandonEdit", addressList[1]);

                    SetField("cityAbandonEdit", addressList[addressList.size() - 1].split(" ")[1]);
                    SetField("postalCodeAbandonEdit", addressList[addressList.size() - 1].split(" ")[0]);
                }

                SetField("phoneAbandonEdit", infos[6]);
                SetField("emailAbandonEdit", infos[7]);
            }

            // Animal
            SetField("dogNameEntryAnimalEdit", infos[10]);
            SetField("chipEntryAnimalEdit", infos[9]);
            SetField("sexEntryAnimalEdit", infos[8]);
            SetField("birthDateEntryAnimalEdit", infos[12]);
            SetField("descriptionEntryAnimalEdit", infos[11]);


            // Sortie

            QStringList destinations = infos[13].split("_-_");
            QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");

            for (int i = 0; i < destinations.size(); i++){
                QString d = destinations[i];

                if (d != ""){
                    destinationsNumber += 1;
                    AddDestPage();
                    QString iString = QString::number(destinationsNumber);

                    QStringList p = d.split("___");

                    SetField("destTypeBox" + iString, p[1], destStacked);
                    SetField("destDateEdit" + iString, p[0], destStacked);
                    SetField("lastNameDestEdit" + iString, p[2], destStacked);
                    SetField("firstNameDestEdit" + iString, p[3], destStacked);
                    QStringList splitAddress = p[4].split("\n");
                    if(splitAddress.count() == 3){
                        SetField("addressDestEdit" + iString, splitAddress[0], destStacked);
                        SetField("address2DestEdit" + iString, splitAddress[1], destStacked);
                        SetField("postalCodeDestEdit" + iString, splitAddress[2].split(" ")[0], destStacked);
                        SetField("cityDestEdit" + iString, splitAddress[2].split(" ")[1], destStacked);
                    }

                    SetField("phoneDestEdit" + iString, p[5], destStacked);
                    SetField("emailDestEdit" + iString, p[6], destStacked);
                }
            }
        }
    }

    else if (type == "care"){
        findChild<QTabWidget*>("careTabWidget")->setCurrentIndex(0);
        if(infos.size() > 17 && infos[0].toInt() > 0){
            currentNecessary.append(infos[0]); // id_care
            currentNecessary.append(infos[1]); // entry_date

            // Entrée
            SetField("careEntryDateEdit", infos[1]);

            SetField("lastNameCareEntryEdit", infos[2]);
            SetField("firstNameCareEntryEdit", infos[3]);

            QStringList addressList = infos[4].split(infos[4].contains("\\n") ? "\\n" : "\n");
            if(addressList.size() < 3)
                qDebug() << "Wrong address format";

            else{
                SetField("addressCareEntryEdit", addressList[0]);
                SetField("address2CareEntryEdit", addressList[1]);

                SetField("cityCareEntryEdit", addressList[addressList.size() - 1].split(" ")[1]);
                SetField("postalCodeCareEntryEdit", addressList[addressList.size() - 1].split(" ")[0]);
            }

            SetField("phoneCareEntryEdit", infos[5]);
            SetField("emailCareEntryEdit", infos[6]);

            // Animal
            SetField("dogNameCareAnimalEdit", infos[7]);
            SetField("chipCareAnimalEdit", infos[8]);
            SetField("sexCareAnimalEdit", infos[9]);
            SetField("birthDateCareAnimalEdit", infos[10]);
            SetField("descriptionCareAnimalEdit", infos[11]);


            // Sortie
            SetField("careDestDateEdit", infos[12]);
            SetField("lastNameCareDestEdit", infos[13]);
            SetField("firstNameCareDestEdit", infos[14]);

            addressList = infos[15].split(infos[15].contains("\\n") ? "\\n" : "\n");
            if(addressList.size() < 3)
                qDebug() << "Wrong address format";

            else{
                SetField("addressCareDestEdit", addressList[0]);
                SetField("address2CareDestEdit", addressList[1]);

                SetField("cityCareDestEdit", addressList[addressList.size() - 1].split(" ")[1]);
                SetField("postalCodeCareDestEdit", addressList[addressList.size() - 1].split(" ")[0]);
            }

            SetField("phoneCareDestEdit", infos[16]);
            SetField("emailCareDestEdit", infos[17]);
        }

    }

    else if (type == "redList" || type == "members"){
        if(infos.size() > 0){
            currentNecessary = infos;
            RemoveCurrent();
        }
    }

    else if (type == "lost"){
        QWidget *lostPage = findChild<QWidget*>("lostEditPage");
        findChild<QTabWidget*>("lostTabWidget")->setCurrentIndex(0);
        if(infos.size() > 11 && infos[11].toInt() > 0){
            // Animal
            SetField("idLostEdit", infos[0], lostPage);
            SetField("nameLostEdit", infos[1], lostPage);
            SetField("speciesLostEdit", infos[2], lostPage);
            SetField("sexLostEdit", infos[3], lostPage);
            SetField("descriptionLostEdit", infos[4], lostPage);
            SetField("lossDateEdit", infos[5], lostPage);
            SetField("lossPlaceEdit", infos[6], lostPage);
            lostPage->findChild<QCheckBox*>("foundLostBox")->setChecked(infos[10] == "1");

            // Propriétaire
            SetField("lastNameLostOwnerEdit", infos[7], lostPage);
            SetField("firstNameLostOwnerEdit", infos[8], lostPage);
            SetField("phoneLostOwnerEdit", infos[9], lostPage);
        }
    }
}



void EditPage::SaveEdit()
{
    QSqlQuery query;
    if(lastType == "entry"){
        QWidget *entryEditPage = findChild<QWidget*>("entryEditPage");
        // Entrée
        QString date_prov = GetField("entryDateEdit");
        QString type_prov = GetField("entryTypeBox");
        QString id_people_prov = "-1";
        QString dogName =  GetField("dogNameEntryAnimalEdit");
        if(GetField("entryTypeBox") == "Abandon"){
            id_people_prov = CreatePersonIfNeeded(QStringList({GetField("lastNameAbandonEdit"),
                                    GetField("firstNameAbandonEdit"),
                                    GetField("phoneAbandonEdit"),
                                    GetField("emailAbandonEdit"),
                                    GetField("addressAbandonEdit") + "\n" +
                                    GetField("address2AbandonEdit") + "\n" +
                                    GetField("postalCodeAbandonEdit") + " " +
                                    GetField("cityAbandonEdit")}));
        }

        else{ // Fourrière
            type_prov += "___" + GetField("poundPlaceEdit");
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
                    // TODO : Add a Destination for death
                    id_peoples.append("-1");
                }

                else if(type == "Entrée au refuge"){
                    // TODO : Add custom data about the shelter
                    id_peoples.append("-1");
                }

                else{
                    id_peoples.append(CreatePersonIfNeeded(QStringList({GetField("lastNameDestEdit" + iString, destStacked),
                                                        GetField("firstNameDestEdit" + iString, destStacked),
                                                        GetField("phoneDestEdit" + iString, destStacked),
                                                        GetField("emailDestEdit" + iString, destStacked),
                                                        GetField("addressDestEdit" + iString, destStacked) + "\n" +
                                                        GetField("address2DestEdit" + iString, destStacked) + "\n" +
                                                        GetField("postalCodeDestEdit" + iString, destStacked) + " " +
                                                        GetField("cityDestEdit" + iString, destStacked)})));
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

        query.exec();

        query.prepare("DELETE FROM Destinations WHERE id_dog = :id_dog");
        query.bindValue(":id_dog", id_dog);
        query.exec();


        for(int i = 0; i < id_peoples.count(); i++){
            query.prepare("INSERT INTO Destinations (id_dog, id_people, date, type) "
                          "VALUES (:id_dog, :id_people, :date, :type)");

            query.bindValue(":id_dog", id_dog);
            query.bindValue(":id_people", id_peoples[i]);
            query.bindValue(":date", dates[i]);
            query.bindValue(":type", types[i]);

            query.exec();
        }

    }

    else if(lastType == "redList"){
        QWidget *redListEditPage = findChild<QWidget*>("redListEditPage");
        QString id_people = CreatePersonIfNeeded(QStringList({GetField("lastNameRedListEdit", redListEditPage),
                                GetField("firstNameRedListEdit", redListEditPage),
                                GetField("phoneRedListEdit", redListEditPage),
                                GetField("emailRedListEdit", redListEditPage),
                                GetField("addressRedListEdit", redListEditPage) + "\n" +
                                GetField("address2RedListEdit", redListEditPage) + "\n" +
                                GetField("postalCodeRedListEdit", redListEditPage) + " " +
                                GetField("cityRedListEdit", redListEditPage)}));
        QString reason = GetField("reasonRedListEdit", redListEditPage);

        query.prepare("INSERT INTO Red_list (id_people, reason) "
                      "VALUES (:id, :reason);");
        query.bindValue(":id", id_people);
        query.bindValue(":reason", reason);
        query.exec();
    }

    else if(lastType == "care"){
        QWidget *careEditPage = findChild<QWidget*>("careEditPage");
        // Entrée
        QString entry_date = GetField("careEntryDateEdit", careEditPage);
        QString id_people_prov = CreatePersonIfNeeded(QStringList({GetField("lastNameCareEntryEdit", careEditPage),
                                GetField("firstNameCareEntryEdit", careEditPage),
                                GetField("phoneCareEntryEdit", careEditPage),
                                GetField("emailCareEntryEdit", careEditPage),
                                GetField("addressCareEntryEdit", careEditPage) + "\n" +
                                GetField("address2CareEntryEdit", careEditPage) + "\n" +
                                GetField("postalCodeCareEntryEdit", careEditPage) + " " +
                                GetField("cityCareEntryEdit", careEditPage)}));

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

        QString id_people_dest = CreatePersonIfNeeded(QStringList({GetField("lastNameCareDestEdit", careEditPage),
                                    GetField("firstNameCareDestEdit", careEditPage),
                                    GetField("phoneCareDestEdit", careEditPage),
                                    GetField("emailCareDestEdit", careEditPage),
                                    GetField("addressCareDestEdit", careEditPage) + "\n" +
                                    GetField("address2CareDestEdit", careEditPage) + "\n" +
                                    GetField("postalCodeCareDestEdit", careEditPage) + " " +
                                    GetField("cityCareDestEdit", careEditPage)}));

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

        query.exec();
    }

    else if(lastType == "members"){
        QWidget *membersEditPage = findChild<QWidget*>("membersEditPage");
        QString id_people = CreatePersonIfNeeded(QStringList({GetField("lastNameMembersEdit", membersEditPage),
                                GetField("firstNameMembersEdit", membersEditPage),
                                GetField("phoneMembersEdit", membersEditPage),
                                GetField("emailMembersEdit", membersEditPage),
                                GetField("addressMembersEdit", membersEditPage) + "\n" +
                                GetField("address2MembersEdit", membersEditPage) + "\n" +
                                GetField("postalCodeMembersEdit", membersEditPage) + " " +
                                GetField("cityMembersEdit", membersEditPage)}));
        QString type = GetField("typeMembersBox", membersEditPage);
        QString amount = GetField("amountMembersEdit", membersEditPage);
        QString date = GetField("dateMembersEdit", membersEditPage);

        query.prepare("INSERT INTO Members (id_people, date, amount, type) "
                      "VALUES (:id, :date, :amount, :type);");
        query.bindValue(":id", id_people);
        query.bindValue(":date", date);
        query.bindValue(":amount", amount);
        query.bindValue(":type", type);
        query.exec();

    }


    QuitEdit();
}
