/**
 * Warehouse manager
 *
 * @file    RightClickButton.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Button emitting both left & right click events
 */

#include "RightClickButton.h"

namespace whm
{
    namespace gui
    {
        QRightClickButton::QRightClickButton(QWidget *parent) :
            QPushButton(parent)
        { }

        void QRightClickButton::mousePressEvent(QMouseEvent *e)
        {
            if(e->button() == Qt::RightButton)
            {
                emit rightClicked();
            }
            else if(e->button() == Qt::LeftButton)
            {
                emit clicked();
            }
        }
    }
}
