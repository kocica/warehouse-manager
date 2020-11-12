/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemConveyor.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse conveyor
 */

#include <iostream>

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialogButtonBox>

#include "UiWarehouseLayout.h"
#include "UiWarehouseItemConveyor.h"
#include "../WarehouseItem.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseItemConveyor_t::UiWarehouseItemConveyor_t(QGraphicsScene* s, MainWindow* ui, ::whm::WarehouseItem_t& i)
            : UiWarehouseItem_t(s, ui, i.getX(), i.getY(), i.getW(), i.getH(), i.getType())
        {
            if (getWhItemType() == WarehouseItemType_t::E_CONVEYOR)
            {
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, getX() + getW()/2 - portSizeX, getY() + getH()/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_LEFT));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, getX() + getW()/2            , getY() + getH()/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_RIGHT));
            }
            else if (getWhItemType() == WarehouseItemType_t::E_CONVEYOR_HUB)
            {
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, getX() + getW()/2 - portSizeX  , getY() + getH()/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_LEFT));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, getX() + getW()/2              , getY() + getH()/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_RIGHT));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 2, getX() + getW()/2 - portSizeX/2, getY() + getH()/2 - portSizeY    , portSizeX, portSizeY, WarehousePortType_t::E_PORT_TOP));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 3, getX() + getW()/2 - portSizeX/2, getY() + getH()/2                , portSizeX, portSizeY, WarehousePortType_t::E_PORT_BOTTOM));
            }

            this->setGraphicItemOrientation(i.getO()); // We have to rotate the object after its constructed

            this->setPixmap(QPixmap(":/img/conv.png"));
        }

        UiWarehouseItemConveyor_t::UiWarehouseItemConveyor_t(QGraphicsScene* s, MainWindow* ui, int32_t x, int32_t y, int32_t w, int32_t h, WarehouseItemType_t t)
            : UiWarehouseItem_t(s, ui, x, y, w, h, t)
        {
            /*if (UiWarehouseLayout_t::getWhLayout().itemsIntersects(this))
            {
                std::cerr << "Collision detected - Item's cannot intersect." << std::endl;

                // TODO: Remove from layout and deleteLater()?
            }*/

            if (t == WarehouseItemType_t::E_CONVEYOR)
            {
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, x + w/2 - portSizeX, y + h/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_LEFT));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, x + w/2            , y + h/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_RIGHT));
            }
            else if (t == WarehouseItemType_t::E_CONVEYOR_HUB)
            {
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, x + w/2 - portSizeX  , y + h/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_LEFT));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, x + w/2              , y + h/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_RIGHT));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 2, x + w/2 - portSizeX/2, y + h/2 - portSizeY    , portSizeX, portSizeY, WarehousePortType_t::E_PORT_TOP));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 3, x + w/2 - portSizeX/2, y + h/2                , portSizeX, portSizeY, WarehousePortType_t::E_PORT_BOTTOM));
            }

            this->setPixmap(QPixmap(":/img/conv.png"));
        }

        void UiWarehouseItemConveyor_t::updateChildrenPositions(double dx, double dy)
        {
            portSizeX = getW() / 5;
            portSizeY = getH() / 5;

            std::for_each(whPorts.begin(), whPorts.end(), [=](auto* p) { p->updatePort(dx/2, dy/2, portSizeX, portSizeY); });
        }
    }
}
