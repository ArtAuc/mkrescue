#include "dogcard.h"

DogCard::DogCard(QWidget *parent, QString id_dog, QString name, QString sex, QString birth, QString description, QString info, QString typeInfo) : DogCard(parent)
{
    QGridLayout *layout = new QGridLayout(this);

    setObjectName("dogCard" + id_dog);

    sexLabel = new QLabel(this);
    sexLabel->setAlignment(Qt::AlignCenter);

    sexIcon = QPixmap("media/" + QString(((sex == "Mâle") ? "male" : "female")) + ".png");
    sexLabel->setPixmap(sexIcon);
    sexLabel->setStyleSheet("padding:3px;");

    QDate date = QDate::fromString(birth, "yyyy-MM-dd");

    int years = QDate::currentDate().year() - date.year();
    int months = QDate::currentDate().month() - date.month();

    if (QDate::currentDate().day() < date.day())
        months--;

    if (months < 0) {
        years--;
        months += 12;
    }

    QString ageString;
    if (years == 1)
        ageString = "1 an, ";
    else if (years > 1)
        ageString = QString::number(years) + " ans, ";

    ageString += QString::number(months) + " mois";
    QLabel *ageLabel = new QLabel(ageString, this);
    ageLabel->setFont(QFont("Arial", 15));

    QLabel *nameLabel = new QLabel(name, this);
    nameLabel->setFont(QFont("Arial", 20, QFont::Bold));

    QString infoString;
    if(typeInfo == "current"){
        infoString = "Entré" + QString((sex == "Mâle") ? "" : "e");
        infoString += " le " + QDate::fromString(info, "yyyy-MM-dd").toString("dd/MM/yyyy");
    }

    QLabel *infoLabel = new QLabel(infoString, this);
    infoLabel->setFont(QFont("Arial", 15));

    QLabel *descriptionLabel = new QLabel(description, this);
    descriptionLabel->setFont(QFont("Arial", 15));


    QToolButton *detailsButton = new QToolButton(this);
    detailsButton->setIcon(QIcon("media/right.png"));
    detailsButton->setIconSize(QSize(50, 50));
    detailsButton->setFixedSize(40, 40);

    connect(detailsButton, SIGNAL(clicked()), parent, SLOT(SelectDogCard()));

    layout->addWidget(sexLabel, 1, 1);
    layout->addWidget(ageLabel, 2, 0, 1, 2);
    layout->addWidget(nameLabel, 1, 0);
    layout->addWidget(infoLabel, 3, 0, 1, 2);
    layout->addWidget(descriptionLabel, 4, 0);
    layout->addWidget(detailsButton, 4, 1);

    setLayout(layout);
}

void DogCard::resizeEvent(QResizeEvent *event){
    QFrame::resizeEvent(event);

    QSize parentSize = qobject_cast<QWidget*>(parent())->size();
    this->setMaximumSize(parentSize / 4);

    int iconSize = 0.1 * width();
    sexLabel->setPixmap(sexIcon.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio));
}
