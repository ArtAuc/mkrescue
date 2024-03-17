#include "editpage.h"

EditPage::EditPage(QWidget *parent)
    : QWidget{parent}
{

}

void EditPage::AddEntry(){
    Edit("entry", {});
}

void EditPage::SwitchPage(QString pageName){
        qobject_cast<QStackedWidget*>(parent())->setCurrentWidget(this);
        findChild<QStackedWidget*>("editStacked")->setCurrentWidget(findChild<QWidget*>(pageName));
}

void EditPage::Edit(QString type, QStringList infos){
    ClearAllPages();
    if(type == "entry"){
        SwitchPage("editEntryPage");

        QWidget* entreeTab = findChild<QWidget*>("entryTab1");

        qDebug() << infos;

        if(infos.size() > 0){
            // Entrée

            entreeTab->findChild<QDateEdit*>("entryDateEdit")->setDate(QDate::fromString(infos[1], "yyyy-MM-dd"));
            entreeTab->findChild<QComboBox*>("entryTypeBox")->setCurrentText(infos[2]);
            entreeTab->findChild<QLineEdit*>("lastNameAbandonEdit")->setText(infos[3]);
            entreeTab->findChild<QLineEdit*>("firstNameAbandonEdit")->setText(infos[4]);

            QStringList addressList = infos[5].split(infos[5].contains("\\n") ? "\\n" : "\n");
            if(addressList.size() < 3)
                qDebug() << "Wrong address format";

            else{
                entreeTab->findChild<QLineEdit*>("addressAbandonEdit")->setText(addressList[0]);
                entreeTab->findChild<QLineEdit*>("address2AbandonEdit")->setText(addressList[1]);

                entreeTab->findChild<QLineEdit*>("cityAbandonEdit")->setText(addressList[addressList.size() - 1].split(" ")[0]);
                entreeTab->findChild<QLineEdit*>("postalCodeAbandonEdit")->setText(addressList[addressList.size() - 1].split(" ")[1]);
            }

            entreeTab->findChild<QLineEdit*>("phoneAbandonEdit")->setText(infos[6]);
            entreeTab->findChild<QLineEdit*>("emailAbandonEdit")->setText(infos[7]);
        }
    }
}

void EditPage::ClearAllPages()
{
    for (auto widget : findChildren<QWidget*>()) {
        if (QDateEdit *dateEdit = qobject_cast<QDateEdit *>(widget))
            dateEdit->setDate(QDate::currentDate());
        else if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget))
            lineEdit->clear();
        else if (QComboBox *box = qobject_cast<QComboBox *>(widget))
            box->setCurrentText("Abandon");
    }
    ChangeEntryType("Abandon");
}

void EditPage::ChangeEntryType(QString type)
{
    QWidget* entreeTab = findChild<QWidget*>("entryTab1");
    for (auto widget : entreeTab->findChildren<QWidget*>()) {
        if(widget->objectName() == "poundPlaceEdit") // Pound
            widget->setVisible(type != "Abandon");
        else if(widget->objectName() == "firstNameAbandonEdit" ||
                widget->objectName() == "lastNameAbandonEdit" ||
                widget->objectName() == "phoneAbandonEdit" ||
                widget->objectName() == "emailAbandonEdit" ||
                widget->objectName() == "addressAbandonEdit" ||
                widget->objectName() == "address2AbandonEdit" ||
                widget->objectName() == "postalCodeAbandonEdit" ||
                widget->objectName() == "cityAbandonEdit") // Abandoned
            widget->setVisible(type == "Abandon");
    }
}
