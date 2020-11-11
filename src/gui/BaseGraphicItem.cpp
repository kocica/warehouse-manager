/**
 * Warehouse manager
 *
 * @file    BaseGraphicItem.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Base graphic item
 */

// Local
#include "Handle.h"
#include "BaseGraphicItem.h"

// Qt
#include <QUuid>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>

// Std
#include <iostream>

namespace whm
{
    namespace gui
    {
        BaseGraphicItem_t::BaseGraphicItem_t(qreal w, qreal h, QGraphicsScene *scene, QGraphicsItem *parent)
            : QGraphicsItem(parent)
            , mSelected(false)
            , mConnected(false)
            , mDrawHandles(true)
            , mLocationSlot(false)
            , mParentItem(parent)
            , mCurrentHandle(0)
        {
            QUuid id;
            id = QUuid::createUuid();
            this->mId = id.toString();
            this->mDrawBoundingRect = true;
            this->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
            if(scene)
            {
                scene->addItem(this);
            }

            minWidth = w / 2;
            minHeight = h / 2;
        }

        void BaseGraphicItem_t::setId(QString id)
        {
            this->mId = id;
        }

        QString BaseGraphicItem_t::id()
        {
            return this->mId;
        }

        void BaseGraphicItem_t::setDrawBoundingRect(bool draw)
        {
            this->mDrawBoundingRect = draw;
        }

        QRectF BaseGraphicItem_t::boundingRect() const
        {
            int size = (mRect.width() + mRect.height())/2 / 10;

            return this->mRect.adjusted(-size/2,-size/2 - (5*size),size/2,size/2);
        }

        QPainterPath BaseGraphicItem_t::shape() const
        {
            QPainterPath path;
            path.setFillRule(Qt::WindingFill);

            if(this->isSelected())
            {
                foreach (Handle *handle, mHandles)
                {
                    switch (handle->shape())
                    {
                        case Handle::HANDLE_SHAPE_RECT:
                            path.addRect(handle->boundingRect());
                            break;
                        case Handle::HANDLE_SHAPE_CIRCLE:
                            path.addRect(handle->boundingRect());
                            break;
                        case Handle::HANDLE_SHAPE_TRIANGLE:
                            break;
                    }
                }
            }

            path.addRect(this->mRect);
            return path;
        }

        void BaseGraphicItem_t::showHandles(bool enabled)
        {
            mDrawHandles = enabled;
        }

        QRectF BaseGraphicItem_t::getRect() const
        {
            return mRect;
        }

        void BaseGraphicItem_t::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
        {
            Q_UNUSED(option);
            Q_UNUSED(widget);

            QPen pen(Qt::black);
            pen.setWidth(this->mLineWidth);
            painter->setPen(pen);

            if(this->isSelected())
            {
                if(this->mSelected)
                {
                    pen.setColor(Qt::red);
                    painter->setPen(pen);
                    painter->drawRect(this->mRect);
                }
                else if(this->mConnected)
                {
                    return;
                }
                else if(this->mDrawBoundingRect && mDrawHandles)
                {
                    pen.setColor(Qt::green);
                    painter->setPen(pen);
                    painter->drawRect(this->mRect);
                }

                QPointF p1;
                QPointF p2;
                painter->setBrush(Qt::white);

                if (mDrawHandles)
                {
                    foreach (Handle *handle, mHandles)
                    {
                        if(handle->type() == Handle::HANDLE_TYPE_ROTATE)
                        {
                            p1 = handle->pos();
                        }
                        if(handle->type() == Handle::HANDLE_TYPE_TOP)
                        {
                            p2 = handle->pos();
                        }
                        if(handle->type() == Handle::HANDLE_TYPE_CTRL)
                        {
                            painter->save();
                            QPen pen(Qt::green);
                            painter->setPen(pen);
                            painter->setBackground(QBrush(Qt::green));
                            painter->drawRect(handle->boundingRect());
                            painter->restore();
                        }
                        else
                        {
                            switch (handle->shape())
                            {
                                case Handle::HANDLE_SHAPE_RECT:
                                    painter->drawRect(handle->boundingRect());
                                    break;
                                case Handle::HANDLE_SHAPE_CIRCLE:
                                    painter->drawEllipse(handle->boundingRect());
                                    break;
                                case Handle::HANDLE_SHAPE_TRIANGLE:
                                    break;
                            }
                        }
                    }
                }

                painter->drawLine(p1,p2);
                painter->drawPoint(this->mOrigin);
            }
        }

