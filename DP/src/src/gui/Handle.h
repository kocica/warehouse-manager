/**
 * Warehouse manager
 *
 * @file    Handle.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Handler class
 */

#pragma once

// Qt
#include <QRectF>

namespace whm
{
    namespace gui
    {
        class Handle
        {
            public:
                enum HandleShape
                {
                    HANDLE_SHAPE_RECT,
                    HANDLE_SHAPE_CIRCLE,
                    HANDLE_SHAPE_TRIANGLE
                };

                enum HandleType
                {
                    HANDLE_TYPE_TOPLEFT,
                    HANDLE_TYPE_TOP,
                    HANDLE_TYPE_TOPRIGHT,
                    HANDLE_TYPE_LEFT,
                    HANDLE_TYPE_RIGHT,
                    HANDLE_TYPE_BOTTOMLEFT,
                    HANDLE_TYPE_BOTTOM,
                    HANDLE_TYPE_BOTTOMRIGHT,
                    HANDLE_TYPE_ROTATE,
                    HANDLE_TYPE_ORIGIN,
                    HANDLE_TYPE_CTRL
                };

                Handle(QPointF pos, int size, HandleShape shape, HandleType type);

                void setShape(HandleShape shape);
                void setType(HandleType type);

                HandleShape shape();
                HandleType type();

                int size();
                void setSize(int size);

                void setPos(QPointF pos);
                QPointF pos();

                QRectF boundingRect() const;
            private:
                QRectF mRect;
                QPointF mPos;

                HandleType mType;
                HandleShape mShape;

                int mSize;
        };
    }
}
