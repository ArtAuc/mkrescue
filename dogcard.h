#ifndef DOGCARD_H
#define DOGCARD_H

#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QGridLayout>
#include <QDate>

class DogCard : public QFrame
{
    Q_OBJECT
public:
    explicit DogCard(QWidget *parent = nullptr) : QFrame(parent) {}
    DogCard(QString id_dog, QString name, QString sex, QString birth, QString info, QString typeInfo);
    void resizeEvent(QResizeEvent *event);

private:
    QPixmap sexIcon;
    QLabel* sexLabel;
};

#endif // DOGCARD_H