        void BaseGraphicItem_t::mousePressEvent(QGraphicsSceneMouseEvent *event)
        {
            if(mLocationSlot)
            {
                dynamic_cast<BaseGraphicItem_t*>(mParentItem)->mousePressEvent(event);
                return;
            }

            QGraphicsItem::mousePressEvent(event);

            if(event->buttons() == Qt::LeftButton)
            {
                foreach (Handle *handle, mHandles)
                {
                    if(handle->boundingRect().contains(event->pos()))
                    {
                        this->mCurrentHandle = handle;
                    }
                }
            }
        }

        QGraphicsItem* BaseGraphicItem_t::getParent() const
        {
            return mParentItem;
        }

        void BaseGraphicItem_t::updateChildren(int dx, int dy, bool s)
        {
            QList<QGraphicsItem*> items = scene()->items();

            foreach(QGraphicsItem* i, items)
            {
                if(auto* baseItem = dynamic_cast<BaseGraphicItem_t*>(i))
                {
                    auto* parentBaseItem = dynamic_cast<BaseGraphicItem_t*>(baseItem->getParent());

                    if (parentBaseItem && this->id() == parentBaseItem->id())
                    {
                        baseItem->shiftGraphicItem(dx, dy, s);
                    }
                }
            }
        }

        void BaseGraphicItem_t::shiftGraphicItem(int dx, int dy, bool s)
        {
            int32_t dx2 = dx;
            int32_t dy2 = dy;

            if(s)
            {
                dx2 /= 2.0;
                dy2 /= 2.0;
            }

            this->mRect.setTopLeft(QPointF(this->mRect.topLeft().x() + dx2, this->mRect.topLeft().y() + dy2));
            this->mRect.setBottomRight(QPointF(this->mRect.bottomRight().x() + dx2, this->mRect.bottomRight().y() + dy2));

            this->mOrigin.setX((dx * mRect.width()) + mRect.center().x());
            this->mOrigin.setY((dy * mRect.height()) + mRect.center().y());

            // TODO: Resizing of ports
            //this->mRect.setWidth(this->mRect.width() + (dx/5.0));
            //this->mRect.setHeight(this->mRect.height() - (dy/5.0));
        }

