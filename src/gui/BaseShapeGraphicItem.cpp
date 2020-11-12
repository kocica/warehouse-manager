/**
 * Warehouse manager
 *
 * @file    BaseShapeGraphicItem.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   
 */

// Local
#include "Handle.h"
#include "BaseShapeGraphicItem.h"

// Qt
#include <QPainter>
#include <QGraphicsScene>

namespace whm
{
    namespace gui
    {
        BaseShapeGraphicItem_t::BaseShapeGraphicItem_t(qreal x, qreal y, qreal w, qreal h, ItemType type, QGraphicsScene *scene, QGraphicsItem *parent)
            : BaseGraphicItem_t(w, h, scene, parent)
        {
            mRect.setRect(x, y, w, h);
            mType = type;
            mBrush = QBrush(Qt::white);
            createHandles();
        }

        void BaseShapeGraphicItem_t::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
        {
            QPen pen(Qt::black);
            pen.setWidth(this->mLineWidth);
            painter->setPen(pen);

            if(mRect.left() > mRect.right())
            {
                qreal left = mRect.left();
                qreal right = mRect.right();
                mRect.setLeft(right);
                mRect.setRight(left);
            }
            if(mRect.top() > mRect.bottom())
            {
                qreal top = mRect.top();
                qreal bottom = mRect.bottom();
                mRect.setTop(bottom);
                mRect.setBottom(top);
            }

            painter->setRenderHint(QPainter::Antialiasing, true);

            if(this->mConnected)
            {
                return;
            }
            else if(this->mSelected)
            {
                pen.setColor(Qt::red);
                painter->setPen(pen);
            }

            switch (mType)
            {
                case ITEM_RECTANGLE:
                    painter->setBrush(mBrush);
                    painter->setBrushOrigin(this->mRect.topLeft().x(), this->mRect.topLeft().y());
                    painter->drawRect(this->mRect);
                    if(mDrawPixmap)
                    {
                        painter->drawPixmap(this->mRect.toRect(),this->mPixmap);
                    }
                    break;
                case ITEM_ELLIPSE:
                    painter->drawEllipse(this->mRect);
                    break;
                case ITEM_LINE:
                    painter->drawLine(this->mRect.topLeft(),this->mRect.bottomRight());
                    break;
                case ITEM_PIXMAP:
                    painter->drawPixmap(this->mRect.toRect(),this->mPixmap);
                    break;
                default:
                    break;
            }
            BaseGraphicItem_t::paint(painter, option, widget);
            scene()->update();
        }

        void BaseShapeGraphicItem_t::setPixmap(QString path)
        {
            this->mDrawPixmap = true;
            this->mPixmap = QPixmap(path);
        }

        QRectF BaseShapeGraphicItem_t::boundingRect() const
        {
            return BaseGraphicItem_t::boundingRect();
        }
    }
}
