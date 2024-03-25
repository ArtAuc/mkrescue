#include "dogcard.h"

DogCard::DogCard(QWidget *parent) : QFrame(parent)
{
    QGridLayout *layout = new QGridLayout(this);

    QLabel *sexLabel = new QLabel(this);
    sexLabel->setAlignment(Qt::AlignCenter);

    QPixmap pixmap("media/female.png");
    QSize labelSize = QSize(30, 30);
    QPixmap scaledPixmap = pixmap.scaled(labelSize, Qt::KeepAspectRatio);
    sexLabel->setPixmap(scaledPixmap);

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
