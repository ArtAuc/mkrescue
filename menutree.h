#ifndef MENUTREE_H
#define MENUTREE_H

#include <QTreeWidget>
#include <QMouseEvent>

class MenuTree : public QTreeWidget
{
public:
    explicit MenuTree(QWidget *parent = nullptr) : QTreeWidget(parent) {
        connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), parent->parent(), SLOT(ChangePage(QTreeWidgetItem*)));

        setMouseTracking(true);

        QPalette palette = this->palette();
        palette.setColor(QPalette::Base, "#3b4b64");
        palette.setColor(QPalette::Text, QColor("#a7b2c8"));

        palette.setColor(QPalette::Highlight, QColor("#45556c"));
        palette.setColor(QPalette::HighlightedText, Qt::white);
        setPalette(palette);
    }

    void collapseAllExcept(QString txt){
        for (int i = 0; i < topLevelItemCount(); ++i) {
            QTreeWidgetItem *topLevelItem = this->topLevelItem(i);
            if (topLevelItem && topLevelItem->text(0) != txt)
                topLevelItem->setExpanded(false);
        }
    }

protected:
    void mouseMoveEvent(QMouseEvent *event) override {
        QTreeWidgetItem *item = itemAt(event->pos());
        if(lastHovered != nullptr){
            lastHovered->setForeground(0, QBrush());
        }


        if (item) {
            item->setForeground(0, QBrush(Qt::white));
        }

        lastHovered = item;
        QTreeWidget::mouseMoveEvent(event);
    }

    void resizeEvent(QResizeEvent *event) override {
        QTreeWidget::resizeEvent(event);
        QFont font = this->font();
        font.setPointSize(width() * 0.05);
        this->setFont(font);
    }


private:
    QTreeWidgetItem* lastHovered = nullptr;
};

#endif // MENUTREE_H
