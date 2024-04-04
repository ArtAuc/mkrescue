#ifndef SAVEDDATA_H
#define SAVEDDATA_H

#include <QFile>
#include <QMessageBox>

class SavedData
{
public:
    SavedData();
    void Save();
    bool Load();
    bool CompareHash(QString hash){return accessHash == hash;}
    QStringList GetShelterInfos();

private:
    QString accessHash;
    QString shelterName;
    QString shelterAddress;
    QString shelterAddress2;
    QString shelterPostalCode;
    QString shelterCity;
    QString shelterPhone;
};

#endif // SAVEDDATA_H
