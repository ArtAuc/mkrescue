#include "database.h"

Database::Database()
{
    _database = QSqlDatabase::addDatabase("QSQLITE");
    _database.setDatabaseName("data.db");
    _database.open();

    Create();
    _database.close();

}

Database::~Database()
{
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
               "id_dog INT,"
               "date_prov DATE,"
               "id_people_prov INT,"
               "destination1 VARCHAR(50),"
               "id_people_dest1 INT,"
               "destination2 VARCHAR(50),"
               "id_people_dest2 INT,"
               "destination3 VARCHAR(50),"
               "id_people_dest3 INT,"
               "destination4 VARCHAR(50),"
               "id_people_dest4 INT,"
               "destination5 VARCHAR(50),"
               "id_people_dest5 INT,"
               "death_cause TEXT,"
               "FOREIGN KEY (id_people_prov) REFERENCES People(id_people),"
               "FOREIGN KEY (id_people_dest1) REFERENCES People(id_people),"
               "FOREIGN KEY (id_people_dest2) REFERENCES People(id_people),"
               "FOREIGN KEY (id_people_dest3) REFERENCES People(id_people),"
               "FOREIGN KEY (id_people_dest4) REFERENCES People(id_people),"
               "FOREIGN KEY (id_people_dest5) REFERENCES People(id_people),"
               "FOREIGN KEY (id_dog) REFERENCES Dogs(id_dog)"
               ");");
}
