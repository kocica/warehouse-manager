/**
 * Warehouse manager
 *
 * @file    UiWarehousePort.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse port
 */

#include <iostream>

#include "UiCursor.h"
#include "UiWarehousePort.h"
#include "UiWarehouseItem.h"
#include "UiWarehouseConnection.h"

namespace whm
{
    namespace gui
    {
        UiWarehousePort_t *UiWarehousePort_t::selectedPort = nullptr;

        UiWarehousePort_t::UiWarehousePort_t(QWidget *parent, MainWindow *ui, UiWarehouseItem_t *whItem, int32_t x, int32_t y)
            : QPushButton(parent)
            , ui(ui)
            , whItem(whItem)
        {
            this->move(QPoint(x, y));
            this->resize(10, 10);
            this->setStyleSheet("background-color: white; outline: none;");
            this->setText("+");
            this->show();
        }

        void UiWarehousePort_t::mousePressEvent(QMouseEvent *event)
        {
            if (event->button() == Qt::LeftButton)
            {
                if (UiCursor_t::getCursor().getMode() == UiCursorMode_t::E_MODE_DELETE)
                {
                    delete this->parentWidget();
                }
                else
                {
                    if (selectedPort == this)
                    {
                        unselect();
                    }
                    else if (selectedPort == nullptr)
                    {
                        select();
                    }
                    else if (selectedPort->parent() == this->parent())
                    {
                        std::cout << "Same object!" << std::endl;
                    }
                    else
                    {
                        auto pos1 = selectedPort->parentWidget()->mapToGlobal(selectedPort->geometry().topLeft());
                        auto pos2 = this->parentWidget()->mapToGlobal(this->geometry().topLeft());

                        if ((pos1.x() == pos2.x()) || (pos1.y() == pos2.y()))
                        {
                            if (isWhItemCombinationAllowed(this->getWhItem()->getWhItemType(),
                                                   selectedPort->getWhItem()->getWhItemType()))
                            {
                                int32_t diff = pos2.x() - pos1.x() - 10;
                                selectedPort->getWhItem()->resize(selectedPort->getWhItem()->width() + diff, selectedPort->getWhItem()->height());
                                new UiWarehouseConnection_t(this, ui, selectedPort, this);
                            }
                            else
                            {
                                std::cout << "Incompatible types!" << std::endl;
                            }
                        }
                        else
                        {
                            std::cout << "Ports are not on the same level!" << std::endl;
                        }
                    }
                }
            }
        }

        UiWarehouseItem_t* UiWarehousePort_t::getWhItem() const
        {
            return this->whItem;
        }

        void UiWarehousePort_t::select()
        {
            selectedPort = this;
            this->setStyleSheet("background-color: yellow;");
        }

        void UiWarehousePort_t::unselect()
        {
            selectedPort = nullptr;
            this->setStyleSheet("background-color: white;");
        }
    }
}