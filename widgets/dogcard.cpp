#include "dogcard.h"

DogCard::DogCard(QWidget *parent, QString chip, QString name, QString sex, QString info2, QString description, QString info1) : DogCard(parent)
{
    this->chip = chip;
    this->dogName = name;
    mainWindow = parent;

    QString typeInfo = "care";
    if(info2.contains("_|_"))
        typeInfo = "out";
    else if (info1.contains("_|_"))
        typeInfo = "current";


    type = typeInfo;

    // Header color according to type
    QString hColor;
    if(typeInfo == "current")
        hColor = "3b4b64";
    else if(typeInfo == "care")
        hColor = "97717a";
    else if(typeInfo == "out")
        hColor = "a3acbd";


    setStyleSheet("QFrame{border-top:3px solid #" + hColor + ";}"
                  "QLabel, QWidget#dogCardsContent, QScrollArea{border:none;}");



    layout = new QGridLayout(this);

    setObjectName("dogCard" + chip);

    sexLabel = new QLabel(this);
    sexLabel->setAlignment(Qt::AlignCenter);

    sexIcon = QPixmap("media/" + QString(((sex == "Mâle") ? "male" : "female")) + ".png");
    sexLabel->setPixmap(sexIcon);
    sexLabel->setStyleSheet("padding:3px;");


    QLabel *nameLabel = new QLabel(name, this);

    nameSexWidget = new QWidget(this);
    QHBoxLayout *nameSexLayout = new QHBoxLayout(nameSexWidget);
    nameSexLayout->setContentsMargins(0, 0, 0, 0);
    nameSexLayout->addWidget(sexLabel);
    nameSexLayout->addWidget(nameLabel);
    nameSexLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Preferred));
    nameSexWidget->setLayout(nameSexLayout);

    QString info1String;
    if(typeInfo == "current" || typeInfo == "out"){
        QStringList splitted = info1.split("_|_");
        QString type_prov = crypto->decryptToString(splitted[1]).split("_|_")[0];
        QString date_prov = splitted[0];

        if(type_prov != "Abandon" && type_prov != "Fourrière")
            type_prov = "Arrivée";


        info1String = type_prov + " : ";
        info1String += QDate::fromString(date_prov, "yyyy-MM-dd").toString("dd/MM/yyyy");
    }

    else if(typeInfo == "care"){
        info1String = "Propriétaire : " + info1;
    }


    info1String = AutoBreak(info1String, 25).split("\n")[0];
    QLabel *info1Label = new QLabel(info1String, this);


    QString info2String;

    if(typeInfo == "out"){ // Here info2 = date_dest_|_type_dest
        QString type_dest = info2.split("_|_")[1];
        if(type_dest.startsWith("Famille")) // So that it fits
            type_dest = "FA";
        else if(type_dest.startsWith("Entrée au"))
            type_dest = "Refuge";

        QString date_dest = info2.split("_|_")[0];
        info2String = type_dest;
        if(sex == "Femelle" && type_dest == "Mort")
            type_dest += "e";
        info2String += " : " + QDate::fromString(date_dest, "yyyy-MM-dd").toString("dd/MM/yyyy");
    }

    else if (typeInfo == "current"){
        info2String = GetAge(QDate::fromString(info2, "yyyy-MM-dd"));
    }

    else if(typeInfo == "care"){
        info2String = "Der. garde : " + QDate::fromString(info2, "yyyy-MM-dd").toString("dd/MM/yyyy");
    }

    info2String = AutoBreak(info2String, 25).split("\n")[0];
    info2Label = new QLabel(info2String, this);

    descriptionLabel = new QLabel(AutoBreak(description, 25).split("\n")[0], this);


    int iconSize = 40;

    detailsButton = new QToolButton(this);
    detailsButton->setIcon(QIcon("media/right.png"));
    detailsButton->setIconSize(QSize(iconSize, iconSize));

    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(SelectDogCard()));
    connect(this, SIGNAL(ClickedHistory(QString,QStringList)), mainWindow, SLOT(TriggerEdit(QString, QStringList)));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(SelectThis()));

    // Only shown if selected
    sterilizedBox = new TriStateCheckBox((sex == "Mâle") ? "Castration" : "Stérilisation", this);
    compatDogBox = new TriStateCheckBox("Compatibilité chien", this);
    compatCatBox = new TriStateCheckBox("Compatibilité chat", this);
    historyScroll = new QScrollArea();
    historyScroll->setObjectName("historyScroll" + chip);
    QWidget *historyWidget = new QWidget(historyScroll);
    historyWidget->setLayout(new QVBoxLayout());
    historyWidget->setStyleSheet("background:white;");
    historyWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    historyScroll->setWidgetResizable(true);
    historyScroll->setWidget(historyWidget);

    // Make labels copiable
    info2Label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    info2Label->setCursor(QCursor(Qt::IBeamCursor));
    descriptionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    descriptionLabel->setCursor(QCursor(Qt::IBeamCursor));
    nameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    nameLabel->setCursor(QCursor(Qt::IBeamCursor));
    info1Label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    info1Label->setCursor(QCursor(Qt::IBeamCursor));

    layout->addWidget(nameSexWidget, 0, 0);
    layout->addWidget(info1Label, 3,  0);
    layout->addWidget(info2Label, 4, 0);
    layout->addWidget(descriptionLabel, 5, 0);
    layout->addWidget(detailsButton, 5, 2);


    nameLabel->setObjectName("nameLabel" + chip);

    setLayout(layout);
}

