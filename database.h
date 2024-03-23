#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QMainWindow>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>

class Database
{
public:
    Database();
    ~Database();

    void Create();
    void ReorderEntryRegistry();
    void ReorderCareRegistry();
    QSqlQuery GetEntryRegistry(QString year, QString search);
    QSqlQuery GetCareRegistry(QString year, QString search);
    QSqlQuery GetRedList(QString search);
    std::vector<QString> GetRegistryYears(QString type);
    void Clean();

 private:
    QSqlDatabase _database;
    void CleanDogs(); // TODO : Remove dog entries that are used by noone
    void CleanPeople();

};

#endif // DATABASE_H
