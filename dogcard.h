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
    DogCard(QWidget *parent, QString id_dog, QString name, QString sex, QString birth, QString description, QString info, QString typeInfo);
    void resizeEvent(QResizeEvent *event);

private:
    QLabel* sexLabel;
};

#endif // DOGCARD_H
