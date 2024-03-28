#ifndef DOGCARD_H
#define DOGCARD_H

#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QGridLayout>
#include <QDate>
#include <QSqlQuery>

class DogCard : public QFrame
{
    Q_OBJECT
public:
    explicit DogCard(QWidget *parent = nullptr) : QFrame(parent) {}
    DogCard(QWidget *parent, QString id_dog, QString name, QString sex, QString info1, QString description, QString info2);
    void resizeEvent(QResizeEvent *event);

private slots:
    void SelectThis();
    void UnselectThis();

private:
    QString id_dog;
    QLabel *sexLabel, *chipLabel, *vetLabel, *birthLabel;
    QToolButton *detailsButton;
    QWidget *mainWindow, *nameSexWidget;
    bool selected = false;
    QGridLayout *layout;
    QPixmap sexIcon;
};

#endif // DOGCARD_H
