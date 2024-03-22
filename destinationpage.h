#ifndef DESTINATIONPAGE_H
#define DESTINATIONPAGE_H


#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>

class DestinationPage : public QWidget {
    Q_OBJECT
public:
    explicit DestinationPage(QWidget *parent = nullptr);
    DestinationPage(int pageNumber);

public slots:
    void ChangeDestType(QString type);
};

#endif
