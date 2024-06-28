#include "database.h"

QSqlQuery Database::GetDogs(QString type, QString search){
    search = RemoveAccents(search);
    QString queryString = "SELECT Dogs.chip, Dogs.name, Dogs.sex, Dogs.birth, Dogs.description, (ES_Registry.date_prov || '_|_' || ES_Registry.type_prov), Dogs.id_dog "
                          "FROM Dogs "
                          "JOIN ES_Registry ON ES_Registry.id_dog = Dogs.id_dog "
                          "JOIN ( "
                          "    SELECT id_dog, MAX(date_prov) AS max_date_prov "
                          "    FROM ES_Registry "
                          "    GROUP BY id_dog "
                          ") AS MaxProv ON ES_Registry.id_dog = MaxProv.id_dog AND ES_Registry.date_prov = MaxProv.max_date_prov "
                          "WHERE (" + noaccentbegin + "Dogs.name" + noaccentend + " LIKE :search OR chip LIKE :search OR " + noaccentbegin + "description" + noaccentend + " LIKE :search "
                          "OR " + noaccentbegin + "Dogs.sex" + noaccentend + " = :exact OR Dogs.birth = :exact OR ES_Registry.date_prov = :exact OR " + noaccentbegin + "ES_Registry.type_prov" + noaccentend + " = :exact_encr) "
                          "AND Dogs.id_dog NOT IN ( "
                          "    SELECT LastestDest.id_dog "
                          "    FROM ( "
                          "        SELECT id_dog, MAX(date_prov) AS max_date_prov "
                          "        FROM ES_Registry "
                          "        GROUP BY id_dog "
                          "    ) AS LastestProv "
                          "    LEFT JOIN ( "
                          "        SELECT type, date_prov, id_dog, MAX(date) AS max_date "
                          "        FROM Destinations "
                          "        GROUP BY date_prov, id_dog "
                          "    ) AS LastestDest ON (LastestDest.id_dog = LastestProv.id_dog AND LastestDest.date_prov = LastestProv.max_date_prov) "
                          "    WHERE ( "
                          "        LastestDest.type = 'Adoption' "
                          "        OR LastestDest.type = 'Propriétaire' "
                          "        OR LastestDest.type = 'Mort' "
                          "        OR LastestDest.type LIKE 'Famille d_accueil' "
                          "    ) "
                          ")";

    if(type.contains("out"))
        queryString +=
                " UNION "
                ""
                "SELECT Dogs.chip, Dogs.name, Dogs.sex, MaxDest.max_date || '_|_' || MaxDest.type, Dogs.description, (ES_Registry.date_prov || '_|_' || ES_Registry.type_prov), Dogs.id_dog "
                "FROM Dogs "
                "JOIN ES_Registry ON ES_Registry.id_dog = Dogs.id_dog "
                "JOIN ( "
                "    SELECT id_dog, MAX(date_prov) AS max_date_prov "
                "    FROM ES_Registry "
                "    GROUP BY id_dog "
                ") AS MaxProv ON ES_Registry.id_dog = MaxProv.id_dog AND ES_Registry.date_prov = MaxProv.max_date_prov "
                "JOIN ( "
                "    SELECT id_dog, MAX(date_prov) AS max_date, type "
                "    FROM Destinations "
                "    GROUP BY id_dog, type "
                ") AS MaxDest ON Dogs.id_dog = MaxDest.id_dog "
                "WHERE ( "
                "    Dogs.name LIKE '%' "
                "    OR chip LIKE '%' "
                "    OR description LIKE '%' "
                "    OR Dogs.sex LIKE '%' "
                "    OR Dogs.birth LIKE '%' "
                "    OR Dogs.description LIKE '%' "
                "    OR ES_Registry.date_prov LIKE '%' "
                "    OR ES_Registry.type_prov LIKE '%' "
                ") "
                "AND Dogs.id_dog IN ( "
                "    SELECT LastestDest.id_dog "
                "    FROM ( "
                "        SELECT id_dog, MAX(date_prov) AS max_date_prov "
                "        FROM ES_Registry "
                "        GROUP BY id_dog "
                "    ) AS LastestProv "
                "    LEFT JOIN ( "
                "        SELECT type, date_prov, id_dog, MAX(date) AS max_date "
                "        FROM Destinations "
                "        GROUP BY date_prov, id_dog "
                "    ) AS LastestDest ON (LastestDest.id_dog = LastestProv.id_dog AND LastestDest.date_prov = LastestProv.max_date_prov) "
                "    WHERE ( "
                "        LastestDest.type = 'Adoption' "
                "        OR LastestDest.type = 'Propriétaire' "
                "        OR LastestDest.type = 'Mort' "
                "        OR LastestDest.type LIKE 'Famille d_accueil' "
                "    ) "
                ") "
                "AND (" + noaccentbegin + "Dogs.name" + noaccentend + " LIKE :search OR chip LIKE :search OR " + noaccentbegin + "description" + noaccentend + " LIKE :search "
                "OR Dogs.sex = :exact OR Dogs.birth = :exact OR Dogs.description = :exact OR ES_Registry.date_prov = :exact OR ES_Registry.type_prov = :exact_encr) "
                "AND Dogs.id_dog NOT IN (SELECT id_dog FROM Care_registry) ";

    if(type.contains("care")){
        queryString +=
                " UNION "
                ""
                "SELECT Dogs.chip, Dogs.name, Dogs.sex, Care_registry.entry_date, Dogs.description, People.last_name || ' ' || People.first_name, Dogs.id_dog "
                "FROM Dogs "
                "LEFT JOIN ( "
                "    SELECT id_dog, MAX(entry_date) AS max_date "
                "    FROM Care_registry "
                "    GROUP BY id_dog "
                ") AS LastCare ON Dogs.id_dog = LastCare.id_dog "
                "LEFT JOIN Care_registry ON Care_registry.id_dog = Care_registry.id_dog AND Care_registry.entry_date = LastCare.max_date "
                "JOIN People ON Care_registry.id_people_prov = People.id_people "
                "WHERE (" + noaccentbegin + "Dogs.name" + noaccentend + " LIKE :search OR chip LIKE :search OR " + noaccentbegin + "description" + noaccentend + " LIKE :search "
                "OR " + noaccentbegin + "Dogs.sex" + noaccentend + " = :exact OR " + noaccentbegin + "Dogs.description" + noaccentend + " = :exact OR Dogs.birth = :exact OR LastCare.max_date = :exact OR " + noaccentbegin + "People.last_name" + noaccentend + " = :exact)"
                "AND (Dogs.id_dog IN ( " // Make sure the dog is out
                "    SELECT LastestDest.id_dog "
                "    FROM ( "
                "        SELECT id_dog, MAX(date_prov) AS max_date_prov "
                "        FROM ES_Registry "
                "        GROUP BY id_dog "
                "    ) AS LastestProv "
                "    LEFT JOIN ( "
                "        SELECT type, date_prov, id_dog, MAX(date) AS max_date "
                "        FROM Destinations "
                "        GROUP BY date_prov, id_dog "
                "    ) AS LastestDest ON (LastestDest.id_dog = LastestProv.id_dog AND LastestDest.date_prov = LastestProv.max_date_prov) "
                "    WHERE ( "
                "        LastestDest.type = 'Adoption' "
                "        OR LastestDest.type = 'Propriétaire' "
                "        OR LastestDest.type = 'Mort' "
                "        OR LastestDest.type LIKE 'Famille d_accueil' "
                "    ) "
                ") OR Dogs.id_dog NOT IN (SELECT id_dog FROM ES_Registry)) ";
    }



    queryString = "SELECT * FROM (" + queryString + ") AS Results "
                    "GROUP BY Results.chip "
                    "ORDER BY MAX(Results.id_dog) DESC;";

    QSqlQuery query;

    query.prepare(queryString);
    query.bindValue(":search", search + "%");
    query.bindValue(":exact", search);
    query.bindValue(":exact_encr", crypto->encryptToString(search));
    HandleErrorExec(&query);


    return query;
}

