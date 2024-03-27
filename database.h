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
    void ReorderMembers();
    QSqlQuery GetDogs(QString type, QString search);
    QSqlQuery GetEntryRegistry(QString year, QString search);
    QSqlQuery GetCareRegistry(QString year, QString search);
    QSqlQuery GetMembers(QString year, QString search);
    QSqlQuery GetRedList(QString search);
    std::vector<QString> GetRegistryYears(QString type);
    void CleanDogs();
    void CleanPeople();
    void MakeRedList();

 private:
    QSqlDatabase _database;

};

#endif // DATABASE_H
