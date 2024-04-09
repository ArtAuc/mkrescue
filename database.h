#ifndef DATABASE_H
#define DATABASE_H

#include "utils.h"
#include "simplecrypt.h"

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
    QSqlQuery GetAdoptionDemand(QString search, bool satisfied = false);
    std::vector<QString> GetRegistryYears(QString type);
    void CleanDogs();
    void CleanPeople();
    void MakeRedList();
    void SetCrypto(SimpleCrypt *crypto);

 private:
    QSqlDatabase _database;
    SimpleCrypt *crypto;
};

#endif // DATABASE_H
