/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemGate.cpp
 * @date    10/11/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse entrance/dispatch gate
 */

#include <iostream>

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialogButtonBox>

#include "UiWarehouseLayout.h"
#include "UiWarehouseItemGate.h"
#include "../WarehouseItem.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseItemGate_t::UiWarehouseItemGate_t(QGraphicsScene* s, MainWindow* ui, ::whm::WarehouseItem_t& i)
            : UiWarehouseItem_t(s, ui, i.getX(), i.getY(), i.getW(), i.getH(), i.getType())
        {
            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, i.getX() + i.getW() / 2 - (portSizeX/2), i.getY() + i.getH() / 2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_MID));

            this->setGraphicItemOrientation(i.getO()); // We have to rotate the object after its constructed

            this->setBrush(Qt::darkYellow);
        }

        UiWarehouseItemGate_t::UiWarehouseItemGate_t(QGraphicsScene* s, MainWindow* ui, int32_t x, int32_t y, int32_t w, int32_t h, WarehouseItemType_t t)
            : UiWarehouseItem_t(s, ui, x, y, w, h, t)
        {
            /*if (UiWarehouseLayout_t::getWhLayout().itemsIntersects(this))
            {
                std::cerr << "Collision detected - Item's cannot intersect." << std::endl;

                // TODO: Remove from layout and deleteLater()?
            }*/

            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, x + w/2 - (portSizeX/2),  y + h/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_MID));

            this->setBrush(Qt::darkYellow);
        }
    }
}