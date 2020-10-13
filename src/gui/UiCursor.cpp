/**
 * Warehouse manager
 *
 * @file    UiCursor.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Class representing cursor in UI
 */

#include "UiCursor.h"

namespace whm
{
    namespace gui
    {
        UiCursor_t& UiCursor_t::getCursor()
        {
            static UiCursor_t c;
            return c;
        }

        UiCursorMode_t UiCursor_t::getMode() const
        {
            return m;
        }

        void UiCursor_t::setMode(const UiCursorMode_t& m_)
        {
            m = m_;
        }

        bool UiCursor_t::isItemSelected() const
        {
            return m != UiCursorMode_t::E_MODE_SELECT &&
                   m != UiCursorMode_t::E_MODE_DELETE;
        }
    }
}
