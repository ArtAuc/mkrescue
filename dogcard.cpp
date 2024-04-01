#include "dogcard.h"

DogCard::DogCard(QWidget *parent, QString chip, QString name, QString sex, QString info2, QString description, QString info1) : DogCard(parent)
{
    this->chip = chip;
    mainWindow = parent;

    QString typeInfo = "care";
    if(info2.contains("___"))
        typeInfo = "out";
    else if (info1.contains("___"))
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
    nameLabel->setStyleSheet("font-size:23pt;"
                             "font-weight:bold;");

    nameSexWidget = new QWidget(this);
    QHBoxLayout *nameSexLayout = new QHBoxLayout(nameSexWidget);
    nameSexLayout->setContentsMargins(0, 0, 0, 0);
    nameSexLayout->addWidget(sexLabel);
    nameSexLayout->addWidget(nameLabel);
    nameSexLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Preferred));
    nameSexWidget->setLayout(nameSexLayout);
    nameSexWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QString info1String;
    if(typeInfo == "current" || typeInfo == "out"){
        QStringList splitted = info1.split("___");
        QString type_prov = splitted[1];
        QString date_prov = splitted[0];
        info1String = type_prov + " : ";
        info1String += QDate::fromString(date_prov, "yyyy-MM-dd").toString("dd/MM/yyyy");
    }

    else if(typeInfo == "care"){
        info1String = "Propriétaire : " + info1;
    }


    QLabel *info1Label = new QLabel(info1String, this);

    QString info2String;

    if(typeInfo == "out"){ // Here info2 = date_dest___type_dest
        QString type_dest = info2.split("___")[1];
        QString date_dest = info2.split("___")[0];
        info2String = type_dest;
        if(sex == "Femelle" && type_dest == "Mort")
            type_dest += "e";
        info2String += " : " + QDate::fromString(date_dest, "yyyy-MM-dd").toString("dd/MM/yyyy");
    }

    else if (typeInfo == "current"){
        QDate date = QDate::fromString(info2, "yyyy-MM-dd");

        int years = QDate::currentDate().year() - date.year();
        int months = QDate::currentDate().month() - date.month();

        if (QDate::currentDate().day() < date.day())
            months--;

        if (months < 0) {
            years--;
            months += 12;
        }

        if (years == 1)
            info2String = "1 an, ";
        else if (years > 1)
            info2String = QString::number(years) + " ans, ";

        info2String += QString::number(months) + " mois";
    }

    else if(typeInfo == "care"){
        info2String = "Dernière garde : " + QDate::fromString(info2, "yyyy-MM-dd").toString("dd/MM/yyyy");
    }

    info2Label = new QLabel(info2String, this);

    descriptionLabel = new QLabel(description, this);


    int iconSize = 40;

    detailsButton = new QToolButton(this);
    detailsButton->setIcon(QIcon("media/right.png"));
    detailsButton->setIconSize(QSize(iconSize, iconSize));
    detailsButton->setFixedSize(iconSize, iconSize);

    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(SelectDogCard()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(SelectThis()));

    // Only shown if selected
    sterilizedBox = new TriStateCheckBox((sex == "Mâle") ? "Castration" : "Stérilisation", this);
    compatDogBox = new TriStateCheckBox("Compatibilité chien", this);
    compatCatBox = new TriStateCheckBox("Compatibilité chat", this);
    historyScroll = new QScrollArea();
    historyScroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    historyScroll->setObjectName("historyScroll" + chip);
    historyScroll->setLayout(new QVBoxLayout());

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
    QFrame::resizeEvent(event);

    float factor;
    QSize parentSize = qobject_cast<QWidget*>(parent())->size();
    if(selected){
        layout->setSpacing(height() / 100);

        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        factor = 0.7;
    }
    else{
        setMaximumSize(parentSize / 4);
        factor = 1.5;
    }


    for(QWidget *c : findChildren<QWidget*>()){
        if(c->objectName() == "nameLabel" + chip){
            c->setStyleSheet("font-size:" + QString::number(0.013 * mainWindow->width()) + "pt;"
                                 "font-weight:bold;");
        }

        else if (qobject_cast<QLabel*>(c)){
            QFont font = c->font();
            font.setPointSize(0.01 * mainWindow->width());
            c->setFont(font);
        }

        if (qobject_cast<TriStateCheckBox*>(c)){
            c->setStyleSheet("font-size:" + QString::number(0.008 * mainWindow->width()) + "pt;");
        }
    }

    historyScroll->setStyleSheet("QScrollArea{"
                                    "background-color:white;"
                                    "margin-left:30px;"
                                    "padding:30px;"
                                    "border-left: 2px solid gray;"
                                    "border-radius:0px;"
                                 "}");

    sexLabel->setPixmap(sexIcon.scaled(factor * size() / 10, Qt::KeepAspectRatio));
}

