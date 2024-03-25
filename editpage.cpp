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

void EditPage::SwitchPage(QString pageName){
    resizeEvent(nullptr);
    qobject_cast<QStackedWidget*>(parent())->setCurrentWidget(this);
    findChild<QStackedWidget*>("editStacked")->setCurrentWidget(findChild<QWidget*>(pageName));
}

void EditPage::SetField(QString name, QString value, QWidget* parent){
    if(parent == nullptr)
        parent = this;

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

QString EditPage::GetField(QString name, QWidget* parent){
    if (parent == nullptr)
        parent = this;

    QObject *childObject = parent->findChild<QWidget*>(name);
    QDateEdit *dateEdit = qobject_cast<QDateEdit*>(childObject);
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(childObject);
    QComboBox *box = qobject_cast<QComboBox*>(childObject);

    if(dateEdit)
        return dateEdit->date().toString("yyyy-MM-dd");

    else if (lineEdit)
        return lineEdit->text();

    else if (box)
        return box->currentText();

    return "";
}


void EditPage::Edit(QString type, QStringList infos){
    lastType = type;
    ClearAllPages();
    currentNecessary.clear();

    if((type != "redList" && type != "members") || infos.isEmpty())
        SwitchPage(type + "EditPage");

    removeButton = findChild<QToolButton*>("removeButton");
    removeButton->setVisible(!infos.isEmpty());

    if(type == "entry"){
        findChild<QTabWidget*>("entryTabWidget")->setCurrentIndex(0);

        if(infos.size() > 14){
            currentNecessary.append(infos[0]); // id_ES
            currentNecessary.append(infos[1]); // date_prov


            // Entrée
            SetField("entryDateEdit", infos[1]);

            QString type_prov = infos[2].split("___")[0];
            SetField("entryTypeBox", type_prov);

            if(type_prov == "Fourrière"){
                SetField("poundPlaceEdit", infos[2].split("___")[1]);
            }

            else{
                SetField("lastNameAbandonEdit", infos[3]);
                SetField("firstNameAbandonEdit", infos[4]);

                QStringList addressList = infos[5].split(infos[5].contains("\\n") ? "\\n" : "\n");
                if(addressList.size() < 3)
                    qDebug() << "Wrong address format";

                else{
                    SetField("addressAbandonEdit", addressList[0]);
                    SetField("address2AbandonEdit", addressList[1]);

                    SetField("cityAbandonEdit", addressList[addressList.size() - 1].split(" ")[1]);
                    SetField("postalCodeAbandonEdit", addressList[addressList.size() - 1].split(" ")[0]);
                }

                SetField("phoneAbandonEdit", infos[6]);
                SetField("emailAbandonEdit", infos[7]);
            }

            // Animal
            SetField("dogNameEdit", infos[10]);
            SetField("chipEdit", infos[9]);
            SetField("sexBox", infos[8]);
            SetField("birthDateEdit", infos[12]);
            SetField("breedEdit", infos[11]);


            // Sortie

            QStringList destinations = infos[13].split("_-_");
            QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");

            for (int i = 0; i < destinations.size(); i++){
                QString d = destinations[i];

                if (d != ""){
                    destinationsNumber += 1;
                    AddDestPage();
                    QString iString = QString::number(destinationsNumber);

                    QStringList p = d.split("___");

                    SetField("destTypeBox" + iString, p[1], destStacked);
                    SetField("destDateEdit" + iString, p[0], destStacked);
                    SetField("lastNameDestEdit" + iString, p[2], destStacked);
                    SetField("firstNameDestEdit" + iString, p[3], destStacked);
                    QStringList splitAddress = p[4].split("\n");
                    if(splitAddress.count() == 3){
                        SetField("addressDestEdit" + iString, splitAddress[0], destStacked);
                        SetField("address2DestEdit" + iString, splitAddress[1], destStacked);
                        SetField("postalCodeDestEdit" + iString, splitAddress[2].split(" ")[0], destStacked);
                        SetField("cityDestEdit" + iString, splitAddress[2].split(" ")[1], destStacked);
                    }

                    SetField("phoneDestEdit" + iString, p[5], destStacked);
                    SetField("emailDestEdit" + iString, p[6], destStacked);
                }
            }
        }
    }

    else if (type == "care"){
        findChild<QTabWidget*>("careTabWidget")->setCurrentIndex(0);
        if(infos.size() > 17 && infos[0].toInt() > 0){
            currentNecessary.append(infos[0]); // id_care
            currentNecessary.append(infos[1]); // entry_date

            // Entrée
            SetField("careEntryDateEdit", infos[1]);

            SetField("lastNameCareEntryEdit", infos[2]);
            SetField("firstNameCareEntryEdit", infos[3]);

            QStringList addressList = infos[4].split(infos[4].contains("\\n") ? "\\n" : "\n");
            if(addressList.size() < 3)
                qDebug() << "Wrong address format";

            else{
                SetField("addressCareEntryEdit", addressList[0]);
                SetField("address2CareEntryEdit", addressList[1]);

                SetField("cityCareEntryEdit", addressList[addressList.size() - 1].split(" ")[1]);
                SetField("postalCodeCareEntryEdit", addressList[addressList.size() - 1].split(" ")[0]);
            }

            SetField("phoneCareEntryEdit", infos[5]);
            SetField("emailCareEntryEdit", infos[6]);

            // Animal
            SetField("dogNameCareAnimalEdit", infos[7]);
            SetField("chipCareAnimalEdit", infos[8]);
            SetField("sexCareAnimalEdit", infos[9]);
            SetField("birthDateCareAnimalEdit", infos[10]);
            SetField("descriptionCareAnimalEdit", infos[11]);


            // Sortie
            SetField("careDestDateEdit", infos[12]);
            SetField("lastNameCareDestEdit", infos[13]);
            SetField("firstNameCareDestEdit", infos[14]);

            addressList = infos[15].split(infos[15].contains("\\n") ? "\\n" : "\n");
            if(addressList.size() < 3)
                qDebug() << "Wrong address format";

            else{
                SetField("addressCareDestEdit", addressList[0]);
                SetField("address2CareDestEdit", addressList[1]);

                SetField("cityCareDestEdit", addressList[addressList.size() - 1].split(" ")[1]);
                SetField("postalCodeCareDestEdit", addressList[addressList.size() - 1].split(" ")[0]);
            }

            SetField("phoneCareDestEdit", infos[16]);
            SetField("emailCareDestEdit", infos[17]);
        }

    }

    else if (type == "redList" || type == "members"){
        if(infos.size() > 0){
            currentNecessary = infos;
            RemoveCurrent();
        }
    }

}

void EditPage::ClearAllPages()
{
    // Destinations ES
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    for (int i = destStacked->count() - 1; i >= 0; --i) {
        QWidget *widget = destStacked->widget(i);
        destStacked->removeWidget(widget);
        delete widget;
    }

    qDeleteAll(destStacked->findChildren<DestinationPage*>());

    destinationsNumber = 0;
    AddDestPage();

    for (QWidget* widget : findChildren<QWidget*>()) {
        if (QDateEdit *dateEdit = qobject_cast<QDateEdit*>(widget))
            dateEdit->setDate(QDate::currentDate());
        else if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(widget))
            lineEdit->clear();
        else if (QComboBox *box = qobject_cast<QComboBox*>(widget))
            box->setCurrentIndex(0);
    }
    ChangeEntryType("Abandon");

    UpdateButtons();
}

