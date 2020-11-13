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
    }
}