QSqlQuery Database::GetEntryRegistry(QString year, QString search) {
    search = RemoveAccents(search);
    QSqlQuery query;
    QString queryString =
        "SELECT ES_registry.id_ES, "
        "       ES_registry.date_prov, "
        "       ES_registry.type_prov, "
        "       People_prov.last_name, "
        "       People_prov.first_name, "
        "       People_prov.address, "
        "       People_prov.phone, "
        "       People_prov.email, "
        "       Dogs.sex, "
        "       Dogs.chip, "
        "       Dogs.name, "
        "       Dogs.description, "
        "       Dogs.birth, "
        "       GROUP_CONCAT(Destinations.date || '_|_' || IFNULL(Destinations.type, '') || '_|_' || IFNULL(People_dest.last_name, '') || '_|_' || IFNULL(People_dest.first_name, '') || '_|_' || IFNULL(People_dest.address, '') || '_|_' || IFNULL(People_dest.phone, '') || '_|_' || IFNULL(People_dest.email, ''), '_-_'), "
        "       ES_registry.death_cause "
        "FROM ES_registry "
        "JOIN People AS People_prov ON ES_registry.id_people_prov = People_prov.id_people "
        "JOIN Dogs ON ES_registry.id_dog = Dogs.id_dog "
        "LEFT JOIN Destinations ON (Dogs.id_dog = Destinations.id_dog AND ES_Registry.date_prov = Destinations.date_prov) "
        "LEFT JOIN People AS People_dest ON Destinations.id_people = People_dest.id_people "
        "WHERE strftime('%Y', ES_registry.date_prov) = :year "
        "AND (" + noaccentbegin + "People_prov.last_name" + noaccentend + " LIKE :search OR " + noaccentbegin + "People_prov.first_name" + noaccentend + " LIKE :search OR REPLACE(People_prov.phone, ' ', '') LIKE :search "
        "OR " + noaccentbegin + "People_dest.last_name" + noaccentend + " LIKE :search OR " + noaccentbegin + "People_dest.first_name" + noaccentend + " LIKE :search OR REPLACE(People_dest.phone, ' ', '') LIKE :search "
        "OR Dogs.chip LIKE :search OR " + noaccentbegin + "Dogs.name" + noaccentend + " LIKE :search" +
        QString((search.contains("@") ? " OR " + noaccentbegin + "People_prov.email" + noaccentend + " LIKE :searchb OR " + noaccentbegin + "People_dest.email" + noaccentend + " LIKE :searchb" : "")) +
        " OR " + noaccentbegin + "Dogs.sex" + noaccentend + " = :exact OR Dogs.birth = :exact OR " + noaccentbegin + "Dogs.description" + noaccentend + " = :exact OR ES_Registry.date_prov = :exact OR ES_Registry.type_prov = :exact_encr OR " + noaccentbegin + "ES_Registry.death_cause" + noaccentend + " = :exact OR Destinations.date = :exact OR Destinations.type = :exact)"
        "GROUP BY Dogs.id_dog, ES_Registry.date_prov, ES_Registry.id_ES "
        "ORDER BY ES_registry.id_ES;";

    query.prepare(queryString);
    query.bindValue(":year", year);
    query.bindValue(":search", search + "%");
    query.bindValue(":searchb", "%" + search + "%");
    query.bindValue(":exact", search);
    query.bindValue(":exact_encr", crypto->encryptToString(search));


    HandleErrorExec(&query);

    return query;
}

