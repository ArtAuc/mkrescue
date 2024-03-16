#ifndef MENUTREE_H
#define MENUTREE_H

#include <QTreeWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTimer>
#include <QToolButton>

class MenuTree : public QTreeWidget
{
    Q_OBJECT
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

public slots:
    void Toggle(){
        QPropertyAnimation *animation = new QPropertyAnimation(this, "maximumWidth");
        animation->setDuration(300);
        if (width() > 0) {
            animation->setStartValue(width());
            animation->setEndValue(0);
        } else {
            animation->setStartValue(0);
            animation->setEndValue(initialWidth); // Set the initial width when showing the menu
        }
        animation->start(QAbstractAnimation::DeleteWhenStopped);
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
        if(width() > 50){
            QFont font = this->font();
            font.setPointSize(width() * 0.05);
            this->setFont(font);

            int iconSize = width() * 0.11;
            setIconSize(QSize(iconSize, iconSize));
        }
    }

    void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const override {
        QTreeWidget::drawBranches(painter, rect, index);
        if (!index.parent().isValid()) {
            painter->save();
            painter->setPen(palette().color(QPalette::Highlight));
            painter->drawLine(rect.topLeft(), QPoint(rect.right(), rect.top()));
            painter->restore();
        }
    }

private:
    QTreeWidgetItem* lastHovered = nullptr;
    int initialWidth = 300;
};

#endif // MENUTREE_H
