#include "editpage.h"

EditPage::EditPage(QWidget *parent)
    : QWidget{parent}
{

}

void EditPage::AddEntry(){
    Edit("entry", {});
}

void EditPage::AddRedList(){
    Edit("redList", {});
}

void EditPage::AddCare(){
    Edit("care", {});
}

void EditPage::AddMember(){
    Edit("members", {});
}

void EditPage::AddLost(){
    Edit("lost", {});
}

void EditPage::SwitchPage(QString pageName){
    resizeEvent(nullptr);
    qobject_cast<QStackedWidget*>(parent())->setCurrentWidget(this);
    findChild<QStackedWidget*>("editStacked")->setCurrentWidget(findChild<QWidget*>(pageName));
}

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

// Returns id_people newly created, or already existent
QString EditPage::CreatePersonIfNeeded(QStringList infos){ // infos = last_name, first_name, phone, email, address
    QSqlQuery query;

    query.exec("SELECT id_people FROM People "
               "WHERE last_name = '" + infos[0] + "' "
               "AND first_name = '" + infos[1] + "' "
               "AND phone = '" + infos[2] + "' "
               "AND email = '" + infos[3] + "' "
               "AND address = '" + infos[4] + "'");

    if (query.next())
        return query.value(0).toString();


    query.clear();

    query.exec("SELECT MAX(MAX(id_people) + 1, 1) FROM People;");

    query.next();
    QString newId = query.value(0).toString();
    if (newId == "")
        newId = "1";


    query.prepare("INSERT INTO People (id_people, last_name, first_name, address, phone, email) "
                  "VALUES (:id, :last_name, :first_name, :address, :phone, :email)");

    query.bindValue(":id", newId);
    query.bindValue(":last_name", infos[0]);
    query.bindValue(":first_name", infos[1]);
    query.bindValue(":address", infos[4]);
    query.bindValue(":phone", infos[2]);
    query.bindValue(":email", infos[3]);
    query.exec();

    return newId;
}

// Returns id_dog newly created, or already existent
QString EditPage::CreateDogIfNeeded(QStringList infos){ // infos = name, chip, sex, description, birth
    QSqlQuery query;

    query.exec("SELECT id_dog FROM Dogs "
                   "WHERE chip = '" + infos[1] + "' "
                   "AND name = '" + infos[0] + "' "
                   "AND sex = '" + infos[2] + "' "
                   "AND description = '" + infos[3] + "' "
                   "AND birth = '" + infos[4] + "'");

    if (query.next())
        return query.value(0).toString();


    query.clear();

    query.exec("SELECT MAX(id_dog) + 1 FROM Dogs;");

    query.next();
    QString newId = query.value(0).toString();
    if (newId == "")
        newId = "1";


    query.prepare("INSERT INTO Dogs (id_dog, name, sex, chip, description, birth) "
                  "VALUES (:id, :name, :sex, :chip, :description, :birth)");

    query.bindValue(":id", newId);
    query.bindValue(":name", infos[0]);
    query.bindValue(":sex", infos[2]);
    query.bindValue(":chip", infos[1]);
    query.bindValue(":description", infos[3]);
    query.bindValue(":birth", infos[4]);

    query.exec();
    return newId;
}

void EditPage::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    QList<QWidget*> children = findChild<QWidget*>(lastType + "EditPage")->findChildren<QWidget*>();

    if(children.size() > 0){
        QFont font = children[0]->font();
        float fontSize = std::max(width() * 0.007, (double) 9);
        font.setPointSize(fontSize);

        for(QWidget* c : children){
            if(!c->objectName().contains("spinbox") && (qobject_cast<QLineEdit*>(c) || qobject_cast<QDateTimeEdit*>(c) || qobject_cast<QComboBox*>(c) || qobject_cast<QDoubleSpinBox*>(c) || qobject_cast<QCheckBox*>(c))){
                c->setFont(font);
                c->setStyleSheet("padding:" + QString::number(fontSize) + "px;");
            }

            else if (qobject_cast<QLabel*>(c))
                c->setFont(font);
        }

        font.setPointSize(fontSize * 1.2);

        for(QPushButton *b : findChildren<QPushButton*>())
        {
            b->setFont(font);
            b->setStyleSheet("padding:" + QString::number(fontSize) + "px;");
        }
    }



    // Margins besides the form
    QMargins margins = layout()->contentsMargins();
    margins.setLeft(0.2 * width());
    margins.setRight(0.2 * width());
    layout()->setContentsMargins(margins);

    // Tab header
    if(lastType == "entry" || lastType == "care" || lastType == "lost"){
        QTabBar* bar = findChild<QWidget*>(lastType + "EditPage")->findChild<QTabBar*>();
        QFont font = bar->font();
        font.setPointSize(width() * 0.01);
        bar->setFont(font);
    }
}

