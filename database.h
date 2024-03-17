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
    std::vector<QString> GetRegistryYears(QString type);

 private:
    QSqlDatabase _database;
};

#endif // DATABASE_H
