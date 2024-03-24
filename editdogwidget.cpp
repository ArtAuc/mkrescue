#include "editdogwidget.h"

EditDogWidget::EditDogWidget(QWidget *parent) : QWidget(parent){}

EditDogWidget::EditDogWidget(QString nameEnd){
       // Create widgets
       QLabel *chipLabel = new QLabel("N° D'IDENTIFICATION");
       QLineEdit *chipEdit = new QLineEdit;

       QLabel *dogNameLabel = new QLabel("NOM");
       QLineEdit *dogNameEdit = new QLineEdit;

       QLabel *descriptionLabel = new QLabel("DESCRIPTION");
       QLineEdit *descriptionEdit = new QLineEdit;

       QLabel *sexLabel = new QLabel("SEXE");
       QComboBox *sexBox = new QComboBox;
       sexBox->addItem("Mâle");
       sexBox->addItem("Femelle");

       QLabel *birthDateLabel = new QLabel("DATE DE NAISSANCE");
       QDateEdit *birthDateEdit = new QDateEdit;

       chipEdit->setObjectName("chip" + nameEnd);
       chipLabel->setObjectName("chip" + nameEnd + "Label");

       dogNameEdit->setObjectName("dogName" + nameEnd);
       dogNameLabel->setObjectName("dogName" + nameEnd + "Label");

       descriptionEdit->setObjectName("description" + nameEnd);
       descriptionLabel->setObjectName("description" + nameEnd + "Label");

       sexBox->setObjectName("sex" + nameEnd);
       sexLabel->setObjectName("sex" + nameEnd + "Label");

       birthDateEdit->setObjectName("birthDate" + nameEnd);
       birthDateLabel->setObjectName("birthDate" + nameEnd + "Label");

       // Create layouts
       QVBoxLayout *chipEditLayout = new QVBoxLayout;
       chipEditLayout->addWidget(chipLabel);
       chipEditLayout->addWidget(chipEdit);

       QVBoxLayout *dogNameEditLayout = new QVBoxLayout;
       dogNameEditLayout->addWidget(dogNameLabel);
       dogNameEditLayout->addWidget(dogNameEdit);

       QVBoxLayout *descriptionEditLayout = new QVBoxLayout;
       descriptionEditLayout->addWidget(descriptionLabel);
       descriptionEditLayout->addWidget(descriptionEdit);

       QVBoxLayout *sexBoxLayout = new QVBoxLayout;
       sexBoxLayout->addWidget(sexLabel);
       sexBoxLayout->addWidget(sexBox);

       QVBoxLayout *birthDateEditLayout = new QVBoxLayout;
       birthDateEditLayout->addWidget(birthDateLabel);
       birthDateEditLayout->addWidget(birthDateEdit);

       QGridLayout *gridLayout = new QGridLayout(this);
       gridLayout->addLayout(chipEditLayout, 1, 1);
       gridLayout->addLayout(dogNameEditLayout, 1, 0);
       gridLayout->addLayout(descriptionEditLayout, 5, 0, 1, 2);
       gridLayout->addLayout(sexBoxLayout, 3, 0);
       gridLayout->addLayout(birthDateEditLayout, 3, 1);

       setLayout(gridLayout);
}

