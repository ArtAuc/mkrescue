#ifndef DATABASE_H
#define DATABASE_H

#include <QThread>
#include <QCryptographicHash>

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
    static void ChangePassword(QString oldPassword, QString newPassword);
    static bool IsInTable(QString table, QString attribute, QString value);


 private:
    QSqlDatabase _database;
    SimpleCrypt *crypto = nullptr;
};

class CleanThread : public QThread {
public:
    CleanThread(SimpleCrypt *crypto){this->crypto = crypto;}
    ~CleanThread(){
    }

    void run() override {
        threadDb = QSqlDatabase::addDatabase("QSQLITE", "secondary");
        threadDb.setDatabaseName("data.db");
        threadDb.open();

        CleanDogs();
        MakeRedList();
        CleanPeople();

        if (threadDb.transaction()) {
            threadDb.commit();
        }

        threadDb.close();
    }

    void CleanDogs();
    void MakeRedList();
    void CleanPeople();

private:
    QSqlDatabase threadDb;
    SimpleCrypt *crypto;
};

#endif // DATABASE_H
