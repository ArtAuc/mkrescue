#include "database.h"

Database::Database()
{
    _database = QSqlDatabase::addDatabase("QSQLITE");
    _database.setDatabaseName("data.db");
    _database.open();

    Create();
}

Database::~Database()
{
    _database.close();
}

void Database::Create()
{
    QFile oldFile("data.db");
    if(oldFile.exists())
        oldFile.copy("data.db.bkp");

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS People ("
               "id_people INTEGER PRIMARY KEY,"
               "last_name VARCHAR(255),"
               "first_name VARCHAR(255),"
               "address TEXT,"
               "phone VARCHAR(20),"
               "email VARCHAR(255)"
               ");");
    query.exec("INSERT INTO People (id_people, last_name, first_name, address, phone, email) "
               "VALUES (-1, '', '', '\n\n ', '', '')"
               ";");
    query.exec("CREATE TABLE IF NOT EXISTS Dogs ("
               "id_dog INT PRIMARY KEY,"
               "name VARCHAR(255),"
               "sex VARCHAR(10),"
               "sterilized BOOLEAN,"
               "chip VARCHAR(50),"
               "description TEXT,"
               "birth DATE,"
               "compat_dog BOOLEAN,"
               "compat_cat BOOLEAN"
               ");");
    query.exec("CREATE TABLE IF NOT EXISTS Members ("
               "id_adhesion INT,"
               "id_people INT,"
               "date DATE,"
               "amount DECIMAL(10, 2),"
               "type VARCHAR(50),"
               "FOREIGN KEY (id_people) REFERENCES People(id_people)"
               ");");
    query.exec("CREATE TABLE IF NOT EXISTS Red_list ("
               "id_people INT,"
               "reason TEXT,"
               "FOREIGN KEY (id_people) REFERENCES People(id_people)"
               ");");
    query.exec("CREATE TABLE IF NOT EXISTS Lost ("
               "id_people INT,"
               "species VARCHAR(50),"
               "description TEXT,"
               "identified BOOLEAN,"
               "date DATE,"
               "place VARCHAR(255),"
               "FOREIGN KEY (id_people) REFERENCES People(id_people)"
               ");");
    query.exec("CREATE TABLE IF NOT EXISTS Adoption_demand ("
               "id_people INT,"
               "sex VARCHAR(10),"
               "breed VARCHAR(100),"
               "age VARCHAR(50),"
               "infos TEXT,"
               "FOREIGN KEY (id_people) REFERENCES People(id_people)"
               ");");
    query.exec("CREATE TABLE IF NOT EXISTS Care_registry ("
               "id_care INT,"
               "id_people_prov INT,"
               "id_people_dest INT,"
               "id_dog INT,"
               "entry_date DATE,"
               "exit_date DATE, "
               "FOREIGN KEY (id_people_prov) REFERENCES People(id_people),"
               "FOREIGN KEY (id_people_dest) REFERENCES People(id_people),"
               "FOREIGN KEY (id_dog) REFERENCES Dogs(id_dog)"
               ");");
    query.exec("CREATE TABLE IF NOT EXISTS Vet ("
               "id_dog INT,"
               "date DATE,"
               "reason TEXT,"
               "FOREIGN KEY (id_dog) REFERENCES Dogs(id_dog)"
               ");");
    query.exec("CREATE TABLE IF NOT EXISTS ES_registry ("
               "id_ES INT,"
               "id_dog INT,"
               "type_prov VARCHAR(50),"
               "date_prov DATE,"
               "id_people_prov INT,"
               "death_cause TEXT,"
               "FOREIGN KEY (id_people_prov) REFERENCES People(id_people),"
               "FOREIGN KEY (id_dog) REFERENCES Dogs(id_dog)"
               ");");
    query.exec("CREATE TABLE IF NOT EXISTS Destinations ("
               "id_dog INT,"
               "id_people INT,"
               "date DATE,"
               "type VARCHAR(50),"
               "FOREIGN KEY (id_dog) REFERENCES Dogs(id_dog),"
               "FOREIGN KEY (id_people) REFERENCES People(id_people)"
               ");");
}


// Reassociate E/S numbers according to chronology, by year
void Database::ReorderEntryRegistry()
{
    QSqlQuery query;

    query.exec("UPDATE ES_registry AS t1 "
               "SET id_ES = ( "
               "    SELECT COUNT(*) + 1 "
               "    FROM ES_registry AS t2 "
               "    WHERE strftime('%Y', t1.date_prov) = strftime('%Y', t2.date_prov) "
               "          AND (strftime('%Y-%m-%d', t1.date_prov) > strftime('%Y-%m-%d', t2.date_prov) "
               "               OR (strftime('%Y-%m-%d', t1.date_prov) = strftime('%Y-%m-%d', t2.date_prov) "
               "                   AND t1.ROWID > t2.ROWID)"
               "    ) "
               "); ");
}

