#ifndef DATABASE_H
#define DATABASE_H

#include "utils.h"

class Database
{
public:
    Database();
    ~Database();

    void Create();
    void ReorderEntryRegistry();
    void ReorderCareRegistry();
    void ReorderMembers();
    QSqlQuery GetDogs(QString type, QString search);
    QSqlQuery GetEntryRegistry(QString year, QString search);
    QSqlQuery GetCareRegistry(QString year, QString search);
    QSqlQuery GetMembers(QString year, QString search);
    QSqlQuery GetRedList(QString search);
    QSqlQuery GetLost(QString search, bool found = false);
    QSqlQuery GetVet(QString search);
    std::vector<QString> GetRegistryYears(QString type);
    void CleanDogs();
    void CleanPeople();
    void MakeRedList();

 private:
    QSqlDatabase _database;

};

#endif // DATABASE_H
