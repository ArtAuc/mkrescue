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
};

#endif // SAVEDDATA_H
