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
               "address VARCHAR(255),"
               "phone VARCHAR(20),"
               "email VARCHAR(255)"
               ");");
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
               "id_people INT,"
               "id_dog INT,"
               "entry_date DATE,"
               "exit_date DATE,"
               "FOREIGN KEY (id_people) REFERENCES People(id_people),"
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

QSqlQuery Database::GetEntryRegistry(QString year) {
    ReorderEntryRegistry();

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
        "       Destinations.date,  "
        "       GROUP_CONCAT(Destinations.type || ';-;' || People_dest.last_name || ';-;' || People_dest.first_name || ';-;' || People_dest.address || ';-;' || People_dest.phone || ';-;' || People_dest.email, ';;;'), "
        "       ES_registry.death_cause "
        "FROM ES_registry "
        "JOIN People AS People_prov ON ES_registry.id_people_prov = People_prov.id_people "
        "JOIN Dogs ON ES_registry.id_dog = Dogs.id_dog "
        "LEFT JOIN Destinations ON Dogs.id_dog = Destinations.id_dog "
        "LEFT JOIN People AS People_dest ON Destinations.id_people = People_dest.id_people "
        //"WHERE strftime('%Y', ES_registry.date_prov) = :year "
        "GROUP BY Dogs.id_dog "
        "ORDER BY ES_registry.id_ES;";

    query.prepare(queryString);
    query.bindValue(":year", year);

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

    return query;
}

std::vector<QString> Database::GetRegistryYears(QString type) {
    QSqlQuery query;
    if (type == "Entry"){
        query.exec("SELECT DISTINCT strftime('%Y', date_prov) FROM ES_Registry;");
    }

    std::vector<QString> years;
    while(query.next()){
        years.push_back(query.value(0).toString());
    }

    return years;
}
