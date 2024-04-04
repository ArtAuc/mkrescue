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


inline void SetField(QString name, QString value, QWidget* parent){
    QObject *childObject = parent->findChild<QWidget*>(name);
    QDateEdit *dateEdit = qobject_cast<QDateEdit*>(childObject);
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(childObject);
    QComboBox *box = qobject_cast<QComboBox*>(childObject);

    if(dateEdit)
        dateEdit->setDate(QDate::fromString(value, "yyyy-MM-dd"));

    else if (lineEdit)
        lineEdit->setText(value);

    else if (box)
        box->setCurrentText(value);
}

inline QString GetField(QString name, QWidget* parent){
    QObject *childObject = parent->findChild<QWidget*>(name);
    QDateEdit *dateEdit = qobject_cast<QDateEdit*>(childObject);
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(childObject);
    QComboBox *box = qobject_cast<QComboBox*>(childObject);
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(childObject);

    if(dateEdit)
        return dateEdit->date().toString("yyyy-MM-dd");
    else if (lineEdit)
        return lineEdit->text();
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
        if(!query->exec())
            QMessageBox::critical(nullptr, "Erreur d'exécution SQL", query->lastError().text());
    }

    else{
        if(!query->exec(queryString))
            QMessageBox::critical(nullptr, "Erreur d'exécution SQL", query->lastError().text());
    }
}

inline bool DoesPeopleExist(QString id_people){
    QSqlQuery query;
    HandleErrorExec(&query, "SELECT COUNT(*) FROM People WHERE id_people = " + id_people);
    query.next();
    return query.value(0).toInt() > 0;
}


#endif // UTILS_H
