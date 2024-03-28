#include "dogcard.h"

DogCard::DogCard(QWidget *parent, QString id_dog, QString name, QString sex, QString info1, QString description, QString info2) : DogCard(parent)
{
    mainWindow = parent;
    QString typeInfo = "care";
    if(info1.contains("___"))
        typeInfo = "out";
    else if (info2.contains("___"))
        typeInfo = "current";


    layout = new QGridLayout(this);

    setObjectName("dogCard" + id_dog);

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

    QString info2String;
    if(typeInfo == "current" || typeInfo == "out"){
        QStringList splitted = info2.split("___");
        QString type_prov = splitted[1];
        QString date_prov = splitted[0];
        info2String = type_prov + " : ";
        info2String += QDate::fromString(date_prov, "yyyy-MM-dd").toString("dd/MM/yyyy");
    }

    else if(typeInfo == "care"){
        info2String = "Propriétaire : " + info2;
    }


    QLabel *info2Label = new QLabel(info2String, this);

    QString ageString;

    if(typeInfo == "out"){ // Here info1 = date_dest___type_dest
        QString type_dest = info1.split("___")[1];
        QString date_dest = info1.split("___")[0];
        ageString = type_dest;
        if(sex == "Femelle" && type_dest == "Mort")
            type_dest += "e";
        ageString += " : " + QDate::fromString(date_dest, "yyyy-MM-dd").toString("dd/MM/yyyy");
    }

    else if (typeInfo == "current"){
        QDate date = QDate::fromString(info1, "yyyy-MM-dd");

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
        ageString = "Dernière garde : " + QDate::fromString(info1, "yyyy-MM-dd").toString("dd/MM/yyyy");
    }

    QLabel *ageLabel = new QLabel(ageString, this);

    QLabel *descriptionLabel = new QLabel(description, this);


    int iconSize = 40;

    detailsButton = new QToolButton(this);
    detailsButton->setIcon(QIcon("media/right.png"));
    detailsButton->setIconSize(QSize(iconSize, iconSize));
    detailsButton->setFixedSize(iconSize, iconSize);

    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(SelectDogCard()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(SelectThis()));


    // Labels only visible when selected
    chipLabel = new QLabel();
    vetLabel = new QLabel();

    layout->addWidget(nameSexWidget, 0, 0);
    layout->addWidget(info2Label, 1,  0);
    layout->addWidget(ageLabel, 2, 0);
    layout->addWidget(descriptionLabel, 5, 0);
    layout->addWidget(detailsButton, 5, 2);

    nameLabel->setObjectName("nameLabel" + id_dog);

    setLayout(layout);
}

void DogCard::resizeEvent(QResizeEvent *event){
    QFrame::resizeEvent(event);

    float factor;
    QSize parentSize = qobject_cast<QWidget*>(parent())->size();
    if(selected){
        if(maximumWidth() <= width() || maximumHeight() <= height())
            setMaximumSize(size() + QSize(3, 3));
        else
            setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        factor = 0.7;
    }
    else{
        setMaximumSize(parentSize / 4);
        factor = 2;
    }

    for(QLabel *label : findChildren<QLabel*>()){
        if(!label->objectName().contains("nameLabel")){
            label->setStyleSheet("font-size:" + QString::number(factor * 0.02 * width()) + "pt;");
        }

        else{
            label->setStyleSheet("font-size:" + QString::number(factor * 0.03 * width()) + "pt;"
                                 "font-weight:bold;");
        }
    }

    sexLabel->setPixmap(sexIcon.scaled(factor * size() / 10, Qt::KeepAspectRatio));
}

void DogCard::SelectThis(){
    disconnect(detailsButton, SIGNAL(clicked()), 0, 0);
    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(UnselectDogCard()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(UnselectThis()));
    selected = true;
    layout->addWidget(vetLabel, 0, 2, 3, 1);

    resizeEvent(nullptr);
}

void DogCard::UnselectThis(){
    disconnect(detailsButton, SIGNAL(clicked()), 0, 0);
    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(SelectDogCard()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(SelectThis()));
    selected = false;
    layout->removeWidget(vetLabel);

    resizeEvent(nullptr);
}
