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
#include <QPlainTextEdit>

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

        bodyTextEdit = findChild<QPlainTextEdit*>("bodyTextEdit");

        findChild<QLabel*>("savedLabel")->hide();
        emailActivatedCheckbox = findChild<QCheckBox*>("bodyCheckBox");
        disconnect(emailActivatedCheckbox, nullptr, nullptr, nullptr);
        connect(emailActivatedCheckbox, &QCheckBox::clicked, this, &SettingsPage::SwitchRecallEmail);
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

        QFile bodyFile("recall_email.txt");
        emailActivatedCheckbox->setChecked(bodyFile.exists());
        SwitchRecallEmail();
    }

    void SetSavedData(SavedData *savedData){this->savedData = savedData;}


public slots:
    void SaveSettings(){
        // Save file
        QSqlQuery query;
        query.prepare("INSERT OR REPLACE INTO People "
                      "(id_people, last_name, phone, address, first_name, email) "
                      "VALUES (-2, :name, :phone, :address, '', '');");
        QString shelterName = EditPage::GetField("nameShelterEdit", this);
        QString shelterPhone = EditPage::GetField("phoneShelterEdit", this);
        QString shelterAddress = crypto->encryptToString(EditPage::GetField("addressShelterEdit", this) + "\n" +
                                                         EditPage::GetField("address2ShelterEdit", this) + "\n" +
                                                         EditPage::GetField("postalCodeShelterEdit", this) + " " + EditPage::GetField("cityShelterEdit", this));

        savedData->SetShelterInfos(shelterName, shelterPhone, shelterAddress);

        query.bindValue(":name", shelterName);
        query.bindValue(":phone", shelterPhone);
        query.bindValue(":address", shelterAddress);

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

        QFile bodyFile("recall_email.txt");
        if(emailActivatedCheckbox->isChecked() && bodyFile.open(QIODevice::WriteOnly)){
            bodyFile.write(bodyTextEdit->toPlainText().toUtf8());
            savedData->SetRecallMailBody(bodyTextEdit->toPlainText().toUtf8());
        }
        else{
            savedData->SetRecallMailBody("");
            bodyFile.remove();
        }
    }

    void SwitchRecallEmail(){
        QFile bodyFile("recall_email.txt");
        if(emailActivatedCheckbox->isChecked()){
            emailActivatedCheckbox->setText("Rappel activé");

            if(bodyFile.exists() && bodyFile.open(QIODevice::ReadOnly)){
                bodyTextEdit->setPlainText(QString::fromUtf8(bodyFile.readAll()));
            }
            else{
                bodyTextEdit->setPlainText("Bonjour $prenom_dest $nom_fam_dest,\n\n"
                                           "Nous vous écrivons pour vous rappeler que votre adhésion à $nom_refuge arrive à son terme le $date_exp.\n\n"
                                           "Votre adhésion nous permet de continuer le sauvetage et le soin des chiens, elle est essentielle pour notre association. "
                                           "Si vous avez des questions ou rencontrez des difficultés pour le renouvellement de votre adhésion, n'hésitez pas à nous contacter par téléphone au $telephone_refuge ou à venir nous voir directement.\n\n"
                                           "Merci pour votre engagement,\n"
                                           "$nom_refuge\n"
                                           "$adresse_refuge");
            }
        }

        else{
            emailActivatedCheckbox->setText("Rappel désactivé");
        }

        findChild<QWidget*>("bodyWidget")->setVisible(emailActivatedCheckbox->isChecked());
    }

private:
    SavedData *savedData;
    QCheckBox *emailActivatedCheckbox;
    QPlainTextEdit *bodyTextEdit;
};

#endif // SETTINGSPAGE_H
