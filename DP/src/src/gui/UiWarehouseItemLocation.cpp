/**
 * Warehouse manager
 *
 * @file    UiWarehouseItemLocation.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse location
 */

#include <iostream>

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialogButtonBox>

#include "../WarehouseItem.h"
#include "UiWarehouseLayout.h"
#include "UiWarehouseItemLocation.h"
#include "../WarehouseLocationRack.h"

namespace whm
{
    namespace gui
    {
        UiWarehouseItemLocation_t::UiWarehouseItemLocation_t(QGraphicsScene* s, MainWindow* ui, ::whm::WarehouseItem_t& i)
            : UiWarehouseItem_t(s, ui, i.getX(), i.getY(), i.getW(), i.getH(), i.getType())
        {
            // Slots
            int32_t slotW = i.getW()/i.getSlotCountX();
            int32_t slotH = i.getH()/i.getSlotCountY();

            int32_t x2 = i.getX();
            for(int32_t c1 = 0; c1 < i.getSlotCountX(); c1++)
            {
                int32_t y2 = i.getY();
                for(int32_t c2 = 0; c2 < i.getSlotCountY(); c2++)
                {
                    whSlots.push_back(new UiWarehouseSlot_t(x2, y2, slotW, slotH, BaseShapeGraphicItem_t::ITEM_RECTANGLE, ui, s, this));
                    y2 += slotH;
                }
                x2 += slotW;
            }

            // Ports
            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, i.getX(),                        i.getY() + i.getH()/2 - portSizeY/2, portSizeX, portSizeY, WarehousePortType_t::E_PORT_LEFT));
            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, i.getX() + i.getW() - portSizeX, i.getY() + i.getH()/2 - portSizeY/2, portSizeX, portSizeY, WarehousePortType_t::E_PORT_RIGHT));

            this->setGraphicItemOrientation(i.getO()); // We have to rotate the object after its constructed

            setDimensions(i.getSlotCountX(), i.getSlotCountY());
        }

        void UiWarehouseItemLocation_t::importSlots(::whm::WarehouseItem_t& i)
        {
            auto* rack = i.getWhLocationRack();

            for(int32_t c1 = 0; c1 < i.getSlotCountX(); c1++)
            {
                for(int32_t c2 = 0; c2 < i.getSlotCountY(); c2++)
                {
                    whSlots[(c1 * i.getSlotCountY()) + c2]->setArticle(rack->at(c1, c2).getArticle());
                }
            }
        }

        std::vector<UiWarehouseSlot_t*> UiWarehouseItemLocation_t::getSlots() const
        {
            return whSlots;
        }

        UiWarehouseItemLocation_t::UiWarehouseItemLocation_t(QGraphicsScene* s, MainWindow* ui, int32_t x, int32_t y, int32_t w, int32_t h, WarehouseItemType_t t)
            : UiWarehouseItem_t(s, ui, x, y, w, h, t)
        {
            auto dialog = new QDialog(ui);
            auto form   = new QFormLayout(dialog);
            auto locX   = new QLineEdit(dialog);
            auto locY   = new QLineEdit(dialog);

            form->addRow(QString("Enter X slot dimenstion: "), locX);
            form->addRow(QString("Enter Y slot dimenstion: "), locY);

            QDialogButtonBox buttons(QDialogButtonBox::Ok, Qt::Horizontal, dialog);
            QObject::connect(&buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
            QObject::connect(&buttons, SIGNAL(rejected()), dialog, SLOT(reject()));
            form->addRow(&buttons);

            if (dialog->exec() == QDialog::Accepted)
            {
                setDimensions(locX->text().toInt(), locY->text().toInt());
            }
            else
            {
                // Item cannot be constructed, deleteLater()
            }

            /*if (UiWarehouseLayout_t::getWhLayout().itemsIntersects(this))
            {
                std::cerr << "Collision detected - Item's cannot intersect." << std::endl;

                // TODO: Remove from layout and deleteLater()?
            }*/

            // Slots
            int32_t slotW = w/locX->text().toInt();
            int32_t slotH = h/locY->text().toInt();

            int32_t x2 = x;
            for(int32_t c1 = 0; c1 < locX->text().toInt(); c1++)
            {
                int32_t y2 = y;
                for(int32_t c2 = 0; c2 < locY->text().toInt(); c2++)
                {
                    whSlots.emplace_back(new UiWarehouseSlot_t(x2, y2, slotW, slotH, BaseShapeGraphicItem_t::ITEM_RECTANGLE, ui, s, this));
                    y2 += slotH;
                }
                x2 += slotW;
            }

            // Ports
            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 0, x,                 y + h/2 - portSizeY/2, portSizeX, portSizeY, WarehousePortType_t::E_PORT_LEFT));
            whPorts.emplace_back(new UiWarehousePort_t(s, this, ui, 1, x + w - portSizeX, y + h/2 - portSizeY/2, portSizeX, portSizeY, WarehousePortType_t::E_PORT_RIGHT));
        }

        void UiWarehouseItemLocation_t::updateChildrenPositions(double dx, double dy)
        {
            portSizeX = portSizeY = std::min(getH(), getW()) / 3;

            (void) dx;
            (void) dy;
            //std::for_each(whPorts.begin(), whPorts.end(), [=](auto* p) { p->updatePort(dx/2.0, dy/2.0, portSizeX, portSizeY); });

            whPorts.at(0)->updatePort(mRect.topLeft().x(),                      mRect.topLeft().y() + getH()/2 - portSizeY/2, portSizeX, portSizeY);
            whPorts.at(1)->updatePort(mRect.topLeft().x() + getW() - portSizeX, mRect.topLeft().y() + getH()/2 - portSizeY/2, portSizeX, portSizeY);

            int32_t slotW = getW() / getSlotCountX();
            int32_t slotH = getH() / getSlotCountY();

            int32_t x = mRect.topLeft().x();
            for(int32_t c1 = 0; c1 < getSlotCountX(); c1++)
            {
                int32_t y = mRect.topLeft().y();
                for(int32_t c2 = 0; c2 < getSlotCountY(); c2++)
                {
                    whSlots[(c1 * getSlotCountY()) + c2]->updateSlot(x, y, slotW, slotH);
                    y += slotH;
                }
                x += slotW;
            }
        }
    }
}
