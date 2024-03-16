#ifndef ENTRYREGISTRY_H
#define ENTRYREGISTRY_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QHeaderView>
#include <QScrollBar>
#include <QToolButton>

class EntryRegistry : public QWidget
{
    Q_OBJECT
public:
    explicit EntryRegistry(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;
    void showEvent( QShowEvent* event ) override;

private:
    void ColorRow(int row, bool gray);

private:
    QTableWidget* table = nullptr;
    QLabel* label1 = nullptr, *label2 = nullptr, *label3 = nullptr;
};

#endif // ENTRYREGISTRY_H
