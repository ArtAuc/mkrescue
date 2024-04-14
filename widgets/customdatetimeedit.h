#ifndef CUSTOMDATETIMEEDIT_H
#define CUSTOMDATETIMEEDIT_H

#include <QDateTimeEdit>
#include <QLineEdit>
#include <QRegularExpression>
#include <QKeyEvent>

class CustomDateTimeEdit : public QLineEdit
{
    Q_OBJECT
public:
    CustomDateTimeEdit(bool dateOnly = true) : QLineEdit(nullptr){
        this->dateOnly = dateOnly;
        SetDate(QDate::currentDate());

        connect(this, &CustomDateTimeEdit::editingFinished, this, &CustomDateTimeEdit::CorrectDate);
        connect(this, &CustomDateTimeEdit::textEdited, this, &CustomDateTimeEdit::ReplaceCharacter);
        connect(this, &CustomDateTimeEdit::selectionChanged, this, &CustomDateTimeEdit::HandleSelection);


    }

    void EnableTime() {
        dateOnly = false;
    }

    bool DateOnly() {return dateOnly;}

    QDate GetDate(){
        CorrectDate();
        return QDate::fromString(text(), displayedFormat);
    }

    QDateTime GetDateTime(){
        CorrectDate();
        return QDateTime::fromString(text(), displayedFormat + " HH:mm");
    }

    void SetDateTime(QDateTime dateTime){
        setText(dateTime.toString(displayedFormat + " HH:mm"));
    }

    void SetDate(QDate date){
        if(dateOnly)
            setText(date.toString(displayedFormat));
        else
            setText(date.toString(displayedFormat + " 00:00"));
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Backspace) {
            int movement = 1;
            if(cursorPosition() > 1 && (text()[cursorPosition() - 1] == ' ' || text()[cursorPosition() - 1] == ':' || text()[cursorPosition() - 1] == '/'))
                movement = 2;

            setCursorPosition(std::max(0, cursorPosition() - movement));

            event->ignore();
            return;
        }

        if (event->text().size() == 0 || (event->text().size() == 1 && event->text().at(0).isDigit())){
            QLineEdit::keyPressEvent(event);
        }
        else {
            event->ignore();
        }
    }

    void mousePressEvent(QMouseEvent *event) override {
        QLineEdit::mousePressEvent(event);


        if(cursorPosition() > 0){
            int sectionStart = text().lastIndexOf(QRegularExpression("[\\s/:]"), cursorPosition() - 1) + 1;
            setCursorPosition(sectionStart);
        }
    }

public slots:
    void ReplaceCharacter(){
        if((dateOnly && text().length() == 11) || (!dateOnly && text().length() == 17)){ // If datetime has all required characters + 1
            int cursorPosition = this->cursorPosition();
            QString text = this->text();
            if(cursorPosition > 0 && cursorPosition < text.length() && (text[cursorPosition] == ' ' || text[cursorPosition] == ':' || text[cursorPosition] == '/')){ // On a special character
                // Switch characters
                text.insert(cursorPosition + 1, text[cursorPosition - 1]);
                text.remove(cursorPosition - 1, 1),

                cursorPosition += 1;
            }
            text.remove(std::min(cursorPosition, int(text.length() - 1)), 1);

            setText(text);
            setCursorPosition(cursorPosition);
        }

    }

    void CorrectDate(){
        QString dateString = text();

        QDateTime currentDateTime;
        if(dateOnly)
            currentDateTime = QDateTime::fromString(dateString, displayedFormat);
        else
            currentDateTime = QDateTime::fromString(dateString, displayedFormat + " HH:mm");

        QRegularExpression rx;
        if(dateOnly)
            rx.setPattern("(\\d+)\\/(\\d+)\\/(\\d+)");
        else
            rx.setPattern("(\\d+)/(\\d+)/(\\d+) (\\d+):(\\d+)");
        QRegularExpressionMatch match = rx.match(dateString);

        if (match.hasMatch()) {
            int day = match.captured(1).toInt();
            int month = match.captured(2).toInt();
            int year = match.captured(3).toInt();

            day = qBound(1, day, 31);
            month = qBound(1, month, 12);
            year = qBound(1900, year, 2100);

            dateString = QString("%1/%2/%3").arg(day, 2, 10, QLatin1Char('0'))
                                               .arg(month, 2, 10, QLatin1Char('0'))
                                               .arg(year);

            if (!dateOnly && match.lastCapturedIndex() == 5) {
                int hour = match.captured(4).toInt();
                int minute = match.captured(5).toInt();

                hour = qBound(0, hour, 23);
                minute = qBound(0, minute, 59);

                dateString += QString(" %1:%2").arg(hour, 2, 10, QLatin1Char('0'))
                                                .arg(minute, 2, 10, QLatin1Char('0'));
            }

            setText(dateString);
        }

        else{
            if(dateOnly)
                SetDate(QDate::currentDate());
            else
                SetDateTime(QDateTime::currentDateTime());
        }
    }

    void HandleSelection(){
        deselect();
    }

private:
    bool dateOnly;
    const QString displayedFormat = "dd/MM/yyyy";
};

#endif // CUSTOMDATETIMEEDIT_H