void EditPage::SameDestCare(){
    QWidget *careEditPage = findChild<QWidget*>("careEditPage");
    SetField("firstNameCareDestEdit", GetField("firstNameCareEntryEdit", careEditPage), careEditPage);
    SetField("lastNameCareDestEdit", GetField("lastNameCareEntryEdit", careEditPage), careEditPage);
    SetField("phoneCareDestEdit", GetField("phoneCareEntryEdit", careEditPage), careEditPage);
    SetField("emailCareDestEdit", GetField("emailCareEntryEdit", careEditPage), careEditPage);
    SetField("addressCareDestEdit", GetField("addressCareEntryEdit", careEditPage), careEditPage);
    SetField("address2CareDestEdit", GetField("address2CareEntryEdit", careEditPage), careEditPage);
    SetField("postalCodeCareDestEdit", GetField("postalCodeCareEntryEdit", careEditPage), careEditPage);
    SetField("cityCareDestEdit", GetField("cityCareEntryEdit", careEditPage), careEditPage);

}

void EditPage::QuitEdit()
{
    QStackedWidget* stackedWidget = qobject_cast<QStackedWidget*>(parent());
    if(lastType == "entry" || lastType == "care")
        stackedWidget->setCurrentWidget(stackedWidget->findChild<QWidget*>(lastType + "RegistryPage"));

    else
        stackedWidget->setCurrentWidget(stackedWidget->findChild<QWidget*>(lastType + "Page"));

    emit RefreshMainWindow(lastType);
}


void EditPage::RemoveCurrent(){
    QMessageBox::StandardButton reply;

    QString message;

    if(lastType == "entry")
        message = "Voulez-vous supprimer cette entrée du registre E/S?";
    else if (lastType == "care")
        message = "Voulez-vous supprimer cette entrée du registre de Garderie ?";
    else if (lastType == "redList")
        message = "Voulez-vous supprimer cette personne de la liste rouge ?";
    else if (lastType == "members")
        message = "Voulez-vous supprimer cette personne du registre des adhérents ?";
    else if (lastType == "lost")
        message = "Voulez-vous supprimer cette entrée des animaux perdus ?";

    reply = QMessageBox::question(nullptr, "Confirmation de suppression", message,
                                              QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes){
        QSqlQuery query;
        if(lastType == "entry"){
            query.prepare("DELETE FROM ES_Registry "
                          "WHERE id_ES = :id "
                          "AND date_prov = :date;");
            query.bindValue(":id", currentNecessary[0]);
            query.bindValue(":date", currentNecessary[1]);
        }

        else if(lastType == "care"){
            query.prepare("DELETE FROM Care_registry "
                          "WHERE id_care = :id "
                          "AND entry_date = :date;");
            query.bindValue(":id", currentNecessary[0]);
            query.bindValue(":date", currentNecessary[1]);
        }

        else if(lastType == "redList"){
            query.prepare("DELETE FROM Red_list "
                       "WHERE id_people = :id;");
            query.bindValue(":id", currentNecessary[0]);
        }

        else if(lastType == "members"){
            query.prepare("DELETE FROM Members "
                          "WHERE id_adhesion = :id "
                          "AND date = :date;");
            query.bindValue(":id", currentNecessary[0]);
            query.bindValue(":date", currentNecessary[1]);
        }

        else if(lastType == "lost"){
            query.prepare("DELETE FROM Lost "
                          "WHERE name = :name "
                          "AND date = :date "
                          "AND id_people = :id;");
            query.bindValue(":name", currentNecessary[0]);
            query.bindValue(":date", currentNecessary[1]);
            query.bindValue(":id", currentNecessary[2]);
        }

        query.exec();

        QuitEdit();
    }
}

