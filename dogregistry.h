#ifndef DOGREGISTRY_H
#define DOGREGISTRY_H

#include "registry.h"


class DogRegistry : public Registry
{
    Q_OBJECT
public:
    explicit DogRegistry(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;
    void showEvent( QShowEvent* event ) override;
    void SetType(QString type){this->type = type;}

private:
    QLabel* label1 = nullptr, *label2 = nullptr, *label3 = nullptr;
    QString type;
};

#endif // DOGREGISTRY_H
