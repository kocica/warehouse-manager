/**
 * Warehouse manager
 *
 * @file    WarehouseItem.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Base class for warehouse items (location shelf, conveyor, ...)
 */

#include <cstdint>

namespace whm
{
    class WarehouseItem_t
    {
        public:
            virtual ~WarehouseItem_t() = default;

#ifdef WHM_GUI
            WarehouseItem_t();
            //WarehouseItem_t() = delete;
            //WarehouseItem_t(gui::UiWarehouseItem_t& o);
            //gui::UiWarehouseItem_t& getUiObject() const;
#else
            WarehouseItem_t();
#endif

            void setX(int32_t x_) { x = x_; }
            void setY(int32_t y_) { y = y_; }

            int32_t getX() const { return x; }
            int32_t getY() const { return y; }

        protected:
            int32_t x;
            int32_t y;
    };
}