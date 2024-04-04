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

class SettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsPage(QWidget *parent = nullptr) : QWidget(parent){}
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

            QLabel *shelterInfosLabel = findChild<QLabel*>("shelterInfosLabel");
            shelterInfosLabel->setFont(font);

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

public slots:
    void LoadSettings(){
        QSqlQuery query;
        query.exec("SELECT last_name, "
                   "phone, "
                   "address "
                   "FROM People "
                   "WHERE id_people = -2;");

        if(query.next()){
        }
    }

    void SaveSettings(){

    }
};

#endif // SETTINGSPAGE_H
