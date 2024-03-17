#include "editpage.h"

EditPage::EditPage(QWidget *parent)
    : QWidget{parent}
{

}

void EditPage::AddEntry(){
    Edit("entry", ";");
}

void EditPage::SwitchPage(QString pageName){
        qobject_cast<QStackedWidget*>(parent())->setCurrentWidget(this);
        findChild<QStackedWidget*>("editStacked")->setCurrentWidget(findChild<QWidget*>(pageName));
}

void EditPage::Edit(QString type, QString infos){
    ClearAllPages();
    if(type == "entry"){ // infos = id_ES;date_prov
        SwitchPage("editEntryPage");

        QString id_ES = infos.split(";")[0];
        QString date_prov = infos.split(";")[1];

        QSqlQuery query;
        query.exec("SELECT * FROM ES_Registry WHERE date_prov = " + date_prov + " AND id_ES = " + id_ES);

        QWidget* entreeTab = findChild<QWidget*>("entryTab1");


        if(query.next()){
            // Entrée
            entreeTab->findChild<QDateEdit*>("entryDateEdit")->setDate(QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"));
        }
    }
}

void EditPage::ClearAllPages()
{
    for (auto widget : findChildren<QWidget*>()) {
        if (QDateEdit *dateEdit = qobject_cast<QDateEdit *>(widget))
            dateEdit->setDate(QDate::currentDate());
        else if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget))
            lineEdit->clear();
        else if (QComboBox *box = qobject_cast<QComboBox *>(widget))
            box->setCurrentText("Abandon");
    }
    ChangeEntryType("Abandon");
}

void EditPage::ChangeEntryType(QString type)
{
    QWidget* entreeTab = findChild<QWidget*>("entryTab1");
    for (auto widget : entreeTab->findChildren<QWidget*>()) {
        if(widget->objectName() == "poundPlaceEdit") // Pound
            widget->setVisible(type != "Abandon");
        else if(widget->objectName() == "firstNameAbandonEdit" ||
                widget->objectName() == "lastNameAbandonEdit" ||
                widget->objectName() == "phoneAbandonEdit" ||
                widget->objectName() == "emailAbandonEdit" ||
                widget->objectName() == "addressAbandonEdit_2" ||
                widget->objectName() == "address2AbandonEdit" ||
                widget->objectName() == "postalCodeAbandonEdit" ||
                widget->objectName() == "cityAbandonEdit") // Abandoned
            widget->setVisible(type == "Abandon");
    }
}