void EditDogWidget::showEvent(QShowEvent* event){
    QWidget::showEvent(event);
    QSqlQuery query;

    QStringList dogNameList, chipList, descriptionList, sexList, birthList;

    query.exec("SELECT name, chip, description, sex, birth "
               "FROM Dogs;");

    while (query.next()) {
        QString dogName = query.value(0).toString().isNull() ? "" : query.value(0).toString();
        QString chip = query.value(1).toString().isNull() ? "" : query.value(1).toString();
        QString description = query.value(2).toString().isNull() ? "" : query.value(2).toString();
        QString sex = query.value(3).toString().isNull() ? "" : query.value(3).toString();
        QString birth = query.value(4).toString().isNull() ? "" : query.value(4).toString();

        dogNameList << dogName;
        chipList << chip;
        descriptionList << description;
        sexList << sex;
        birthList << birth;
    }


    QList<QLineEdit*> lineEdits = this->findChildren<QLineEdit*>();
    foreach (QLineEdit* lineEdit, lineEdits) {
        QString editName = lineEdit->objectName();
        if (!editName.contains("spinbox")) {
            QCompleter* completer = nullptr;
            if (editName.contains("dogName"))
                completer = new QCompleter(new QStringListModel(dogNameList, this), lineEdit);
            else if (editName.contains("chip"))
                completer = new QCompleter(new QStringListModel(chipList, this), lineEdit);
            else if (editName.contains("description"))
                completer = new QCompleter(new QStringListModel(descriptionList, this), lineEdit);
            else if (editName.contains("sex"))
                completer = new QCompleter(new QStringListModel(sexList, this), lineEdit);
            else if (editName.contains("birth")) // TODO : birth is not a line edit
                completer = new QCompleter(new QStringListModel(birthList, this), lineEdit);


            if (completer) {
                completer->setCaseSensitivity(Qt::CaseInsensitive);
                lineEdit->setCompleter(completer);
                if(editName.contains("dogName") || editName.contains("chip")){
                    connect(completer, SIGNAL(activated(QString)), this, SLOT(FillOtherFields(QString)));
                    connect(completer, SIGNAL(highlighted(QString)), this, SLOT(PreviewOtherFields(QString)));
                    connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(PreviewOtherFields()));
                }
            }
        }
    }
}


void EditDogWidget::FillOtherFields(QString s){
    QCompleter* completer = qobject_cast<QCompleter*>(QObject::sender());
    QAbstractItemModel *model = nullptr;
    if(completer)
        model = completer->model();

    int row = -1;

    if (model) {
        int rowCount = model->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            QModelIndex index = model->index(i, 0);
            QVariant data = model->data(index, Qt::DisplayRole);
            if (data.isValid() && data.toString() == s){
                row = i;
                break;
            }
        }
    }

    QList<QLineEdit*> lineEdits = this->findChildren<QLineEdit*>();
    if(row >= 0){
        foreach (QLineEdit* lineEdit, lineEdits) {
            if (!lineEdit->objectName().contains("spinbox")) {
                QStringList list;
                QAbstractItemModel *model = lineEdit->completer()->model();

                if (model) {
                    int rowCount = model->rowCount();
                    for (int i = 0; i < rowCount; ++i) {
                        QModelIndex index = model->index(i, 0);
                        QVariant data = model->data(index, Qt::DisplayRole);
                        if (data.isValid())
                            list << data.toString();
                    }
                }

                if(list.isEmpty())
                    list.append("");

                lineEdit->setText(list[row]);
            }
        }
    }
}


void EditDogWidget::PreviewOtherFields(QString s){
    QCompleter* completer = qobject_cast<QCompleter*>(QObject::sender());
    QAbstractItemModel *model = nullptr;
    if(completer)
        model = completer->model();

    int row = -1;

    if (model) {
        int rowCount = model->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            QModelIndex index = model->index(i, 0);
            QVariant data = model->data(index, Qt::DisplayRole);
            if (data.isValid() && data.toString() == s){
                row = i;
                break;
            }
        }
    }

    QList<QLineEdit*> lineEdits = this->findChildren<QLineEdit*>();
    if(row >= 0){
        foreach (QLineEdit* lineEdit, lineEdits) {
            if (!lineEdit->objectName().contains("spinbox")) {
                QStringList list;
                QAbstractItemModel *model = lineEdit->completer()->model();

                if (model) {
                    int rowCount = model->rowCount();
                    for (int i = 0; i < rowCount; ++i) {
                        QModelIndex index = model->index(i, 0);
                        QVariant data = model->data(index, Qt::DisplayRole);
                        if (data.isValid())
                            list << data.toString();
                    }
                }

                if(list.isEmpty())
                    list.append("");

                lineEdit->setPlaceholderText(list[row]);
            }
        }
    }

    else{
        foreach (QLineEdit* lineEdit, lineEdits)
            lineEdit->setPlaceholderText("");
    }
}
