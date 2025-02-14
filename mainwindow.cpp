#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    screenGeometry = QApplication::primaryScreen()->geometry();
    setWindowTitle("MKRescue");
    savedData = new SavedData();
    ui->settingsPage->SetSavedData(savedData);
    ui->homePage->SetSavedData(savedData);

    ui->stackedWidget->setCurrentWidget(ui->loginPage);
    ToggleLock();

    ui->loginPage->SetMode(!savedData->Load());

    connect(ui->loginPage, SIGNAL(Unlock(QByteArray, QString, QString)), this, SLOT(ToggleLock(QByteArray, QString, QString)));


    connect(ui->menuButton, SIGNAL(clicked(bool)), ui->menuTree, SLOT(Toggle()));
    connect(ui->menuButton, SIGNAL(clicked(bool)), this, SLOT(ToggleModifyButtons()));
    connect(ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(Search(QString)));
    connect(ui->searchIcon, SIGNAL(clicked(bool)), ui->searchLine, SLOT(setFocus()));
    connect(ui->yearBox, &QComboBox::currentTextChanged, this, [=]() {Search();});
    ui->searchIcon->setIcon(QIcon("media/search.svg"));
    connect(ui->menuLogoLabel, &ClickableLabel::clicked, this, [=](){ui->menuTree->setCurrentItem(ui->menuTree->topLevelItem(0));}); // Go back to homepage if menu logo clicked

    InitExportButtons();

    // editPage
    connect(ui->entryAddButton, &QToolButton::clicked, this, [=](){ TriggerEdit("entry", {}); });
    connect(ui->redListAddButton, &QToolButton::clicked, this, [=](){ TriggerEdit("redList", {}); });
    connect(ui->careAddButton, &QToolButton::clicked, this, [=](){ TriggerEdit("care", {}); });
    connect(ui->membersAddButton, &QToolButton::clicked, this, [=](){ TriggerEdit("members", {}); });
    connect(ui->lostAddButton, &QToolButton::clicked, this, [=](){ TriggerEdit("lost", {}); });
    connect(ui->vetAddButton, &QToolButton::clicked, this, [=](){ TriggerEdit("vet", {}); });
    connect(ui->adoptionDemandAddButton, &QToolButton::clicked, this, [=](){ TriggerEdit("adoptionDemand", {}); });
    connect(ui->sponsorsAddButton, &QToolButton::clicked, this, [=](){ TriggerEdit("sponsors", {}); });
    connect(ui->entryTypeBox, SIGNAL(currentTextChanged(QString)), ui->editPage, SLOT(ChangeEntryType(QString)));
    connect(ui->submitButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(SaveEdit()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(ChangePage()));
    connect(ui->removeButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(RemoveCurrent()));
    connect(ui->editPage, SIGNAL(RefreshMainWindow()), this, SLOT(Clean()));
    connect(ui->editPage, SIGNAL(RefreshMainWindow()), this, SLOT(ChangePage()));
    connect(ui->prevDestButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(PrevDestPage()));
    connect(ui->nextDestButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(NextDestPage()));
    connect(ui->sameCareButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(SameDestCare()));
    connect(ui->foundLostBox, SIGNAL(clicked(bool)), this, SLOT(ToggleFoundBoxText()));
    connect(ui->satisfiedAdoptionDemandBox, SIGNAL(clicked(bool)), this, SLOT(ToggleSatisfiedBoxText()));
    connect(ui->reasonVetAnimalBox, SIGNAL(currentTextChanged(QString)), this, SLOT(ToggleReasonEdit()));
    connect(ui->homePage, SIGNAL(TriggerEditHome(QString,QStringList)), this, SLOT(TriggerEdit(QString,QStringList)));
    connect(ui->groupedVetButton, &QPushButton::clicked, ui->editPage, &EditPage::GroupedVet);
    connect(ui->groupedPdfButton, &QPushButton::clicked, this, &MainWindow::ExportGroupedVet);
    ui->removeButton->setIcon(QIcon("media/trash.svg"));
    ui->dateVetAnimalEdit->EnableTime();

    InitEditWidgets();

    // Filter checkboxes
    connect(ui->outCheckbox, &QCheckBox::clicked, this, [this]() {LoadDogCards(ui->searchLine->text());});
    connect(ui->careCheckbox, &QCheckBox::clicked, this, [this]() {LoadDogCards(ui->searchLine->text());});
    connect(ui->foundCheckBox, &QCheckBox::clicked, this, [this]() {LoadLost(ui->searchLine->text());});
    connect(ui->satisfiedCheckBox, &QCheckBox::clicked, this, [this]() {LoadAdoptionDemand(ui->searchLine->text());});
    connect(ui->oldCheckBox, &QCheckBox::clicked, this, [this]() {LoadVet(ui->searchLine->text());});
    connect(ui->pastSponsorsCheckbox, &QCheckBox::clicked, this, [this]() {LoadSponsors(ui->searchLine->text());});


    ui->entryRegistryPage->SetType("entry");
    ui->entryLabelLayout->setAlignment(Qt::AlignLeft);

    ui->careRegistryPage->SetType("care");
    ui->careLabelLayout->setAlignment(Qt::AlignLeft);

    ui->membersPage->SetType("members");
    ui->redListPage->SetType("redList");
    ui->lostPage->SetType("lost");
    ui->vetPage->SetType("vet");
    ui->adoptionDemandPage->SetType("adoptionDemand");
    ui->sponsorsPage->SetType("sponsors");
}


