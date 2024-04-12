#ifndef UTILS_H
#define UTILS_H

#include <QSqlDatabase>
#include <QMainWindow>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QWidget>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QDateEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QLayout>
#include <QSpinBox>
#include <QLabel>
#include <QToolButton>
#include <QGraphicsColorizeEffect>
#include <QSqlError>
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>
#include <QCheckBox>
#include <QTextEdit>


inline void SetField(QString name, QString value, QWidget* parent){
    QObject *childObject = parent->findChild<QWidget*>(name);
    QDateEdit *dateEdit = qobject_cast<QDateEdit*>(childObject);
    QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(childObject);
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(childObject);
    QTextEdit *textEdit = qobject_cast<QTextEdit*>(childObject);
    QComboBox *box = qobject_cast<QComboBox*>(childObject);
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(childObject);

    if(dateEdit)
        dateEdit->setDate(QDate::fromString(value, "yyyy-MM-dd"));
    if(dateTimeEdit)
        dateTimeEdit->setDateTime(QDateTime::fromString(value, "yyyy-MM-ddTHH:mm"));
    else if (lineEdit)
        lineEdit->setText(value);
    else if (textEdit)
        textEdit->setText(value);
    else if (box)
        box->setCurrentText(value);
    else if (spinBox)
        spinBox->setValue(value.toDouble());
}

inline QString GetField(QString name, QWidget* parent){
    QObject *childObject = parent->findChild<QWidget*>(name);
    QDateEdit *dateEdit = qobject_cast<QDateEdit*>(childObject);
    QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(childObject);
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(childObject);
    QTextEdit *textEdit = qobject_cast<QTextEdit*>(childObject);
    QComboBox *box = qobject_cast<QComboBox*>(childObject);
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(childObject);

    if(dateEdit)
        return dateEdit->date().toString("yyyy-MM-dd");
    if(dateEdit)
        return dateTimeEdit->dateTime().toString("yyyy-MM-ddTHH:mm");
    else if (lineEdit)
        return lineEdit->text();
    else if (textEdit)
        return textEdit->toPlainText();
    else if (box)
        return box->currentText();
    else if (spinBox)
        return QString::number(spinBox->value());

    return "";
}

inline QStringList AddressList(QString address){
    QStringList addressList;

    addressList = address.split(address.contains("\\n") ? "\\n" : "\n");

    if(addressList.size() != 3){
        QMessageBox::critical(nullptr, "Erreur", "Erreur dans le format de l'adresse postale");
        return QStringList({"", "", "", ""});
    }

    else{
        QStringList postalCity = addressList[2].split(" ");
        if(postalCity.size() != 2){
            QMessageBox::critical(nullptr, "Erreur", "Erreur dans le format du code postal");
            return QStringList({"", "", "", ""});
        }

        addressList[2] = postalCity[0];
        addressList.append(postalCity[1]);
    }

    return addressList;
}

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

inline bool IsInTable(QString table, QString attribute, QString value){
    QSqlQuery query;
    HandleErrorExec(&query, "SELECT COUNT(*) FROM " + table + " WHERE " + attribute + " = " + value);
    query.next();
    return query.value(0).toInt() > 0;
}

inline auto dateComparator = [](const QString& dest1, const QString& dest2) {
    QStringList p1 = dest1.split("_|_");
    QStringList p2 = dest2.split("_|_");

    QDate date1 = QDate::fromString(p1[0], "yyyy-MM-dd");
    QDate date2 = QDate::fromString(p2[0], "yyyy-MM-dd");

    return date1 < date2;
};

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


#endif // UTILS_H