QSqlQuery Database::GetCareRegistry(QString year, QString search) {
    search = RemoveAccents(search);
    QSqlQuery query;
    QString queryString =
        "SELECT Care_registry.id_care, "
        "       Care_registry.entry_date, "
        "       People_prov.last_name, "
        "       People_prov.first_name, "
        "       People_prov.address, "
        "       People_prov.phone, "
        "       People_prov.email, "
        "       Dogs.sex, "
        "       Dogs.chip, "
        "       Dogs.name, "
        "       Dogs.description, "
        "       Dogs.birth, "
        "       Care_registry.exit_date, "
        "       People_dest.last_name, "
        "       People_dest.first_name, "
        "       People_dest.address, "
        "       People_dest.phone, "
        "       People_dest.email "
        "FROM Care_registry "
        "JOIN People AS People_prov ON Care_registry.id_people_prov = People_prov.id_people "
        "JOIN Dogs ON Care_registry.id_dog = Dogs.id_dog "
        "LEFT JOIN People AS People_dest ON Care_registry.id_people_dest = People_dest.id_people "
        "WHERE strftime('%Y', Care_registry.entry_date) = :year "
        "AND (" + noaccentbegin + "People_prov.last_name" + noaccentend + " LIKE :search OR " + noaccentbegin + "People_prov.first_name" + noaccentend + " LIKE :search OR REPLACE(People_prov.phone, ' ', '') LIKE :search "
        "OR " + noaccentbegin + "People_dest.last_name" + noaccentend + " LIKE :search OR " + noaccentbegin + "People_dest.first_name" + noaccentend + " LIKE :search OR REPLACE(People_dest.phone, ' ', '') LIKE :search "
        "OR Dogs.chip LIKE :search OR " + noaccentbegin + "Dogs.name" + noaccentend + " LIKE :search " +
        QString((search.contains("@") ? " OR " + noaccentbegin + "People_prov.email" + noaccentend + " LIKE :searchb OR " + noaccentbegin + "People_dest.email" + noaccentend + " LIKE :searchb" : "")) +
        " OR " + noaccentbegin + "Dogs.sex" + noaccentend + " = :exact OR Dogs.birth = :exact OR " + noaccentbegin + "Dogs.description" + noaccentend + " = :exact OR Care_registry.entry_date = :exact) "
        "ORDER BY Care_registry.id_care;";

    query.prepare(queryString);
    query.bindValue(":year", year);
    query.bindValue(":search", search + "%");
    query.bindValue(":exact", search);
    query.bindValue(":searchb", "%" + search + "%");

    HandleErrorExec(&query);

    return query;
}

