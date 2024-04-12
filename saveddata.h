#ifndef SAVEDDATA_H
#define SAVEDDATA_H

#include <QFile>
#include <QMessageBox>
#include <QtNetwork>
#include <QStringConverter>
#include <QToolButton>
#include <QRunnable>
#include <QMovie>
#include <QtConcurrent/QtConcurrent>

#include "simplecrypt.h"


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
    void SetCrypto(SimpleCrypt *crypto, QString email, QString appPassword, QToolButton *syncButton);
    void Synchronize();
    void SynchronizeWorker();
    QString SendEmail(QString subject, QString filePath);

public slots:
    void Save();

signals:
    void SynchronizationFinished(QStringList errors);

private:
    SimpleCrypt *crypto = nullptr;
    QString accessHash;
    QString encryptedEmail, encryptedAppPassword;
    QString lastTimeSync, lastTimeExport;
    QToolButton *syncButton;
    bool synchronizing = false;
    QMovie *syncMovie;
};

#endif // SAVEDDATA_H
