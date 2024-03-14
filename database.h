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

 private:
    QSqlDatabase _database;
};

#endif // DATABASE_H
