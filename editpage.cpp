#include "editpage.h"

EditPage::EditPage(QWidget *parent)
    : QWidget{parent}
{

}

void EditPage::AddEntry(){
    Edit("entry", {});
}

void EditPage::SwitchPage(QString pageName){
        qobject_cast<QStackedWidget*>(parent())->setCurrentWidget(this);
        findChild<QStackedWidget*>("editStacked")->setCurrentWidget(findChild<QWidget*>(pageName));
}

void EditPage::Edit(QString type, QStringList infos){
    lastType = type;
    ClearAllPages();

    if(type == "entry"){
        SwitchPage("editEntryPage");

        QWidget* entreeTab = findChild<QWidget*>("entryTab1");

        if(infos.size() > 14){
            // Entrée
            entreeTab->findChild<QDateEdit*>("entryDateEdit")->setDate(QDate::fromString(infos[1], "yyyy-MM-dd"));
            entreeTab->findChild<QComboBox*>("entryTypeBox")->setCurrentText(infos[2]);
            entreeTab->findChild<QLineEdit*>("lastNameAbandonEdit")->setText(infos[3]);
            entreeTab->findChild<QLineEdit*>("firstNameAbandonEdit")->setText(infos[4]);

            QStringList addressList = infos[5].split(infos[5].contains("\\n") ? "\\n" : "\n");
            if(addressList.size() < 3)
                qDebug() << "Wrong address format";

            else{
                entreeTab->findChild<QLineEdit*>("addressAbandonEdit")->setText(addressList[0]);
                entreeTab->findChild<QLineEdit*>("address2AbandonEdit")->setText(addressList[1]);

                entreeTab->findChild<QLineEdit*>("cityAbandonEdit")->setText(addressList[addressList.size() - 1].split(" ")[0]);
                entreeTab->findChild<QLineEdit*>("postalCodeAbandonEdit")->setText(addressList[addressList.size() - 1].split(" ")[1]);
            }

            entreeTab->findChild<QLineEdit*>("phoneAbandonEdit")->setText(infos[6]);
            entreeTab->findChild<QLineEdit*>("emailAbandonEdit")->setText(infos[7]);

            // Animal

            // Sortie
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
                widget->objectName() == "addressAbandonEdit" ||
                widget->objectName() == "address2AbandonEdit" ||
                widget->objectName() == "postalCodeAbandonEdit" ||
                widget->objectName() == "cityAbandonEdit") // Abandoned
            widget->setVisible(type == "Abandon");
    }
}

void EditPage::SaveEdit()
{
    QSqlQuery query;
    if(lastType == "entry"){
        QWidget* entreeTab = findChild<QWidget*>("entryTab1");
        QString date_prov = entreeTab->findChild<QDateEdit*>("entryDateEdit")->date().toString("yyyy-MM-dd");
        QString id_dog = "1";
        QString id_people_prov = CreatePersonIfNeeded(QStringList({entreeTab->findChild<QLineEdit*>("lastNameAbandonEdit")->text(),
                                    entreeTab->findChild<QLineEdit*>("firstNameAbandonEdit")->text(),
                                    entreeTab->findChild<QLineEdit*>("phoneAbandonEdit")->text(),
                                    entreeTab->findChild<QLineEdit*>("emailAbandonEdit")->text(),
                                    entreeTab->findChild<QLineEdit*>("addressAbandonEdit")->text() + "\n" +
                                    entreeTab->findChild<QLineEdit*>("address2AbandonEdit")->text() + "\n" +
                                    entreeTab->findChild<QLineEdit*>("postalCodeAbandonEdit")->text() + " " +
                                    entreeTab->findChild<QLineEdit*>("cityAbandonEdit")->text()}));

        query.exec("SELECT id_ES "
                   "FROM ES_Registry "
                   "WHERE id_dog = '" + id_dog + "' "
                   "AND date_prov = '" + date_prov + "';");


        QString queryString;
        if(query.next()){
            queryString = "UPDATE ES_Registry "
                          "SET id_dog = '" + id_dog + "', "
                          "date_prov = '" + date_prov + "', "
                          "id_people_prov = " + id_people_prov + ", "
                          "type_prov = '" + entreeTab->findChild<QComboBox*>("entryTypeBox")->currentText() + "' "
                          "WHERE id_ES = '" + query.value(0).toString() + "';";
        }
        else
            queryString = "INSERT INTO ES_Registry (id_dog, type_prov, date_prov, id_people_prov, death_cause) "
                              "VALUES ("
                              "1, '" +
                                entreeTab->findChild<QComboBox*>("entryTypeBox")->currentText() + "', '" +
                                date_prov + "', " +
                                id_people_prov +
                              ", '' "
                              ");";

        qDebug() << queryString;

        query.exec(queryString);
    }

    QuitEdit();
}

void EditPage::QuitEdit()
{
    QStackedWidget* stackedWidget = qobject_cast<QStackedWidget*>(parent());
    stackedWidget->setCurrentWidget(stackedWidget->findChild<QWidget*>(lastType + "RegistryPage"));
}

// Returns id_people newly created, or already existent
QString EditPage::CreatePersonIfNeeded(QStringList infos){ // infos = last_name, first_name, phone, email, address
    QSqlQuery query;
    query.exec("SELECT id_people, phone, email FROM People "
               "WHERE last_name = '" + infos[0] + "' "
               "AND first_name = '" + infos[1] + "';");
    if (query.next()){
        if(query.value(1).toString() == ""
                || query.value(2).toString() == ""
                || infos[2] == ""
                || infos[3] == ""
                || query.value(1).toString() == infos[2]
                || query.value(2).toString() == infos[3])

            // Update if new
            if(query.value(1).toString() == "")
                query.exec("UPDATE People "
                           "SET phone = '" + infos[2] +
                           "';");
            if(query.value(2).toString() == "")
                query.exec("UPDATE People "
                           "SET email = '" + infos[3] +
                           "';");

            return query.value(0).toString();
    }

    query.clear();

    query.exec("SELECT MAX(id_people) + 1 FROM People;");

    QString newId;
    if (query.next() && query.value(0).toString() != "")
        newId = query.value(0).toString();
    else
        newId = "1";


    query.exec("INSERT INTO People (id_people, last_name, first_name, address, phone, email)"
               "VALUES ('" + newId + "', '" + infos[0] + "', '" + infos[1] + "', '" + infos[4] + "', '" + infos[2] + "', '" + infos[3] + "')");

    return newId;
}