MainWindow::~MainWindow()
{
    delete ui;
    delete crypto;
}

void MainWindow::InitEditWidgets(){
    QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->entryTab1->layout());
    if(gridLayout){ // Insert people entry editor
        gridLayout->addWidget(new EditPeopleWidget("AbandonEdit"), gridLayout->rowCount(), 1, 1, 2);
        gridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding), gridLayout->rowCount(), 1, 1, 2);
    }
    ui->entryTab2->layout()->addWidget(new EditDogWidget("EntryAnimalEdit"));

    ui->redListEditPage->layout()->addWidget(new EditPeopleWidget("RedListEdit"));

    gridLayout = qobject_cast<QGridLayout*>(ui->careTab1->layout());
    if(gridLayout){
        gridLayout->addWidget(new EditPeopleWidget("CareEntryEdit"), gridLayout->rowCount(), 1, 1, 2);
        gridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding), gridLayout->rowCount(), 1, 1, 2);
    }

    ui->careTab2->layout()->addWidget(new EditDogWidget("CareAnimalEdit"));

    gridLayout = qobject_cast<QGridLayout*>(ui->careTab3->layout());
    if(gridLayout){
        gridLayout->addWidget(new EditPeopleWidget("CareDestEdit"), gridLayout->rowCount(), 0, 1, 2);
        gridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding), gridLayout->rowCount(), 0, 1, 2);
    }


    gridLayout = qobject_cast<QGridLayout*>(ui->membersEditPage->layout());
    if(gridLayout){
        gridLayout->addWidget(new EditPeopleWidget("MembersEdit"), gridLayout->rowCount(), 0, 1, 2);
    }


    ui->lostTab2->layout()->addWidget(new EditPeopleWidget("LostOwnerEdit"));

    gridLayout = qobject_cast<QGridLayout*>(ui->vetEditPage->layout());
    if(gridLayout){
        gridLayout->addWidget(new EditDogWidget("VetAnimalEdit"), 1, 0, 1, 2);
    }

    ui->adoptionDemandTab2->layout()->addWidget(new EditPeopleWidget("AdoptionDemandEdit"));

    gridLayout = qobject_cast<QGridLayout*>(ui->sponsorsTab1->layout());
    if(gridLayout){
        gridLayout->addWidget(new EditDogWidget("SponsorsAnimalEdit"), 1, 0, 1, 4);
    }
    ui->sponsorsTab2->layout()->addWidget(new EditPeopleWidget("SponsorsEdit"));
    ui->endDateSponsorsEdit->SetInvalidable();


    ui->careTab2->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->entryTab2->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->lostTab1->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->lostTab2->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->adoptionDemandTab1->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->adoptionDemandTab2->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->sponsorsTab1->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->sponsorsTab2->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
}


