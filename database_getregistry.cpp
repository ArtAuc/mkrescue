#include "database.h"

QSqlQuery Database::GetDogs(QString type, QString search){
    QString queryString = "SELECT Dogs.chip, Dogs.name, Dogs.sex, Dogs.birth, Dogs.description, (ES_Registry.date_prov || '_|_' || ES_Registry.type_prov), Dogs.id_dog "
                          "FROM Dogs "
                          "JOIN ES_Registry ON ES_Registry.id_dog = Dogs.id_dog "
                          "LEFT JOIN ("
                          "    SELECT id_dog, MAX(date) AS max_date "
                          "    FROM Destinations "
                          "    GROUP BY id_dog "
                          ") AS LastDest ON Dogs.id_dog = LastDest.id_dog "
                          "LEFT JOIN Destinations ON Destinations.id_dog = Dogs.id_dog AND Destinations.date = LastDest.max_date "
                          "WHERE (Dogs.name LIKE :search OR chip LIKE :search OR description LIKE :search "
                          "OR Dogs.sex = :exact OR Dogs.birth = :exact OR Dogs.description = :exact OR ES_Registry.date_prov = :exact OR ES_Registry.type_prov = :exact_encr) "
                          "AND (Destinations.type IS NULL OR Destinations.type = 'Entrée au refuge') "
                          "AND Dogs.id_dog NOT IN (SELECT id_dog FROM Care_registry)";

    if(type.contains("out"))
        queryString +=
                " UNION "
                ""
                "SELECT Dogs.chip, Dogs.name, Dogs.sex, Destinations.date || '_|_' || Destinations.type, Dogs.description, (ES_Registry.date_prov || '_|_' || ES_Registry.type_prov), Dogs.id_dog "
                "FROM Dogs "
                "JOIN ES_Registry ON ES_Registry.id_dog = Dogs.id_dog "
                "JOIN ( "
                "    SELECT id_dog, date, MAX(date) AS max_date, MAX(id_people) AS max_people_id "
                "    FROM Destinations "
                "    GROUP BY id_dog "
                ") AS LastDest ON Dogs.id_dog = LastDest.id_dog "
                "LEFT JOIN Destinations ON Destinations.id_dog = Dogs.id_dog AND Destinations.date = LastDest.max_date AND Destinations.id_people = LastDest.max_people_id "
                "WHERE (Dogs.name LIKE :search OR chip LIKE :search OR description LIKE :search) "
                "OR Dogs.sex = :exact OR Dogs.birth = :exact OR Dogs.description = :exact OR Destinations.date = :exact OR Destinations.type = :exact OR ES_Registry.date_prov = :exact OR ES_Registry.type_prov = :exact_encr "
                "AND NOT (Destinations.type IS NULL OR Destinations.type = 'Entrée au refuge') "
                "AND Dogs.id_dog NOT IN (SELECT id_dog FROM Care_registry)";

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
                "WHERE (Dogs.name LIKE :search OR chip LIKE :search OR description LIKE :search "
                "OR Dogs.sex = :exact OR Dogs.description = :exact OR Dogs.birth = :exact OR LastCare.max_date = :exact OR People.last_name = :exact)";
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
        "AND (People_prov.last_name LIKE :search OR People_prov.first_name LIKE :search OR People_prov.phone LIKE :search "
        "OR People_dest.last_name LIKE :search OR People_dest.first_name LIKE :search OR People_dest.phone LIKE :search "
        "OR Dogs.chip LIKE :search OR Dogs.name LIKE :search" +
        QString((search.contains("@") ? " OR People_prov.email LIKE :searchb OR People_dest.email LIKE :searchb" : "")) +
        " OR Dogs.sex = :exact OR Dogs.birth = :exact OR Dogs.description = :exact OR ES_Registry.date_prov = :exact OR ES_Registry.type_prov = :exact_encr OR ES_Registry.death_cause = :exact OR Destinations.date = :exact OR Destinations.type = :exact)"
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
        "AND (People_prov.last_name LIKE :search OR People_prov.first_name LIKE :search OR People_prov.phone LIKE :search "
        "OR People_dest.last_name LIKE :search OR People_dest.first_name LIKE :search OR People_dest.phone LIKE :search "
        "OR Dogs.chip LIKE :search OR Dogs.name LIKE :search " +
        QString((search.contains("@") ? " OR People_prov.email LIKE :searchb OR People_dest.email LIKE :searchb" : "")) +
        " OR Dogs.sex = :exact OR Dogs.birth = :exact OR Dogs.description = :exact OR Care_registry.entry_date = :exact) "
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
        "AND (People.last_name LIKE :search OR People.first_name LIKE :search OR People.phone LIKE :search " +
        QString((search.contains("@") ? " OR People.email LIKE :searchb" : "")) +
        " OR Members.amount = :exact OR Members.amount = :exact OR Members.date = :exact) "
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
    QSqlQuery query;
    QString queryString = "SELECT People.last_name, "
                          "People.first_name, "
                          "People.phone, "
                          "GROUP_CONCAT(Red_list.reason, '_|_'), "
                          "People.id_people "
                          "FROM People "
                          "JOIN Red_list ON People.id_people = Red_list.id_people "
                          "WHERE (People.last_name LIKE :search OR People.first_name LIKE :search OR People.phone LIKE :search "
                          "OR Red_list.reason = :exact) "
                          "GROUP BY People.id_people "
                          "ORDER BY People.id_people DESC;";

    query.prepare(queryString);
    query.bindValue(":search", search + "%");
    query.bindValue(":exact", search);

    HandleErrorExec(&query);

    return query;
}

QSqlQuery Database::GetLost(QString search, bool found) {
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
                          "WHERE (People.last_name LIKE :search OR People.phone LIKE :search OR identification LIKE :search OR name LIKE :search "
                          "OR date = :exact OR place = :exact OR description = :exact OR People.first_name = :exact OR sex = :exact) ";

    if(!found) // Remove dogs already found
        queryString += "AND found = 0 ";

    queryString += "ORDER BY date;";

    query.prepare(queryString);
    query.bindValue(":search", search + "%");
    query.bindValue(":exact", search);

    HandleErrorExec(&query);

    return query;
}

QSqlQuery Database::GetVet(QString search) {
    QSqlQuery query;
    QString queryString = "SELECT Vet.date, "
                          "Dogs.name, "
                          "Dogs.chip, "
                          "Vet.reason, "
                          "Dogs.id_dog  "
                          "FROM Vet "
                          "JOIN Dogs ON Dogs.id_dog = Vet.id_dog "
                          "WHERE (Dogs.name LIKE :search OR Dogs.chip LIKE :search "
                          "OR Dogs.sex = :exact OR Vet.reason = :exact) "
                          "ORDER BY date;";


    query.prepare(queryString);
    query.bindValue(":search", search + "%");
    query.bindValue(":exact", search);

    HandleErrorExec(&query);

    return query;
}

QSqlQuery Database::GetAdoptionDemand(QString search, bool satisfied) {
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
                          "WHERE (People.last_name LIKE :search OR People.first_name LIKE :search OR People.phone LIKE :search OR Adoption_demand.breed LIKE :search "
                          "OR Adoption_demand.sex = :exact OR Adoption_demand.infos = :exact OR Adoption_demand.age = :exact)";


    if(!satisfied) // Remove dogs already found
        queryString += " AND Adoption_demand.satisfied = 0;";

    query.prepare(queryString);
    query.bindValue(":search", search + "%");
    query.bindValue(":exact", search);

    HandleErrorExec(&query);

    return query;
}