void DogCard::resizeEvent(QResizeEvent *event){
    if(sexLabel){
        QFrame::resizeEvent(event);

        float factor;
        QSize parentSize = qobject_cast<QWidget*>(parent()->parent())->size();
        if(selected){
            layout->setSpacing(height() / 100);

            setMaximumSize(QWIDGETSIZE_MAX, 0.95 * parentSize.height());
            factor = 1.5;
        }
        else{
            setMaximumSize(parentSize / 4);
            factor = 1;
        }


        nameSexWidget->setMaximumWidth(parentSize.width() / 4);

        for(QWidget *c : findChildren<QWidget*>()){
            if(c->objectName() == "nameLabel" + chip){
                QFont font = c->font();
                font.setBold(true);
                font.setPointSizeF(0.013 * factor * mainWindow->width());
                c->setFont(font);
            }

            else if (qobject_cast<QLabel*>(c)){
                QFont font = c->font();
                font.setPointSize(0.01 * mainWindow->width());
                c->setFont(font);
            }

            if (qobject_cast<TriStateCheckBox*>(c) || qobject_cast<QPushButton*>(c) || qobject_cast<CustomDateTimeEdit*>(c)){
                QFont font = c->font();
                font.setPointSizeF(0.008 * mainWindow->width());
                c->setFont(font);
            }
        }

        historyScroll->setStyleSheet("QScrollArea{"
                                        "background-color:white;"
                                        "margin-left:30px;"
                                        "padding-left:30px;"
                                        "border-left: 2px solid gray;"
                                        "border-radius:0px;"
                                     "}");

        sexLabel->setPixmap(sexIcon.scaled(0.02 * factor * mainWindow->width(), 0.02 * factor * mainWindow->width(), Qt::KeepAspectRatio));
    }
}

void DogCard::SelectThis(){
    selected = true;

    layout->removeWidget(detailsButton);
    layout->addWidget(detailsButton, 0, 2);
    disconnect(detailsButton, nullptr, nullptr, nullptr);
    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(UnselectDogCard()));
    detailsButton->setIcon(QIcon("media/cross.png"));

    QPushButton *prescButton = new QPushButton("Ordonnances");

    connect(prescButton, SIGNAL(clicked()), this, SLOT(HandlePrescription()));

    // Infos summary (on the left)
    QSqlQuery query;
    query.prepare("SELECT chip, birth, sterilized, compat_dog, compat_cat "
               "FROM Dogs "
               "WHERE chip = :chip;");

    query.bindValue(":chip", chip);
    HandleErrorExec(&query);


    QLabel *chipLabel, *birthLabel;
    chipLabel = new QLabel();
    birthLabel = new QLabel();
    chipLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    chipLabel->setCursor(QCursor(Qt::IBeamCursor));

    birthLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    birthLabel->setCursor(QCursor(Qt::IBeamCursor));


    if(query.next()){

        chipLabel->setText(query.value(0).toString());
        birthLabel->setText("Naissance : " + QDate::fromString(query.value(1).toString(), "yyyy-MM-dd").toString("dd/MM/yyyy") + "\n");
        sterilizedBox->SqlToState(query.value(2).toString());
        compatDogBox->SqlToState(query.value(3).toString());
        compatCatBox->SqlToState(query.value(4).toString());
    }

    // Modify age
    if(type == "current"){
        birthLabel->setText(birthLabel->text() + "Âge : " + info2Label->text() + "\n");
        layout->removeWidget(info2Label);
        info2Label->hide();
    }

    descriptionLabel->setText(descriptionLabel->text() + "\n");

    QWidget *vaccineWidget = new QWidget;

    CreateLastVaccine(vaccineWidget);

    // History (on the right)
    CreateHistory();


    layout->addWidget(chipLabel, 1, 0);
    layout->addWidget(birthLabel, 2, 0);
    layout->addWidget(sterilizedBox, 6, 0);
    layout->addWidget(compatDogBox, 7, 0);
    layout->addWidget(compatCatBox, 8, 0);
    layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), 9, 0);
    layout->addWidget(prescButton, 10, 0);
    layout->addWidget(vaccineWidget, 11, 0);
    layout->addWidget(historyScroll, 1, 1, layout->rowCount() - 1, 1);

    layout->setContentsMargins(layout->contentsMargins() + 50);
    qobject_cast<QWidget*>(parent())->layout()->setContentsMargins(6,0,6,6);


    resizeEvent(nullptr);
}