void MainWindow::ToggleLock(QByteArray h, QString email, QString appPassword){
    ui->topHorizontalWidget->hide();
    ui->menuTree->hide();
    ui->menuLogoLabel->hide();

    if(h != ""){
        if(!savedData->HashExists()){ // New installation
            savedData->SetHash(QCryptographicHash::hash(QString(h.toHex() + "refuge510refuge").toUtf8(), QCryptographicHash::Sha256));
            savedData->SetLastTimeSync(QDateTime::currentDateTime());
        }

        else if(!savedData->CompareHash(QCryptographicHash::hash(QString(h.toHex() + "refuge510refuge").toUtf8(), QCryptographicHash::Sha256))) // Passwords don't match
            return;

        // Login sucessful
        crypto = new SimpleCrypt;
        quint64 key;
        QDataStream stream(&h, QIODevice::ReadOnly);
        stream >> key;
        crypto->setKey(key);

        db.Init();

        savedData->Init(email, appPassword, ui->syncButton);
        savedData->Save();

        ui->loginPage->setContentsMargins(0,0,0,0);

        ChangePage(ui->menuTree->topLevelItem(0));
        ui->menuTree->topLevelItem(0)->setSelected(true);
        ui->topHorizontalWidget->show();
        ui->menuTree->show();
        ui->menuLogoLabel->show();
        resizeEvent(nullptr);


        // To send recall mails to members
        QSqlQuery query;
        HandleErrorExec(&query, "SELECT last_name, phone, address FROM People WHERE id_people = -2;");
        query.next();
        QString shelterName = query.value(0).toString();
        QString shelterPhone = query.value(1).toString();
        QString shelterAddress = query.value(2).toString();

        savedData->SetShelterInfos(shelterName, shelterPhone, shelterAddress);
    }
}

void MainWindow::ToggleFoundBoxText(){
    QString newText = "Retrouvé";
    if(!ui->foundLostBox->isChecked())
        newText = "Non Retrouvé";
    ui->foundLostBox->setText(newText);
}

void MainWindow::ToggleSatisfiedBoxText(){
    QString newText = "Satisfaite";
    if(!ui->satisfiedAdoptionDemandBox->isChecked())
        newText = "Non Satisfaite";
    ui->satisfiedAdoptionDemandBox->setText(newText);
}

// Change selected page from stacked widget, based on the selected menu item
void MainWindow::ChangePage(QTreeWidgetItem* item)
{
    if(item == nullptr){ // Coming back from edit page
        if(ui->menuTree->selectedItems().size() > 0)
            item = ui->menuTree->selectedItems()[0];
        else
            item = ui->menuTree->currentItem();

        QString txt = item->text(0).trimmed();
        if(txt == "Registres" || txt == "Autres")
            item = item->child(0);
    }

    if(!item)
        return;

    ui->searchLine->clear();

    QStackedWidget* stacked = ui->stackedWidget;
    QString txt = item->text(0).trimmed();



    if (txt == "Entrées/Sorties"){
        stacked->setCurrentWidget(ui->entryRegistryPage);
        LoadEntryRegistry(QString::number(QDate::currentDate().year()));
        ui->entryRegistryPage->resizeEvent(nullptr);
    }
    else if (txt == "Garderie"){
        stacked->setCurrentWidget(ui->careRegistryPage);
        LoadCareRegistry(QString::number(QDate::currentDate().year()));
        ui->careRegistryPage->resizeEvent(nullptr);
    }
    else if (txt == "Adhérents"){
        stacked->setCurrentWidget(ui->membersPage);
        LoadMembers(QString::number(QDate::currentDate().year()));
        ui->membersPage->resizeEvent(nullptr);
    }
    else if (txt == "Liste rouge adoptants"){
        stacked->setCurrentWidget(ui->redListPage);
        LoadRedList();
    }
    else if (txt == "Animaux perdus"){
        stacked->setCurrentWidget(ui->lostPage);
        LoadLost();
    }
    else if (txt == "Demandes d'adoption"){
        stacked->setCurrentWidget(ui->adoptionDemandPage);
        LoadAdoptionDemand();
    }
    else if (txt == "Parrainage"){
        stacked->setCurrentWidget(ui->sponsorsPage);
        LoadSponsors();
    }

    else{
        ui->menuTree->collapseAllExcept(txt);
        if(txt == "Accueil"){
            stacked->setCurrentWidget(ui->homePage);
            ui->homePage->LoadAlerts(savedData->GetLastTimeExport());
        }
        else if (txt == "Fiches chiens"){
            LoadDogCards();
            stacked->setCurrentWidget(ui->dogCardsPage);
        }
        else if (txt == "Vétérinaire"){
            stacked->setCurrentWidget(ui->vetPage);
            LoadVet();
        }
        else if (txt == "Paramètres"){
            stacked->setCurrentWidget(ui->settingsPage);
            findChild<SettingsPage*>()->LoadSettings();
        }
    }

    if (txt == "Registres"  || txt == "Autres")
        item->setExpanded(!item->isExpanded());

    else{
        ui->titleLabel->setText(txt);
        ui->yearBox     ->setVisible(txt == "Entrées/Sorties" || txt == "Garderie" || txt == "Adhérents");
        ui->searchLine->setVisible(txt != "Accueil" && txt != "Paramètres");
        ui->searchIcon->setVisible(txt != "Accueil" && txt != "Paramètres");
    }

    resizeEvent(nullptr);
}

