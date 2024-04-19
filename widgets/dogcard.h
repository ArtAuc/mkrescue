#ifndef DOGCARD_H
#define DOGCARD_H

#include <QScrollArea>
#include <QToolButton>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QDir>
#include <QStandardPaths>
#include <QProcess>
#include <QFileDialog>

#include "tristatecheckbox.h"
#include "utils.h"
#include "simplecrypt.h"
#include "clickablelabel.h"
#include "customdatetimeedit.h"

class DogCard : public QFrame
{
    Q_OBJECT
public:
    explicit DogCard(QWidget *parent = nullptr) : QFrame(parent) {}
    DogCard(QWidget *parent, QString chip, QString name, QString sex, QString info1, QString description, QString info2, SimpleCrypt *crypto);
    void resizeEvent(QResizeEvent *event);
    void CreateHistory();
    void CreateLastVaccine(QWidget* vaccineWidget);

private slots:
    void SelectThis();
    void SaveCard();
    void HandlePrescription();
    void OpenPrescriptionFolder();
    void AddPrescription();
    void HandleClickedHistory(QString type, QStringList necessary){emit ClickedHistory(type, necessary);}

signals:
    void ClickedHistory(QString type, QStringList necessary);

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
    SimpleCrypt *crypto;
};

#endif // DOGCARD_H