QSqlQuery Database::GetMembers(QString year, QString search) {
    search = RemoveAccents(search);
    QSqlQuery query;
    QString queryString =
        "SELECT Members.id_adhesion, "
        "       Members.date, "
        "       People.last_name, "
        "       People.first_name, "
        "       People.address, "
        "       People.phone, "
        "       People.email, "
        "       Members.type, "
        "       Members.amount "
        "FROM Members "
        "JOIN People ON Members.id_people = People.id_people "
        "WHERE strftime('%Y', Members.date) = :year "
        "AND (" + noaccentbegin + "People.last_name" + noaccentend + " LIKE :search OR " + noaccentbegin + "People.first_name" + noaccentend + " LIKE :search OR REPLACE(People.phone, ' ', '') LIKE :search " +
        QString((search.contains("@") ? " OR " + noaccentbegin + "People.email" + noaccentend + " LIKE :searchb" : "")) +
        " OR Members.amount = :exact OR Members.date = :exact) "
        "ORDER BY Members.id_adhesion;";

    query.prepare(queryString);
    query.bindValue(":year", year);
    query.bindValue(":search", search + "%");
    query.bindValue(":exact", search);
    query.bindValue(":searchb", "%" + search + "%");

    HandleErrorExec(&query);

    return query;
}


std::vector<QString> Database::GetRegistryYears(QString type) {
    QSqlQuery query;
    if (type == "entry"){
        HandleErrorExec(&query, "SELECT DISTINCT strftime('%Y', ES_Registry.date_prov) AS year "
                   "FROM ES_Registry "
                   "ORDER BY year ASC;");
    }

    else if (type == "care"){
        HandleErrorExec(&query, "SELECT DISTINCT strftime('%Y', entry_date) AS year "
                   "FROM Care_registry "
                   "ORDER BY year ASC;");
    }

    else if (type == "members"){
        HandleErrorExec(&query, "SELECT DISTINCT strftime('%Y', date) AS year "
                   "FROM Members "
                   "ORDER BY year ASC;");
    }

    std::vector<QString> years;

    while(query.next()){
        years.push_back(query.value(0).toString());
    }


    return years;
}

QSqlQuery Database::GetRedList(QString search) {
    search = RemoveAccents(search);
    QSqlQuery query;
    QString queryString = "SELECT People.last_name, "
                          "People.first_name, "
                          "People.phone, "
                          "GROUP_CONCAT(Red_list.reason, '_|_'), "
                          "People.id_people "
                          "FROM People "
                          "JOIN Red_list ON People.id_people = Red_list.id_people "
                          "WHERE (" + noaccentbegin + "People.last_name" + noaccentend + " LIKE :search OR " + noaccentbegin + "People.first_name" + noaccentend + " LIKE :search OR REPLACE(People.phone, ' ', '') LIKE :search "
                          "OR " + noaccentbegin + "Red_list.reason" + noaccentend + " = :exact) "
                          "GROUP BY People.id_people "
                          "ORDER BY People.id_people DESC;";

    query.prepare(queryString);
    query.bindValue(":search", search + "%");
    query.bindValue(":exact", search);

    HandleErrorExec(&query);

    return query;
}

QSqlQuery Database::GetLost(QString search, bool found) {
    search = RemoveAccents(search);
    QSqlQuery query;
    QString queryString = "SELECT identification, "
                          "name, "
                          "species, "
                          "sex, "
                          "description, "
                          "date, "
                          "place, "
                          "People.last_name, "
                          "People.first_name, "
                          "People.phone, "
                          "found, "
                          "People.id_people "
                          "FROM Lost "
                          "JOIN People ON People.id_people = Lost.id_people "
                          "WHERE (" + noaccentbegin + "People.last_name" + noaccentend + " LIKE :search OR REPLACE(People.phone, ' ', '') LIKE :search OR identification LIKE :search OR name LIKE :search "
                          "OR date = :exact OR " + noaccentbegin + "place" + noaccentend + " = :exact OR " + noaccentbegin + "description" + noaccentend + " = :exact OR " + noaccentbegin + "People.first_name" + noaccentend + " = :exact OR " + noaccentbegin + "sex" + noaccentend + " = :exact) ";

    if(!found) // Remove dogs already found
        queryString += "AND found = 0 ";

    queryString += "ORDER BY date;";

    query.prepare(queryString);
    query.bindValue(":search", search + "%");
    query.bindValue(":exact", search);

    HandleErrorExec(&query);

    return query;
}

