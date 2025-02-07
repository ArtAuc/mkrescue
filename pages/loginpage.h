#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QLabel>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QLayout>

class LoginPage : public QWidget
{
    Q_OBJECT
public:
    explicit LoginPage(QWidget *parent = nullptr) : QWidget(parent){
        icon = QPixmap("media/logo_title.png");
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

    void ShakeButton(){
        // Shake if wrong password
        if (!shakeAnimation || shakeAnimation->state() != QAbstractAnimation::Running){
            if(shakeAnimation != nullptr)
                delete shakeAnimation;

            shakeAnimation = new QPropertyAnimation(nextButton, "geometry");
            shakeAnimation->setDuration(100);
            shakeAnimation->setLoopCount(2);
            shakeAnimation->setEasingCurve(QEasingCurve::OutInQuad);

            QRect startRect = nextButton->geometry();
            QRect endRect = startRect.translated(10, 0);

            shakeAnimation->setStartValue(startRect);
            shakeAnimation->setKeyValueAt(0.5, endRect);
            shakeAnimation->setEndValue(startRect);

            shakeAnimation->start();
        }
    }


public slots:
    void NextStep(){
        if(label->text().startsWith("Nouvelle")){
            hash = QCryptographicHash::hash((lineEdit->text() + "refuge510refuge").toUtf8(), QCryptographicHash::Sha256);
            label->setText("Confirmez le mot de passe");
        }

        else if(label->text().startsWith("Confirmez")){
            QByteArray newHash = QCryptographicHash::hash((lineEdit->text() + "refuge510refuge").toUtf8(), QCryptographicHash::Sha256);
            if(newHash == hash){
                label->setText("Entrez l'adresse email de sauvegarde");
                lineEdit->setEchoMode(QLineEdit::Normal);
            }
            else{
                ShakeButton();
                label->setText("Nouvelle installation : choisissez un mot de passe (non modifiable)");
            }
        }

        else if(label->text().startsWith("Entrez l'adresse email de sauvegarde")){
            email = lineEdit->text();
            label->setText("Entrez le mot de passe application correspondant");
            lineEdit->setEchoMode(QLineEdit::Password);
        }

        else if(label->text().startsWith("Entrez le mot de passe application correspondant")){
            appPassword = lineEdit->text();
            emit Unlock(hash, email, appPassword);
        }

        else if(label->text().startsWith("Veuillez")){
            emit Unlock(QCryptographicHash::hash((lineEdit->text() + "refuge510refuge").toUtf8(), QCryptographicHash::Sha256));
            ShakeButton();
        }

        lineEdit->clear();
    }

signals:
    void Unlock(QByteArray h, QString email = "", QString appPassword = "");

private:
    QPixmap icon;
    QLabel *label, *iconLabel;
    QLineEdit *lineEdit;
    QByteArray hash;
    QString email;
    QString appPassword;
    QPushButton *nextButton;
    QPropertyAnimation *shakeAnimation = nullptr;
};

#endif // LOGINPAGE_H
