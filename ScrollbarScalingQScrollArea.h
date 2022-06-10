#ifndef SCROLLBARSCALINGQSCROLLAREA_H
#define SCROLLBARSCALINGQSCROLLAREA_H
#include <QScrollBar>
#include <QScrollArea>

namespace Ui {
class ScrollbarScalingQScrollArea ;
}
class ScrollbarScalingQScrollArea : public QScrollArea
{
    /**
     * @brief This class extends QScrollArea in such a way
     * that when the scrollbar appears it does not
     * display over the rightmost widgets but first
     * resizes the content to house the entire scrollbar
     * on the right.
     */
    Q_OBJECT

public:
    ScrollbarScalingQScrollArea(QWidget * parent=nullptr);
    QSize sizeHint() const override;
};
#endif // SCROLLBARSCALINGQSCROLLAREA_H
