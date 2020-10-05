/**
 * Warehouse manager
 *
 * @file    BaseGraphicItem.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Base graphic item
 */

#pragma once

// Qt
#include <QList>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

// Local
#include "Handle.h"

namespace whm
{
    namespace gui
    {
        class BaseGraphicItem_t : public QGraphicsItem
        {
            public:
                BaseGraphicItem_t(QGraphicsScene *scene = 0, QGraphicsItem *parent = 0);
                ~BaseGraphicItem_t() = default;

                virtual QRectF boundingRect() const;
                QPainterPath shape() const;
                virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

                void setId(QString id);
                QString id();

            protected:
                QVariant itemChange(GraphicsItemChange change, const QVariant &value);
                void mousePressEvent(QGraphicsSceneMouseEvent *event);
                void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
                void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
                void setDrawBoundingRect(bool draw);
                void createHandles();

                QRectF mRect;
                QPointF mOrigin;
                bool mDrawBoundingRect;
                bool mSelected;
                bool mConnected;
                QList<Handle *> mHandles;

            private:
                Handle *mCurrentHandle;
                QString mId;
        };
    }
}
