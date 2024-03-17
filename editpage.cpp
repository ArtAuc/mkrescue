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

void EditPage::SetField(QString name, QString value){
    QObject *childObject = findChild<QWidget*>(name);
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

QString EditPage::GetField(QString name){
    QObject *childObject = findChild<QWidget*>(name);
    QDateEdit *dateEdit = qobject_cast<QDateEdit*>(childObject);
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(childObject);
    QComboBox *box = qobject_cast<QComboBox*>(childObject);

    if(dateEdit)
        return dateEdit->date().toString("yyyy-MM-dd");

    else if (lineEdit)
        return lineEdit->text();

    else if (box)
        return box->currentText();
}


void EditPage::Edit(QString type, QStringList infos){
    lastType = type;
    ClearAllPages();

    if(type == "entry"){
        SwitchPage("editEntryPage");
        findChild<QTabWidget*>("entryTabWidget")->setCurrentIndex(0);


        if(infos.size() > 14){
            // Entrée
            SetField("entryDateEdit", infos[1]);
            SetField("entryTypeBox", infos[2]);
            SetField("lastNameAbandonEdit", infos[3]);
            SetField("firstNameAbandonEdit", infos[4]);

            QStringList addressList = infos[5].split(infos[5].contains("\\n") ? "\\n" : "\n");
            if(addressList.size() < 3)
                qDebug() << "Wrong address format";

            else{
                SetField("addressAbandonEdit", addressList[0]);
                SetField("address2AbandonEdit", addressList[1]);

                SetField("cityAbandonEdit", addressList[addressList.size() - 1].split(" ")[0]);
                SetField("postalCodeAbandonEdit", addressList[addressList.size() - 1].split(" ")[1]);
            }

            SetField("phoneAbandonEdit", infos[6]);
            SetField("emailAbandonEdit", infos[7]);

            // Animal
            SetField("dogNameEdit", infos[10]);
            SetField("chipEdit", infos[9]);
            SetField("sexBox", infos[8]);
            SetField("birthDateEdit", infos[12]);
            SetField("breedEdit", infos[11]);


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
        // Entrée
        QString date_prov = GetField("entryDateEdit");
        QString id_people_prov = CreatePersonIfNeeded(QStringList({GetField("lastNameAbandonEdit"),
                                    GetField("firstNameAbandonEdit"),
                                    GetField("phoneAbandonEdit"),
                                    GetField("emailAbandonEdit"),
                                    GetField("addressAbandonEdit") + "\n" +
                                    GetField("address2AbandonEdit") + "\n" +
                                    GetField("postalCodeAbandonEdit") + " " +
                                    GetField("cityAbandonEdit")}));

        // Animal
        QString id_dog = CreateDogIfNeeded(QStringList({
                                                           GetField("dogNameEdit"),
                                                           GetField("chipEdit"),
                                                           GetField("sexBox"),
                                                           GetField("breedEdit"),
                                                           GetField("birthDateEdit")
                                                       }));


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
                          "type_prov = '" + GetField("entryTypeBox") + "' "
                          "WHERE id_ES = '" + query.value(0).toString() + "';";
        }
        else
            queryString = "INSERT INTO ES_Registry (id_dog, type_prov, date_prov, id_people_prov, death_cause) "
                              "VALUES (" +
                              id_dog + ", '" +
                                GetField("entryTypeBox") + "', '" +
                                date_prov + "', " +
                                id_people_prov +
                              ", '' "
                              ");";

        query.exec(queryString);
    }

    QuitEdit();
}

void EditPage::QuitEdit()
{
    QStackedWidget* stackedWidget = qobject_cast<QStackedWidget*>(parent());
    stackedWidget->setCurrentWidget(stackedWidget->findChild<QWidget*>(lastType + "RegistryPage"));

    RefreshMainWindow(lastType);
}

// Returns id_people newly created, or already existent
QString EditPage::CreatePersonIfNeeded(QStringList infos){ // infos = last_name, first_name, phone, email, address
    QSqlQuery query;

    // Matching first_name and last_name
    query.exec("SELECT id_people, phone, email FROM People "
               "WHERE last_name = '" + infos[0] + "' "
               "AND first_name = '" + infos[1] + "';");
    if (query.next()){
        QString oldPhone = query.value(1).toString();
        QString oldAddress = query.value(2).toString();
        QString id = query.value(0).toString();
        if((oldPhone == infos[2] && oldPhone != "")
                || (oldAddress == infos[3] && oldAddress != "") || (oldAddress == "" && oldPhone == "")){

            // Update if new
            query.exec("UPDATE People "
                           "SET phone = '" + infos[2] + "' "
                           "WHERE id_people = " + id +
                           ";");
            query.exec("UPDATE People "
                           "SET email = '" + infos[3] + "' "
                           "WHERE id_people = " + id +
                           ";");

            return id;
        }
    }

    query.clear();

    // TODO : Match phone numbers

    query.exec("SELECT MAX(id_people) + 1 FROM People;");

    query.next();
    QString newId = query.value(0).toString();
    if (newId == "")
        newId = "1";


    query.exec("INSERT INTO People (id_people, last_name, first_name, address, phone, email) "
               "VALUES ('" + newId + "', '" + infos[0] + "', '" + infos[1] + "', '" + infos[4] + "', '" + infos[2] + "', '" + infos[3] + "')");

    return newId;
}

// Returns id_dog newly created, or already existent
QString EditPage::CreateDogIfNeeded(QStringList infos){ // infos = name, chip, sex, description, birth
    QSqlQuery query;

    // Matching chip
    if(infos[1].length() == 15){
        query.exec("SELECT id_dog FROM Dogs "
                   "WHERE chip = '" + infos[1] + "';");
        if (query.next()){
            QString id = query.value(0).toString();

            query.exec("UPDATE Dogs "
                          "SET name = '" + infos[0] + "', "
                          "sex = '" + infos[2] + "', "
                          "description = '" + infos[3] + "', "
                          "birth = '" + infos[4] + "' "
                          "WHERE chip = '" + infos[1] + "'");

            return id;
        }
    }


    query.clear();

    query.exec("SELECT MAX(id_dog) + 1 FROM Dogs;");

    query.next();
    QString newId = query.value(0).toString();
    if (newId == "")
        newId = "1";


    query.exec("INSERT INTO Dogs (id_dog, name, sex, chip, description, birth) "
               "VALUES ('" + newId + "', '" + infos[0] + "', '" + infos[2] + "', '" + infos[1] + "', '" + infos[3] + "', '" + infos[4] + "')");

    return newId;
}
