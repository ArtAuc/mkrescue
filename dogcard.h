#ifndef DOGCARD_H
#define DOGCARD_H

#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QGridLayout>
#include <QDate>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class DogCard : public QFrame
{
    Q_OBJECT
public:
    explicit DogCard(QWidget *parent = nullptr) : QFrame(parent) {}
    DogCard(QWidget *parent, QString id_dog, QString name, QString sex, QString birth, QString description, QString info);
    void resizeEvent(QResizeEvent *event);


private slots:
    void SelectThis();
    void UnselectThis();

private:
    QLabel* sexLabel;
    QToolButton *detailsButton;
    QWidget* mainWindow;
    bool selected = false;
};

#endif // DOGCARD_H
