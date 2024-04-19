#include "editpage.h"


void EditPage::ChangeEntryType(QString type)
{
    QWidget* entreeTab = findChild<QWidget*>("entryTab1");
    for (QWidget* widget : entreeTab->findChildren<QWidget*>()) {
        if(widget->objectName().startsWith("otherTypeEdit")) // Pound || Cession
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

    if(type == "Fourrière")
        entreeTab->findChild<QLabel*>("otherTypeEditLabel")->setText("LIEU");
    else if(type == "Cession")
        entreeTab->findChild<QLabel*>("otherTypeEditLabel")->setText("ENTITÉ");
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
        if (CustomDateTimeEdit *dateEdit = qobject_cast<CustomDateTimeEdit*>(widget)){
            dateEdit->SetDate(QDate::currentDate());
        }
        else if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(widget))
            lineEdit->clear();
        else if (QComboBox *box = qobject_cast<QComboBox*>(widget))
            box->setCurrentIndex(0);
        else if (QTextEdit *textEdit = qobject_cast<QTextEdit*>(widget))
            textEdit->clear();
    }
    ChangeEntryType("Abandon");

    findChild<QWidget*>("reasonVetAnimalEdit")->hide();
    for(ClickableLabel *c : findChildren<ClickableLabel*>())
        if(c->objectName().startsWith("dogGroupedVetLabel"))
            c->deleteLater();

    UpdateButtons();

    for(EditDogWidget *c : findChildren<EditDogWidget*>())
        c->SetOldId("-1");

    for(EditPeopleWidget *c : findChildren<EditPeopleWidget*>())
        c->SetOldId("-1");
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

    for(EditPeopleWidget *c : findChildren<EditPeopleWidget*>()){
        c->SetCrypto(crypto);
    }
}

void EditPage::GroupedVet(){
    QWidget *vetEditPage = findChild<QWidget*>("vetEditPage");
    groupedVetIds.clear();
    findChild<QPushButton*>("groupedVetButton")->hide();
    findChild<QPushButton*>("groupedPdfButton")->show();

    for(QWidget *c : vetEditPage->findChildren<QWidget*>()){
        QString name = c->objectName();
        if (name.contains("VetAnimalEdit") && !name.startsWith("dogNameVetAnimalEdit") && name != "VetAnimalEdit" && !name.startsWith("date") && !name.startsWith("reason"))
            c->hide();
    }

    EditDogWidget *editWidget = vetEditPage->findChild<EditDogWidget*>();
    disconnect(editWidget, nullptr, nullptr, nullptr);

    connect(editWidget, &EditDogWidget::SelectedDog, [this, vetEditPage]{
        QTimer::singleShot(100, [=]{
            QString dogName = GetField("dogNameVetAnimalEdit", vetEditPage);
            QString chip = GetField("chipVetAnimalEdit", vetEditPage);
            QString sex = GetField("sexVetAnimalEdit", vetEditPage);
            QString birthDate = GetField("birthDateVetAnimalEdit", vetEditPage);
            QString description = GetField("descriptionVetAnimalEdit", vetEditPage);
            qDebug() << dogName;

            QSqlQuery query;
            query.prepare("SELECT id_dog "
                          "FROM Dogs "
                          "WHERE name = :dogName "
                          "AND chip = :chip "
                          "AND sex = :sex "
                          "AND birth = :birthDate "
                          "AND description = :description");

            query.bindValue(":dogName", dogName);
            query.bindValue(":chip", chip);
            query.bindValue(":sex", sex);
            query.bindValue(":birthDate", birthDate);
            query.bindValue(":description", description);

            HandleErrorExec(&query);
            if(query.next()){
                QString id_dog = query.value(0).toString();
                AddVetLabel(id_dog, dogName, chip, sex, birthDate, description);
            }

            QLineEdit *edit = vetEditPage->findChild<QLineEdit*>("dogNameVetAnimalEdit");
            edit->clear();
            edit->setPlaceholderText("");

            resizeEvent(nullptr);
        });
    });

    if(editWidget->findChild<QLineEdit*>("dogNameVetAnimalEdit")->text() != "")
        editWidget->SelectedDog();
}

void EditPage::AddVetLabel(QString id_dog, QString dogName, QString chip, QString sex, QString birthDate, QString description){
    if(!groupedVetIds.contains(id_dog)){
        groupedVetIds.append(id_dog);

        ClickableLabel *label = new ClickableLabel();
        label->setText(dogName + "|" + chip + "|" + sex + "|" + QDate::fromString(birthDate, "yyyy-MM-dd").toString("dd/MM/yyyy") + "|" + description);

        connect(label, &ClickableLabel::clicked, this, [label, id_dog, this](){
            groupedVetIds.removeOne(id_dog);
            label->deleteLater();
        });
        label->setObjectName("dogGroupedVetLabel");
        qobject_cast<QVBoxLayout*>(findChild<QWidget*>("groupedVetScrollWidget")->layout())->insertWidget(0, label);

    }
}