        void BaseGraphicItem_t::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
        {
            if(mLocationSlot)
            {
                dynamic_cast<BaseGraphicItem_t*>(mParentItem)->mouseMoveEvent(event);
            }

            if(!mDrawHandles)
            {
                return;
            }

            if(event->buttons() == Qt::LeftButton && mCurrentHandle)
            {
                int32_t rotation{ 0 };
                qreal dx = (mOrigin.x() - mRect.center().x()) / mRect.width();
                qreal dy = (mOrigin.y() - mRect.center().y()) / mRect.height();

                auto backupRect = this->mRect;

                switch (mCurrentHandle->type())
                {
                    case Handle::HANDLE_TYPE_LEFT:
                        this->mRect.setLeft(event->pos().x());
                        if(this->mRect.width() < minWidth || this->mRect.height() < minHeight)
                        {
                            this->mRect = backupRect;
                            return;
                        }
                        updateChildren(event->pos().x() - backupRect.topLeft().x(), 0);
                        mOrigin.setX((dx * mRect.width()) + mRect.center().x());
                        break;
                    case Handle::HANDLE_TYPE_RIGHT:
                        this->mRect.setRight(event->pos().x());
                        if(this->mRect.width() < minWidth || this->mRect.height() < minHeight)
                        {
                            this->mRect = backupRect;
                            return;
                        }
                        updateChildren(event->pos().x() - backupRect.topRight().x(), 0);
                        mOrigin.setX((dx * mRect.width()) + mRect.center().x());
                        break;
                    case Handle::HANDLE_TYPE_TOP:
                        this->mRect.setTop(event->pos().y());
                        if(this->mRect.width() < minWidth || this->mRect.height() < minHeight)
                        {
                            this->mRect = backupRect;
                            return;
                        }
                        updateChildren(0, event->pos().y() - backupRect.topLeft().y());
                        mOrigin.setY((dy * mRect.height()) + mRect.center().y());
                        break;
                    case Handle::HANDLE_TYPE_BOTTOM:
                        this->mRect.setBottom(event->pos().y());
                        if(this->mRect.width() < minWidth || this->mRect.height() < minHeight)
                        {
                            this->mRect = backupRect;
                            return;
                        }
                        updateChildren(0, event->pos().y() - backupRect.bottomLeft().y());
                        mOrigin.setY((dy * mRect.height()) + mRect.center().y());
                        break;
                    case Handle::HANDLE_TYPE_TOPLEFT:
                        this->mRect.setTopLeft(event->pos());
                        if(this->mRect.width() < minWidth || this->mRect.height() < minHeight)
                        {
                            this->mRect = backupRect;
                            return;
                        }
                        updateChildren(event->pos().x() - backupRect.topLeft().x(), event->pos().y() - backupRect.topLeft().y());
                        mOrigin.setX((dx * mRect.width()) + mRect.center().x());
                        mOrigin.setY((dy * mRect.height()) + mRect.center().y());
                        break;
                    case Handle::HANDLE_TYPE_TOPRIGHT:
                        this->mRect.setTopRight(event->pos());
                        if(this->mRect.width() < minWidth || this->mRect.height() < minHeight)
                        {
                            this->mRect = backupRect;
                            return;
                        }
                        updateChildren(event->pos().x() - backupRect.topRight().x(), event->pos().y() - backupRect.topRight().y());
                        mOrigin.setX((dx * mRect.width()) + mRect.center().x());
                        mOrigin.setY((dy * mRect.height()) + mRect.center().y());
                        break;
                    case Handle::HANDLE_TYPE_BOTTOMLEFT:
                        this->mRect.setBottomLeft(event->pos());
                        if(this->mRect.width() < minWidth || this->mRect.height() < minHeight)
                        {
                            this->mRect = backupRect;
                            return;
                        }
                        updateChildren(event->pos().x() - backupRect.bottomLeft().x(), event->pos().y() - backupRect.bottomLeft().y());
                        mOrigin.setX((dx * mRect.width()) + mRect.center().x());
                        mOrigin.setY((dy * mRect.height()) + mRect.center().y());
                        break;
                    case Handle::HANDLE_TYPE_BOTTOMRIGHT:
                        this->mRect.setBottomRight(event->pos());
                        if(this->mRect.width() < minWidth || this->mRect.height() < minHeight)
                        {
                            this->mRect = backupRect;
                            return;
                        }
                        updateChildren(event->pos().x() - backupRect.bottomRight().x(), event->pos().y() - backupRect.bottomRight().y());
                        mOrigin.setX((dx * mRect.width()) + mRect.center().x());
                        mOrigin.setY((dy * mRect.height()) + mRect.center().y());
                        break;
                    case Handle::HANDLE_TYPE_ROTATE:
                        rotation = (int(-QLineF(event->scenePos(),     mapToScene(mOrigin)).angle() / 90) * 90)+
                                   (int( QLineF(event->lastScenePos(), mapToScene(mOrigin)).angle() / 90) * 90);
                        this->mOrientation = (this->mOrientation + rotation) % 360;
                        this->setTransform(QTransform().translate(mOrigin.x(), mOrigin.y())
                                                       .rotate(rotation)
                                                       .translate(-mOrigin.x(), -mOrigin.y()), true);
                        break;
                    case Handle::HANDLE_TYPE_ORIGIN:
                        mCurrentHandle->setPos(event->pos());
                        this->mOrigin = mCurrentHandle->pos();
                        break;
                    case Handle::HANDLE_TYPE_CTRL:
                        mCurrentHandle->setPos(event->pos());
                        break;
                    default:
                        QGraphicsItem::mouseMoveEvent(event);
                        break;
                }

                updateHandles();
            }
            else
            {
                QGraphicsItem::mouseMoveEvent(event);
            }
        }

