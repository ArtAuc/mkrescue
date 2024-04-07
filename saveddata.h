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
    bool CompareHash(QByteArray h){return accessHash == h.toHex();}
    QStringList GetShelterInfos();
    bool HashExists(){return !accessHash.isEmpty();}
    void SetHash(QByteArray h){accessHash = h.toHex();}

private:
    QString accessHash = "";
};

#endif // SAVEDDATA_H