void Database::ReorderCareRegistry(){
    QSqlQuery query;

    query.exec("UPDATE Care_registry AS t1 "
               "SET id_care = ( "
               "    SELECT COUNT(*) + 1 "
               "    FROM Care_registry AS t2 "
               "    WHERE strftime('%Y', t1.entry_date) = strftime('%Y', t2.entry_date) "
               "          AND (strftime('%Y-%m-%d', t1.entry_date) > strftime('%Y-%m-%d', t2.entry_date) "
               "               OR (strftime('%Y-%m-%d', t1.entry_date) = strftime('%Y-%m-%d', t2.entry_date) "
               "                   AND t1.ROWID > t2.ROWID)"
               "    ) "
               "); ");

}

void Database::ReorderMembers(){
    QSqlQuery query;

    query.exec("UPDATE Members AS t1 "
               "SET id_adhesion = ( "
               "    SELECT COUNT(*) + 1 "
               "    FROM Members AS t2 "
               "    WHERE strftime('%Y', t1.date) = strftime('%Y', t2.date) "
               "          AND (strftime('%Y-%m-%d', t1.date) > strftime('%Y-%m-%d', t2.date) "
               "               OR (strftime('%Y-%m-%d', t1.date) = strftime('%Y-%m-%d', t2.date) "
               "                   AND t1.ROWID > t2.ROWID)"
               "    ) "
               "); ");

}

QSqlQuery Database::GetCurrentESDogs(QString search){
    QSqlQuery query;
    QString queryString =
            "SELECT Dogs.id_dog, Dogs.name, Dogs.sex, Dogs.birth, ES_Registry.date_prov "
            "FROM Dogs "
            "JOIN ES_Registry ON ES_Registry.id_dog = Dogs.id_dog "
            "LEFT JOIN Destinations ON Destinations.id_dog = Dogs.id_dog "
            "WHERE (Dogs.name LIKE :search OR chip LIKE :search OR description LIKE :search) "
            "AND (Destinations.type IS NULL OR Destinations.type = 'Entrée au refuge') " // To get dogs currently in shelter only
            "ORDER BY Dogs.id_dog DESC;";

    query.prepare(queryString);
    query.bindValue(":search", search + "%");
    query.exec();

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
        "       GROUP_CONCAT(Destinations.date || '___' || Destinations.type || '___' || People_dest.last_name || '___' || People_dest.first_name || '___' || People_dest.address || '___' || People_dest.phone || '___' || People_dest.email, '_-_'), "
        "       ES_registry.death_cause "
        "FROM ES_registry "
        "JOIN People AS People_prov ON ES_registry.id_people_prov = People_prov.id_people "
        "JOIN Dogs ON ES_registry.id_dog = Dogs.id_dog "
        "LEFT JOIN Destinations ON Dogs.id_dog = Destinations.id_dog "
        "LEFT JOIN People AS People_dest ON Destinations.id_people = People_dest.id_people "
        "WHERE strftime('%Y', ES_registry.date_prov) = :year "
        "AND (People_prov.last_name LIKE :search OR People_prov.phone LIKE :search "
        "OR People_dest.last_name LIKE :search OR People_dest.phone LIKE :search "
        "OR Dogs.chip LIKE :search OR Dogs.name LIKE :search" +
        QString((search.contains("@") ? " OR People_prov.email LIKE :searchb OR People_dest.email LIKE :searchb" : "")) +
        ")"
        "GROUP BY Dogs.id_dog "
        "ORDER BY ES_registry.id_ES;";

    query.prepare(queryString);
    query.bindValue(":year", year);
    query.bindValue(":search", search + "%");
    query.bindValue(":searchb", "%" + search + "%");


    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

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
        "AND (People_prov.last_name LIKE :search OR People_prov.phone LIKE :search "
        "OR People_dest.last_name LIKE :search OR People_dest.phone LIKE :search "
        "OR Dogs.chip LIKE :search OR Dogs.name LIKE :search " +
        QString((search.contains("@") ? " OR People_prov.email LIKE :searchb OR People_dest.email LIKE :searchb" : "")) +
        ") "
        "ORDER BY Care_registry.id_care;";

    query.prepare(queryString);
    query.bindValue(":year", year);
    query.bindValue(":search", search + "%");
    query.bindValue(":searchb", "%" + search + "%");

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

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
        "AND (People.last_name LIKE :search OR People.phone LIKE :search " +
        QString((search.contains("@") ? " OR People.email LIKE :searchb" : "")) +
        ") "
        "ORDER BY Members.id_adhesion;";

    query.prepare(queryString);
    query.bindValue(":year", year);
    query.bindValue(":search", search + "%");
    query.bindValue(":searchb", "%" + search + "%");

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

    return query;
}


