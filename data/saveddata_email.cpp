#include "saveddata.h"

QString SavedData::SynchronizeEmail(QString subject, QString filePath){
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


QString SavedData::SendMembersEmail(QString address, QString firstName, QString lastName, QString expirationDate){
    if(crypto){
        QString from = crypto->decryptToString(encryptedEmail);
        QString password = crypto->decryptToString(encryptedAppPassword);
        QString to = address;
        QString displayFrom = shelterName;
        QString subject = "Rappel d'Adhésion - " + shelterName;
        QString body = recallMailBody.replace("$prenom_dest", firstName).replace("$nom_fam_dest", lastName).replace("$date_exp", expirationDate).replace("$nom_refuge", shelterName).replace("$telephone_refuge", shelterPhone).replace("$adresse_refuge", crypto->decryptToString(shelterAddress));
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
         QString header = "From: " + displayFrom + " <" + from + ">\r\n"
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
