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
        UiWarehouseSlot_t::UiWarehouseSlot_t(int32_t x, int32_t y, int32_t w, int32_t h, BaseShapeGraphicItem_t::ItemType t, QGraphicsScene* s, QGraphicsItem* p)
            : BaseShapeGraphicItem_t(x, y, w, h, t, s, p)
        {
            this->showHandles(false);
            this->setLineWidth(1);
        }

        void UiWarehouseSlot_t::setArticle(const std::string& a)
        {
            if(!a.empty())
            {
                this->article = a;
                this->setBrush(Qt::magenta);
            }
        }
    }
}