QSqlQuery Database::GetVet(QString search, bool old) {
    search = RemoveAccents(search);
    QSqlQuery query;
    QString queryString = "SELECT Vet.date, "
                          "Dogs.name, "
                          "Dogs.chip, "
                          "Vet.reason, "
                          "Dogs.id_dog  "
                          "FROM Vet "
                          "JOIN Dogs ON Dogs.id_dog = Vet.id_dog "
                          "WHERE (" + noaccentbegin + "Dogs.name" + noaccentend + " LIKE :search OR Dogs.chip LIKE :search "
                          "OR " + noaccentbegin + "Dogs.sex" + noaccentend + " = :exact OR " + noaccentbegin + "Vet.reason" + noaccentend + " = :exact) ";

    if(!old)
        queryString += "AND Vet.date > DATE('now') ";

    queryString += "ORDER BY date";

    query.prepare(queryString);
    query.bindValue(":search", search + "%");
    query.bindValue(":exact", search);

    HandleErrorExec(&query);

    return query;
}

QSqlQuery Database::GetAdoptionDemand(QString search, bool satisfied) {
    search = RemoveAccents(search);
    QSqlQuery query;
    QString queryString = "SELECT People.last_name, "
                          "People.first_name, "
                          "People.phone, "
                          "Adoption_demand.sex, "
                          "Adoption_demand.age, "
                          "Adoption_demand.breed, "
                          "Adoption_demand.infos, "
                          "People.id_people "
                          "FROM Adoption_demand "
                          "JOIN People ON People.id_people = Adoption_demand.id_people "
                          "WHERE (" + noaccentbegin + "People.last_name" + noaccentend + " LIKE :search OR " + noaccentbegin + "People.first_name" + noaccentend + " LIKE :search OR REPLACE(People.phone, ' ', '') LIKE :search OR " + noaccentbegin + "Adoption_demand.breed" + noaccentend + " LIKE :search "
                          "OR " + noaccentbegin + "Adoption_demand.sex" + noaccentend + " = :exact OR " + noaccentbegin + "Adoption_demand.infos" + noaccentend + " = :exact OR " + noaccentbegin + "Adoption_demand.age" + noaccentend + " = :exact)";


    if(!satisfied) // Remove dogs already found
        queryString += " AND Adoption_demand.satisfied = 0;";

    query.prepare(queryString);
    query.bindValue(":search", search + "%");
    query.bindValue(":exact", search);

    HandleErrorExec(&query);

    return query;
}

QSqlQuery Database::GetSponsors(QString search, bool past) {
    search = RemoveAccents(search);
    QSqlQuery query;
    QString queryString = "SELECT People.last_name, "
                          "People.first_name, "
                          "People.phone, "
                          "Dogs.name, "
                          "Dogs.description, "
                          "Sponsors.amount, "
                          "Sponsors.start_date, "
                          "Sponsors.end_date "
                          "FROM Sponsors "
                          "JOIN People ON People.id_people = Sponsors.id_people "
                          "JOIN Dogs ON Dogs.id_dog = Sponsors.id_dog "
                          "WHERE (" + noaccentbegin + "People.last_name" + noaccentend + " LIKE :search OR " + noaccentbegin + "People.first_name" + noaccentend + " LIKE :search OR REPLACE(People.phone, ' ', '') LIKE :search OR " + noaccentbegin + "Dogs.chip" + noaccentend + " LIKE :search "
                          "OR " + noaccentbegin + "Dogs.description" + noaccentend + " LIKE :search OR " + noaccentbegin + "Dogs.name" + noaccentend + " LIKE :search)";


    if(!past) // Remove past sponsors
        queryString += " AND (Sponsors.end_date >= DATE('now') OR Sponsors.end_date IS NULL);";

    query.prepare(queryString);
    query.bindValue(":search", search + "%");
    query.bindValue(":exact", search);

    HandleErrorExec(&query);

    return query;
}
