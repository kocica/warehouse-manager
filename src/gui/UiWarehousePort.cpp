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

        UiWarehousePort_t::UiWarehousePort_t(QGraphicsScene *s, QGraphicsItem* parent, MainWindow *ui, UiWarehouseItem_t *whItem, int32_t x, int32_t y)
            : BaseShapeGraphicItem_t(x, y, 25, 25, BaseShapeGraphicItem_t::ITEM_RECTANGLE, s, parent)
            , ui(ui)
            , whItem(whItem)
        {
            this->showHandles(false);

            /*switch(whItem->getWhItemType())
            {
                case UiWarehouseItemType_t::E_CONVEYOR_R:
                    this->setText(">");
                    break;
                case UiWarehouseItemType_t::E_CONVEYOR_L:
                    this->setText("<");
                    break;
                case UiWarehouseItemType_t::E_CONVEYOR_U:
                    this->setText("^");
                    break;
                case UiWarehouseItemType_t::E_CONVEYOR_D:
                    this->setText("v");
                    break;
                case UiWarehouseItemType_t::E_CONVEYOR_HUB:
                    this->setText("+");
                    break;
                case UiWarehouseItemType_t::E_LOCATION_SHELF:
                    this->setText("*");
                    break;
                default:
                    this->setText("?");
            }*/
        }

        UiWarehousePort_t::~UiWarehousePort_t()
        {
            if (this->isConnected())
            {
                delete whConn;
            }
        }

        void UiWarehousePort_t::mousePressEvent(QGraphicsSceneMouseEvent* event)
        {
            if (event->button() == Qt::LeftButton)
            {
                if (UiCursor_t::getCursor().getMode() == UiCursorMode_t::E_MODE_DELETE)
                {
                    //scene->removeItem()
                    std::cout << "Remove parent" << std::endl;
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
                    else if (selectedPort->getWhItem() == this->getWhItem())
                    {
                        std::cout << "Same object!" << std::endl;
                    }
                    else
                    {
                        /*auto pos1 = selectedPort->parentWidget()->mapToGlobal(selectedPort->geometry().topLeft());
                        auto pos2 = this->parentWidget()->mapToGlobal(this->geometry().topLeft());

                        auto globalPos1 = ui->getWidgetPosition(pos1);
                        auto globalPos2 = ui->getWidgetPosition(pos2);

                        (void) globalPos1;*/

                        //if ((pos1.x() == pos2.x()) || (pos1.y() == pos2.y()))
                        //{
                            if (isWhItemCombinationAllowed(this->getWhItem()->getWhItemType(),
                                                   selectedPort->getWhItem()->getWhItemType()))
                            {
                                /*if(selectedPort->getWhItem()->getWhItemType() == UiWarehouseItemType_t::E_CONVEYOR_R)
                                {
                                    int32_t diff = pos2.x() - pos1.x() - 10;
                                    selectedPort->getWhItem()->resize(selectedPort->getWhItem()->width() + diff, selectedPort->getWhItem()->height());
                                    selectedPort->move(selectedPort->getWhItem()->width() - 10, 10/2);
                                }
                                else if(selectedPort->getWhItem()->getWhItemType() == UiWarehouseItemType_t::E_CONVEYOR_L)
                                {
                                    int32_t diff = pos1.x() - pos2.x() - 10;
                                    selectedPort->getWhItem()->move(globalPos2.x() + 10, globalPos2.y() - 10/2);
                                    selectedPort->getWhItem()->resize(selectedPort->getWhItem()->width() + diff, selectedPort->getWhItem()->height());
                                }
                                else if(selectedPort->getWhItem()->getWhItemType() == UiWarehouseItemType_t::E_CONVEYOR_U)
                                {
                                    int32_t diff = pos2.y() - pos1.y() - 10;
                                    selectedPort->getWhItem()->move(globalPos2.x() - 10/2, globalPos2.y() + 10);
                                    selectedPort->getWhItem()->resize(selectedPort->getWhItem()->width(), selectedPort->getWhItem()->height() - diff);
                                }
                                else if(selectedPort->getWhItem()->getWhItemType() == UiWarehouseItemType_t::E_CONVEYOR_D)
                                {
                                    int32_t diff = pos2.y() - pos1.y() - 10;
                                    selectedPort->getWhItem()->resize(selectedPort->getWhItem()->width(), selectedPort->getWhItem()->height() + diff);
                                    selectedPort->move(10/2, selectedPort->getWhItem()->height() - 10);
                                }
                                else if(selectedPort->getWhItem()->getWhItemType() == UiWarehouseItemType_t::E_CONVEYOR_HUB)
                                {
                                    if(They are not next to each other false)
                                    {
                                        std::cout << "Hub and conv have to be next each other" << std::endl;
                                    }
                                }
                                else
                                {
                                    std::cout << "Can't connect this wh item type combination" << std::endl;
                                    return;
                                }*/

                                this->whConn = new UiWarehouseConnection_t(selectedPort, this);
                                selectedPort->setWhConn(this->whConn);
                                selectedPort->connect();
                                connect();
                            }
                            else
                            {
                                std::cout << "Incompatible types!" << std::endl;
                            }
                        //}
                        //else
                        //{
                        //    std::cout << "Ports are not on the same level!" << std::endl;
                        //}
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
            mSelected = true;
        }

        void UiWarehousePort_t::unselect()
        {
            selectedPort = nullptr;
            mSelected = false;
        }

        void UiWarehousePort_t::connect()
        {
            selectedPort = nullptr;
            mConnected = true;
        }

        bool UiWarehousePort_t::isConnected() const
        {
            return whConn;
        }

        void UiWarehousePort_t::setWhConn(UiWarehouseConnection_t* newConn)
        {
            if(this->whConn != nullptr)
            {
                std::cout << "Alrady connected!" << std::endl;
            }
            else
            {
                this->whConn = newConn;
            }
        }

        void UiWarehousePort_t::dump() const
        {
            std::cout << std::endl << "    - Dump warehouse port on item ID <" << this->getWhItem()->getWhItemID() << ">" << std::endl;

            if(this->isConnected())
            {
                whConn->dump();
            }
        }

        void UiWarehousePort_t::disconnect()
        {
            if(this->isConnected())
            {
                mConnected = false;
                whConn = nullptr;
            }
        }
    }
}