void MainWindow::InitYearRegistry(QString type, QString year){ // Only for care, entry and members
    db.ReorderEntryRegistry();

    // Init yearBox
    std::vector<QString> years = db.GetRegistryYears(type);

    QComboBox* box = ui->yearBox;

    disconnect(box, nullptr, nullptr, nullptr);

    box->clear();

    for(QString y : years){
        if(y != QString::number(QDate::currentDate().year()))
            box->addItem(y);
    }

    box->addItem(QString::number(QDate::currentDate().year()));

    box->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    box->adjustSize();

    int yearIndex = box->findText(year);
    if(yearIndex >= 0)
        box->setCurrentIndex(yearIndex);
    else
        box->setCurrentIndex(box->count() - 1);

    connect(box, &QComboBox::currentTextChanged, this, [=](){Search();});

    resizeEvent(nullptr);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if(event){
        QSize newSize = event->size();
        if (newSize.width() > screenGeometry.width() || newSize.height() > screenGeometry.height()) {
            QSize adjustedSize(qMin(newSize.width(), screenGeometry.width()), qMin(newSize.height(), screenGeometry.height()));
            this->resize(adjustedSize);
        }
    }

    QMainWindow::resizeEvent(event);

    ui->menuTree->SetInitialWidth(width() * 0.15);

    QFont font = ui->titleLabel->font();
    font.setPointSize(0.02 * ui->stackedWidget->width());
    ui->titleLabel->setFont(font);

    QFont comboBoxFont = ui->yearBox->font();
    comboBoxFont.setPointSizeF(0.015 * ui->stackedWidget->width());
    ui->yearBox->setFont(comboBoxFont);

    comboBoxFont.setPointSizeF(0.01 * ui->stackedWidget->width());

    ui->searchLine->setFont(comboBoxFont);

    QSize textSize = ui->yearBox->fontMetrics().size(Qt::TextSingleLine, ui->yearBox->currentText());
    ui->yearBox->setFixedSize(textSize + QSize(35, 35));

    for (DogCard* c : ui->dogCardsContent->findChildren<DogCard*>())
        c->resizeEvent(event);


    QFont checkboxFont = ui->careCheckbox->font();
    checkboxFont.setPointSize(0.01 * ui->dogCardsPage->width());
    ui->careCheckbox->setFont(checkboxFont);
    ui->outCheckbox->setFont(checkboxFont);
    checkboxFont.setPointSize(0.01 * ui->lostPage->width());
    ui->foundCheckBox->setFont(checkboxFont);
    checkboxFont.setPointSize(0.01 * ui->adoptionDemandPage->width());
    ui->satisfiedCheckBox->setFont(checkboxFont);
    checkboxFont.setPointSize(0.01 * ui->sponsorsPage->width());
    ui->pastSponsorsCheckbox->setFont(checkboxFont);
    checkboxFont.setPointSize(0.01 * ui->vetPage->width());
    ui->oldCheckBox->setFont(checkboxFont);

    QTimer::singleShot(50,[this](){
        ui->searchIcon->setIconSize(QSize(ui->searchLine->height(), ui->searchLine->height()));
        ui->searchIcon->setFixedHeight(ui->searchLine->height());
    });

    int syncWidth = 0.03 * width();
    ui->syncButton->setIconSize(QSize(syncWidth, syncWidth));
    ui->syncButton->setFixedHeight(syncWidth);
    ui->removeButton->setFixedWidth(ui->removeButton->height());


    ui->homePage->resizeEvent(event);
}

