#ifndef DOGCARD_H
#define DOGCARD_H

#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QGridLayout>

class DogCard : public QFrame
{
    Q_OBJECT
public:
    explicit DogCard(QWidget *parent = nullptr) : QFrame(parent) {}
    DogCard(QString id_dog, QString name, QString sex, QString birth, QString date_prov);
    void resizeEvent(QResizeEvent *event);

private:
    QPixmap sexIcon;
    QLabel* sexLabel;
};

#endif // DOGCARD_H
