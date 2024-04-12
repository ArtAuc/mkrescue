#include "saveddata.h"

SavedData::SavedData()
{
    QObject::connect(this, &SavedData::SynchronizationFinished, this, [this](QStringList errors){
        syncMovie->stop();
        syncButton->setIcon(QIcon("media/sync.svg"));
        delete syncMovie;

        bool success = true;
        for (QString e : errors){
            if(e != ""){
                QMessageBox::critical(nullptr, "Erreur de sauvegarde", e);
                success = false;
                break;
            }
        }

        if(success){
            QMessageBox::information(nullptr, "Succès", "La sauvegarde a réussi.");
            lastTimeSync = QDateTime::currentDateTime().toString("yyyy-MM-ddTHH:mm:ss");
        }

        Save();

        synchronizing = false;
    });
}

bool SavedData::Load(){
    QFile file("save");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);

        accessHash = in.readLine();
        encryptedEmail = in.readLine();
        encryptedAppPassword = in.readLine();
        lastTimeSync = in.readLine();
        lastTimeExport = in.readLine();


        if(!accessHash.isEmpty() && !encryptedEmail.isEmpty() && !encryptedAppPassword.isEmpty() && !lastTimeSync.isEmpty() && !lastTimeExport.isEmpty()){
            file.close();

            return true;
        }

        file.close();
    }

    accessHash.clear();
    encryptedEmail.clear();
    encryptedAppPassword.clear();
    lastTimeSync.clear();
    lastTimeExport.clear();


    file.open(QIODevice::ReadWrite); // (Re)create file if not valid
    file.close();

    return false;
}


void SavedData::Save(){
    QFile file("save");
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::critical(nullptr, "Erreur", "Sauvegarde locale des données impossible.");
    }

    else{
        QTextStream out(&file);

        out << accessHash << "\n";
        out << encryptedEmail << "\n";
        out << encryptedAppPassword << "\n";
        out << lastTimeSync << "\n";
        if(!QDate::fromString(lastTimeExport, "yyyy-MM-dd").isValid())
            out << "Never";
        else
            out << lastTimeExport;
    }

    file.close();
}



void SavedData::SetCrypto(SimpleCrypt *crypto, QString email, QString appPassword, QToolButton *syncButton){
    this->crypto = crypto;
    this->syncButton = syncButton;

    if(!email.isEmpty() && !appPassword.isEmpty()){
        encryptedEmail = crypto->encryptToString(email);
        encryptedAppPassword = crypto->encryptToString(appPassword);
    }

    connect(syncButton, &QToolButton::clicked, this, &SavedData::Synchronize);

    if(lastTimeSync == "" || QDateTime::fromString(lastTimeSync, "yyyy-MM-ddTHH:mm:ss").daysTo(QDateTime::currentDateTime()) > 1)
        syncButton->setIcon(QIcon("media/sync_red.svg"));
    else
        syncButton->setIcon(QIcon("media/sync.svg"));
}


