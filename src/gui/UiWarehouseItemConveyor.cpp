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
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, getX() - portSizeX, getY() + getH()/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_LEFT));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, getX() + getW(),    getY() + getH()/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_RIGHT));
            }
            else if (getWhItemType() == WarehouseItemType_t::E_CONVEYOR_HUB)
            {
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, getX() - portSizeX,              getY() + getH()/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_LEFT));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, getX() + getW(),                 getY() + getH()/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_RIGHT));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 2, getX() + getW()/2 - portSizeX/2, getY() - portSizeY,                portSizeX, portSizeY, WarehousePortType_t::E_PORT_TOP));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 3, getX() + getW()/2 - portSizeX/2, getY() + getH(),                   portSizeX, portSizeY, WarehousePortType_t::E_PORT_BOTTOM));
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
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, x - portSizeX, y + h/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_LEFT));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, x + w,         y + h/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_RIGHT));
            }
            else if (t == WarehouseItemType_t::E_CONVEYOR_HUB)
            {
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, x - portSizeX,         y + h/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_LEFT));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, x + w,                 y + h/2 - (portSizeY/2), portSizeX, portSizeY, WarehousePortType_t::E_PORT_RIGHT));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 2, x + w/2 - portSizeX/2, y - portSizeY,           portSizeX, portSizeY, WarehousePortType_t::E_PORT_TOP));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 3, x + w/2 - portSizeX/2, y + h,                   portSizeX, portSizeY, WarehousePortType_t::E_PORT_BOTTOM));
            }

            this->setPixmap(QPixmap(":/img/conv.png"));
        }

        void UiWarehouseItemConveyor_t::updateChildrenPositions(double dx, double dy)
        {
            portSizeX = portSizeY = std::min(getH(), getW()) / 3;

            (void) dx;
            (void) dy;
            //std::for_each(whPorts.begin(), whPorts.end(), [=](auto* p) { p->updatePort(dx/2.0, dy/2.0, portSizeX, portSizeY); });

            if (getWhItemType() == WarehouseItemType_t::E_CONVEYOR)
            {
                whPorts.at(0)->updatePort(mRect.topLeft().x() - portSizeX, mRect.topLeft().y() + getH()/2 - (portSizeY/2), portSizeX, portSizeY);
                whPorts.at(1)->updatePort(mRect.topLeft().x() + getW(),    mRect.topLeft().y() + getH()/2 - (portSizeY/2), portSizeX, portSizeY);
            }
            else if (getWhItemType() == WarehouseItemType_t::E_CONVEYOR_HUB)
            {
               whPorts.at(0)->updatePort(mRect.topLeft().x() - portSizeX,              mRect.topLeft().y() + getH()/2 - (portSizeY/2), portSizeX, portSizeY);
               whPorts.at(1)->updatePort(mRect.topLeft().x() + getW(),                 mRect.topLeft().y() + getH()/2 - (portSizeY/2), portSizeX, portSizeY);
               whPorts.at(2)->updatePort(mRect.topLeft().x() + getW()/2 - portSizeX/2, mRect.topLeft().y() - portSizeY,                portSizeX, portSizeY);
               whPorts.at(3)->updatePort(mRect.topLeft().x() + getW()/2 - portSizeX/2, mRect.topLeft().y() + getH(),                   portSizeX, portSizeY);
            }
        }
    }
}
