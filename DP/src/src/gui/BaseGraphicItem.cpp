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
#include "UiWarehouseItem.h"
#include "UiWarehouseLayout.h"
#include "UiWarehouseItemGate.h"
#include "UiWarehouseItemConveyor.h"
#include "UiWarehouseItemLocation.h"

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
        BaseGraphicItem_t::BaseGraphicItem_t(qreal w, qreal h, MainWindow* ui, QGraphicsScene *scene, QGraphicsItem *parent)
            : QGraphicsRectItem(parent)
            , mConnected(false)
            , mDrawHandles(true)
            , mLocationSlot(false)
            , mAllowChange(false)
            , mCheckPorts(false)
            , mParentItem(parent)
            , ui(ui)
            , mCurrentHandle(0)
        {
            QUuid id;
            id = QUuid::createUuid();
            this->mId = id.toString();
            this->mDrawBoundingRect = true;
            this->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
            if(scene)
            {
                if(parent == nullptr)
                {
                    scene->addItem(this);
                }
            }

            minWidth = w / 2;
            minHeight = h / 2;

            this->setZValue(1);
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
            int size = std::min(mRect.width(), mRect.height()) / 8;

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

        QRectF BaseGraphicItem_t::getRect()
        {
            auto t = QTransform().translate(mOrigin.x(), mOrigin.y()).rotate(mOrientation).translate(-mOrigin.x(), -mOrigin.y());
            auto p = t.mapToPolygon(mRect.toRect());
            auto r = p.boundingRect();
            auto o = this->mOrientation;
            this->setGraphicItemOrientation(0);
            r.setTopLeft(QPoint(this->scenePos().x() + r.topLeft().x(), this->scenePos().y() + r.topLeft().y()));
            r.setBottomRight(QPoint(this->scenePos().x() + r.bottomRight().x(), this->scenePos().y() + r.bottomRight().y()));
            this->setGraphicItemOrientation(o);

            return r;
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
                if(this->mConnected)
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

                    painter->drawLine(p1,p2);
                    painter->drawPoint(this->mOrigin);
                }
            }
        }

        void BaseGraphicItem_t::mousePressEvent(QGraphicsSceneMouseEvent *event)
        {
            if(ui->isOptimizationActive())
            {
                QMessageBox::warning(nullptr, "Warning", "Cannot modify layout while simulation/optimization is running!");
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Cannot modify layout while simulation/optimization is running!");
                return;
            }

            if(mLocationSlot)
            {
                dynamic_cast<BaseGraphicItem_t*>(mParentItem)->mousePressEvent(event);
                return;
            }

            QGraphicsRectItem::mousePressEvent(event);

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

        void BaseGraphicItem_t::updateChildren(double dx, double dy)
        {
            QList<QGraphicsItem*> items = scene()->items();

            foreach(QGraphicsItem* i, items)
            {
                if(auto* whPort = dynamic_cast<UiWarehousePort_t*>(i))
                {
                    auto* whItemLoc = dynamic_cast<UiWarehouseItemLocation_t*>(whPort->getParent());

                    if (whItemLoc && this->id() == whItemLoc->id())
                    {
                        whItemLoc->updateChildrenPositions(dx, dy); return;
                    }

                    auto* whItemConv = dynamic_cast<UiWarehouseItemConveyor_t*>(whPort->getParent());

                    if (whItemConv && this->id() == whItemConv->id())
                    {
                        whItemConv->updateChildrenPositions(dx, dy); return;
                    }

                    auto* whItemGate = dynamic_cast<UiWarehouseItemGate_t*>(whPort->getParent());

                    if (whItemGate && this->id() == whItemGate->id())
                    {
                        whItemGate->updateChildrenPositions(dx, dy); return;
                    }
                }
            }
        }

        void BaseGraphicItem_t::updateChildrenPositions(double dx, double dy)
        {
            (void)dx;
            (void)dy;

            // Reimplement in derivate classes
        }

        void BaseGraphicItem_t::shiftGraphicItem(int dx, int dy)
        {
            this->mRect.setTopLeft(QPointF(this->mRect.topLeft().x() + dx/2.0, this->mRect.topLeft().y() + dy/2.0));
            this->mRect.setBottomRight(QPointF(this->mRect.bottomRight().x() + dx/2.0, this->mRect.bottomRight().y() + dy/2.0));

            this->mOrigin.setX((dx * mRect.width()) + mRect.center().x());
            this->mOrigin.setY((dy * mRect.height()) + mRect.center().y());
        }

        void BaseGraphicItem_t::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
        {
            if(ui->isOptimizationActive())
            {
                QMessageBox::warning(nullptr, "Warning", "Cannot modify layout while simulation/optimization is running!");
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Cannot modify layout while simulation/optimization is running!");
                return;
            }

            if(mLocationSlot)
            {
                dynamic_cast<BaseGraphicItem_t*>(mParentItem)->mouseMoveEvent(event);
            }

            if(!mDrawHandles)
            {
                return;
            }

            int gridSize = UiWarehouseLayout_t::getWhLayout().getRatio();

            QPointF newPos = event->pos();

            if((int)newPos.x() % gridSize != 0)
            {
                int x = (int)newPos.x() - (int)newPos.x() % gridSize;
                newPos.setX(x);
            }
            if((int)newPos.y() % gridSize != 0)
            {
                int y = (int)newPos.y() - (int)newPos.y() % gridSize;
                newPos.setY(y);
            }

            event->setPos(newPos);

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
                    {
                        rotation = (int(-QLineF(event->scenePos(),     mapToScene(mOrigin)).angle() / 90) * 90) +
                                   (int( QLineF(event->lastScenePos(), mapToScene(mOrigin)).angle() / 90) * 90) ;
                        setOrientationAligned((this->mOrientation + rotation) % 360);
                        break;
                    }
                    case Handle::HANDLE_TYPE_ORIGIN:
                        mCurrentHandle->setPos(event->pos());
                        this->mOrigin = mCurrentHandle->pos();
                        break;
                    case Handle::HANDLE_TYPE_CTRL:
                        mCurrentHandle->setPos(event->pos());
                        break;
                    default:
                        QGraphicsRectItem::mouseMoveEvent(event);
                        break;
                }

                updateHandles();

                this->setRect(this->mRect);
            }
            else
            {
                mAllowChange = true;
                mCheckPorts  = true;

                if(auto* whItem = dynamic_cast<UiWarehouseItem_t*>(this))
                {
                    if(whItem && whItem->isConnected())
                    {
                        QMessageBox::warning(nullptr, "Warning", "Cannot move connected items!");
                        Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Cannot move connected items!");
                        return;
                    }
                }

                QGraphicsRectItem::mouseMoveEvent(event);
            }
        }

        void BaseGraphicItem_t::updateHandles()
        {
            QPointF left(mRect.left(),mRect.top() + mRect.height()/2);
            QPointF right(mRect.right(),mRect.top() + mRect.height()/2);
            QPointF top(mRect.left()+mRect.width()/2,mRect.top());
            QPointF bottom(mRect.left()+mRect.width()/2,mRect.bottom());
            int size = std::min(mRect.width(), mRect.height()) / 8;
            QPointF rotate(top.x(),top.y()-(5*size));

            foreach (Handle *handle, mHandles)
            {
                handle->setSize(size);

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
            o = o % 360;
            int32_t angle = o - this->mOrientation;

            this->mOrientation = o;
            this->setTransform(QTransform().translate(mOrigin.x(), mOrigin.y())
                                           .rotate(angle)
                                           .translate(-mOrigin.x(), -mOrigin.y()), true);
        }

        void BaseGraphicItem_t::setOrientationAligned(int o)
        {
            setGraphicItemOrientation(o);

            /*auto gridSize = UiWarehouseLayout_t::getWhLayout().getRatio();

            auto t = QTransform().translate(mOrigin.x(), mOrigin.y()).rotate(o).translate(-mOrigin.x(), -mOrigin.y());
            auto p = t.mapToPolygon(mRect.toRect());
            auto r = p.boundingRect();

            this->setGraphicItemOrientation(0);
            r.setTopLeft(QPoint(this->scenePos().x() + r.topLeft().x(), this->scenePos().y() + r.topLeft().y()));
            r.setBottomRight(QPoint(this->scenePos().x() + r.bottomRight().x(), this->scenePos().y() + r.bottomRight().y()));
            this->setGraphicItemOrientation(o);

            auto diffx = (int)r.topLeft().x() % gridSize;
            auto diffy = (int)r.topLeft().y() % gridSize;
            mRect.setTopLeft(QPointF(mRect.topLeft().x() - diffx, mRect.topLeft().y() - diffy));
            mRect.setBottomRight(QPointF(mRect.bottomRight().x() - diffx, mRect.bottomRight().y() - diffy));
            updateChildren(diffx, diffy);
            mOrigin.setX(mRect.center().x());
            mOrigin.setY(mRect.center().y());*/
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
            if(ui->isOptimizationActive())
            {
                QMessageBox::warning(nullptr, "Warning", "Cannot modify layout while simulation/optimization is running!");
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Cannot modify layout while simulation/optimization is running!");
                return;
            }

            if(mLocationSlot)
            {
                dynamic_cast<BaseGraphicItem_t*>(mParentItem)->mouseReleaseEvent(event);
                return;
            }

            if(auto* whItem = dynamic_cast<UiWarehouseItem_t*>(this))
            {
                for(auto* whPort : whItem->getWhPorts())
                {
                    foreach(QGraphicsItem* i, scene()->items())
                    {
                        if(auto* scenePort = dynamic_cast<UiWarehousePort_t*>(i))
                        {
                            if(scenePort->getWhItem() != whPort->getWhItem())
                            {
                                if(whPort != scenePort && !whPort->isConnected() && !scenePort->isConnected())
                                {
                                    scenePort->expand(10);
                                    if(whPort->collidesWithItem(scenePort))
                                    {
                                        whPort->select();
                                        auto* e = new QGraphicsSceneMouseEvent();
                                        e->setButton(Qt::LeftButton);
                                        scenePort->mousePressEvent(e);
                                    }
                                    scenePort->shrink(10);
                                }
                            }
                        }
                    }
                }
            }

            QGraphicsRectItem::mouseReleaseEvent(event);
            this->mCurrentHandle = 0;
        }

        QVariant BaseGraphicItem_t::itemChange(GraphicsItemChange change, const QVariant& value)
        {
            if(change == ItemPositionChange && scene() && mAllowChange)
            {
                int gridSize = UiWarehouseLayout_t::getWhLayout().getRatio();

                QPointF newPos = value.toPointF();

                if((int)newPos.x() % gridSize != 0)
                {
                    int x = (int)newPos.x() - (int)newPos.x() % gridSize;
                    newPos.setX(x);
                }
                if((int)newPos.y() % gridSize != 0)
                {
                    int y = (int)newPos.y() - (int)newPos.y() % gridSize;
                    newPos.setY(y);
                }

                mAllowChange = !mAllowChange;

                return newPos;
            }
            else if(change == ItemPositionHasChanged && scene() && mCheckPorts)
            {
                if(auto* whItem = dynamic_cast<UiWarehouseItem_t*>(this))
                {
                    std::vector<UiWarehousePort_t*> selectedPorts;

                    for(auto* whPort : whItem->getWhPorts())
                    {
                        foreach(QGraphicsItem* i, scene()->items())
                        {
                            if(auto* scenePort = dynamic_cast<UiWarehousePort_t*>(i))
                            {
                                if(scenePort->getWhItem() != whPort->getWhItem())
                                {
                                    if(whPort != scenePort && !whPort->isConnected() && !scenePort->isConnected())
                                    {
                                        if(whPort->collidesWithItem(scenePort))
                                        {
                                            selectedPorts.push_back(whPort);
                                            selectedPorts.push_back(scenePort);
                                        }
                                        else
                                        {
                                            whPort->unmark();
                                            scenePort->unmark();
                                        }
                                    }
                                }
                            }
                        }
                    }

                    std::for_each(selectedPorts.begin(), selectedPorts.end(), [](auto* p){ p->mark(); });
                }

                mCheckPorts = !mCheckPorts;
            }

            return QGraphicsRectItem::itemChange(change, value);
        }

        void BaseGraphicItem_t::createHandles()
        {
            int size = std::min(mRect.width(), mRect.height()) / 8;

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