void SavedData::SynchronizeWorker(){
    QStringList errors;

    // history
    QFile historyFile("history");
    if (historyFile.exists()){
        errors.append(SendEmail("History", "history"));
        historyFile.remove();

        // data.db
        errors.append(SendEmail("BDD", "data.db"));
    }



    // Prescriptions save
    QDirIterator iterator("prescriptions/", QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while(iterator.hasNext()){
        QString fileName = iterator.next();
        QRegularExpression regex("(\\d{2}-\\d{2}-\\d{4}_\\d{2}-\\d{2}-\\d{2})");
        QRegularExpressionMatch match = regex.match(fileName);
        if (match.hasMatch()) {
             QDateTime dateTime = QDateTime::fromString(match.captured(1), "dd-MM-yyyy_hh-mm-ss");

             if(lastTimeSync == "" || dateTime > QDateTime::fromString(lastTimeSync, "yyyy-MM-ddTHH:mm:ss"))
                 SendEmail("Ordonnance", fileName);
        }
    }

    emit SynchronizationFinished(errors);
}

QString SavedData::SendEmail(QString subject, QString filePath){
    if(crypto){
        QString from = crypto->decryptToString(encryptedEmail);
        QString password = crypto->decryptToString(encryptedAppPassword);
        QString to = from;
        subject += " - " + QDateTime::currentDateTime().toString("yyyy-MM-ddTHH:mm:ss");
        QString body = "";
        QString smtpServer = "smtp.gmail.com";
        int port = 465; // Gmail SMTP port

        // Establish connection
         QSslSocket socket;
         socket.connectToHostEncrypted(smtpServer, port);
         if (!socket.waitForConnected()) {
             if(socket.errorString().contains("Host not found"))
                return "Pas de connexion internet";
             else
                return socket.errorString();
         }

         // Read initial server response
         if (!socket.waitForReadyRead()) {
             return socket.errorString();
         }

         // Send EHLO command
         socket.write("EHLO localhost\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             return socket.errorString();
         }

         // Login with username and password
         QString loginCommand = "AUTH LOGIN\r\n";
         socket.write(loginCommand.toUtf8());
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             return socket.errorString();
         }

         QString username = QByteArray().append(from.toUtf8()).toBase64();
         QString pass = QByteArray().append(password.toUtf8()).toBase64();
         socket.write(username.toUtf8() + "\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             return socket.errorString();
         }

         socket.write(pass.toUtf8() + "\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             return socket.errorString();
         }
         if(socket.readAll().contains("Username and Password not accepted")){
             return "Les identifiants de sauvegarde n'ont pas été acceptés.";
         }

         // Send mail from and to
         socket.write("MAIL FROM: <" + from.toUtf8() + ">\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             return socket.errorString();
         }

         socket.write("RCPT TO: <" + to.toUtf8() + ">\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             return socket.errorString();
         }

         // Send data
         socket.write("DATA\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             return socket.errorString();
         }

         // Send mail content
         QByteArray boundary = "boundary_" + QByteArray::number(QDateTime::currentMSecsSinceEpoch());
         QString header = "From: " + from + "\r\n"
                          "To: " + to + "\r\n"
                          "Subject: " + subject + "\r\n"
                          "MIME-Version: 1.0\r\n"
                          "Content-Type: multipart/mixed; boundary=" + boundary + "\r\n"
                          "\r\n";
         socket.write(header.toUtf8());

         // Body
         QString bodyPart = "--" + boundary + "\r\n"
                            "Content-Type: text/plain; charset=UTF-8\r\n"
                            "\r\n" +
                            body +
                            "\r\n";
         socket.write(bodyPart.toUtf8());

         // Attachment
         QFile file(filePath);
         if (file.open(QIODevice::ReadOnly)) {
             QString attachmentPart = "--" + boundary + "\r\n"
                                      "Content-Type: application/octet-stream\r\n"
                                      "Content-Disposition: attachment; filename=\"" + file.fileName() + ".bs4\"\r\n"
                                      "\r\n";
             socket.write(attachmentPart.toUtf8());
             socket.write(file.readAll().toBase64());
         }
         else{
            return "Fichier " + file.fileName() + " non trouvé.";
         }

         // End of email
         QString endPart = "\r\n--" + boundary + "--\r\n.\r\n";
         socket.write(endPart.toUtf8());

         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             return socket.errorString();
         }

         // Quit
         socket.write("QUIT\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             return socket.errorString();
         }

        socket.close();

        return "";
    }

    return "Crypto non initialisée";
}

void SavedData::Synchronize(){
    if(crypto && !synchronizing){
        // Sync button animation
        synchronizing = true;
        syncMovie = new QMovie;
        syncMovie->setFileName("media/sync.gif");
        QObject::connect(syncMovie, &QMovie::frameChanged, [this](){
            syncButton->setIcon(syncMovie->currentPixmap());
        });

        syncMovie->start();

        QtConcurrent::run(&SavedData::SynchronizeWorker, this);
    }
}
