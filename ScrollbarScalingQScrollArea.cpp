#include "ScrollbarScalingQScrollArea.h"

ScrollbarScalingQScrollArea::ScrollbarScalingQScrollArea(QWidget * parent)
: QScrollArea(parent)
{
}

QSize ScrollbarScalingQScrollArea::sizeHint() const
{
    auto newSize = QScrollArea::sizeHint();
    newSize.setWidth(newSize.width()
                        + verticalScrollBar()->sizeHint().width());
    return newSize;
}