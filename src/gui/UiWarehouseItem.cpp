/**
 * Warehouse manager
 *
 * @file    UiWarehouseItem.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse item (location, conv, ...)
 */

#include <iostream>

#include <QMessageBox>

#include "UiCursor.h"
#include "UiWarehouseItem.h"
#include "UiWarehousePort.h"
#include "UiWarehouseLayout.h"

namespace whm
{
    namespace gui
    {
        std::map<WarehouseItemType_t, std::vector<WarehouseItemType_t>> allowedCombinations =
        {
            { WarehouseItemType_t::E_CONVEYOR,           { WarehouseItemType_t::E_CONVEYOR, WarehouseItemType_t::E_LOCATION_SHELF, WarehouseItemType_t::E_CONVEYOR_HUB,
                                                           WarehouseItemType_t::E_WAREHOUSE_ENTRANCE, WarehouseItemType_t::E_WAREHOUSE_DISPATCH }},
            { WarehouseItemType_t::E_CONVEYOR_HUB,       { WarehouseItemType_t::E_CONVEYOR, WarehouseItemType_t::E_CONVEYOR_HUB, WarehouseItemType_t::E_LOCATION_SHELF }},
            { WarehouseItemType_t::E_LOCATION_SHELF,     { WarehouseItemType_t::E_CONVEYOR, WarehouseItemType_t::E_CONVEYOR_HUB }},
            { WarehouseItemType_t::E_WAREHOUSE_ENTRANCE, { WarehouseItemType_t::E_CONVEYOR }},
            { WarehouseItemType_t::E_WAREHOUSE_DISPATCH, { WarehouseItemType_t::E_CONVEYOR }}
        };

        bool isWhItemCombinationAllowed(WarehouseItemType_t lhs, WarehouseItemType_t rhs)
        {
            std::vector<WarehouseItemType_t>& allowedTypes = allowedCombinations[lhs];

            return std::any_of(allowedTypes.begin(), allowedTypes.end(),
                               [&](WarehouseItemType_t& whItemType) -> bool
                               {
                                   return whItemType == rhs;
                               });
        }

        UiWarehouseItem_t::UiWarehouseItem_t(QGraphicsScene* s, MainWindow* ui, int32_t x, int32_t y, int32_t w, int32_t h, WarehouseItemType_t type)
            : BaseShapeGraphicItem_t(x, y, w, h, BaseShapeGraphicItem_t::ITEM_RECTANGLE, ui, s)
            , ui(ui)
            , scene(s)
            , heatRect(new QGraphicsRectItem())
            , portSizeX{ w / 5 }
            , portSizeY{ h / 5 }
            , whItemID(UiWarehouseLayout_t::getWhLayout().getNextWhItemID())
            , whItemType(type)
        {
            scene->addItem(heatRect);

            info = scene->addText(QString("ID: ") + QString::number(whItemID) +
                                  QString("\nType: ") + QString::number(to_underlying(whItemType)));

            setAcceptHoverEvents(true);

            QObject::connect(&infoTimeout, &QTimer::timeout, [&](){ info->hide(); });
        }

        void UiWarehouseItem_t::moveItem(qreal x, qreal y)
        {
            qreal w = this->mRect.width();
            qreal h = this->mRect.height();

            this->updateChildren(x, y);

            this->mRect.setTopLeft(QPoint(this->mRect.topLeft().x() + x, this->mRect.topLeft().y() + y));
            this->mRect.setBottomRight(QPoint(this->mRect.topLeft().x() + w, this->mRect.topLeft().y() + h));

            this->mOrigin.setX(this->mOrigin.x() + x);
            this->mOrigin.setY(this->mOrigin.y() + y);

            this->updateHandles();
        }

        bool UiWarehouseItem_t::isConnected() const
        {
            return std::any_of(whPorts.begin(), whPorts.end(), [](auto* p)
                                                               -> bool
                                                               {
                                                                   return p->isConnected();
                                                               });
        }

