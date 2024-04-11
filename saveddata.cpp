#include "saveddata.h"

SavedData::SavedData()
{
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
        out << encryptedAppPassword;
    }

    file.close();
}

void SavedData::SetCrypto(SimpleCrypt *crypto, QString email, QString appPassword){
    this->crypto = crypto;

    if(!email.isEmpty() && !appPassword.isEmpty()){
        encryptedEmail = crypto->encryptToString(email);
        encryptedAppPassword = crypto->encryptToString(appPassword);
    }
}

bool SavedData::Load(){
    QFile file("save");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);

        accessHash = in.readLine();
        encryptedEmail = in.readLine();
        encryptedAppPassword = in.readLine();

        if(!accessHash.isEmpty() && !encryptedEmail.isEmpty() && !encryptedAppPassword.isEmpty()){
            file.close();

            return true;
        }

        file.close();
    }

    file.open(QIODevice::ReadWrite); // (Re)create file if not valid
    file.close();

    return false;
}

void SavedData::SendEmail(QString subject, QString filePath){
    if(crypto){
        QString from = crypto->decryptToString(encryptedEmail);
        QString to = from;
        subject += " " + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH:mm:ss");
        QString body = "";
        QString password = crypto->decryptToString(encryptedAppPassword);
        QString smtpServer = "smtp.gmail.com";
        int port = 465; // Gmail SMTP port

        // Establish connection
         QSslSocket socket;
         socket.connectToHostEncrypted(smtpServer, port);
         if (!socket.waitForConnected()) {
             qDebug() << "Error: " << socket.errorString();
             return;
         }

         // Read initial server response
         if (!socket.waitForReadyRead()) {
             qDebug() << "Error: " << socket.errorString();
             return;
         }
         qDebug() << "Response:" << socket.readAll();

         // Send EHLO command
         socket.write("EHLO localhost\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             qDebug() << "Error: " << socket.errorString();
             return;
         }
         qDebug() << "Response:" << socket.readAll();

         // Login with username and password
         QString loginCommand = "AUTH LOGIN\r\n";
         socket.write(loginCommand.toUtf8());
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             qDebug() << "Error: " << socket.errorString();
             return;
         }
         qDebug() << "Response:" << socket.readAll();

         QString username = QByteArray().append(from.toUtf8()).toBase64();
         QString pass = QByteArray().append(password.toUtf8()).toBase64();
         socket.write(username.toUtf8() + "\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             qDebug() << "Error: " << socket.errorString();
             return;
         }
         qDebug() << "Response:" << socket.readAll();

         socket.write(pass.toUtf8() + "\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             qDebug() << "Error: " << socket.errorString();
             return;
         }
         qDebug() << "Response:" << socket.readAll();

         // Send mail from and to
         socket.write("MAIL FROM: <" + from.toUtf8() + ">\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             qDebug() << "Error: " << socket.errorString();
             return;
         }
         qDebug() << "Response:" << socket.readAll();

         socket.write("RCPT TO: <" + to.toUtf8() + ">\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             qDebug() << "Error: " << socket.errorString();
             return;
         }
         qDebug() << "Response:" << socket.readAll();

         // Send data
         socket.write("DATA\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             qDebug() << "Error: " << socket.errorString();
             return;
         }
         qDebug() << "Response:" << socket.readAll();

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
             file.close();
         }

         // End of email
         QString endPart = "\r\n--" + boundary + "--\r\n.\r\n";
         socket.write(endPart.toUtf8());

         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             qDebug() << "Error: " << socket.errorString();
             return;
         }
         qDebug() << "Response:" << socket.readAll();

         // Quit
         socket.write("QUIT\r\n");
         if (!socket.waitForBytesWritten() || !socket.waitForReadyRead()) {
             qDebug() << "Error: " << socket.errorString();
             return;
         }
         qDebug() << "Response:" << socket.readAll();

        socket.close();
    }
}

void SavedData::Synchronize(){
}
