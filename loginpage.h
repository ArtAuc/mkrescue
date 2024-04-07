#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QLabel>
#include <QLineEdit>
#include <QCryptographicHash>

class LoginPage : public QWidget
{
    Q_OBJECT
public:
    explicit LoginPage(QWidget *parent = nullptr) : QWidget(parent){

    }

    void resizeEvent(QResizeEvent *event) override{
        QWidget::resizeEvent(event);
        setContentsMargins(0.11 * width(), 0, 0.11 * width(), 0);
    }


    void SetMode(bool creating){
        label = findChild<QLabel*>();
        lineEdit = findChild<QLineEdit*>();

        connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(NextStep()));

        if(creating)
            label->setText("Nouvelle installation : choisissez un mot de passe (non modifiable)");

        else
            label->setText("Veuillez entrer le mot de passe");
    }

public slots:
    void NextStep(){
        if(label->text().startsWith("Nouvelle")){
            hash = QCryptographicHash::hash((lineEdit->text() + "refuge").toUtf8(), QCryptographicHash::Sha256);
            label->setText("Confirmez le mot de passe");
        }

        else if(label->text().startsWith("Confirmez")){
            QByteArray newHash = QCryptographicHash::hash((lineEdit->text() + "refuge").toUtf8(), QCryptographicHash::Sha256);
            if(newHash == hash)
                label->setText("Entrez la clé Google Drive API (non modifiable)");
            else
                label->setText("Nouvelle installation : choisissez un mot de passe (non modifiable)");
        }

        else if(label->text().startsWith("Entrez")){
            emit Unlock(hash);
        }

        else if(label->text().startsWith("Veuillez")){
            emit Unlock(QCryptographicHash::hash((lineEdit->text() + "refuge").toUtf8(), QCryptographicHash::Sha256));
        }

        lineEdit->clear();
    }

signals:
    void Unlock(QByteArray h);

private:
    QLabel *label;
    QLineEdit *lineEdit;
    QByteArray hash;
};

#endif // LOGINPAGE_H
