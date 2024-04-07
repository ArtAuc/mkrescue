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
    HandleErrorExec(&query, "CREATE TABLE IF NOT EXISTS People ("
               "id_people INTEGER PRIMARY KEY,"
               "last_name VARCHAR(255),"
               "first_name VARCHAR(255),"
               "address TEXT,"
               "phone VARCHAR(20),"
               "email VARCHAR(255)"
               ");");
    if(!IsInTable("People", "id_people", "-1"))
        HandleErrorExec(&query, "INSERT INTO People (id_people, last_name, first_name, address, phone, email) "
                   "VALUES (-1, '', '', '\n\n ', '', '')"
                   ";");
    if(!IsInTable("People", "id_people", "-2"))
        HandleErrorExec(&query, "INSERT INTO People (id_people, last_name, first_name, address, phone, email) "
                   "VALUES (-2, 'Refuge', '', '\n\n ', '', '')"
                   ";");
    HandleErrorExec(&query, "CREATE TABLE IF NOT EXISTS Dogs ("
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
    HandleErrorExec(&query, "CREATE TABLE IF NOT EXISTS Members ("
               "id_adhesion INT,"
               "id_people INT,"
               "date DATE,"
               "amount DECIMAL(10, 2),"
               "type VARCHAR(50),"
               "FOREIGN KEY (id_people) REFERENCES People(id_people)"
               ");");
    HandleErrorExec(&query, "CREATE TABLE IF NOT EXISTS Red_list ("
               "id_people INT,"
               "reason TEXT,"
               "FOREIGN KEY (id_people) REFERENCES People(id_people)"
               ");");
    HandleErrorExec(&query, "CREATE TABLE IF NOT EXISTS Lost ("
               "id_people INT,"
               "species VARCHAR(50),"
               "name VARCHAR(255),"
               "found BOOLEAN,"
               "description TEXT,"
               "identification VARCHAR(255),"
               "sex VARCHAR(50),"
               "date DATE,"
               "place VARCHAR(255),"
               "FOREIGN KEY (id_people) REFERENCES People(id_people)"
               ");");
    HandleErrorExec(&query, "CREATE TABLE IF NOT EXISTS Adoption_demand ("
               "id_people INT,"
               "sex VARCHAR(10),"
               "breed VARCHAR(100),"
               "age VARCHAR(50),"
               "infos TEXT,"
               "FOREIGN KEY (id_people) REFERENCES People(id_people)"
               ");");
    HandleErrorExec(&query, "CREATE TABLE IF NOT EXISTS Care_registry ("
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
    HandleErrorExec(&query, "CREATE TABLE IF NOT EXISTS Vet ("
               "id_dog INT,"
               "date DATE,"
               "reason TEXT,"
               "FOREIGN KEY (id_dog) REFERENCES Dogs(id_dog)"
               ");");
    HandleErrorExec(&query, "CREATE TABLE IF NOT EXISTS ES_registry ("
               "id_ES INT,"
               "id_dog INT,"
               "type_prov VARCHAR(50),"
               "date_prov DATE,"
               "id_people_prov INT,"
               "death_cause TEXT,"
               "FOREIGN KEY (id_people_prov) REFERENCES People(id_people),"
               "FOREIGN KEY (id_dog) REFERENCES Dogs(id_dog)"
               ");");
    HandleErrorExec(&query, "CREATE TABLE IF NOT EXISTS Destinations ("
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

    HandleErrorExec(&query, "UPDATE ES_registry AS t1 "
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

    HandleErrorExec(&query, "UPDATE Care_registry AS t1 "
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

    HandleErrorExec(&query, "UPDATE Members AS t1 "
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

void Database::CleanDogs(){
    QSqlQuery query;

    HandleErrorExec(&query, "SELECT DISTINCT id_dog "
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
            HandleErrorExec(&query);

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
            HandleErrorExec(&query);


            // Delete Destinations
            query.prepare("DELETE FROM Destinations "
                          "WHERE id_dog = :id;");
            query.bindValue(":id", id);
            HandleErrorExec(&query);

        }
    }
}

void Database::MakeRedList(){
    QSqlQuery query;

    HandleErrorExec(&query, "DELETE FROM Red_list "
               "WHERE reason LIKE 'Abandon de % le __/__/____';");


    HandleErrorExec(&query, "SELECT id_people_prov, Dogs.name, date_prov "
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
        HandleErrorExec(&query);

    }
}

void Database::CleanPeople(){
    QSqlQuery query;
    HandleErrorExec(&query, "SELECT DISTINCT id_people "
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
                                "Members WHERE id_people",
                                "Lost WHERE id_people"
                               };

    for(QString id : id_peoples){
        bool used = false;

        for (QString s : queryStrings){
            query.prepare("SELECT COUNT(*) FROM " + s + " = :id;");
            query.bindValue(":id", id);

            HandleErrorExec(&query);

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
            HandleErrorExec(&query);
        }
    }
}
