#include "editpage.h"

EditPage::EditPage(QWidget *parent)
    : QWidget{parent}
{

}

void EditPage::SwitchPage(QString pageName){
    resizeEvent(nullptr);
    qobject_cast<QStackedWidget*>(parent())->setCurrentWidget(this);
    findChild<QStackedWidget*>("editStacked")->setCurrentWidget(findChild<QWidget*>(pageName));
}

void EditPage::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    QList<QWidget*> children = findChild<QWidget*>(lastType + "EditPage")->findChildren<QWidget*>();

    if(children.size() > 0){
        QFont font = children[0]->font();
        float fontSize = width() * 0.007;
        font.setPointSize(fontSize);

        for(QWidget* c : children){
            if(!c->objectName().contains("spinbox") && (qobject_cast<QLineEdit*>(c) || qobject_cast<QTextEdit*>(c) || qobject_cast<QComboBox*>(c) || qobject_cast<QDoubleSpinBox*>(c) || qobject_cast<QCheckBox*>(c))){
                c->setFont(font);
                c->setStyleSheet("padding:" + QString::number(fontSize / 2) + "px;");
            }

            else if (qobject_cast<QLabel*>(c))
                c->setFont(font);
        }

        font.setPointSize(fontSize * 1.2);

        for(QPushButton *b : findChildren<QPushButton*>())
        {
            b->setFont(font);
            if(!b->objectName().startsWith("submitButton"))
                b->setStyleSheet("padding:" + QString::number(fontSize) + "px;");
            else
                b->setStyleSheet("QPushButton{padding:" + QString::number(fontSize) + "px;background:#749674;} QPushButton:hover{background:#91ac91}");
        }
    }



    // Margins besides the form
    QMargins margins = layout()->contentsMargins();
    margins.setLeft(0.2 * width());
    margins.setRight(0.2 * width());
    layout()->setContentsMargins(margins);

    // Tab header
    if(lastType == "entry" || lastType == "care" || lastType == "lost" || lastType == "adoptionDemand"){
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
    cleaning = true;
    emit RefreshMainWindow();
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
    else if (lastType == "vet")
        message = "Voulez-vous supprimer ce RDV Vétérinaire ?";
    else if (lastType == "adoptionDemand")
        message = "Voulez-vous supprimer cette demande d'adoption ?";

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
            HandleErrorExec(&query);

            query.prepare("DELETE FROM Destinations "
                          "WHERE id_dog = :id "
                          "AND date_prov = :date;");
            query.bindValue(":id", currentNecessary[0]);
            query.bindValue(":date", currentNecessary[1]);
            HandleErrorExec(&query);
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

        else if(lastType == "vet"){
            query.prepare("DELETE FROM Vet "
                          "WHERE date = :date "
                          "AND id_dog = :id_dog;");
            query.bindValue(":date", currentNecessary[0]);
            query.bindValue(":id_dog", currentNecessary[1]);
        }

        else if(lastType == "adoptionDemand"){
            query.prepare("DELETE FROM Adoption_demand "
                          "WHERE breed = :breed "
                          "AND id_people = :id_people;");
            query.bindValue(":breed", currentNecessary[0]);
            query.bindValue(":id_people", currentNecessary[1]);
        }

        HandleErrorExec(&query);

        QuitEdit();
    }
}

void EditPage::AssignIdPeople(QWidget *currentPage){
    if(currentPage == nullptr)
        return;

    QSqlQuery query;
    for(EditPeopleWidget *editPeopleWidget : currentPage->findChildren<EditPeopleWidget*>()){
        editPeopleWidget->SetCrypto(crypto);

        query.prepare("SELECT id_people "
                      "FROM People "
                      "WHERE last_name = :last_name "
                      "AND first_name = :first_name "
                      "AND address = :address "
                      "AND phone = :phone "
                      "AND email = :email;");


        QString lastName = GetField("lastName" + editPeopleWidget->objectName(), editPeopleWidget);
        QString firstName = GetField("firstName" + editPeopleWidget->objectName(), editPeopleWidget);
        QString address = GetField("address" + editPeopleWidget->objectName(), editPeopleWidget) + "\n" +
                          GetField("address2" + editPeopleWidget->objectName(), editPeopleWidget) + "\n" +
                          GetField("postalCode" + editPeopleWidget->objectName(), editPeopleWidget) + " " + GetField("city" + editPeopleWidget->objectName(), editPeopleWidget);
        QString phone = GetField("phone" + editPeopleWidget->objectName(), editPeopleWidget);
        QString email = GetField("email" + editPeopleWidget->objectName(), editPeopleWidget);

        query.bindValue(":last_name", lastName);
        query.bindValue(":first_name", firstName);
        query.bindValue(":address", crypto->encryptToString(address));
        query.bindValue(":phone", phone);
        query.bindValue(":email", email);

        HandleErrorExec(&query);

        query.next();
        editPeopleWidget->SetOldId(query.value(0).toString());
    }
}

void EditPage::AssignIdDog(QWidget *currentPage) {
    if (currentPage == nullptr)
        return;

    QSqlQuery query;
    for (EditDogWidget *editDogWidget : currentPage->findChildren<EditDogWidget*>()) {
        query.prepare("SELECT id_dog "
                      "FROM Dogs "
                      "WHERE name = :name "
                      "AND sex = :sex "
                      "AND chip = :chip "
                      "AND description = :description "
                      "AND birth = :birth;");

        QString name = GetField("dogName" + editDogWidget->objectName(), editDogWidget);
        QString sex = GetField("sex" + editDogWidget->objectName(), editDogWidget);
        QString chip = GetField("chip" + editDogWidget->objectName(), editDogWidget);
        QString description = GetField("description" + editDogWidget->objectName(), editDogWidget);
        QString birth = GetField("birthDate" + editDogWidget->objectName(), editDogWidget);

        query.bindValue(":name", name);
        query.bindValue(":sex", sex);
        query.bindValue(":chip", chip);
        query.bindValue(":description", description);
        query.bindValue(":birth", birth);

        HandleErrorExec(&query);

        if(query.next()) // If not the case, probably creating a new dog
            editDogWidget->SetOldId(query.value(0).toString());
    }
}
