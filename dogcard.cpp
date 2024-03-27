#include "dogcard.h"

DogCard::DogCard(QWidget *parent, QString id_dog, QString name, QString sex, QString birth, QString description, QString info) : DogCard(parent)
{
    mainWindow = parent;
    QString typeInfo = "care";
    if(birth.contains("___"))
        typeInfo = "out";
    else if (info.contains("___"))
        typeInfo = "current";


    QGridLayout *layout = new QGridLayout(this);

    setObjectName("dogCard" + id_dog);

    sexLabel = new QLabel(this);
    sexLabel->setAlignment(Qt::AlignCenter);

    int iconSize = 40;

    QPixmap sexIcon("media/" + QString(((sex == "Mâle") ? "male" : "female")) + ".png");
    sexIcon = sexIcon.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio);
    sexLabel->setPixmap(sexIcon);
    sexLabel->setStyleSheet("padding:3px;");



    QLabel *nameLabel = new QLabel(name, this);
    nameLabel->setStyleSheet("font-size:23pt;"
                             "font-weight:bold;");


    QString infoString;
    if(typeInfo == "current" || typeInfo == "out"){
        QStringList splitted = info.split("___");
        QString type_prov = splitted[1];
        QString date_prov = splitted[0];
        infoString = type_prov + " : ";
        infoString += QDate::fromString(date_prov, "yyyy-MM-dd").toString("dd/MM/yyyy");
    }

    else if(typeInfo == "care"){
        infoString = "Propriétaire : " + info;
    }


    QLabel *infoLabel = new QLabel(infoString, this);


    QString ageString;

    if(typeInfo == "out"){ // In this case, birth = date_dest___type_dest
        QString type_dest = birth.split("___")[1];
        QString date_dest = birth.split("___")[0];
        ageString = type_dest;
        if(sex == "Femelle" && type_dest == "Mort")
            type_dest += "e";
        ageString += " : " + QDate::fromString(date_dest, "yyyy-MM-dd").toString("dd/MM/yyyy");
    }

    else if (typeInfo == "current"){
        QDate date = QDate::fromString(birth, "yyyy-MM-dd");

        int years = QDate::currentDate().year() - date.year();
        int months = QDate::currentDate().month() - date.month();

        if (QDate::currentDate().day() < date.day())
            months--;

        if (months < 0) {
            years--;
            months += 12;
        }

        if (years == 1)
            ageString = "1 an, ";
        else if (years > 1)
            ageString = QString::number(years) + " ans, ";

        ageString += QString::number(months) + " mois";
    }

    else if(typeInfo == "care"){
        ageString = "Dernière garde : " + birth;
    }

    QLabel *ageLabel = new QLabel(ageString, this);


    QLabel *descriptionLabel = new QLabel(description, this);


    detailsButton = new QToolButton(this);
    detailsButton->setIcon(QIcon("media/right.png"));
    detailsButton->setIconSize(QSize(iconSize, iconSize));
    detailsButton->setFixedSize(iconSize, iconSize);

    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(SelectDogCard()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(SelectThis()));

    layout->addWidget(sexLabel, 1, 1);
    layout->addWidget(nameLabel, 1, 0);
    layout->addWidget(infoLabel, 2, 0, 1, 2);
    layout->addWidget(ageLabel, 3, 0, 1, 2);
    layout->addWidget(descriptionLabel, 4, 0);
    layout->addWidget(detailsButton, 4, 1);

    setLayout(layout);
}

void DogCard::resizeEvent(QResizeEvent *event){
    QFrame::resizeEvent(event);

    QSize parentSize = qobject_cast<QWidget*>(parent())->size();
    if(selected)
        setFixedSize(parentSize);
    else{
        setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        setMaximumSize(parentSize / 4);
    }
}

void DogCard::SelectThis(){
    disconnect(detailsButton, SIGNAL(clicked()), 0, 0);
    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(UnselectDogCard()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(UnselectThis()));
    selected = true;
    resizeEvent(nullptr);
}

void DogCard::UnselectThis(){
    disconnect(detailsButton, SIGNAL(clicked()), 0, 0);
    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(SelectDogCard()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(SelectThis()));
    selected = false;
    resizeEvent(nullptr);
}
