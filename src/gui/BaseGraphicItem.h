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
                BaseGraphicItem_t(qreal w, qreal h, QGraphicsScene *scene = 0, QGraphicsItem *parent = 0);
                ~BaseGraphicItem_t() = default;

                virtual QRectF boundingRect() const;
                QPainterPath shape() const;
                virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

                void setId(QString id);
                QString id();

                QRectF getRect() const;

                void showHandles(bool enabled);
                void updateChildren(int dx, int dy);
                void shiftGraphicItem(int dx, int dy);
                void setGraphicItemOrientation(int o);

                QGraphicsItem* getParent() const;

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
                bool mDrawHandles;

                int32_t mOrientation{ 0 };

                QList<Handle *> mHandles;
                QGraphicsItem* mParentItem{ nullptr };

                int32_t minWidth{ 0 };
                int32_t minHeight{ 0 };

            private:
                Handle *mCurrentHandle;
                QString mId;
        };
    }
}
