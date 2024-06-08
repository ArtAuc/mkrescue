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
               "last_name TEXT,"
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
               "satisfied BOOLEAN,"
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
               "date_prov DATE,"
               "FOREIGN KEY (id_dog) REFERENCES Dogs(id_dog),"
               "FOREIGN KEY (id_people) REFERENCES People(id_people)"
               ");");

    // Redate empty dates from members, see commit
    HandleErrorExec(&query, "UPDATE Members "
                            "SET date = DATE('now') "
                            "WHERE date IS NULL");
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

bool Database::IsInTable(QString table, QString attribute, QString value){
    QSqlQuery query;
    HandleErrorExec(&query, "SELECT COUNT(*) FROM " + table + " WHERE " + attribute + " = " + value);
    query.next();
    return query.value(0).toInt() > 0;
}

void CleanThread::CleanDogs(){
    QSqlQuery query(threadDb);

    HandleErrorExec(&query, "SELECT DISTINCT id_dog "
               "FROM Dogs");

    QStringList id_dogs;
    while(query.next()){
        id_dogs.append(query.value(0).toString());
    }

    QStringList queryStrings = {"ES_Registry WHERE id_dog",
                                "Care_Registry WHERE id_dog",
                                "Vet WHERE id_dog"
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

void CleanThread::MakeRedList(){
    if(crypto){
        QSqlQuery query(threadDb);

        HandleErrorExec(&query, "DELETE FROM Red_list "
                   "WHERE reason LIKE 'Abandon de % le __/__/____';");

        HandleErrorExec(&query, "SELECT id_people_prov, Dogs.name, ES_Registry.date_prov "
                   "FROM ES_Registry "
                   "JOIN Dogs ON Dogs.id_dog = ES_Registry.id_dog "
                   "JOIN People ON id_people_prov = People.id_people "
                   "WHERE type_prov = '" + crypto->encryptToString(QString("Abandon")) + "' "
                   "AND (People.last_name != '' OR People.first_name != '' OR People.phone != '')");


        QStringList id_peoples;
        QStringList reasons;
        while(query.next()){
            id_peoples.append(query.value(0).toString());
            reasons.append("Abandon de " + query.value(1).toString() + " le " + QDate::fromString(query.value(2).toString(), "yyyy-MM-dd").toString("dd/MM/yyyy"));
        }


        // Single query to insert is way faster
        QString insertQuery = "INSERT INTO Red_list (id_people, reason) VALUES ";
        for(int i = 0; i < reasons.length(); i++){
            insertQuery += "(:id" + QString::number(i) + ", :reason" + QString::number(i) + "), ";
        }
        insertQuery.chop(2);
        query.prepare(insertQuery);

        for(int i = 0; i < reasons.length(); i++){
            query.bindValue(":id" + QString::number(i), id_peoples[i]);
            query.bindValue(":reason" + QString::number(i), reasons[i]);
        }

        HandleErrorExec(&query);
    }
}

void CleanThread::CleanPeople(){
    QSqlQuery query(threadDb);
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
                                "Lost WHERE id_people",
                                "Adoption_demand WHERE id_people"
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

void Database::Init(){
    QSqlQuery query;
    query.prepare("UPDATE People "
                  "SET address = :empty "
                  "WHERE id_people = -1");

    query.bindValue(":empty", crypto->encryptToString(QString("\n\n ")));
    HandleErrorExec(&query);

    HandleErrorExec(&query, "SELECT address "
                            "FROM People "
                            "WHERE id_people = -2");
    query.next();
    if(query.value(0).toString() == "\n\n "){
        query.prepare("UPDATE People "
                      "SET address = :empty "
                      "WHERE id_people = -2");

        query.bindValue(":empty", crypto->encryptToString(QString("\n\n ")));
        HandleErrorExec(&query);
    }
}

void Database::ChangePassword(QString oldPassword, QString newPassword){
    SimpleCrypt oldCrypto, newCrypto;
    QByteArray old_h = QCryptographicHash::hash((oldPassword + "refuge510refuge").toUtf8(), QCryptographicHash::Sha256);
    quint64 old_key;
    QDataStream old_stream(&old_h, QIODevice::ReadOnly);
    old_stream >> old_key;
    oldCrypto.setKey(old_key);

    QByteArray new_h = QCryptographicHash::hash((newPassword + "refuge510refuge").toUtf8(), QCryptographicHash::Sha256);
    quint64 new_key;
    QDataStream new_stream(&new_h, QIODevice::ReadOnly);
    new_stream >> new_key;

    newCrypto.setKey(new_key);

    QSqlQuery loadQuery;
    HandleErrorExec(&loadQuery, "SELECT id_people, address FROM People");
    while(loadQuery.next()){
        QSqlQuery saveQuery;
        saveQuery.prepare("UPDATE People "
                          "SET address = :address "
                          "WHERE id_people = :id_people");
        saveQuery.bindValue(":id_people", loadQuery.value(0).toInt());
        saveQuery.bindValue(":address", newCrypto.encryptToString(oldCrypto.decryptToString(loadQuery.value(1).toString())));
        HandleErrorExec(&saveQuery);
    }

    HandleErrorExec(&loadQuery, "SELECT id_ES, date_prov, type_prov FROM ES_Registry");
    while(loadQuery.next()){
        QSqlQuery saveQuery;
        saveQuery.prepare("UPDATE ES_Registry "
                          "SET type_prov = :type_prov "
                          "WHERE id_ES = :id_ES "
                          "AND date_prov = :date_prov");
        saveQuery.bindValue(":id_ES", loadQuery.value(0).toInt());
        saveQuery.bindValue(":date_prov", loadQuery.value(1).toDate());
        saveQuery.bindValue(":type_prov", newCrypto.encryptToString(oldCrypto.decryptToString(loadQuery.value(2).toString())));
        HandleErrorExec(&saveQuery);
    }

    // Replace first line of save (hashed password)
    QFile file("save");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }
    file.close();

    lines[0] = QCryptographicHash::hash(QString(new_h.toHex() + "refuge510refuge").toUtf8(), QCryptographicHash::Sha256).toHex();
    lines[1] = newCrypto.encryptToString(oldCrypto.decryptToString(lines[1]));
    lines[2] = newCrypto.encryptToString(oldCrypto.decryptToString(lines[2]));

    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);

    QTextStream out(&file);
    for (const QString& line : lines)
        out << line << '\n';
    file.close();
}