std::vector<QString> Database::GetRegistryYears(QString type) {
    QSqlQuery query;
    if (type == "entry"){
        query.exec("SELECT DISTINCT strftime('%Y', date_prov) AS year "
                   "FROM ES_Registry "
                   "ORDER BY year ASC;");
    }

    else if (type == "care"){
        query.exec("SELECT DISTINCT strftime('%Y', entry_date) AS year "
                   "FROM Care_registry "
                   "ORDER BY year ASC;");
    }

    else if (type == "members"){
        query.exec("SELECT DISTINCT strftime('%Y', date) AS year "
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
                          "GROUP_CONCAT(Red_list.reason, '___'), "
                          "People.id_people "
                          "FROM People "
                          "JOIN Red_list ON People.id_people = Red_list.id_people "
                          "WHERE (People.last_name LIKE :search OR People.phone LIKE :search) "
                          "GROUP BY People.id_people "
                          "ORDER BY People.id_people DESC;";

    query.prepare(queryString);
    query.bindValue(":search", search + "%");

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

    return query;
}

void Database::CleanDogs(){
    QSqlQuery query;

    query.exec("SELECT DISTINCT id_dog "
               "FROM Dogs");

    QStringList id_dogs;
    while(query.next()){
        id_dogs.append(query.value(0).toString());
    }

    QStringList queryStrings = {"ES_Registry WHERE id_dog",
                                "Care_Registry WHERE id_dog"
                               };

    for(QString id : id_dogs){
        bool used = false;
        for(QString s : queryStrings){
            query.prepare("SELECT COUNT(*) "
                          "FROM " + s + " = :id;");
            query.bindValue(":id", id);
            query.exec();
            query.next();

            if(query.value(0).toInt() > 0){
                used = true;
                break;
            }
        }

        if(!used){
            // Delete from Dogs
            query.prepare("DELETE FROM Dogs "
                          "WHERE id_dog = :id;");
            query.bindValue(":id", id);
            query.exec();

            // Delete Destinations
            query.prepare("DELETE FROM Destinations "
                          "WHERE id_dog = :id;");
            query.bindValue(":id", id);
            query.exec();
        }
    }
}

void Database::MakeRedList(){
    QSqlQuery query;

    query.exec("DELETE FROM Red_list "
               "WHERE reason LIKE 'Abandon de % le __/__/____';");


    query.exec("SELECT id_people_prov, Dogs.name, date_prov "
               "FROM ES_Registry "
               "JOIN Dogs ON Dogs.id_dog = ES_Registry.id_dog "
               "WHERE type_prov = 'Abandon'");

    QStringList id_peoples;
    QStringList reasons;
    while(query.next()){
        id_peoples.append(query.value(0).toString());
        reasons.append("Abandon de " + query.value(1).toString() + " le " + QDate::fromString(query.value(2).toString(), "yyyy-MM-dd").toString("dd/MM/yyyy"));
    }

    for(int i = 0; i < reasons.length(); i++){
        query.prepare("INSERT INTO Red_list (id_people, reason) "
                      "VALUES (:id, :reason)");
        query.bindValue(":id", id_peoples[i]);
        query.bindValue(":reason", reasons[i]);
        query.exec();
    }
}

void Database::CleanPeople(){
    QSqlQuery query;
    query.exec("SELECT DISTINCT id_people "
               "FROM People "
               "WHERE id_people > 0");

    QStringList id_peoples;
    while(query.next()){
        id_peoples.append(query.value(0).toString());
    }

    QStringList queryStrings = {"ES_Registry WHERE id_people_prov",
                                "Care_registry WHERE id_people_prov",
                                "Care_registry WHERE id_people_dest",
                                "Destinations WHERE id_people",
                                "Red_list WHERE id_people",
                                "Members WHERE id_people"
                               };

    for(QString id : id_peoples){
        bool used = false;

        for (QString s : queryStrings){
            query.prepare("SELECT COUNT(*) FROM " + s + " = :id;");
            query.bindValue(":id", id);

            query.exec();
            query.next();

            if(query.value(0).toInt() > 0){
                used = true;
                break;
            }
        }

        if(!used){ // Delete from database
            query.prepare("DELETE FROM People "
                          "WHERE id_people = :id");
            query.bindValue(":id", id);
            query.exec();
        }
    }
}
