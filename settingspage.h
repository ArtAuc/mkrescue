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

#include "utils.h"
#include "simplecrypt.h"

class SettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsPage(QWidget *parent = nullptr) : QWidget(parent){

    }

    void showEvent(QShowEvent *event) override {
        QWidget::showEvent(event);
        connect(findChild<QPushButton*>("saveSettingsButton"), &QPushButton::clicked, this, &SettingsPage::SaveSettings);
    }

    void SetCrypto(SimpleCrypt *crypto){this->crypto = crypto;}

    void resizeEvent(QResizeEvent *event) override{
        QWidget::resizeEvent(event);
        QList<QWidget*> children = findChildren<QWidget*>();
        if(children.size() > 0){
            QFont font = children[0]->font();
            float fontSize = std::max(width() * 0.007, (double) 9);
            font.setPointSize(fontSize);

            for(QWidget* c : children){
                if(!c->objectName().contains("spinbox") && (qobject_cast<QLineEdit*>(c) || qobject_cast<QDateTimeEdit*>(c) || qobject_cast<QComboBox*>(c) || qobject_cast<QCheckBox*>(c))){
                    c->setFont(font);
                    c->setStyleSheet("padding:" + QString::number(fontSize) + "px;");
                }

                else if (qobject_cast<QLabel*>(c)){
                    c->setFont(font);
                }
            }

            font.setPointSize(fontSize * 1.2);

            findChild<QLabel*>("shelterInfosLabel")->setFont(font);

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
            SetField("nameShelterEdit", query.value(0).toString(), this);
            SetField("phoneShelterEdit", query.value(1).toString(), this);
            QStringList addressList = AddressList(crypto->decryptToString(query.value(2).toString()));
            SetField("addressShelterEdit", addressList[0], this);
            SetField("address2ShelterEdit", addressList[1], this);
            SetField("postalCodeShelterEdit", addressList[2], this);
            SetField("cityShelterEdit", addressList[3], this);
        }
    }


public slots:
    void SaveSettings(){
        QSqlQuery query;
        query.prepare("INSERT OR REPLACE INTO People "
                      "(id_people, last_name, phone, address, first_name, email) "
                      "VALUES (-2, :name, :phone, :address, '', '');");
        query.bindValue(":name", GetField("nameShelterEdit", this));
        query.bindValue(":phone", GetField("phoneShelterEdit", this));
        query.bindValue(":address",crypto->encryptToString(GetField("addressShelterEdit", this) + "\n" +
                                                            GetField("address2ShelterEdit", this) + "\n" +
                                                            GetField("postalCodeShelterEdit", this) + " " + GetField("cityShelterEdit", this)));

        if(!query.exec()){
            QMessageBox::critical(nullptr, "Erreur", "Erreur dans la sauvegarde des informations sur le refuge");
        }
    }

private:
    SimpleCrypt *crypto;
};

#endif // SETTINGSPAGE_H
