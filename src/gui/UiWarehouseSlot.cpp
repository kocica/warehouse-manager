/**
 * Warehouse manager
 *
 * @file    UiWarehouseSlot.cpp
 * @date    11/10/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse slot
 */

#include "UiWarehouseSlot.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseSlot_t::UiWarehouseSlot_t(int32_t x, int32_t y, int32_t w, int32_t h, BaseShapeGraphicItem_t::ItemType t, MainWindow* ui, QGraphicsScene* s, QGraphicsItem* p)
            : BaseShapeGraphicItem_t(x, y, w, h, t, ui, s, p)
        {
            this->showHandles(false);
            this->setLocationSlot(true);
            this->setLineWidth(10);
        }

        void UiWarehouseSlot_t::setArticle(const std::string& a)
        {
            if(!a.empty())
            {
                this->article = a;
                this->setBrush(Qt::magenta);
            }
            else
            {
                this->article = std::string();
                this->setBrush(Qt::white);
            }
        }

        std::string UiWarehouseSlot_t::getArticle() const
        {
            return this->article;
        }

        /**
         * @author  John1024 (https://stackoverflow.com/users/3030305/john1024)
         * @date    Dec 26 '13 at 23:18
         * @link    https://stackoverflow.com/a/20792531/8254699
         */
        void UiWarehouseSlot_t::setSlotHeat(int32_t max, int32_t min, int32_t val)
        {
            double ratio = 2 * double(val - min) / double(max - min);

            int32_t b = std::max(0.0, 255 * (1 - ratio));
            int32_t r = std::max(0.0, 255 * (ratio - 1));
            int32_t g = 255 - b - r;

            this->setBrush(QColor::fromRgb(r, g, b));
        }

        void UiWarehouseSlot_t::updateSlot(double x, double y, int32_t w, int32_t h)
        {
            this->mRect.setTopLeft(QPointF(x, y));
            this->mRect.setBottomRight(QPointF(x + w, y + h));

            this->mOrigin.setX(mRect.center().x());
            this->mOrigin.setY(mRect.center().y());
        }

        void UiWarehouseSlot_t::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
        {
            QString text;

            text += QString("<table style='background-color:#BD1E51;color:#F1B814'>");

            text += QString("<tr><td><b>Article: </b></td><td>") + QString::fromStdString(article) + QString("</td></tr>");
            text += QString("<tr><td><b>Quantity: </b></td><td>") + QString::number(quantity) + QString("</td></tr>");

            text += QString("</table>");

            info->setPos(event->scenePos());
            info->setZValue(10);
            info->setFont(QFont("Helvetica", 20));
            info->setHtml(text);
            info->show();

            QGraphicsItem::hoverEnterEvent(event);
        }

        void UiWarehouseSlot_t::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
        {
            info->hide();

            QGraphicsItem::hoverLeaveEvent(event);
        }
    }
}
