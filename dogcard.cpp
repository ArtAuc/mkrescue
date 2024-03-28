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

    descriptionLabel = new QLabel(description, this);


    int iconSize = 40;

    detailsButton = new QToolButton(this);
    detailsButton->setIcon(QIcon("media/right.png"));
    detailsButton->setIconSize(QSize(iconSize, iconSize));
    detailsButton->setFixedSize(iconSize, iconSize);

    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(SelectDogCard()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(SelectThis()));

    // Only shown if selected
    sterilizedBox = new TriStateCheckBox((sex == "Mâle") ? "Castré" : "Stérilisée", this);
    compatDogBox = new TriStateCheckBox("Compatible chien", this);
    compatCatBox = new TriStateCheckBox("Compatible chat", this);
    vetLabel = new QLabel("C");
    vetLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    vetLabel->setObjectName("vetLabel" + id_dog);

    layout->addWidget(nameSexWidget, 0, 0);
    layout->addWidget(info1Label, 2,  0);
    layout->addWidget(info2Label, 4, 0);
    layout->addWidget(descriptionLabel, 5, 0);
    layout->addWidget(detailsButton, 5, 2);
    layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), 9, 0);

    nameLabel->setObjectName("nameLabel" + id_dog);

    setLayout(layout);
}

void DogCard::resizeEvent(QResizeEvent *event){
    QFrame::resizeEvent(event);

    float factor;
    QSize parentSize = qobject_cast<QWidget*>(parent())->size();
    if(selected){
        layout->setSpacing(height() / 100);
        //layout->setContentsMargins(width() / 10, height() / 10, width() / 10, height() / 10);

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


    for(QWidget *c : findChildren<QWidget*>()){
        if(c->objectName() == "nameLabel" + id_dog){
            c->setStyleSheet("font-size:" + QString::number(factor * 0.03 * width()) + "pt;"
                                 "font-weight:bold;");
        }

        else if (qobject_cast<QLabel*>(c) || qobject_cast<TriStateCheckBox*>(c)){
            c->setStyleSheet("font-size:" + QString::number(factor * 0.02 * width()) + "pt;");
        }
    }

    vetLabel->setStyleSheet(vetLabel->styleSheet() + "margin-left:30px;"
                                                     "padding:30px;"
                                                     "border-left: 2px solid gray;");

    sexLabel->setPixmap(sexIcon.scaled(factor * size() / 10, Qt::KeepAspectRatio));
}

void DogCard::SelectThis(){
    selected = true;

    layout->removeWidget(detailsButton);
    layout->addWidget(detailsButton, 9, 2);
    disconnect(detailsButton, SIGNAL(clicked()), 0, 0);
    connect(detailsButton, SIGNAL(clicked()), mainWindow, SLOT(UnselectDogCard()));

    QSqlQuery query;
    query.exec("SELECT chip, birth, sterilized, compat_dog, compat_cat "
               "FROM Dogs "
               "WHERE id_dog = " + id_dog + ";");

    QLabel *chipLabel, *birthLabel;
    chipLabel = new QLabel();
    birthLabel = new QLabel();

    if(query.next()){

        chipLabel->setText(query.value(0).toString());
        birthLabel->setText("\nNaissance : " + QDate::fromString(query.value(1).toString(), "yyyy-MM-dd").toString("dd/MM/yyyy"));
        sterilizedBox->SqlToState(query.value(2).toString());
        compatDogBox->SqlToState(query.value(3).toString());
        compatCatBox->SqlToState(query.value(4).toString());
    }

    descriptionLabel->setText(descriptionLabel->text() + "\n");

    layout->addWidget(chipLabel, 1, 0);
    layout->addWidget(birthLabel, 3, 0);
    layout->addWidget(sterilizedBox, 6, 0);
    layout->addWidget(compatDogBox, 7, 0);
    layout->addWidget(compatCatBox, 8, 0);
    layout->addWidget(vetLabel, 0, 1, layout->rowCount(), 1);

    layout->setContentsMargins(layout->contentsMargins() + 50);


    resizeEvent(nullptr);
}


void DogCard::SaveCard(){
    QSqlQuery query;
    query.prepare("UPDATE Dogs "
                  "SET sterilized = :sterilized, "
                  "compat_dog = :compat_dog, "
                  "compat_cat = :compat_cat "
                  "WHERE id_dog = :id;");

    query.bindValue(":sterilized", sterilizedBox->StateToSql());
    query.bindValue(":compat_dog", compatDogBox->StateToSql());
    query.bindValue(":compat_cat", compatCatBox->StateToSql());
    query.bindValue(":id", id_dog);

    query.exec();
}
