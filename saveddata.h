#ifndef SAVEDDATA_H
#define SAVEDDATA_H

#include <QFile>
#include <QMessageBox>
#include <QtNetwork>
#include <QStringConverter>

#include "simplecrypt.h"

class SavedData : public QObject
{
    Q_OBJECT
public:
    SavedData();
    void Save();
    bool Load();
    bool CompareHash(QByteArray h){return accessHash == h.toHex();}
    QStringList GetShelterInfos();
    bool HashExists(){return !accessHash.isEmpty();}
    void SetHash(QByteArray h){accessHash = h.toHex();}
    void Synchronize();
    void SetCrypto(SimpleCrypt *crypto, QString email, QString appPassword);
    QString SendEmail(QString subject, QString filePath);


signals:
    void SynchronizationStarted();
    void SynchronizationFinished();

private:
    SimpleCrypt *crypto = nullptr;
    QString accessHash;
    QString encryptedEmail;
    QString encryptedAppPassword;
    QString lastTimeSync;
};

#endif // SAVEDDATA_H
