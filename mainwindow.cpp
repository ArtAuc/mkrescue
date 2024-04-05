#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    if(!savedData.Load()){
        // TODO : Initial login page (choose pwd and drive API)
    }

    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    ui->stackedWidget->setCurrentWidget(ui->dogCardsPage);

    connect(ui->menuButton, SIGNAL(clicked(bool)), ui->menuTree, SLOT(Toggle()));
    connect(ui->menuButton, SIGNAL(clicked(bool)), this, SLOT(ToggleModifyButtons()));
    connect(ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(Search(QString)));

    InitExportButtons();

    // editPage
    connect(ui->entryAddButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(AddEntry()));
    connect(ui->redListAddButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(AddRedList()));
    connect(ui->careAddButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(AddCare()));
    connect(ui->membersAddButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(AddMember()));
    connect(ui->lostAddButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(AddLost()));
    connect(ui->entryTypeBox, SIGNAL(currentTextChanged(QString)), ui->editPage, SLOT(ChangeEntryType(QString)));
    connect(ui->submitButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(SaveEdit()));
    connect(ui->submitButton, SIGNAL(clicked()), this, SLOT(Clean()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(QuitEdit()));
    connect(ui->removeButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(RemoveCurrent()));
    connect(ui->editPage, SIGNAL(RefreshMainWindow(QString)), this, SLOT(RefreshPage(QString)));
    connect(ui->prevDestButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(PrevDestPage()));
    connect(ui->nextDestButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(NextDestPage()));
    connect(ui->sameCareButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(SameDestCare()));
    connect(ui->foundLostBox, SIGNAL(clicked(bool)), this, SLOT(ToggleFoundBoxText()));

    InitEditWidgets();


    // Filter checkboxes
    connect(ui->outCheckbox, &QCheckBox::clicked, this, [this]() {LoadDogCards(ui->searchLine->text());});
    connect(ui->careCheckbox, &QCheckBox::clicked, this, [this]() {LoadDogCards(ui->searchLine->text());});
    connect(ui->foundCheckBox, &QCheckBox::clicked, this, [this]() {LoadLost(ui->searchLine->text());});


    ui->entryRegistryPage->SetType("entry");
    ui->entryLabelLayout->setAlignment(Qt::AlignLeft);

    ui->careRegistryPage->SetType("care");
    ui->careLabelLayout->setAlignment(Qt::AlignLeft);

    ui->membersPage->SetType("members");
    ui->redListPage->SetType("redList");
    ui->lostPage->SetType("lost");


    LoadDogCards();

    Clean();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitEditWidgets(){
    QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->entryTab1->layout());
    if(gridLayout){ // Insert people entry editor
        gridLayout->addWidget(new EditPeopleWidget("AbandonEdit"), gridLayout->rowCount(), 1, 1, 2);
        gridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding), gridLayout->rowCount(), 1, 1, 2);
    }
    ui->entryTab2->layout()->addWidget(new EditDogWidget("EntryAnimalEdit"));

    ui->redListEditPage->layout()->addWidget(new EditPeopleWidget("RedListEdit", true));

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


    ui->lostTab2->layout()->addWidget(new EditPeopleWidget("LostOwnerEdit", true));

    ui->careTab2->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->entryTab2->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->lostTab1->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
    ui->lostTab2->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
}


void MainWindow::ToggleFoundBoxText(){
    QString newText = "Retrouvé";
    if(!ui->foundLostBox->isChecked())
        newText = "Non Retrouvé";
    ui->foundLostBox->setText(newText);
}