void DogCard::CreateLastVaccine(QWidget *vaccineWidget){
    vaccineWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    vaccineWidget->setLayout(new QHBoxLayout);
    QLabel *vaccineLabel = new QLabel("Dernier vaccin : ");

    CustomDateTimeEdit *vaccineEdit = new CustomDateTimeEdit();
    vaccineEdit->SetInvalidable();

    vaccineWidget->layout()->addWidget(vaccineLabel);
    vaccineWidget->layout()->addWidget(vaccineEdit);

    QSqlQuery query;
    query.prepare("SELECT MAX(Dogs.id_dog) FROM Dogs WHERE chip = :chip");
    query.bindValue(":chip", chip);
    HandleErrorExec(&query);
    query.next();

    QString id_dog = query.value(0).toString();

    query.prepare("SELECT Vet.date "
                  "FROM Vet "
                  "WHERE Vet.id_dog = :id "
                  "AND Vet.reason LIKE 'Vaccin%' "
                  "AND Vet.date < DATE('now') "
                  "ORDER BY Vet.date DESC");
    query.bindValue(":id", id_dog);
    HandleErrorExec(&query);

    if(query.next()){
        vaccineEdit->SetDate(query.value(0).toDate());
    }

    else
        vaccineEdit->setText("xx/xx/xxxx");

    connect(vaccineEdit, &CustomDateTimeEdit::textChanged, this, [this, vaccineEdit, id_dog](){
        QSqlQuery query;
        query.prepare("DELETE FROM Vet "
                      "WHERE Vet.id_dog = :id "
                      "AND Vet.date NOT LIKE '%T%'");
        query.bindValue(":id", id_dog);
        HandleErrorExec(&query);

        QDate vaccineDate = QDate::fromString(vaccineEdit->text(), "dd/MM/yyyy");
        if(vaccineDate.isValid() && vaccineDate <= QDate::currentDate()){
            query.prepare("INSERT INTO Vet (id_dog, date, reason) "
                          "VALUES(:id, :date, 'Vaccin')");
            query.bindValue(":id", id_dog);
            query.bindValue(":date", vaccineDate);

            HandleErrorExec(&query);
        }
    });
}


void DogCard::SaveCard(){
    QSqlQuery query;
    query.prepare("UPDATE Dogs "
                  "SET sterilized = :sterilized, "
                  "compat_dog = :compat_dog, "
                  "compat_cat = :compat_cat "
                  "WHERE chip = :chip;");

    query.bindValue(":sterilized", sterilizedBox->StateToSql());
    query.bindValue(":compat_dog", compatDogBox->StateToSql());
    query.bindValue(":compat_cat", compatCatBox->StateToSql());
    query.bindValue(":chip", chip);

    HandleErrorExec(&query);
}