        UiWarehouseItem_t::~UiWarehouseItem_t()
        {
            for (UiWarehousePort_t* whPort : whPorts)
            {
                delete whPort;
            }

            whPorts.clear();

            info->hide();
            delete info;
        }

        QRectF UiWarehouseItem_t::getRect()
        {
            auto t = QTransform().translate(mOrigin.x(), mOrigin.y()).rotate(mOrientation).translate(-mOrigin.x(), -mOrigin.y());
            auto p = t.mapToPolygon(mRect.toRect());
            auto r = p.boundingRect();
            auto o = this->getO();
            this->setO(0);
            r.setTopLeft(QPoint(this->scenePos().x() + r.topLeft().x(), this->scenePos().y() + r.topLeft().y()));
            r.setBottomRight(QPoint(this->scenePos().x() + r.bottomRight().x(), this->scenePos().y() + r.bottomRight().y()));
            this->setO(o);

            return r;
        }

        WarehouseItemType_t UiWarehouseItem_t::getWhItemType() const
        {
            return whItemType;
        }

        UiWarehouseItem_t::UiWarehousePortContainer_t UiWarehouseItem_t::getWhPorts() const
        {
            return whPorts;
        }

        void UiWarehouseItem_t::eraseFromLayout()
        {
            UiWarehouseLayout_t::getWhLayout().eraseWhItem(this);
        }

        void UiWarehouseItem_t::removeWhItem()
        {
            eraseFromLayout();
            qApp->processEvents();
            //scene->removeItem(this);
            delete this;
        }

        void UiWarehouseItem_t::mousePressEvent(QGraphicsSceneMouseEvent *event)
        {
            info->hide();

            if (event->button() == Qt::LeftButton)
            {
                if(UiCursor_t::getCursor().getMode() == UiCursorMode_t::E_MODE_DELETE)
                {
                    removeWhItem();
                    return;
                }
                else if(UiCursor_t::getCursor().isItemSelected())
                {
                    ui->resetCursor();
                }
            }

            BaseGraphicItem_t::mousePressEvent(event);
        }

        void UiWarehouseItem_t::disconnect()
        {
            for (UiWarehousePort_t* whPort : whPorts)
            {
                if(whPort->isConnected())
                {
                    UiWarehouseConnection_t* whConn = whPort->getWhConn();

                    delete whConn;
                }
            }
        }

        void UiWarehouseItem_t::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
        {
            info->setPos(mRect.topLeft().x(), mRect.topLeft().y());
            info->setFont(QFont("Helvetica", (getW() + getH())/30));
            info->show();
            infoTimeout.start(1000);

            QGraphicsItem::hoverEnterEvent(event);
        }

        void UiWarehouseItem_t::setItemHeat(double h)
        {
            double ratio = 2 * double(h - 0.0) / double(1.0 - h);

            int32_t b = std::max(0.0, 255 * (1 - ratio));
            int32_t r = std::max(0.0, 255 * (ratio - 1));
            int32_t g = 255 - b - r;

            auto boundingRect = getRect();
            boundingRect.setTopLeft(QPointF(boundingRect.topLeft().x() - 10, boundingRect.topLeft().y() - 10));
            boundingRect.setBottomRight(QPointF(boundingRect.bottomRight().x() + 10, boundingRect.bottomRight().y() + 10));

            heatRect->setRect(boundingRect);
            heatRect->setBrush(QColor::fromRgb(r, g, b));
        }

        void UiWarehouseItem_t::dump() const
        {
            std::cout << "==============================================" << std::endl;
            std::cout << "  [UI] Warehouse item ID <" << whItemID << "> Type <" << to_underlying(whItemType) << ">" << std::endl;
            std::cout << "==============================================" << std::endl;

            std::cout << "x: " << getX() << " y: " << getY() << " w: " << getW() << " h: " << getH() << " o: " << getO() << std::endl;

            std::for_each(whPorts.begin(), whPorts.end(),
                          [](UiWarehousePort_t* p)
                          {
                              p->dump();
                          });
        }
    }
}