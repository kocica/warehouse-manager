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
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, getX() + getW()/2 - portSizeX, getY() + getH()/2 - (portSizeY/2), portSizeX, portSizeY));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, getX() + getW()/2            , getY() + getH()/2 - (portSizeY/2), portSizeX, portSizeY));
            }
            else if (getWhItemType() == WarehouseItemType_t::E_CONVEYOR_HUB)
            {
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, getX() + getW()/2 - portSizeX  , getY() + getH()/2 - (portSizeY/2), portSizeX, portSizeY));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, getX() + getW()/2              , getY() + getH()/2 - (portSizeY/2), portSizeX, portSizeY));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 2, getX() + getW()/2 - portSizeX/2, getY() + getH()/2 - portSizeY    , portSizeX, portSizeY));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 3, getX() + getW()/2 - portSizeX/2, getY() + getH()/2                , portSizeX, portSizeY));
            }

            this->setGraphicItemOrientation(i.getO()); // We have to rotate the object after its constructed
        }

        UiWarehouseItemConveyor_t::UiWarehouseItemConveyor_t(QGraphicsScene* s, MainWindow* ui, int32_t x, int32_t y, int32_t w, int32_t h, WarehouseItemType_t t)
            : UiWarehouseItem_t(s, ui, x, y, w, h, t)
        {
            /*auto dialog = new QDialog(this);
            auto form   = new QFormLayout(dialog);
            auto len    = new QLineEdit(dialog);

            form->addRow(QString("Enter conveyor length: "), len);

            QDialogButtonBox buttons(QDialogButtonBox::Ok, Qt::Horizontal, dialog);
            QObject::connect(&buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
            form->addRow(&buttons);

            if (dialog->exec() == QDialog::Accepted)
            {
                // TODO: Horizontal conv x = 10 y = length
                sizeX = len->text().toInt();
                sizeY = 10; // TODO: Port size
            }*/

            /*if (UiWarehouseLayout_t::getWhLayout().itemsIntersects(this))
            {
                std::cerr << "Collision detected - Item's cannot intersect." << std::endl;

                // TODO: Remove from layout and deleteLater()?
            }*/

            if (t == WarehouseItemType_t::E_CONVEYOR)
            {
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, x + w/2 - portSizeX, y + h/2 - (portSizeY/2), portSizeX, portSizeY));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, x + w/2            , y + h/2 - (portSizeY/2), portSizeX, portSizeY));
            }
            else if (t == WarehouseItemType_t::E_CONVEYOR_HUB)
            {
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, x + w/2 - portSizeX  , y + h/2 - (portSizeY/2), portSizeX, portSizeY));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, x + w/2              , y + h/2 - (portSizeY/2), portSizeX, portSizeY));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 2, x + w/2 - portSizeX/2, y + h/2 - portSizeY    , portSizeX, portSizeY));
                whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 3, x + w/2 - portSizeX/2, y + h/2                , portSizeX, portSizeY));
            }
        }
    }
}