void EditPage::ChangeEntryType(QString type)
{
    QWidget* entreeTab = findChild<QWidget*>("entryTab1");
    for (QWidget* widget : entreeTab->findChildren<QWidget*>()) {
        if(widget->objectName().startsWith("poundPlaceEdit")) // Pound
            widget->setVisible(type != "Abandon");
        else if(widget->objectName().startsWith("firstNameAbandonEdit") ||
                widget->objectName().startsWith("lastNameAbandonEdit") ||
                widget->objectName().startsWith("phoneAbandonEdit") ||
                widget->objectName().startsWith("emailAbandonEdit")||
                widget->objectName().startsWith("addressAbandonEdit") ||
                widget->objectName().startsWith("address2AbandonEdit") ||
                widget->objectName().startsWith("postalCodeAbandonEdit") ||
                widget->objectName().startsWith("cityAbandonEdit")) // Abandoned
            widget->setVisible(type == "Abandon");
    }
}

void EditPage::SaveEdit()
{
    QSqlQuery query;
    if(lastType == "entry"){
        // Entrée
        QString date_prov = GetField("entryDateEdit");
        QString type_prov = GetField("entryTypeBox");
        QString id_people_prov = "-1";
        QString dogName =  GetField("dogNameEdit");
        if(GetField("entryTypeBox") == "Abandon"){
            id_people_prov = CreatePersonIfNeeded(QStringList({GetField("lastNameAbandonEdit"),
                                    GetField("firstNameAbandonEdit"),
                                    GetField("phoneAbandonEdit"),
                                    GetField("emailAbandonEdit"),
                                    GetField("addressAbandonEdit") + "\n" +
                                    GetField("address2AbandonEdit") + "\n" +
                                    GetField("postalCodeAbandonEdit") + " " +
                                    GetField("cityAbandonEdit")}));
        }

        else{ // Fourrière
            type_prov += "___" + GetField("poundPlaceEdit");
        }

        // Animal
        QString id_dog = CreateDogIfNeeded(QStringList({
                                                           dogName,
                                                           GetField("chipEdit"),
                                                           GetField("sexBox"),
                                                           GetField("breedEdit"),
                                                           GetField("birthDateEdit")
                                                       }));
        // Sortie
        QStringList death_causes, id_peoples, dates, types;

        QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
        for(int i = 1; i < destStacked->count() + 1; i++){
            QString iString = QString::number(i);
            QString type = GetField("destTypeBox" + iString, destStacked);
            if(type != ""){
                types.append(type);
                dates.append(GetField("destDateEdit" + iString, destStacked));

                if(type == "Mort"){
                    death_causes.append(GetField("deathCauseEdit" + iString, destStacked));
                    // TODO : Add a Destination for death
                    id_peoples.append("-1");
                }

                else if(type == "Entrée au refuge"){
                    // TODO : Add custom data about the shelter
                    id_peoples.append("-1");
                }

                else{
                    id_peoples.append(CreatePersonIfNeeded(QStringList({GetField("lastNameDestEdit" + iString, destStacked),
                                                        GetField("firstNameDestEdit" + iString, destStacked),
                                                        GetField("phoneDestEdit" + iString, destStacked),
                                                        GetField("emailDestEdit" + iString, destStacked),
                                                        GetField("addressDestEdit" + iString, destStacked) + "\n" +
                                                        GetField("address2DestEdit" + iString, destStacked) + "\n" +
                                                        GetField("postalCodeDestEdit" + iString, destStacked) + " " +
                                                        GetField("cityDestEdit" + iString, destStacked)})));
                }
            }
        }

        if(death_causes.isEmpty())
            death_causes.append("");


        QString queryString;
        if(!currentNecessary.isEmpty()){ // Modifying
            queryString = "UPDATE ES_Registry "
                          "SET id_dog = " + id_dog + ", "
                          "date_prov = '" + date_prov + "', "
                          "id_people_prov = " + id_people_prov + ", "
                          "type_prov = '" + type_prov + "', "
                          "death_cause = '" + death_causes[0] + "' "
                          "WHERE id_ES = " + currentNecessary[0] +
                          " AND date_prov = '" + currentNecessary[1] + "';";
        }
        else{ // Creating
            queryString = "INSERT INTO ES_Registry (id_dog, type_prov, date_prov, id_people_prov, death_cause) "
                              "VALUES (" +
                              id_dog + ", '" +
                                type_prov + "', '" +
                                date_prov + "', " +
                                id_people_prov + ", '" +
                                death_causes[0] +
                              "');";
        }

        query.exec(queryString);

        query.exec("DELETE FROM Destinations "
                   "WHERE id_dog = " + id_dog + ";");


        for(int i = 0; i < id_peoples.count(); i++){
            query.exec("INSERT INTO Destinations (id_dog, id_people, date, type)"
                       "VALUES (" +
                       id_dog + ", " +
                       id_peoples[i] + ", '" +
                       dates[i] + "', '" +
                       types[i] + "');");
        }

    }

    else if(lastType == "redList"){
        QWidget *redListEditPage = findChild<QWidget*>("redListEditPage");
        QString id_people = CreatePersonIfNeeded(QStringList({GetField("lastNameRedListEdit", redListEditPage),
                                GetField("firstNameRedListEdit", redListEditPage),
                                GetField("phoneRedListEdit", redListEditPage),
                                GetField("emailRedListEdit", redListEditPage),
                                GetField("addressRedListEdit", redListEditPage) + "\n" +
                                GetField("address2RedListEdit", redListEditPage) + "\n" +
                                GetField("postalCodeRedListEdit", redListEditPage) + " " +
                                GetField("cityRedListEdit", redListEditPage)}));
        QString reason = GetField("reasonRedListEdit", redListEditPage);

        query.prepare("INSERT INTO Red_list (id_people, reason) "
                      "VALUES (:id, :reason);");
        query.bindValue(":id", id_people);
        query.bindValue(":reason", reason);
        query.exec();
    }

    if(lastType == "care"){
        QWidget *careEditPage = findChild<QWidget*>("careEditPage");
        // Entrée
        QString entry_date = GetField("careEntryDateEdit", careEditPage);
        QString id_people_prov = CreatePersonIfNeeded(QStringList({GetField("lastNameCareEntryEdit", careEditPage),
                                GetField("firstNameCareEntryEdit", careEditPage),
                                GetField("phoneCareEntryEdit", careEditPage),
                                GetField("emailCareEntryEdit", careEditPage),
                                GetField("addressCareEntryEdit", careEditPage) + "\n" +
                                GetField("address2CareEntryEdit", careEditPage) + "\n" +
                                GetField("postalCodeCareEntryEdit", careEditPage) + " " +
                                GetField("cityCareEntryEdit", careEditPage)}));

        // Animal
        QString id_dog = CreateDogIfNeeded(QStringList({
                                                           GetField("dogNameCareAnimalEdit", careEditPage),
                                                           GetField("chipCareAnimalEdit", careEditPage),
                                                           GetField("sexCareAnimalEdit", careEditPage),
                                                           GetField("descriptionCareAnimalEdit", careEditPage),
                                                           GetField("birthDateCareAnimalEdit", careEditPage)
                                                       }));

        // Sortie
        QString exit_date = GetField("careDestDateEdit", careEditPage);

        QString id_people_dest = CreatePersonIfNeeded(QStringList({GetField("lastNameCareDestEdit", careEditPage),
                                    GetField("firstNameCareDestEdit", careEditPage),
                                    GetField("phoneCareDestEdit", careEditPage),
                                    GetField("emailCareDestEdit", careEditPage),
                                    GetField("addressCareDestEdit", careEditPage) + "\n" +
                                    GetField("address2CareDestEdit", careEditPage) + "\n" +
                                    GetField("postalCodeCareDestEdit", careEditPage) + " " +
                                    GetField("cityCareDestEdit", careEditPage)}));

        QString queryString;
        if(!currentNecessary.isEmpty()){ // Modifying
            queryString = "UPDATE Care_registry "
                          "SET id_dog = " + id_dog + ", "
                          "entry_date = '" + entry_date + "', "
                          "id_people_prov = " + id_people_prov + ", "
                          "exit_date = '" + exit_date + "', "
                          "id_people_dest = " + id_people_dest + " "
                          "WHERE id_care = " + currentNecessary[0] +
                          " AND entry_date = '" + currentNecessary[1] + "';";
        }

        else{ // Creating
            queryString = "INSERT INTO Care_registry (id_dog, entry_date, id_people_prov, exit_date, id_people_dest) "
                          "VALUES (" + id_dog + ", '" + entry_date + "', " + id_people_prov + ", '" + exit_date + "', " + id_people_dest + ")";
        }

        query.exec(queryString);
    }


    QuitEdit();
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


    query.exec("INSERT INTO People (id_people, last_name, first_name, address, phone, email) "
               "VALUES ('" + newId + "', '" + infos[0] + "', '" + infos[1] + "', '" + infos[4] + "', '" + infos[2] + "', '" + infos[3] + "')");

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


    query.exec("INSERT INTO Dogs (id_dog, name, sex, chip, description, birth) "
               "VALUES ('" + newId + "', '" + infos[0] + "', '" + infos[2] + "', '" + infos[1] + "', '" + infos[3] + "', '" + infos[4] + "')");

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
            if(!c->objectName().contains("spinbox") && (qobject_cast<QLineEdit*>(c) || qobject_cast<QDateTimeEdit*>(c) || qobject_cast<QComboBox*>(c) || qobject_cast<QDoubleSpinBox*>(c))){
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
    if(lastType == "entry" || lastType == "care"){
        QTabBar* bar = findChild<QWidget*>(lastType + "EditPage")->findChild<QTabBar*>();
        QFont font = bar->font();
        font.setPointSize(width() * 0.01);
        bar->setFont(font);
    }
}

void EditPage::UpdateDestinationPages(QString type){
    // Update stacked widget
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    int currentPage = destStacked->currentIndex() + 1;
    if(type != ""){
        if(currentPage > destinationsNumber){
            destinationsNumber += 1;
            AddDestPage();
        }
    }

    else{
        // Clear pages after current
        destinationsNumber = currentPage - 1;

        for (int i = destStacked->count() - 1; i >= currentPage; i--)
            destStacked->removeWidget(destStacked->widget(i));
    }

    UpdateButtons();
}

void EditPage::UpdateButtons(){
    QToolButton* prevButton = findChild<QToolButton*>("prevDestButton");
    QToolButton* nextButton = findChild<QToolButton*>("nextDestButton");
    int currentPage = findChild<QStackedWidget*>("destStackedWidget")->currentIndex() + 1;

    if (currentPage <= 1){ // Gray prev button
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect(prevButton);
        effect->setColor(Qt::gray);
        prevButton->setGraphicsEffect(effect);
    }

    else
        prevButton->setGraphicsEffect(nullptr);

    if (currentPage > destinationsNumber){ // Gray next button
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect(nextButton);
        effect->setColor(Qt::gray);
        nextButton->setGraphicsEffect(effect);
    }

    else
        nextButton->setGraphicsEffect(nullptr);

    resizeEvent(nullptr);
}

void EditPage::PrevDestPage(){
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    int currentPage = destStacked->currentIndex() + 1;

    if(currentPage > 1){
        currentPage -= 1;
        destStacked->setCurrentIndex(currentPage - 1);
        UpdateButtons();
    }
}

void EditPage::NextDestPage(){
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    int currentPage = destStacked->currentIndex() + 1;

    if(currentPage < destStacked->count()){
        currentPage += 1;
        destStacked->setCurrentIndex(currentPage - 1);
        UpdateButtons();
    }
}

void EditPage::AddDestPage(){
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    DestinationPage* page = new DestinationPage(QString::number(destinationsNumber + 1));
    destStacked->insertWidget(destinationsNumber, page);
    page->ChangeDestType("");
    connect(page->findChild<QComboBox*>(), SIGNAL(currentTextChanged(QString)), page, SLOT(ChangeDestType(QString)));
    connect(page->findChild<QComboBox*>(), SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateDestinationPages(QString)));
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

        query.exec();

        QuitEdit();
    }
}