void MainWindow::ToggleModifyButtons()
{
    for(QToolButton* but : modifyButtons){
        if(but != nullptr){
            QIcon icon = but->icon();
            but->setIcon(QIcon());
            QTimer::singleShot(310, [but, icon](){but->setIcon(icon);});
        }
    }

}

void MainWindow::ToggleReasonEdit(){
    ui->reasonVetAnimalEdit->setVisible(ui->reasonVetAnimalBox->currentText() == "Autre");
}

void MainWindow::Search(QString search){
    if(search == nullptr)
        search = ui->searchLine->text();

    QDate dateSearch = QDate::fromString(search, "dd/MM/yyyy");
    if(dateSearch.isValid())
        search = dateSearch.toString("yyyy-MM-dd");

    QString pageName = ui->stackedWidget->currentWidget()->objectName();
    if(pageName == "entryRegistryPage")
        LoadEntryRegistry(ui->yearBox->currentText(), search);
    else if(pageName == "careRegistryPage")
        LoadCareRegistry(ui->yearBox->currentText(), search);
    else if (pageName == "redListPage")
        LoadRedList(search);
    else if(pageName == "dogCardsPage"){
        LoadDogCards(search);
        resizeEvent(nullptr);
        return; // As this page is not a registry
    }
    else if (pageName == "membersPage")
        LoadMembers(ui->yearBox->currentText(), search);
    else if (pageName == "lostPage")
        LoadLost(search);
    else if (pageName == "vetPage")
        LoadVet(search);
    else if(pageName == "adoptionDemandPage")
        LoadAdoptionDemand(search);
    else if(pageName == "sponsorsPage")
        LoadSponsors(search);

    Registry *registry = findChild<Registry*>(pageName);
    if(registry)
        registry->resizeEvent(nullptr);
}

