#ifndef UTILS_H
#define UTILS_H


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


#endif // UTILS_H
