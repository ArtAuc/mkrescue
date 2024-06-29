#ifndef SAVEDDATA_H
#define SAVEDDATA_H

#include <QToolButton>
#include <QDateTime>
#include <QMessageBox>
#include <QFile>
#include <QMovie>
#include <QDirIterator>
#include <QtConcurrent/QtConcurrent>
#include <QSslSocket>

#include "utils.h"


class SavedData : public QObject
{
    Q_OBJECT
public:
    SavedData();
    bool Load();
    bool CompareHash(QByteArray h){return accessHash == h.toHex();}
    QDate GetLastTimeExport(){return QDate::fromString(lastTimeExport, "yyyy-MM-dd");}
    void SetLastTimeExport(QDate date){lastTimeExport = date.toString("yyyy-MM-dd");}
    void SetLastTimeSync(QDateTime dateTime){lastTimeSync = dateTime.toString("yyyy-MM-ddTHH:mm:ss");}
    bool HashExists(){return !accessHash.isEmpty();}
    void SetHash(QByteArray h){accessHash = h.toHex();}
    void Init(QString email, QString appPassword, QToolButton *syncButton);
    void Synchronize();
    void SynchronizeWorker();
    QString SynchronizeEmail(QString subject, QString filePath);
    QString MaxDogs(){return maxDogs;}
    void SetMaxDogs(QString maxDogs){this->maxDogs = maxDogs; Save();}
    QString MaxDays(){return maxDays;}
    void SetMaxDays(QString maxDays){this->maxDays = maxDays; Save();}
    QString SendMembersEmail(QString address, QString firstName, QString lastName, QString expirationDate);
    void SetShelterInfos(QString name, QString phone, QString address){shelterName = name; shelterPhone = phone; shelterAddress = address;}
    void SetRecallMailBody(QString txt){recallMailBody = txt;}

public slots:
    void Save();

signals:
    void SynchronizationFinished(QStringList errors);

private:
    QString accessHash;
    QString encryptedEmail, encryptedAppPassword;
    QString lastTimeSync, lastTimeExport;
    QToolButton *syncButton;
    bool synchronizing = false;
    QMovie *syncMovie;
    QString maxDogs = "1";
    QString maxDays = "0";
    QString shelterName, shelterPhone, shelterAddress;
    QString recallMailBody = "";
    QStringList emailsToSend = {};
};

#endif // SAVEDDATA_H