void DogCard::CreateHistory(){
    QSqlQuery query;
    QString queryString;
    QVBoxLayout* histLayout = qobject_cast<QVBoxLayout*>(historyScroll->widget()->layout());

    // ES
    queryString = "SELECT ES_Registry.type_prov, People.last_name, People.first_name, People.phone, ES_Registry.date_prov AS date, 'entry', ES_Registry.id_ES, ES_Registry.date_prov "
                  "FROM ES_Registry "
                  "JOIN People on ES_Registry.id_people_prov = People.id_people "
                  "JOIN Dogs ON ES_Registry.id_dog = Dogs.id_dog "
                  "WHERE Dogs.chip = :chip";

    // Destinations
    queryString += " UNION "
                   "SELECT Destinations.type, People.last_name, People.first_name, People.phone, Destinations.date AS date, 'destination', ES_Registry.id_ES, ES_Registry.date_prov "
                   "FROM Destinations "
                   "JOIN People on Destinations.id_people = People.id_people "
                   "JOIN Dogs ON Destinations.id_dog = Dogs.id_dog "
                   "JOIN ES_Registry ON (ES_Registry.id_dog = Dogs.id_dog AND ES_Registry.date_prov = Destinations.date_prov) "
                   "WHERE Dogs.chip = :chip";

    // Care
    queryString += " UNION "
                   "SELECT Care_registry.exit_date, People.last_name, People.first_name, People.phone, Care_registry.entry_date AS date, 'care', Care_registry.id_care, Care_registry.entry_date "
                   "FROM Care_registry "
                   "JOIN People on Care_registry.id_people_prov = People.id_people "
                   "JOIN Dogs ON Care_registry.id_dog = Dogs.id_dog "
                   "WHERE Dogs.chip = :chip";

    // Vet
    queryString += " UNION "
                   "SELECT Vet.reason, '', '', '', Vet.date AS date, 'vet', Vet.date, Dogs.id_dog "
                   "FROM Vet "
                   "JOIN Dogs ON Vet.id_dog = Dogs.id_dog "
                   "WHERE Dogs.chip = :chip "
                   "AND Vet.date LIKE '%T%'";

    // Sponsor
    queryString += " UNION "
                   "SELECT Sponsors.end_date, People.last_name, People.first_name, People.phone, Sponsors.start_date AS date, 'sponsors', People.id_people, Dogs.id_dog "
                   "FROM Sponsors "
                   "JOIN Dogs ON Sponsors.id_dog = Dogs.id_dog "
                   "JOIN People ON Sponsors.id_people = People.id_people "
                   "WHERE Dogs.chip = :chip";


    // Sort by descending date
    queryString = "SELECT * FROM (" + queryString + ") AS Results "
                    "ORDER BY Results.date DESC;";

    query.prepare(queryString);
    query.bindValue(":chip", chip);
    HandleErrorExec(&query);


    while(query.next()){
        QString type = query.value(5).toString();
        ClickableLabel* histLabel = new ClickableLabel();
        QString colorString;
        QString dateString(query.value(4).toDate().toString("dd/MM/yyyy"));
        QString type_prov = query.value(0).toString();
        if(type == "entry"){
            type_prov = crypto->decryptToString(type_prov);
            if(type_prov.startsWith("Fourrière_|_"))
                histLabel->setText(dateString + " : <b>Fourrière</b> (" +
                               type_prov.split("_|_")[1] + ")");

            else if(type_prov.contains("_|_"))
                histLabel->setText(dateString + " : " +
                               AutoBreak(type_prov.split("_|_")[1], 50).split("\n")[0]);

            else
                histLabel->setText(dateString + " : <b>" +
                               type_prov + "</b> (" +
                               (query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString()).trimmed() + ")");
            colorString = "#3b4b64";
        }

        else if(type == "destination"){

            histLabel->setText(dateString + " : <b>" +
                           query.value(0).toString() + "</b> (" +
                           (query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString()).trimmed() + ")");

            if(query.value(0).toString() == "Mort")
                histLabel->setText(dateString + " : <b>Mort</b>");


            colorString = "#a3acbd";
        }

        else if(type == "care"){
            QString exitDateString(QDate::fromString(query.value(0).toString(), "yyyy-MM-dd").toString("dd/MM/yyyy"));
            histLabel->setText(dateString +
                                QString(exitDateString.isEmpty() ? "" : (" au " + exitDateString)) +
                                " : <b>Garderie</b> (" +
                                (query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString()).trimmed() + ")");

            colorString = "#97717a";
        }

        else if(type == "vet"){
            dateString = (query.value(4).toDateTime().toString("dd/MM/yyyy h:mm"));
            histLabel->setText(dateString +
                                " : <b>RDV Vétérinaire</b> (" +
                                query.value(0).toString() + ")");

            colorString = "#2cc09d";
        }

        else if(type == "sponsors"){
            QString endDateString(QDate::fromString(query.value(0).toString(), "yyyy-MM-dd").toString("dd/MM/yyyy"));
            histLabel->setText(dateString +
                               QString(endDateString.isEmpty() ? "" : (" au " + endDateString)) +
                                " : <b>Parrainage</b> (" +
                               (query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString()).trimmed() + ")");

            colorString = "#d3ed8c";
        }

        // Handle clicking on hyperlink
        QStringList necessary = {query.value(6).toString(), query.value(7).toString()};

        if(type == "destination")
            type = "entry";
        connect(histLabel, &ClickableLabel::clicked, this, [this, type, necessary](){emit ClickedHistory(type, necessary);});


        histLabel->setStyleSheet("QLabel{"
                                 "  border-left:2px solid " + colorString + ";"
                                 "  border-radius:0px;"
                                 "}");
        histLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        histLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        histLabel->setCursor(QCursor(Qt::IBeamCursor));


        histLayout->addWidget(histLabel);
    }

    histLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));
}

