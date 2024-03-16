#ifndef ENTRYREGISTRY_H
#define ENTRYREGISTRY_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QHeaderView>
#include <QScrollBar>


class EntryRegistry : public QWidget
{
    Q_OBJECT
public:
    explicit EntryRegistry(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;
    void showEvent( QShowEvent* event ) override;

signals:

private:
    void ColorRow(int row, QColor color);

private:
    QTableWidget* table = nullptr;
    QLabel* label1 = nullptr, *label2 = nullptr, *label3 = nullptr;
};

#endif // ENTRYREGISTRY_H
