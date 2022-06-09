#ifndef SCROLLBARSCALINGQSCROLLAREA_H
#define SCROLLBARSCALINGQSCROLLAREA_H
#include <QScrollBar>
#include <QScrollArea>

namespace Ui {
class ScrollbarScalingQScrollArea ;
}
class ScrollbarScalingQScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    ScrollbarScalingQScrollArea(QWidget * parent=nullptr);
    QSize sizeHint() const override;
};
#endif // SCROLLBARSCALINGQSCROLLAREA_H
