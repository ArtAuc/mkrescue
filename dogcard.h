#ifndef DOGCARD_H
#define DOGCARD_H

#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QGridLayout>
#include <QDate>
#include <QSqlQuery>
#include <QRegularExpression>
#include <QScrollArea>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QDir>
#include <QStandardPaths>
#include <QFileDialog>

#include "tristatecheckbox.h"
#include "utils.h"
#include "simplecrypt.h"

class DogCard : public QFrame
{
    Q_OBJECT
public:
    explicit DogCard(QWidget *parent = nullptr) : QFrame(parent) {}
    DogCard(QWidget *parent, QString id_dog, QString name, QString sex, QString info1, QString description, QString info2);
    void resizeEvent(QResizeEvent *event);
    void CreateHistory();

private slots:
    void SelectThis();
    void SaveCard();
    void HandlePrescription();
    void OpenPrescriptionFolder();
    void AddPrescription();

private:
    QString StateToSql(Qt::CheckState state);
    QString chip, dogName;
    QLabel *sexLabel, *descriptionLabel, *info2Label;
    QScrollArea *historyScroll;
    QToolButton *detailsButton;
    QWidget *mainWindow, *nameSexWidget;
    bool selected = false;
    QGridLayout *layout;
    QPixmap sexIcon;
    TriStateCheckBox *sterilizedBox, *compatDogBox, *compatCatBox;
    QString type;
};

#endif // DOGCARD_H
