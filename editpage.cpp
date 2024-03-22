#include "editpage.h"

EditPage::EditPage(QWidget *parent)
    : QWidget{parent}
{

}

void EditPage::AddEntry(){
    currentId = -1;
    Edit("entry", {});
}

void EditPage::SwitchPage(QString pageName){
    resizeEvent(nullptr);
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
        SwitchPage("entryEditPage");
        findChild<QTabWidget*>("entryTabWidget")->setCurrentIndex(0);


        if(infos.size() > 14){
            currentId = infos[0].toInt();

            // Entrée
            SetField("entryDateEdit", infos[1]);

            QString type_prov = infos[2].split(";-;")[0];
            SetField("entryTypeBox", type_prov);

            if(type_prov == "Fourrière"){
                SetField("poundPlaceEdit", infos[2].split(";-;")[1]);
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
            SetField("deathCauseEdit", infos[14]);
        }
    }
}

void EditPage::ClearAllPages()
{
    for (QWidget* widget : findChildren<QWidget*>()) {
        if (QDateEdit *dateEdit = qobject_cast<QDateEdit*>(widget))
            dateEdit->setDate(QDate::currentDate());
        else if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(widget))
            lineEdit->clear();
        else if (QComboBox *box = qobject_cast<QComboBox*>(widget))
            box->setCurrentIndex(0);
    }
    ChangeEntryType("Abandon");


    // Destinations ES
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    while (destStacked->count() > 0) {
        QWidget* widget = destStacked->widget(0);
        destStacked->removeWidget(widget);
        delete widget;
    }

    destinationsNumber = 0;
    AddDestPage();
}

void EditPage::ChangeDestType(QString type){
    // Update dest pages accordingly
    /*QString currentDestType = findChild<QComboBox*>("destTypeBox")->currentText();
    int currentPage = findChild<QStackedWidget*>("destStackedWidget")->currentIndex() + 1;

    if(currentDestType == ""){
        destinationsNumber = currentPage;
    }

    else if(currentPage == destinationsNumber){
        // Create new dest page
        destinationsNumber += 1;

        QStackedWidget* stacked = findChild<QStackedWidget*>("destStackedWidget");
        DestinationPage* newPage = new DestinationPage(stacked);
        stacked->insertWidget(destinationsNumber - 1, newPage);
    }


    UpdateDestinationPages();*/
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
        if(GetField("entryTypeBox") == "Abandon")
            id_people_prov = CreatePersonIfNeeded(QStringList({GetField("lastNameAbandonEdit"),
                                    GetField("firstNameAbandonEdit"),
                                    GetField("phoneAbandonEdit"),
                                    GetField("emailAbandonEdit"),
                                    GetField("addressAbandonEdit") + "\n" +
                                    GetField("address2AbandonEdit") + "\n" +
                                    GetField("postalCodeAbandonEdit") + " " +
                                    GetField("cityAbandonEdit")}));

        else{ // Fourrière
            type_prov += ";-;" + GetField("poundPlaceEdit");
        }

        // Animal
        QString id_dog = CreateDogIfNeeded(QStringList({
                                                           GetField("dogNameEdit"),
                                                           GetField("chipEdit"),
                                                           GetField("sexBox"),
                                                           GetField("breedEdit"),
                                                           GetField("birthDateEdit")
                                                       }));

        // Sortie
        QString death_cause = GetField("deathCauseEdit");


        QString queryString;
        if(currentId >= 0){ // Modifying
            queryString = "UPDATE ES_Registry "
                          "SET id_dog = '" + id_dog + "', "
                          "date_prov = '" + date_prov + "', "
                          "id_people_prov = " + id_people_prov + ", "
                          "type_prov = '" + type_prov + "', "
                          "death_cause = '" + death_cause + "' "
                          "WHERE id_ES = '" + QString::number(currentId) + "';";
        }
        else // Creating
            queryString = "INSERT INTO ES_Registry (id_dog, type_prov, date_prov, id_people_prov, death_cause) "
                              "VALUES (" +
                              id_dog + ", '" +
                                type_prov + "', '" +
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
    QList<QWidget*> children = findChild<QWidget*>(lastType + "EditPage")->findChildren<QWidget*>();

    if(children.size() > 0){
        QFont font = children[0]->font();
        float fontSize = std::max(width() * 0.007, (double) 10);
        font.setPointSize(fontSize);

        for(QWidget* c : children){
            if(!c->objectName().contains("spinbox") && (qobject_cast<QLineEdit*>(c) || qobject_cast<QDateTimeEdit*>(c) || qobject_cast<QComboBox*>(c))){
                c->setFont(font);
                c->setStyleSheet("padding:" + QString::number(fontSize) + "px;");
            }

            else if (qobject_cast<QLabel*>(c))
                c->setFont(font);
        }
    }

    // Margins besides the form
    QMargins margins = layout()->contentsMargins();
    margins.setLeft(0.2 * width());
    margins.setRight(0.2 * width());
    layout()->setContentsMargins(margins);

    // Tab header
    QTabBar* bar = findChild<QWidget*>(lastType + "EditPage")->findChild<QTabBar*>();
    QFont font = bar->font();
    font.setPointSize(width() * 0.02);
    bar->setFont(font);
}

void EditPage::UpdateDestinationPages(QString type){
    // Update stacked widget
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    int currentPage = destStacked->currentIndex() + 1;

    if(type != ""){
        if(currentPage >= destinationsNumber){
            destinationsNumber += 1;
            AddDestPage();
        }
    }

    else{
        // Clear pages after current
        destinationsNumber = currentPage - 1;

        for(int i = currentPage + 1; i < destStacked->count(); i++){
            destStacked->removeWidget(destStacked->widget(i));
        }
    }

    // Update buttons
    QToolButton* prevButton = findChild<QToolButton*>("prevDestButton");
    QToolButton* nextButton = findChild<QToolButton*>("nextDestButton");

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

}

void EditPage::PrevDestPage(){
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    int currentPage = destStacked->currentIndex() + 1;

    if(currentPage > 1){
        currentPage -= 1;
        destStacked->setCurrentIndex(currentPage - 1);
    }

    UpdateDestinationPages(destStacked->widget(currentPage - 1)->findChild<QComboBox*>()->currentText());
}

void EditPage::NextDestPage(){
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    int currentPage = destStacked->currentIndex() + 1;

    if(currentPage < destStacked->count()){
        currentPage += 1;
        destStacked->setCurrentIndex(currentPage - 1);
    }

    UpdateDestinationPages(destStacked->widget(currentPage - 1)->findChild<QComboBox*>()->currentText());
}

void EditPage::AddDestPage(){
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    DestinationPage* page = new DestinationPage();
    destStacked->insertWidget(destinationsNumber, page);
    page->ChangeDestType("");
    connect(page->findChild<QComboBox*>(), SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateDestinationPages(QString)));
    connect(page->findChild<QComboBox*>(), SIGNAL(currentTextChanged(QString)), page, SLOT(ChangeDestType(QString)));
}
