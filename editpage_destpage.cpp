#include "editpage.h"

void EditPage::ClearAllPages()
{
    // Destinations ES
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    for (int i = destStacked->count() - 1; i >= 0; --i) {
        QWidget *widget = destStacked->widget(i);
        destStacked->removeWidget(widget);
        delete widget;
    }

    qDeleteAll(destStacked->findChildren<DestinationPage*>());

    destinationsNumber = 0;
    AddDestPage();

    for (QWidget* widget : findChildren<QWidget*>()) {
        if (QDateEdit *dateEdit = qobject_cast<QDateEdit*>(widget))
            dateEdit->setDate(QDate::currentDate());
        else if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(widget))
            lineEdit->clear();
        else if (QComboBox *box = qobject_cast<QComboBox*>(widget))
            box->setCurrentIndex(0);
    }
    ChangeEntryType("Abandon");

    UpdateButtons();
}

void EditPage::UpdateDestinationPages(QString type){
    // Update stacked widget
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    int currentPage = destStacked->currentIndex() + 1;
    if(type != ""){
        if(currentPage > destinationsNumber){
            destinationsNumber += 1;
            AddDestPage();
        }
    }

    else{
        // Clear pages after current
        destinationsNumber = currentPage - 1;

        for (int i = destStacked->count() - 1; i >= currentPage; i--)
            destStacked->removeWidget(destStacked->widget(i));
    }

    UpdateButtons();
}

void EditPage::UpdateButtons(){
    QToolButton* prevButton = findChild<QToolButton*>("prevDestButton");
    QToolButton* nextButton = findChild<QToolButton*>("nextDestButton");
    int currentPage = findChild<QStackedWidget*>("destStackedWidget")->currentIndex() + 1;

    if (currentPage <= 1){ // Gray prev button
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect(prevButton);
        effect->setColor(Qt::gray);
        prevButton->setGraphicsEffect(effect);
    }

    else
        prevButton->setGraphicsEffect(nullptr);

    if (currentPage > destinationsNumber){ // Gray next button
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect(nextButton);
        effect->setColor(Qt::gray);
        nextButton->setGraphicsEffect(effect);
    }

    else
        nextButton->setGraphicsEffect(nullptr);

    resizeEvent(nullptr);
}

void EditPage::PrevDestPage(){
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    int currentPage = destStacked->currentIndex() + 1;

    if(currentPage > 1){
        currentPage -= 1;
        destStacked->setCurrentIndex(currentPage - 1);
        UpdateButtons();
    }
}

void EditPage::NextDestPage(){
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    int currentPage = destStacked->currentIndex() + 1;

    if(currentPage < destStacked->count()){
        currentPage += 1;
        destStacked->setCurrentIndex(currentPage - 1);
        UpdateButtons();
    }
}

void EditPage::AddDestPage(){
    QStackedWidget* destStacked = findChild<QStackedWidget*>("destStackedWidget");
    DestinationPage* page = new DestinationPage(QString::number(destinationsNumber + 1));
    destStacked->insertWidget(destinationsNumber, page);
    page->ChangeDestType("");
    connect(page->findChild<QComboBox*>(), SIGNAL(currentTextChanged(QString)), page, SLOT(ChangeDestType(QString)));
    connect(page->findChild<QComboBox*>(), SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateDestinationPages(QString)));
}