void DogCard::SelectThis(){
    selected = true;

    layout->removeWidget(detailsButton);
    layout->addWidget(detailsButton, 0, 2);
    disconnect(detailsButton, SIGNAL(clicked()), 0, 0);
    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(UnselectDogCard()));
    detailsButton->setIcon(QIcon("media/cross.png"));

    // Infos summary (on the left)
    QSqlQuery query;
    query.exec("SELECT chip, birth, sterilized, compat_dog, compat_cat "
               "FROM Dogs "
               "WHERE chip = " + chip + ";");

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
        info2Label->setText("");
    }

    descriptionLabel->setText(descriptionLabel->text() + "\n");

    // History (on the right)
    CreateHistory();


    layout->addWidget(chipLabel, 1, 0);
    layout->addWidget(birthLabel, 2, 0);
    layout->addWidget(sterilizedBox, 6, 0);
    layout->addWidget(compatDogBox, 7, 0);
    layout->addWidget(compatCatBox, 8, 0);
    layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), 9, 0);
    layout->addWidget(historyScroll, 0, 1, layout->rowCount(), 1);

    layout->setContentsMargins(layout->contentsMargins() + 50);


    resizeEvent(nullptr);
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

    query.exec();
}

void DogCard::CreateHistory(){
    QSqlQuery query;
    QString queryString;
    QVBoxLayout* histLayout = qobject_cast<QVBoxLayout*>(historyScroll->layout());

    // ES
    queryString = "SELECT ES_Registry.type_prov, People.last_name, People.first_name, People.phone, ES_Registry.date_prov AS date, 'ES' "
                  "FROM ES_Registry "
                  "JOIN People on ES_Registry.id_people_prov = People.id_people "
                  "JOIN Dogs ON ES_Registry.id_dog = Dogs.id_dog "
                  "WHERE Dogs.chip = :chip";

    // Destinations
    queryString += " UNION "
                   "SELECT Destinations.type, People.last_name, People.first_name, People.phone, Destinations.date AS date, 'Destination' "
                   "FROM Destinations "
                   "JOIN People on Destinations.id_people = People.id_people "
                   "JOIN Dogs ON Destinations.id_dog = Dogs.id_dog "
                   "WHERE Dogs.chip = :chip";

    // Care
    queryString += " UNION "
                   "SELECT Care_registry.exit_date, People.last_name, People.first_name, People.phone, Care_registry.entry_date AS date, 'Care' "
                   "FROM Care_registry "
                   "JOIN People on Care_registry.id_people_prov = People.id_people "
                   "JOIN Dogs ON Care_registry.id_dog = Dogs.id_dog "
                   "WHERE Dogs.chip = :chip";


    // Vet

    // Sort by descending date
    queryString = "SELECT * FROM (" + queryString + ") AS Results "
                    "ORDER BY Results.date DESC;";

    query.prepare(queryString);
    query.bindValue(":chip", chip);
    query.exec();

    while(query.next()){
        QString type = query.value(5).toString();
        QLabel* histLabel = new QLabel();
        QString colorString;
        QString dateString(QDate::fromString(query.value(4).toString(), "yyyy-MM-dd").toString("dd/MM/yyyy"));
        if(type == "ES"){
            if(query.value(0).toString().startsWith("Fourrière___"))
                histLabel->setText(dateString + " : <b>Fourrière</b> (" +
                               query.value(0).toString().split("___")[1] + ")");

            else
                histLabel->setText(dateString + " : <b>" +
                               query.value(0).toString() + "</b> (" +
                               (query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString()).trimmed() + ")");
            colorString = "#3b4b64";
        }

        else if(type == "Destination"){
            if(query.value(0).toString() == "Mort")
                histLabel->setText(dateString + " : <b>Mort</b>");

            histLabel->setText(dateString + " : <b>" +
                           query.value(0).toString() + "</b> (" +
                           (query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString()).trimmed() + ")");


            colorString = "#a3acbd";
        }

        else if(type == "Care"){
            QString exitDateString(QDate::fromString(query.value(0).toString(), "yyyy-MM-dd").toString("dd/MM/yyyy"));
            histLabel->setText(dateString +
                                QString(exitDateString.isEmpty() ? "" : (" au " + exitDateString)) +
                                " : <b>Garderie</b> (" +
                                (query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString()).trimmed() + ")");

        }



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
