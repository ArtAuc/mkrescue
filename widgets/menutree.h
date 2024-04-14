#ifndef MENUTREE_H
#define MENUTREE_H

#include <QTreeWidget>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QLabel>

class TreeItemDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem newOption(option);

        int padding = 20;
        newOption.rect.setX(padding);

        if (option.state & QStyle::State_MouseOver || option.state & QStyle::State_Selected) {
            newOption.palette.setColor(QPalette::Text, Qt::white);
            painter->fillRect(0, option.rect.top(), 0.9 * padding, option.rect.height(), QColor("#20718e"));
        }

        if (option.state & QStyle::State_Selected) {
            newOption.palette.setColor(QPalette::Highlight, QColor("#45556c"));
        }


        QStyledItemDelegate::paint(painter, newOption, index);

    }
};


class MenuTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit MenuTree(QWidget *parent = nullptr) : QTreeWidget(parent) {
        setFocusPolicy(Qt::NoFocus);
        setIndentation(0);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        TreeItemDelegate *delegate = new TreeItemDelegate(this);
        setItemDelegate(delegate);

        setSelectionMode(QAbstractItemView::SingleSelection);
        connect(this, SIGNAL(itemPressed(QTreeWidgetItem*, int)),
                parent->parent(), SLOT(ChangePage(QTreeWidgetItem*)));

        setMouseTracking(true);

        QPalette palette = this->palette();
        palette.setColor(QPalette::Base, "#3b4b64");
        palette.setColor(QPalette::Text, QColor("#a7b2c8"));

        setPalette(palette);

        logo = QPixmap("media/logo_title_side.png");
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

    void resizeEvent(QResizeEvent *event) override {
        QTreeWidget::resizeEvent(event);
        setColumnWidth(0, 0 * width());
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

private:
    int initialWidth = 300;
    QLabel *menuLogoLabel = nullptr;
    QPixmap logo;
};

#endif // MENUTREE_H