        void BaseGraphicItem_t::updateHandles()
        {
            QPointF left(mRect.left(),mRect.top() + mRect.height()/2);
            QPointF right(mRect.right(),mRect.top() + mRect.height()/2);
            QPointF top(mRect.left()+mRect.width()/2,mRect.top());
            QPointF bottom(mRect.left()+mRect.width()/2,mRect.bottom());
            int size = (mRect.width() + mRect.height())/2 / 10;
            QPointF rotate(top.x(),top.y()-(5*size));

            foreach (Handle *handle, mHandles)
            {
                switch (handle->type())
                {
                    case Handle::HANDLE_TYPE_LEFT:
                        handle->setPos(left);
                        break;
                    case Handle::HANDLE_TYPE_RIGHT:
                        handle->setPos(right);
                        break;
                    case Handle::HANDLE_TYPE_TOP:
                        handle->setPos(top);
                        break;
                    case Handle::HANDLE_TYPE_BOTTOM:
                        handle->setPos(bottom);
                        break;
                    case Handle::HANDLE_TYPE_TOPLEFT:
                        handle->setPos(mRect.topLeft());
                        break;
                    case Handle::HANDLE_TYPE_TOPRIGHT:
                        handle->setPos(mRect.topRight());
                        break;
                    case Handle::HANDLE_TYPE_BOTTOMLEFT:
                        handle->setPos(mRect.bottomLeft());
                        break;
                    case Handle::HANDLE_TYPE_BOTTOMRIGHT:
                        handle->setPos(mRect.bottomRight());
                        break;
                    case Handle::HANDLE_TYPE_ROTATE:
                        handle->setPos(rotate);
                        break;
                    case Handle::HANDLE_TYPE_ORIGIN:
                        handle->setPos(mOrigin);
                        break;
                    default:
                        break;
                }
            }
        }

        void BaseGraphicItem_t::setGraphicItemOrientation(int o)
        {
            int32_t angle = o - this->mOrientation;

            this->mOrientation = o;
            this->setTransform(QTransform().translate(mOrigin.x(), mOrigin.y())
                                           .rotate(angle)
                                           .translate(-mOrigin.x(), -mOrigin.y()), true);
        }

        void BaseGraphicItem_t::setLineWidth(int32_t w)
        {
            this->mLineWidth = w;
        }

        void BaseGraphicItem_t::setBrush(QBrush b)
        {
            this->mBrush = b;
        }

        void BaseGraphicItem_t::setLocationSlot(bool s)
        {
            this->mLocationSlot = s;
        }

        void BaseGraphicItem_t::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
        {
            if(mLocationSlot)
            {
                dynamic_cast<BaseGraphicItem_t*>(mParentItem)->mouseReleaseEvent(event);
                return;
            }

            QGraphicsItem::mouseReleaseEvent(event);
            this->mCurrentHandle = 0;
        }

        QVariant BaseGraphicItem_t::itemChange(GraphicsItemChange change, const QVariant &value)
        {
            if (change == ItemPositionChange && scene())
            {
                int gridSize = 100;
                bool gridEnabled = false;

                QPointF newPos = value.toPointF();
                if (gridEnabled)
                {
                    if((int)newPos.x()%gridSize != 0)
                    {
                        int x = (int)newPos.x() - (int)newPos.x()%gridSize;
                        newPos.setX(x);
                    }
                    if((int)newPos.y()%gridSize != 0)
                    {
                        int y = (int)newPos.y() - (int)newPos.y()%gridSize;
                        newPos.setY(y);
                    }
                }
                return newPos;
            }

            return QGraphicsItem::itemChange(change, value);
        }

        void BaseGraphicItem_t::createHandles()
        {
            int size = (mRect.width() + mRect.height())/2 / 10;

            mOrigin = this->mRect.center();
            QPointF left(mRect.left(),mRect.top() + mRect.height()/2);
            QPointF right(mRect.right(),mRect.top() + mRect.height()/2);
            QPointF top(mRect.left()+mRect.width()/2,mRect.top());
            QPointF bottom(mRect.left()+mRect.width()/2,mRect.bottom());
            QPointF rotate(top.x(),top.y()-(size* 5));

            mHandles<<new Handle(mRect.topLeft(),size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_TOPLEFT);
            mHandles<<new Handle(top,size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_TOP);
            mHandles<<new Handle(mRect.topRight(),size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_TOPRIGHT);
            mHandles<<new Handle(left,size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_LEFT);
            mHandles<<new Handle(right,size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_RIGHT);
            mHandles<<new Handle(mRect.bottomLeft(),size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_BOTTOMLEFT);
            mHandles<<new Handle(bottom,size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_BOTTOM);
            mHandles<<new Handle(mRect.bottomRight(),size,Handle::HANDLE_SHAPE_RECT,Handle::HANDLE_TYPE_BOTTOMRIGHT);
            mHandles<<new Handle(rotate,size,Handle::HANDLE_SHAPE_CIRCLE,Handle::HANDLE_TYPE_ROTATE);
            mHandles<<new Handle(mOrigin,size,Handle::HANDLE_SHAPE_CIRCLE,Handle::HANDLE_TYPE_ORIGIN);
        }
    }
}