// Only necessary infos are sent because the list is stored when the slot is connected
void MainWindow::TriggerEdit(QString type, QStringList necessary){
    ui->searchLine->hide();
    ui->searchIcon->hide();
    QStringList infos;

    if(!necessary.isEmpty()){ // Editing

        QSqlQuery query;

        if(type == "entry"){
            query.prepare("SELECT ES_registry.id_ES, "
                                    "ES_registry.date_prov, "
                                    "ES_registry.type_prov, "
                                    "People_prov.last_name, "
                                    "People_prov.first_name, "
                                    "People_prov.address, "
                                    "People_prov.phone, "
                                    "People_prov.email, "
                                    "Dogs.sex, "
                                    "Dogs.chip, "
                                    "Dogs.name, "
                                    "Dogs.description, "
                                    "Dogs.birth, "
                                    "GROUP_CONCAT(Destinations.date || '_|_' || IFNULL(Destinations.type, '') || '_|_' || IFNULL(People_dest.last_name, '') || '_|_' || IFNULL(People_dest.first_name, '') || '_|_' || IFNULL(People_dest.address, '') || '_|_' || IFNULL(People_dest.phone, '') || '_|_' || IFNULL(People_dest.email, ''), '_-_'), "
                                    "ES_registry.death_cause "
                                    "FROM ES_registry "
                                    "JOIN People AS People_prov ON ES_registry.id_people_prov = People_prov.id_people "
                                    "JOIN Dogs ON ES_registry.id_dog = Dogs.id_dog "
                                    "LEFT JOIN Destinations ON (Dogs.id_dog = Destinations.id_dog AND ES_Registry.date_prov = Destinations.date_prov) "
                                    "LEFT JOIN People AS People_dest ON Destinations.id_people = People_dest.id_people "
                                    "WHERE id_ES = :id_ES AND ES_registry.date_prov = :date_prov "
                                    "GROUP BY Dogs.id_dog ");

            query.bindValue(":id_ES", necessary[0]);
            query.bindValue(":date_prov", necessary[1]);

            HandleErrorExec(&query);

            query.next();

            for(int i = 0; i < query.record().count(); i++)
                infos.append(query.value(i).toString());
        }

        else if (type == "care"){
            query.prepare("SELECT Care_registry.id_care, "
                                   "Care_registry.entry_date, "
                                   "People_prov.last_name, "
                                   "People_prov.first_name, "
                                   "People_prov.address, "
                                   "People_prov.phone, "
                                   "People_prov.email, "
                                   "Dogs.name, "
                                   "Dogs.chip, "
                                   "Dogs.sex, "
                                   "Dogs.birth, "
                                   "Dogs.description, "
                                   "Care_registry.exit_date, "
                                   "People_dest.last_name, "
                                   "People_dest.first_name, "
                                   "People_dest.address, "
                                   "People_dest.phone, "
                                   "People_dest.email "
                                   "FROM Care_registry "
                                   "JOIN People AS People_prov ON Care_registry.id_people_prov = People_prov.id_people "
                                   "JOIN People AS People_dest ON Care_registry.id_people_dest = People_dest.id_people "
                                   "JOIN Dogs ON Care_registry.id_dog = Dogs.id_dog "
                                   "WHERE id_care = :id_care "
                                   "AND entry_date = :entry_date");

            query.bindValue(":id_care", necessary[0]);
            query.bindValue(":entry_date", necessary[1]);

            HandleErrorExec(&query);

            query.next();

            for(int i = 0; i < query.record().count(); i++)
                infos.append(query.value(i).toString());
        }

        else if (type == "members"){
            query.prepare("SELECT Members.id_adhesion, "
                            "       Members.date, "
                            "       People.last_name, "
                            "       People.first_name, "
                            "       People.address, "
                            "       People.phone, "
                            "       People.email, "
                            "       Members.type, "
                            "       Members.amount "
                            "FROM Members "
                            "JOIN People ON Members.id_people = People.id_people "
                            "WHERE Members.id_adhesion = :id_adhesion "
                            "AND Members.date = :date");

            query.bindValue(":id_adhesion", necessary[0]);
            query.bindValue(":date", necessary[1]);

            HandleErrorExec(&query);
            query.next();

            for(int i = 0; i < query.record().count(); i++)
                infos.append(query.value(i).toString());
        }

        // Delete
        else if (type == "redList"){
            infos = necessary;
        }

        else if (type == "lost"){
            query.prepare("SELECT identification, "
                                   "name, "
                                   "species, "
                                   "sex, "
                                   "description, "
                                   "date, "
                                   "place, "
                                   "found, "
                                   "People.last_name, "
                                   "People.first_name, "
                                   "People.phone, "
                                   "People.email, "
                                   "People.address, "
                                   "People.id_people "
                                   "FROM Lost "
                                   "JOIN People ON People.id_people = Lost.id_people "
                                   "WHERE name = :name "
                                   "AND date = :date "
                                   "AND People.id_people = :id_people");

            query.bindValue(":name", necessary[0]);
            query.bindValue(":date", necessary[1]);
            query.bindValue(":id_people", necessary[2]);

            HandleErrorExec(&query);
            query.next();

            for(int i = 0; i < query.record().count(); i++)
                infos.append(query.value(i).toString());
        }

        else if (type == "vet"){
            query.prepare("SELECT Vet.date, "
                                 "Dogs.name, "
                                 "Dogs.chip, "
                                 "Dogs.sex, "
                                 "Dogs.birth, "
                                 "Dogs.description, "
                                 "Vet.reason, "
                                 "Dogs.id_dog  "
                                 "FROM Vet "
                                 "JOIN Dogs ON Dogs.id_dog = Vet.id_dog "
                                 "WHERE Vet.date = :date "
                                 "AND Dogs.id_dog = :id_dog");

            query.bindValue(":date", necessary[0]);
            query.bindValue(":id_dog", necessary[1]);

            HandleErrorExec(&query);
            query.next();

            for(int i = 0; i < query.record().count(); i++)
                infos.append(query.value(i).toString());
        }

        else if(type == "vaccine"){ // Add vaccine recall, called by homepage
            ui->editPage->Edit("vet", {});
            ui->editPage->GroupedVet();
            for(QString id_dog : necessary){
                ui->editPage->AddVetLabel(id_dog);
            }

            return;
        }

        else if(type == "adoptionDemand"){
            query.prepare("SELECT People.last_name, "
                          "People.first_name, "
                          "People.address, "
                          "People.phone, "
                          "People.email, "
                          "Adoption_demand.sex, "
                          "Adoption_demand.age, "
                          "Adoption_demand.breed, "
                          "Adoption_demand.satisfied, "
                          "Adoption_demand.infos, "
                          "People.id_people "
                          "FROM Adoption_demand "
                          "JOIN People ON People.id_people = Adoption_demand.id_people "
                          "WHERE Adoption_demand.breed = :breed "
                          "AND People.id_people = :id_people");

            query.bindValue(":breed", necessary[0]);
            query.bindValue(":id_people", necessary[1]);

            HandleErrorExec(&query);
            query.next();

            for(int i = 0; i < query.record().count(); i++)
                infos.append(query.value(i).toString());
        }

        else if(type == "sponsors"){
            query.prepare("SELECT People.last_name, "
                          "People.first_name, "
                          "People.address, "
                          "People.phone, "
                          "People.email, "
                          "Dogs.name, "
                          "Dogs.chip, "
                          "Dogs.sex, "
                          "Dogs.birth, "
                          "Dogs.description, "
                          "Sponsors.amount, "
                          "Sponsors.start_date, "
                          "Sponsors.end_date, "
                          "Sponsors.id_people, "
                          "Sponsors.id_dog "
                          "FROM Sponsors "
                          "JOIN People ON People.id_people = Sponsors.id_people "
                          "JOIN Dogs ON Dogs.id_dog = Sponsors.id_dog "
                          "WHERE People.id_people = :id_people "
                          "AND Dogs.id_dog = :id_dog");

            query.bindValue(":id_people", necessary[0]);
            query.bindValue(":id_dog", necessary[1]);

            HandleErrorExec(&query);
            query.next();

            for(int i = 0; i < query.record().count(); i++)
                infos.append(query.value(i).toString());
        }
    }

    ui->editPage->Edit(type, infos);
}

QString MainWindow::ClearUselessBreaks(QString s){
    while(s.contains(" \n"))
        s = s.replace(" \n", "\n");

    while(s.contains("\n\n"))
        s = s.replace("\n\n", "\n");
    s = s.trimmed();
    return s;
}

void MainWindow::Clean(){
    QSqlDatabase::removeDatabase("secondary");

    CleanThread *thread = new CleanThread();

    QObject::connect(thread, &CleanThread::finished, ui->editPage, &EditPage::FinishedCleaning);
    QObject::connect(thread, &CleanThread::finished, thread, &CleanThread::deleteLater);
    QObject::connect(thread, &CleanThread::finished, this, [=](){ChangePage();});

    thread->start();

}

void MainWindow::SelectDogCard(){
    DogCard *selectedCard = qobject_cast<DogCard*>(QObject::sender()->parent());

    for (DogCard* c : findChildren<DogCard*>()){
        if(c->objectName() != selectedCard->objectName())
            c->setVisible(false);
    }
}

void MainWindow::UnselectDogCard(){
    LoadDogCards(ui->searchLine->text());
}
