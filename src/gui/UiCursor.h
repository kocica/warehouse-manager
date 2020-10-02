/**
 * Warehouse manager
 *
 * @file    UiCursor.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Class representing cursor in UI
 */

#pragma once

namespace whm
{
    namespace gui
    {
        enum class UiCursorMode_t
        {
            E_MODE_SELECT,
            E_MODE_DELETE,
            E_MODE_WH_ITEM_LOC,
            E_MODE_WH_ITEM_CONV
        };

        class UiCursor_t
        {
            public:
                UiCursor_t() = default;
                ~UiCursor_t() = default;

                static UiCursor_t& getCursor();

                UiCursorMode_t getMode() const;
                void setMode(const UiCursorMode_t&);

            private:
                UiCursorMode_t m{ UiCursorMode_t::E_MODE_SELECT };
        };
    }
}