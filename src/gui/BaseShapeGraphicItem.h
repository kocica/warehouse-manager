/**
 * Warehouse manager
 *
 * @file    BaseShapeGraphicItem.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   
 */

#pragma once

#include "BaseGraphicItem.h"

namespace whm
{
    namespace gui
    {
        class BaseShapeGraphicItem_t : public BaseGraphicItem_t
        {
            public:
                enum ItemType
                {
                    ITEM_LINE,
                    ITEM_CIRCLE,
                    ITEM_TRIANGLE,
                    ITEM_RECTANGLE,
                    ITEM_ELLIPSE,
                    ITEM_PIXMAP
                };

                BaseShapeGraphicItem_t(qreal, qreal, qreal, qreal, ItemType, QGraphicsScene*, QGraphicsItem* p = 0);
                ~BaseShapeGraphicItem_t() = default;

                void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

                QRectF boundingRect() const;

                void setType();
                void setPixmap(QString path);

            private:
                ItemType mType;
                QPixmap mPixmap;
                bool mDrawPixmap{ false };
        };
    }
}
