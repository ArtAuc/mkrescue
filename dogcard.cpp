#include "dogcard.h"

DogCard::DogCard(QWidget *parent, QString id_dog, QString name, QString sex, QString info2, QString description, QString info1) : DogCard(parent)
{
    this->id_dog = id_dog;
    mainWindow = parent;

    QString typeInfo = "care";
    if(info2.contains("___"))
        typeInfo = "out";
    else if (info1.contains("___"))
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

    QLabel *info2Label = new QLabel(info2String, this);

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
    vetLabel = new QLabel("C");
    birthLabel = new QLabel();
    vetLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(nameSexWidget, 0, 0);
    layout->addWidget(info1Label, 3,  0);
    layout->addWidget(info2Label, 4, 0);
    layout->addWidget(descriptionLabel, 5, 0);
    layout->addWidget(detailsButton, 5, 2);

    nameLabel->setObjectName("nameLabel" + id_dog);
    vetLabel->setObjectName("vetLabel" + id_dog);

    setLayout(layout);
}

void DogCard::resizeEvent(QResizeEvent *event){
    QFrame::resizeEvent(event);

    float factor;
    QSize parentSize = qobject_cast<QWidget*>(parent())->size();
    if(selected){
        if(maximumWidth() <= width() || maximumHeight() <= height())
            setMaximumSize(size() + QSize(5, 5));
        else
            setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        factor = 0.7;
    }
    else{
        setMaximumSize(parentSize / 4);
        factor = 2;
    }

    for(QLabel *label : findChildren<QLabel*>()){
        if(label->objectName() == "nameLabel" + id_dog){
            label->setStyleSheet("font-size:" + QString::number(factor * 0.03 * width()) + "pt;"
                                 "font-weight:bold;");
        }

        else{
            label->setStyleSheet("font-size:" + QString::number(factor * 0.02 * width()) + "pt;");
        }
    }

    vetLabel->setStyleSheet(vetLabel->styleSheet() + "margin-left:30px;"
                                                     "padding:30px;"
                                                     "border-left: 2px solid gray;");

    sexLabel->setPixmap(sexIcon.scaled(factor * size() / 10, Qt::KeepAspectRatio));
}

void DogCard::SelectThis(){
    disconnect(detailsButton, SIGNAL(clicked()), 0, 0);
    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(UnselectDogCard()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(UnselectThis()));
    selected = true;

    QSqlQuery query;
    query.exec("SELECT chip, birth "
               "FROM Dogs "
               "WHERE id_dog = " + id_dog + ";");

    if(query.next()){
        chipLabel->setText(query.value(0).toString());
        birthLabel->setText("Naissance : " + QDate::fromString(query.value(1).toString(), "yyyy-MM-dd").toString("dd/MM/yyyy"));
    }

    layout->addWidget(vetLabel, 0, 1, 3, 1);
    layout->addWidget(chipLabel, 1, 0);
    layout->addWidget(birthLabel, 2, 0);

    resizeEvent(nullptr);
}

void DogCard::UnselectThis(){
    disconnect(detailsButton, SIGNAL(clicked()), 0, 0);
    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(SelectDogCard()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(SelectThis()));
    selected = false;
    layout->removeWidget(chipLabel);
    layout->removeWidget(birthLabel);
    layout->removeWidget(vetLabel);

    resizeEvent(nullptr);
}
