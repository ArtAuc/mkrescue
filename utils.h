#ifndef UTILS_H
#define UTILS_H

#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QRegularExpression>
#include <QFile>
#include <QDateTime>


inline void HandleErrorExec(QSqlQuery *query, QString queryString = NULL){
    if(queryString.isNull()){
        if(!query->exec()){
            QMessageBox::critical(nullptr, "Erreur d'exécution SQL", query->lastError().text() + "\n" + query->executedQuery());
        }
    }

    else{
        if(!query->exec(queryString)){
            QMessageBox::critical(nullptr, "Erreur d'exécution SQL", query->lastError().text() + "\n" + query->executedQuery());
        }
    }

    // Save query into history
    queryString = query->executedQuery();

    QRegularExpression abandonRegex("Abandon de .* le ..\\/..\\/....");
    if(query->boundValues().size() == 0 || abandonRegex.match(query->boundValue(1).toString()).hasMatch() || queryString.contains("SET address = :empty"))
        return; // Filter out some unwanted queries

    if(queryString.contains("INSERT INTO") || queryString.contains("UPDATE") || queryString.contains("DELETE FROM")){
        QFile file("history");
        if (!file.exists()) {
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QMessageBox::critical(nullptr, "Erreur d'exécution SQL", "Le fichier d'historique n'a pas pu être créé.");
                return;
            }
            file.close();
        }

        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << QDateTime::currentDateTime().toString("yyyy-MM-ddTHH:mm:ss") << "\n";
            out << queryString;
            out << ";\n\n";

            QString values;
            for (QVariant v : query->boundValues())
                values += v.toString() + ", ";

            values.chop(2);

            out << values << "\n\n;;;;;\n\n";
            file.close();
        }
        else
            QMessageBox::critical(nullptr, "Erreur d'exécution SQL", "Le fichier d'historique n'a pas pu être ouvert.");
    }
}


inline QString AutoBreak(QString s, int threshold){
    QString result;
    while(s.length() > threshold) {
        int pos = -1; // First ' ' or '-' until threshold
        for (int i = 0; i < threshold && i < s.length(); ++i) {
            if (s[i] == ' ' || s[i] == '-' || s[i] == '/') {
                pos = i;
            }
        }

        if (pos != -1){
            if(s[pos] == '/')
                s.insert(pos, '\n');
            else
                s.insert(pos + 1, '\n');
        }
        else
            s.insert(threshold, '\n');

        result += s.split("\n")[0] + "\n";
        s = s.split("\n")[1];
    }

    result += s;

    return result;
}

inline QStringList UniqueList(QStringList list){
    QSet<QString> set;
    for(QString s : list)
        set.insert(s);

    return set.values();
}

inline QString GetAge(QDate birth){
    QString s;

    int years = QDate::currentDate().year() - birth.year();
    int months = QDate::currentDate().month() - birth.month();

    if (QDate::currentDate().day() < birth.day())
        months--;

    if (months < 0) {
        years--;
        months += 12;
    }

    if (years == 1)
        s = "1 an, ";
    else if (years > 1)
        s = QString::number(years) + " ans, ";

    s += QString::number(months) + " mois";

    return s;
}

#endif // UTILS_H