void DogCard::OpenPrescriptionFolder() {
    QString folderPath = "prescriptions/" + chip + "/";

    QDir directory(folderPath);
    if (!directory.exists()) {
        QMessageBox::information(nullptr, "Aucune ordonnance", "Aucune ordonnance n'a été trouvée pour ce chien");
        return;
    }

#ifdef Q_OS_WIN
    QString command = "explorer.exe /select,\"" + QDir::toNativeSeparators(folderPath) + "\"";
    QProcess::startDetached(command);
#elif defined(Q_OS_MAC)
    QStringList arguments;
    arguments << "-R" << folderPath;
    QProcess::startDetached("open", arguments);
#elif defined(Q_OS_LINUX)
    QStringList arguments;
    arguments << folderPath;
    QProcess::startDetached("xdg-open", arguments);
#else
    // Unsupported platform
    QMessageBox::critical(nullptr, "Erreur", "Erreur d'ouverture du dossier " + QDir::toNativeSeparators(folderPath);
#endif
}

void DogCard::AddPrescription(){
    QString dirPath = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Documents numérisés/");
    if (!QDir(dirPath).exists())
        dirPath = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Sélectionnez l'ordonnance", dirPath, "Ordonnances (*.jpg *.jpeg *.png *.gif *.bmp *.pdf);;Tous les fichiers (*)");

    if (!filePath.isEmpty()) {
        QString chipDirectory = QDir::toNativeSeparators("prescriptions/" + chip + "/");
        if (!QDir(".").mkpath(chipDirectory)) {
            QMessageBox::critical(nullptr, "Échec", "La création du répertoire pour le chien a échoué");
            return;
        }


        QString fileName = "Ordonnance_" + dogName + "_" + QDateTime::currentDateTime().toString("dd-MM-yyyy_hh-mm-ss") + "." + QFileInfo(filePath).completeSuffix();
        QString destinationPath = chipDirectory + fileName;


        if (QFile::copy(filePath, destinationPath))
            QMessageBox::information(nullptr, "Succès", "L'ordonnance a été ajoutée pour " + dogName);
        else
            QMessageBox::critical(nullptr, "Échec", "La copie de l'ordonnance a échoué");
    }

    else
        QMessageBox::critical(nullptr, "Aucun fichier sélectionné", "L'ordonnance n'a pas été selectionnée.");
}

void DogCard::HandlePrescription(){
    QMessageBox msgBox;
    msgBox.setWindowTitle("Ordonnances");
    msgBox.setText("Ordonnances de " + dogName);

    QPushButton *addButton = msgBox.addButton("Ajouter", QMessageBox::AcceptRole);
    QPushButton *browseButton = msgBox.addButton("Parcourir", QMessageBox::HelpRole);
    QPushButton *cancelButton = msgBox.addButton("Annuler", QMessageBox::RejectRole);

    // Show the message box
    msgBox.exec();

    // Check which button was clicked
    if (msgBox.clickedButton() == addButton)
        AddPrescription();
    else if (msgBox.clickedButton() == browseButton)
        OpenPrescriptionFolder();
    else if(msgBox.clickedButton() == cancelButton){}
}
