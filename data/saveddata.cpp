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
                syncButton->setIcon(QIcon("media/sync_red.svg"));
                success = false;
                break;
            }
        }

        if(success){
            QMessageBox::information(nullptr, "Succès", "La sauvegarde a réussi.");
            QFile("history").remove();
            lastTimeSync = QDateTime::currentDateTime().toString("yyyy-MM-ddTHH:mm:ss");
        }

        Save();

        synchronizing = false;
    });
}

bool SavedData::Load(){
    QFile bodyFile("recall_email.txt");
    if (bodyFile.open(QIODevice::ReadOnly)) {
        recallMailBody = bodyFile.readAll();
    }

    QFile file("save");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);

        accessHash = in.readLine();
        encryptedEmail = in.readLine();
        encryptedAppPassword = in.readLine();
        lastTimeSync = in.readLine();
        lastTimeExport = in.readLine();
        maxDogs = in.readLine();
        maxDays = in.readLine();


        if(!accessHash.isEmpty() && !encryptedEmail.isEmpty() && !encryptedAppPassword.isEmpty() && !lastTimeSync.isEmpty() && !lastTimeExport.isEmpty() && !maxDogs.isEmpty() && !maxDays.isEmpty()){
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

        out << "\n" + maxDogs + "\n";
        out << maxDays;
    }

    file.close();
}



void SavedData::Init(QString email, QString appPassword, QToolButton *syncButton){
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
        errors.append(SynchronizeEmail("History", "history"));

        // data.db
        errors.append(SynchronizeEmail("BDD", "data.db"));
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
                 SynchronizeEmail("Ordonnance", fileName);
        }
    }

    // Members recall mails
    if(recallMailBody != ""){
        QFile membersFile("members");
        if(!membersFile.exists()){
            membersFile.open(QIODevice::WriteOnly);
            membersFile.close();
        }

        QStringList alreadySentMails = {};
        QStringList datesSent = {};
        membersFile.open(QIODevice::ReadOnly);
        for(QString line : QString(membersFile.readAll()).split("\n")){
            QDate date = QDate::fromString(line.split("_|_")[0], "dd/MM/yyyy");
            if (date.addMonths(2) > QDate::currentDate()){
                datesSent.append(date.toString("dd/MM/yyyy"));
                alreadySentMails.append(crypto->decryptToString(line.split("_|_")[1]));
            }
        }

        membersFile.close();


        for(QString toSend : emailsToSend){
            QStringList infos = toSend.split("_|_");
            if(!alreadySentMails.contains(infos[3])){ // Has mail already been sent ?
                QThread::sleep(5);
                errors.append(SendMembersEmail(infos[3], infos[1], infos[2], infos[0]));
                datesSent.append(infos[0]);
                alreadySentMails.append(infos[3]);
            }
        }

        // Save sent mails to file
        membersFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
        for(int i = 0; i < datesSent.length(); i++){
            membersFile.write(QString(datesSent[i] + "_|_" + crypto->encryptToString(alreadySentMails[i]) + "\n").toUtf8());
        }

       membersFile.close();

    }

    emit SynchronizationFinished(errors);
}

void SavedData::Synchronize(){
    if(crypto && !synchronizing){
        synchronizing = true;

        emailsToSend = {};
        QSqlQuery query;
        HandleErrorExec(&query, "SELECT result.latest_exp_date, result.first_name, result.last_name, result.email FROM ("
                                    "SELECT MAX(exp_date) AS latest_exp_date, subquery.first_name, subquery.last_name, subquery.email "
                                    "FROM ( "
                                        "SELECT DATE(Members.date, '+1 year') AS exp_date, People.first_name, People.last_name, People.email "
                                        "FROM People "
                                        "JOIN Members ON Members.id_people = People.id_people "
                                        "AND People.email LIKE '%@%.%'"
                                    ") AS subquery "
                                    "GROUP BY subquery.email) AS result "
                                "WHERE result.latest_exp_date BETWEEN DATE('now') AND DATE('now', '+1 month')");
        while (query.next()) {
            QString expDate = query.value(0).toDate().toString("dd/MM/yyyy");
            QString firstName = query.value(1).toString();
            QString lastName = query.value(2).toString();
            QString email = query.value(3).toString();

            QString fullNameWithEmail = QString("%1_|_%2_|_%3_|_%4").arg(expDate, firstName, lastName, email);

            emailsToSend.append(fullNameWithEmail);
        }


        // Sync button animation
        syncMovie = new QMovie;
        syncMovie->setFileName("media/sync.gif");
        QObject::connect(syncMovie, &QMovie::frameChanged, [this](){
            syncButton->setIcon(syncMovie->currentPixmap());
        });

        syncMovie->start();

        auto future = QtConcurrent::run(&SavedData::SynchronizeWorker, this);
        Q_UNUSED(future);
    }
}
