#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
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
    connect(ui->entryTypeBox, SIGNAL(currentTextChanged(QString)), ui->editPage, SLOT(ChangeEntryType(QString)));
    connect(ui->submitButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(SaveEdit()));
    connect(ui->submitButton, SIGNAL(clicked()), this, SLOT(Clean()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(QuitEdit()));
    connect(ui->removeButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(RemoveCurrent()));
    connect(ui->editPage, SIGNAL(RefreshMainWindow(QString)), this, SLOT(RefreshPage(QString)));
    connect(ui->prevDestButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(PrevDestPage()));
    connect(ui->nextDestButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(NextDestPage()));
    connect(ui->sameCareButton, SIGNAL(clicked(bool)), ui->editPage, SLOT(SameDestCare()));
    QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->entryTab1->layout());
    if(gridLayout){ // Insert people entry editor
        gridLayout->addWidget(new EditPeopleWidget("AbandonEdit"), gridLayout->rowCount(), 1, 1, 2);
    }

    ui->redListEditPage->layout()->addWidget(new EditPeopleWidget("RedListEdit", true));

    gridLayout = qobject_cast<QGridLayout*>(ui->careTab1->layout());
    if(gridLayout){
        gridLayout->addWidget(new EditPeopleWidget("CareEntryEdit"), gridLayout->rowCount(), 1, 1, 2);
    }

    ui->careTab2->layout()->addWidget(new EditDogWidget("CareAnimalEdit"));
    gridLayout = qobject_cast<QGridLayout*>(ui->careTab3->layout());
    if(gridLayout){
        gridLayout->addWidget(new EditPeopleWidget("CareDestEdit"), gridLayout->rowCount(), 0, 1, 2);
    }

    gridLayout = qobject_cast<QGridLayout*>(ui->membersEditPage->layout());
    if(gridLayout){
        gridLayout->addWidget(new EditPeopleWidget("MembersEdit"), gridLayout->rowCount(), 0, 1, 2);
    }


    // Dog cards checkboxes
    connect(ui->outCheckbox, &QCheckBox::clicked, this, [this]() {LoadDogCards(ui->searchLine->text());});
    connect(ui->careCheckbox, &QCheckBox::clicked, this, [this]() {LoadDogCards(ui->searchLine->text());});


    ui->entryRegistryPage->SetType("entry");
    ui->entryLabelLayout->setAlignment(Qt::AlignLeft);

    ui->careRegistryPage->SetType("care");
    ui->careLabelLayout->setAlignment(Qt::AlignLeft);

    ui->membersPage->SetType("members");
    ui->redListPage->SetType("redList");


    LoadDogCards();

    Clean();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Change selected page from stacked widget, based on the selected menu item
void MainWindow::ChangePage(QTreeWidgetItem* item)
{
    QStackedWidget* stacked = ui->stackedWidget;
    QString txt = item->text(0).trimmed();

    QComboBox* box = ui->yearBox;
    QObject::disconnect(box, nullptr, this, nullptr);
    modifyButtons.clear();


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
    else if (txt == "Animaux perdus")
        stacked->setCurrentWidget(ui->lostPage);
    else if (txt == "Demandes d'adoption")
        stacked->setCurrentWidget(ui->adoptionDemandPage);

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
        ui->searchLine->setVisible(txt != "Accueil");
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
}

void MainWindow::ToggleModifyButtons()
{
    for(QToolButton* but : modifyButtons){
        QIcon icon = but->icon();
        but->setIcon(QIcon());
        QTimer::singleShot(310, [but, icon](){but->setIcon(icon);});
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
}

void MainWindow::TriggerEdit(QString type, QStringList necessary){
    QSqlQuery query;
    QStringList infos;

    if(type == "entry"){
        query.exec("SELECT ES_registry.id_ES, "
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
                    "GROUP_CONCAT(Destinations.date || '___' || Destinations.type || '___' || People_dest.last_name || '___' || People_dest.first_name || '___' || People_dest.address || '___' || People_dest.phone || '___' || People_dest.email, '_-_'), "
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
        query.exec("SELECT Care_registry.id_care, "
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

    // Delete
    else if (type == "redList" || type == "members"){
        infos = necessary;
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
}

QString MainWindow::ClearUselessBreaks(QString s){
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
