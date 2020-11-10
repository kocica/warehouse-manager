/**
 * Warehouse manager
 *
 * @file    UiWarehouseSlot.h
 * @date    11/10/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse slot
 */

#pragma once

#include "../Logger.h"
#include "BaseShapeGraphicItem.h"

namespace whm
{
    namespace gui
    {
        class UiWarehouseSlot_t : public BaseShapeGraphicItem_t
        {
            public:
                UiWarehouseSlot_t(int32_t, int32_t, int32_t, int32_t, BaseShapeGraphicItem_t::ItemType, QGraphicsScene*, QGraphicsItem*);
                ~UiWarehouseSlot_t() = default;

                void setArticle(const std::string&);

            private:
                std::string article;
        };
    }
}
