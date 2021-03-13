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
#include "mainwindow.h"
#include "BaseShapeGraphicItem.h"

namespace whm
{
    namespace gui
    {
        class UiWarehouseSlot_t : public BaseShapeGraphicItem_t
        {
            public:
                UiWarehouseSlot_t(int32_t, int32_t, int32_t, int32_t, BaseShapeGraphicItem_t::ItemType, MainWindow*, QGraphicsScene*, QGraphicsItem*);
                ~UiWarehouseSlot_t() = default;

                std::string getArticle() const;
                void setArticle(const std::string&);

                /**
                 * @author  John1024 (https://stackoverflow.com/users/3030305/john1024)
                 * @date    Dec 26 '13 at 23:18
                 * @link    Q https://stackoverflow.com/q/20792445/8254699
                 * @link    A https://stackoverflow.com/a/20792531/8254699
                 */
                void setSlotHeat(int32_t, int32_t, int32_t);

                void updateSlot(double, double, int32_t, int32_t);

            protected:
                void hoverEnterEvent(QGraphicsSceneHoverEvent*);
                void hoverLeaveEvent(QGraphicsSceneHoverEvent*);

            private:
                std::string article;
                int32_t quantity{ 0 };
        };
    }
}