// Change selected page from stacked widget, based on the selected menu item
void MainWindow::ChangePage(QTreeWidgetItem* item)
{
    QStackedWidget* stacked = ui->stackedWidget;
    QString txt = item->text(0).trimmed();

    QComboBox* box = ui->yearBox;
    QObject::disconnect(box, nullptr, this, nullptr);


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
        resizeEvent(nullptr);
    }
    else if (txt == "Demandes d'adoption")
        stacked->setCurrentWidget(ui->adoptionDemandPage);

    else if (txt == "Paramètres"){
        stacked->setCurrentWidget(ui->settingsPage);
        findChild<SettingsPage*>()->LoadSettings();
    }

    else{
        ui->menuTree->collapseAllExcept(txt);
        if(txt == "Accueil")
            stacked->setCurrentWidget(ui->homePage);
        else if (txt == "Fiches chiens"){
            LoadDogCards();
            stacked->setCurrentWidget(ui->dogCardsPage);
        }
        else if (txt == "Vétérinaire")
            stacked->setCurrentWidget(ui->vetPage);
    }

    if (txt == "Registres"  || txt == "Autres")
        item->setExpanded(!item->isExpanded());

    else{
        ui->titleLabel->setText(txt);
        box->setVisible(txt == "Entrées/Sorties" || txt == "Garderie" || txt == "Adhérents");
        ui->searchLine->setVisible(txt != "Accueil" && txt != "Paramètres");
    }
}

void MainWindow::InitYearRegistry(QString type, QString year){ // Only for care and entry
    db.ReorderEntryRegistry();

    // Init yearBox
    std::vector<QString> years = db.GetRegistryYears(type);

    QComboBox* box = ui->yearBox;
    QObject::disconnect(box, nullptr, this, nullptr);
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

    resizeEvent(nullptr);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
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

void MainWindow::Search(QString search){
    QString pageName = ui->stackedWidget->currentWidget()->objectName();
    if(pageName == "entryRegistryPage")
        LoadEntryRegistry(ui->yearBox->currentText(), search);
    else if(pageName == "careRegistryPage")
        LoadCareRegistry(ui->yearBox->currentText(), search);
    else if (pageName == "redListPage")
        LoadRedList(search);
    else if(pageName == "dogCardsPage")
        LoadDogCards(search);
    else if (pageName == "membersPage")
        LoadMembers(ui->yearBox->currentText(), search);
    else if (pageName == "lostPage")
        LoadLost(search);

    findChild<Registry*>(pageName)->resizeEvent(nullptr);
}

// Only necessary infos are sent because the list is stored when the slot is connected
void MainWindow::TriggerEdit(QString type, QStringList necessary){
    QSqlQuery query;
    QStringList infos;

    if(type == "entry"){
        HandleErrorExec(&query, "SELECT ES_registry.id_ES, "
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
                    "LEFT JOIN Destinations ON Dogs.id_dog = Destinations.id_dog "
                    "LEFT JOIN People AS People_dest ON Destinations.id_people = People_dest.id_people "
                    "WHERE id_ES = " + necessary[0] + " AND date_prov = '" + necessary[1] + "'"
                    "GROUP BY Dogs.id_dog ");

        query.next();

        for(int i = 0; i < query.record().count(); i++)
            infos.append(query.value(i).toString());
    }

    else if (type == "care"){
        HandleErrorExec(&query, "SELECT Care_registry.id_care, "
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
                   "WHERE id_care = " + necessary[0] +
                   " AND entry_date = '" + necessary[1] + "';");

        query.next();

        for(int i = 0; i < query.record().count(); i++)
            infos.append(query.value(i).toString());
    }

    else if (type == "members"){
        HandleErrorExec(&query,
                "SELECT Members.id_adhesion, "
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
                "WHERE Members.id_adhesion = " + necessary[0] +
                " AND Members.date = '" + necessary[1] + "';");

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
                   "WHERE name = '" + necessary[0] +
                   "' AND date = '" + necessary[1] +
                   "' AND People.id_people = " + necessary[2] + ";");

        if(!query.exec())
            QMessageBox::critical(nullptr, "Erreur de la requête SQL", query.lastError().text());

        query.next();

        for(int i = 0; i < query.record().count(); i++)
            infos.append(query.value(i).toString());
    }


    ui->editPage->Edit(type, infos);
}

void MainWindow::RefreshPage(QString type){
    if(type == "entry")
        LoadEntryRegistry(ui->yearBox->currentText());
    else if(type == "care")
        LoadCareRegistry(ui->yearBox->currentText());
    else if(type == "members")
        LoadMembers(ui->yearBox->currentText());

    else if(type == "redList"){
        db.MakeRedList();
        LoadRedList();
    }

    else if(type == "lost"){
        LoadLost();
    }

    resizeEvent(nullptr);
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
    db.CleanDogs();
    db.MakeRedList();
    db.CleanPeople();
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
