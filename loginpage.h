#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QLabel>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QPushButton>
#include <QLayout>

class LoginPage : public QWidget
{
    Q_OBJECT
public:
    explicit LoginPage(QWidget *parent = nullptr) : QWidget(parent){
        icon = QPixmap("media/logo.png");
    }

    void resizeEvent(QResizeEvent *event) override{
        QWidget::resizeEvent(event);

        QFont font = label->font();
        float fontSize = std::max(width() * 0.01, (double) 9);
        font.setPointSize(fontSize);

        lineEdit->setFont(font);
        lineEdit->setStyleSheet("padding:" + QString::number(fontSize) + "px;");

        label->setFont(font);

        nextButton->setFont(font);
        nextButton->setStyleSheet("padding:" + QString::number(fontSize) + "px;");

        float elementsWidth = 0.35 * width();
        lineEdit->setMaximumWidth(elementsWidth);
        nextButton->setMaximumWidth(elementsWidth);

        iconLabel->setPixmap(icon.scaled(0.3 * width(), 0.3 * width(), Qt::KeepAspectRatio));
    }


    void SetMode(bool creating){
        label = findChild<QLabel*>("loginLabel");
        iconLabel = findChild<QLabel*>("loginIcon");
        lineEdit = findChild<QLineEdit*>();
        nextButton = findChild<QPushButton*>();

        connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(NextStep()));
        connect(nextButton, SIGNAL(clicked()), this, SLOT(NextStep()));

        if(creating)
            label->setText("Nouvelle installation : choisissez un mot de passe (non modifiable)");

        else
            label->setText("Veuillez entrer le mot de passe");

        lineEdit->setEchoMode(QLineEdit::Password);
        lineEdit->setPlaceholderText("Mot de passe");
    }

public slots:
    void NextStep(){
        if(label->text().startsWith("Nouvelle")){
            hash = QCryptographicHash::hash((lineEdit->text() + "refuge").toUtf8(), QCryptographicHash::Sha256);
            label->setText("Confirmez le mot de passe");
        }

        else if(label->text().startsWith("Confirmez")){
            QByteArray newHash = QCryptographicHash::hash((lineEdit->text() + "refuge").toUtf8(), QCryptographicHash::Sha256);
            if(newHash == hash){
                label->setText("Entrez la clé Google Drive API (non modifiable)");
                lineEdit->setEchoMode(QLineEdit::Normal);
            }
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
    QPixmap icon;
    QLabel *label, *iconLabel;
    QLineEdit *lineEdit;
    QByteArray hash;
    QPushButton *nextButton;
};

#endif // LOGINPAGE_H
