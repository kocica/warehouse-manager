/**
 * Warehouse manager
 *
 * @file    RightClickButton.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Button emitting both left & right click events
 */

#pragma once

#include <QPushButton>
#include <QMouseEvent>

namespace whm
{
    namespace gui
    {
        class QRightClickButton : public QPushButton
        {
            Q_OBJECT

        public:
            explicit QRightClickButton(QWidget *parent = 0);

        private slots:
            void mousePressEvent(QMouseEvent *e);

        public signals:
            void rightClicked();
            void clicked();
        };
    }
}
