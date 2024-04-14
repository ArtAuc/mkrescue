#include "editdogwidget.h"

EditDogWidget::EditDogWidget(QWidget *parent) : QWidget(parent){}

EditDogWidget::EditDogWidget(QString nameEnd){
        setObjectName(nameEnd);

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
       CustomDateTimeEdit *birthDateEdit = new CustomDateTimeEdit;

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
       QWidget::setTabOrder(dogNameEdit, chipEdit);
       QWidget::setTabOrder(chipEdit, sexBox);
       QWidget::setTabOrder(sexBox, birthDateEdit);
       QWidget::setTabOrder(birthDateEdit, descriptionEdit);

       dogNameEdit->setMaxLength(255);
       chipEdit->setMaxLength(50);
       descriptionEdit->setMaxLength(512);

       QRegularExpression disallowSeparator("\\A[^|]{0,255}\\z");
       QRegularExpressionValidator *validator = new QRegularExpressionValidator(disallowSeparator, this); // To prevent parsing problems (separators are made of |)

       for(QLineEdit *lineEdit : findChildren<QLineEdit*>()){
           lineEdit->setValidator(validator);
           QObject::connect(lineEdit, &QLineEdit::textEdited, this, &EditDogWidget::LineEditFormat);
       }
}

void EditDogWidget::showEvent(QShowEvent* event){
    QWidget::showEvent(event);
    QSqlQuery query;

    QStringList chipList, dogNameList;
    birthList.clear();
    sexList.clear();
    descriptionList.clear();


    HandleErrorExec(&query, "SELECT name, chip, description, sex, birth "
               "FROM Dogs;");

    while (query.next()) {
        QString dogName = query.value(0).toString().isNull() ? "" : query.value(0).toString();
        QString chip = query.value(1).toString().isNull() ? "" : query.value(1).toString();
        QString description = query.value(2).toString().isNull() ? "" : query.value(2).toString();
        QString sex = query.value(3).toString().isNull() ? "" : query.value(3).toString();
        QString birth = query.value(4).toString().isNull() ? "" : query.value(4).toString();

        dogNameList << dogName + (chip.isEmpty() ? "" : "|") + chip;
        chipList << chip + (dogName.isEmpty() ? "" : "|") + dogName;
        descriptionList << description;
        sexList << sex;
        birthList << birth;
    }


    QList<QLineEdit*> lineEdits = this->findChildren<QLineEdit*>();
    foreach (QLineEdit* lineEdit, lineEdits) {
        QString editName = lineEdit->objectName();
        if (!editName.contains("spinbox") && !editName.startsWith("birthDate")) {
            QCompleter* completer = nullptr;
            if (editName.contains("dogName"))
                completer = new QCompleter(new QStringListModel(dogNameList, this), lineEdit);
            else if (editName.contains("chip"))
                completer = new QCompleter(new QStringListModel(chipList, this), lineEdit);
            else if (editName.contains("description"))
                completer = new QCompleter(new QStringListModel(UniqueList(descriptionList), this), lineEdit);


            if (completer) {
                completer->setCaseSensitivity(Qt::CaseInsensitive);
                lineEdit->setCompleter(completer);
                if(editName.contains("dogName") || editName.contains("chip")){
                    connect(completer, QOverload<const QString&>::of(&QCompleter::activated), this, [=](const QString& s){ ProcessFields(s, false); });
                    connect(completer, QOverload<const QString&>::of(&QCompleter::highlighted), this, [=](const QString& s){ ProcessFields(s, true); });
                    connect(lineEdit, &QLineEdit::editingFinished, this, [=](){ ProcessFields("", true); });
                }
            }
        }
    }
}

void EditDogWidget::ProcessFields(QString s, bool isPreview){
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
            QString editName = lineEdit->objectName();
            if (!editName.contains("spinbox") && !editName.startsWith("birthDate")) {
                QStringList list;
                if(editName.startsWith("description"))
                    list = descriptionList;
                else{
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
                }

                QTimer::singleShot(50, [=]() {
                    if (isPreview)
                        lineEdit->setPlaceholderText(list[row].split("|")[0]);
                    else
                        lineEdit->setText(list[row].split("|")[0]);
                });
            }
        }

        QComboBox* sexBox = findChild<QComboBox*>();
        sexBox->setCurrentText(sexList[row]);

        CustomDateTimeEdit* birthEdit = findChild<CustomDateTimeEdit*>();
        birthEdit->SetDate(QDate::fromString(birthList[row], "yyyy-MM-dd"));
    }

    else{
        foreach (QLineEdit* lineEdit, lineEdits)
            lineEdit->setPlaceholderText("");
    }
}

void EditDogWidget::LineEditFormat(QString text){
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
    int cursorPosition = lineEdit->cursorPosition();

    if(lineEdit->objectName().startsWith("dogName")){
        QString formattedText;
        QRegularExpression regex("[-\\s]");
        QStringList words = text.split(regex);
        for (const QString &word : words) {
            if (!formattedText.isEmpty())
                formattedText.append("-");
            formattedText.append(word.left(1).toUpper() + word.mid(1).toLower());
        }
        lineEdit->setText(formattedText);
    }

    lineEdit->setCursorPosition(cursorPosition);
}
