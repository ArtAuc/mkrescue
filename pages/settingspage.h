#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QLayout>
#include <QSqlQuery>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QTimer>

#include "utils.h"
#include "editpage/editpage.h"
#include "../data/saveddata.h"

class SettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsPage(QWidget *parent = nullptr) : QWidget(parent){

    }

    void showEvent(QShowEvent *event) override {
        QWidget::showEvent(event);
        connect(findChild<QPushButton*>("saveSettingsButton"), &QPushButton::clicked, this, &SettingsPage::SaveSettings);
        findChild<QTabWidget*>()->setCurrentIndex(0);

        findChild<QLabel*>("savedLabel")->hide();
    }

    void resizeEvent(QResizeEvent *event) override{
        QWidget::resizeEvent(event);
        QList<QWidget*> children = findChildren<QWidget*>();
        if(children.size() > 0){
            QFont font = children[0]->font();
            float fontSize = std::max(width() * 0.007, (double) 9);
            font.setPointSize(fontSize);

            for(QWidget* c : children){
                if(!c->objectName().contains("spinbox") && (qobject_cast<QLineEdit*>(c) || qobject_cast<QDateTimeEdit*>(c) || qobject_cast<QComboBox*>(c) || qobject_cast<QCheckBox*>(c) || qobject_cast<QDoubleSpinBox*>(c))){
                    c->setFont(font);
                    c->setStyleSheet("padding:" + QString::number(fontSize) + "px;");
                }

                else if (qobject_cast<QLabel*>(c)){
                    c->setFont(font);
                }
            }

            font.setPointSize(fontSize * 1.2);

            for(QPushButton *b : findChildren<QPushButton*>())
            {
                b->setFont(font);
                b->setStyleSheet("padding:" + QString::number(fontSize) + "px;");
            }
        }

        // Margins
        QMargins margins = layout()->contentsMargins();
        margins.setLeft(0.2 * width());
        margins.setRight(0.2 * width());
        layout()->setContentsMargins(margins);
    }

    void LoadSettings(){
        QSqlQuery query;
        if(!query.exec("SELECT last_name, "
                   "phone, "
                   "address "
                   "FROM People "
                   "WHERE id_people = -2;"))
            QMessageBox::critical(nullptr, "Erreur", "Erreur dans le chargement des informations sur le refuge");

        if(query.next()){
            EditPage::SetField("nameShelterEdit", query.value(0).toString(), this);
            EditPage::SetField("phoneShelterEdit", query.value(1).toString(), this);
            QStringList addressList = EditPage::AddressList(crypto->decryptToString(query.value(2).toString()));
            EditPage::SetField("addressShelterEdit", addressList[0], this);
            EditPage::SetField("address2ShelterEdit", addressList[1], this);
            EditPage::SetField("postalCodeShelterEdit", addressList[2], this);
            EditPage::SetField("cityShelterEdit", addressList[3], this);
        }

        EditPage::SetField("maxDogsSpin", savedData->MaxDogs(), this);
        EditPage::SetField("maxDaysSpin", savedData->MaxDays(), this);
    }

    void SetSavedData(SavedData *savedData){this->savedData = savedData;}


public slots:
    void SaveSettings(){
        QSqlQuery query;
        query.prepare("INSERT OR REPLACE INTO People "
                      "(id_people, last_name, phone, address, first_name, email) "
                      "VALUES (-2, :name, :phone, :address, '', '');");
        query.bindValue(":name", EditPage::GetField("nameShelterEdit", this));
        query.bindValue(":phone", EditPage::GetField("phoneShelterEdit", this));
        query.bindValue(":address",crypto->encryptToString(EditPage::GetField("addressShelterEdit", this) + "\n" +
                                                            EditPage::GetField("address2ShelterEdit", this) + "\n" +
                                                            EditPage::GetField("postalCodeShelterEdit", this) + " " + EditPage::GetField("cityShelterEdit", this)));

        if(!query.exec()){
            QMessageBox::critical(nullptr, "Erreur", "Erreur dans la sauvegarde des informations sur le refuge");
        }

        else{
            QLabel *savedLabel = findChild<QLabel*>("savedLabel");
            savedLabel->show();
            QTimer::singleShot(3000, [=]() {savedLabel->hide();});
        }

        savedData->SetMaxDogs(EditPage::GetField("maxDogsSpin", this));
        savedData->SetMaxDays(EditPage::GetField("maxDaysSpin", this));
    }

private:
    SavedData *savedData;
};

#endif // SETTINGSPAGE_H
