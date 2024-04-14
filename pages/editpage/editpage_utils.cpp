#include "editpage.h"


QStringList EditPage::AddressList(QString address){
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

void EditPage::SetField(QString name, QString value, QWidget* parent){
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

QString EditPage::GetField(QString name, QWidget* parent){
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

bool EditPage::DateComparator(const QString& dest1, const QString& dest2){
    QStringList p1 = dest1.split("_|_");
    QStringList p2 = dest2.split("_|_");

    QDate date1 = QDate::fromString(p1[0], "yyyy-MM-dd");
    QDate date2 = QDate::fromString(p2[0], "yyyy-MM-dd");

    return date1 < date2;

}
