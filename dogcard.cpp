#include "dogcard.h"

DogCard::DogCard(QString id_dog, QString name, QString sex, QString birth, QString date_prov)
{
    QGridLayout *layout = new QGridLayout(this);

    sexLabel = new QLabel(this);
    sexLabel->setAlignment(Qt::AlignCenter);


    sexIcon = QPixmap("media/" + QString(((sex == "Mâle") ? "male" : "female")) + ".png");
    sexLabel->setPixmap(sexIcon);

    QLabel *ageLabel = new QLabel("3 ans 7 mois", this);
    ageLabel->setFont(QFont("Arial", 15));

    QLabel *nameLabel = new QLabel("Nomauniesrta ", this);
    nameLabel->setFont(QFont("Arial", 20, QFont::Bold));

    QLabel *entryLabel = new QLabel("Entré le XX/XX/XXXX", this);
    entryLabel->setFont(QFont("Arial", 15));

    QLabel *descriptionLabel = new QLabel("X Malinois", this);
    descriptionLabel->setFont(QFont("Arial", 15));


    QToolButton *toolButton = new QToolButton(this);
    toolButton->setText(">");


    layout->addWidget(sexLabel, 1, 1);
    layout->addWidget(ageLabel, 2, 0, 1, 2);
    layout->addWidget(nameLabel, 1, 0);
    layout->addWidget(entryLabel, 3, 0, 1, 2);
    layout->addWidget(descriptionLabel, 4, 0);
    layout->addWidget(toolButton, 4, 1);

    setLayout(layout);
}

void DogCard::resizeEvent(QResizeEvent *event){
    QFrame::resizeEvent(event);

    QSize parentSize = qobject_cast<QWidget*>(parent())->size();
    this->setMaximumSize(parentSize / 3);

    int iconSize = 0.05 * width();
    sexLabel->setPixmap(sexIcon.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio));
}
