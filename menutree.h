#ifndef MENUTREE_H
#define MENUTREE_H

#include <QTreeWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTimer>
#include <QToolButton>
#include <QFontDatabase>
#include <QLabel>

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

        logo = QPixmap("media/logo_title_dark.png");
    }

    void collapseAllExcept(QString txt){
        for (int i = 0; i < topLevelItemCount(); ++i) {
            QTreeWidgetItem *topLevelItem = this->topLevelItem(i);
            if (topLevelItem && topLevelItem->text(0) != txt)
                topLevelItem->setExpanded(false);
        }
    }

    void SetInitialWidth(int initialWidth){
        if (width() > 0)
            setMaximumWidth(initialWidth);

        this->initialWidth = initialWidth;
    }


public slots:
    void Toggle(){
        QPropertyAnimation *animation = new QPropertyAnimation(this, "maximumWidth");
        animation->setDuration(300);
        if (width() > 0) {
            initialWidth = width();
            animation->setStartValue(width());
            animation->setEndValue(0);
        } else {
            animation->setStartValue(0);
            animation->setEndValue(initialWidth); // Set the initial width when showing the menu
        }
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

protected:
    void showEvent(QShowEvent *event) override{
        QTreeWidget::showEvent(event);

        menuLogoLabel = parent()->findChild<QLabel*>("menuLogoLabel");

        resizeEvent(nullptr);
    }

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
        if(width() > 10){
            QFont font = this->font();
            font.setPointSizeF(width() * 0.05);
            this->setFont(font);

            int iconSize = width() * 0.11;
            setIconSize(QSize(iconSize, iconSize));
        }

        if(menuLogoLabel != nullptr){
            menuLogoLabel->setPixmap(logo.scaled(width(), width(), Qt::KeepAspectRatio));
            menuLogoLabel->setVisible(width() > 10);
        }
        else
            menuLogoLabel = parent()->findChild<QLabel*>("menuLogoLabel");
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
    QLabel *menuLogoLabel = nullptr;
    QPixmap logo;
};

#endif // MENUTREE_H
