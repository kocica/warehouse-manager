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
#include <QSettings>
#include <QGraphicsItem>
#include <QGraphicsScene>

// Std
#include <iostream>

namespace whm
{
    namespace gui
    {
        BaseGraphicItem_t::BaseGraphicItem_t(QGraphicsScene *scene, QGraphicsItem *parent)
            : QGraphicsItem(parent)
            , mSelected(false)
            , mConnected(false)
            , mDrawHandles(true)
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
            QSettings settings;

            int size = settings.value("drawing/hanleSize", 10).toInt();

            return this->mRect.adjusted(-size/2,-size/2 - 50,size/2,size/2);
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
            pen.setWidth(3);
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
                    pen.setColor(Qt::blue);
                    painter->setPen(pen);
                    painter->drawRect(this->mRect);
                }
                else if(this->mDrawBoundingRect && mDrawHandles)
                {
                    pen.setColor(Qt::green);
                    painter->setPen(pen);
                    painter->drawRect(this->mRect);
                }

                QPointF p1;
                QPointF p2;

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

        void BaseGraphicItem_t::updateChildren(int dx, int dy)
        {
            QList<QGraphicsItem*> items = scene()->items();

            foreach(QGraphicsItem* i, items)
            {
                if(auto* baseItem = dynamic_cast<BaseGraphicItem_t*>(i))
                {
                    auto* parentBaseItem = dynamic_cast<BaseGraphicItem_t*>(baseItem->getParent());

                    if (parentBaseItem && this->id() == parentBaseItem->id())
                    {
                        baseItem->shiftGraphicItem(dx, dy);
                    }
                }
            }
        }

        void BaseGraphicItem_t::shiftGraphicItem(int dx, int dy)
        {
            this->mRect.setTopLeft(QPointF(this->mRect.topLeft().x() + (dx/2.0), this->mRect.topLeft().y() + (dy/2.0)));
            this->mRect.setBottomRight(QPointF(this->mRect.bottomRight().x() + (dx/2.0), this->mRect.bottomRight().y() + (dy/2.0)));

            this->mOrigin.setX((dx * mRect.width()) + mRect.center().x());
            this->mOrigin.setY((dy * mRect.height()) + mRect.center().y());

            // TODO: Resizing of ports
            //this->mRect.setWidth(this->mRect.width() + (dx/5.0));
            //this->mRect.setHeight(this->mRect.height() - (dy/5.0));
        }

        void BaseGraphicItem_t::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
        {
            if(!mDrawHandles)
            {
                return;
            }

            if(event->buttons() == Qt::LeftButton && mCurrentHandle)
            {
                int32_t rotation{ 0 };
                qreal dx = (mOrigin.x() - mRect.center().x()) / mRect.width();
                qreal dy = (mOrigin.y() - mRect.center().y()) / mRect.height();

                switch (mCurrentHandle->type())
                {
                    case Handle::HANDLE_TYPE_LEFT:
                        updateChildren(event->pos().x() - mRect.topLeft().x(), 0);
                        this->mRect.setLeft(event->pos().x());
                        mOrigin.setX((dx * mRect.width()) + mRect.center().x());
                        break;
                    case Handle::HANDLE_TYPE_RIGHT:
                        updateChildren(event->pos().x() - mRect.topRight().x(), 0);
                        this->mRect.setRight(event->pos().x());
                        mOrigin.setX((dx * mRect.width()) + mRect.center().x());
                        break;
                    case Handle::HANDLE_TYPE_TOP:
                        updateChildren(0, event->pos().y() - mRect.topLeft().y());
                        this->mRect.setTop(event->pos().y());
                        mOrigin.setY((dy * mRect.height()) + mRect.center().y());
                        break;
                    case Handle::HANDLE_TYPE_BOTTOM:
                        updateChildren(0, event->pos().y() - mRect.bottomLeft().y());
                        this->mRect.setBottom(event->pos().y());
                        mOrigin.setY((dy * mRect.height()) + mRect.center().y());
                        break;
                    case Handle::HANDLE_TYPE_TOPLEFT:
                        updateChildren(event->pos().x() - mRect.topLeft().x(), event->pos().y() - mRect.topLeft().y());
                        this->mRect.setTopLeft(event->pos());
                        mOrigin.setX((dx * mRect.width()) + mRect.center().x());
                        mOrigin.setY((dy * mRect.height()) + mRect.center().y());
                        break;
                    case Handle::HANDLE_TYPE_TOPRIGHT:
                        updateChildren(event->pos().x() - mRect.topRight().x(), event->pos().y() - mRect.topRight().y());
                        this->mRect.setTopRight(event->pos());
                        mOrigin.setX((dx * mRect.width()) + mRect.center().x());
                        mOrigin.setY((dy * mRect.height()) + mRect.center().y());
                        break;
                    case Handle::HANDLE_TYPE_BOTTOMLEFT:
                        updateChildren(event->pos().x() - mRect.bottomLeft().x(), event->pos().y() - mRect.bottomLeft().y());
                        this->mRect.setBottomLeft(event->pos());
                        mOrigin.setX((dx * mRect.width()) + mRect.center().x());
                        mOrigin.setY((dy * mRect.height()) + mRect.center().y());
                        break;
                    case Handle::HANDLE_TYPE_BOTTOMRIGHT:
                        updateChildren(event->pos().x() - mRect.bottomRight().x(), event->pos().y() - mRect.bottomRight().y());
                        this->mRect.setBottomRight(event->pos());
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

                QPointF left(mRect.left(),mRect.top() + mRect.height()/2);
                QPointF right(mRect.right(),mRect.top() + mRect.height()/2);
                QPointF top(mRect.left()+mRect.width()/2,mRect.top());
                QPointF bottom(mRect.left()+mRect.width()/2,mRect.bottom());
                QPointF rotate(top.x(),top.y()-50);

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
            else
            {
                QGraphicsItem::mouseMoveEvent(event);
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

        void BaseGraphicItem_t::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
        {
            QGraphicsItem::mouseReleaseEvent(event);
            this->mCurrentHandle = 0;
        }

        QVariant BaseGraphicItem_t::itemChange(GraphicsItemChange change, const QVariant &value)
        {
            if (change == ItemPositionChange && scene())
            {
                QSettings settings;
                int gridSize = settings.value("drawing/gridSize").toInt();
                bool gridEnabled = settings.value("drawing/gridEnabled").toBool();

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
            QSettings settings;
            int size = settings.value("drawing/hanleSize", 10).toInt();

            mOrigin = this->mRect.center();
            QPointF left(mRect.left(),mRect.top() + mRect.height()/2);
            QPointF right(mRect.right(),mRect.top() + mRect.height()/2);
            QPointF top(mRect.left()+mRect.width()/2,mRect.top());
            QPointF bottom(mRect.left()+mRect.width()/2,mRect.bottom());
            QPointF rotate(top.x(),top.y()-50);

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
