/* *************************************************************************
 *  Copyright 2012 Jakob Gruber                                            *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */


#ifndef GRAPHICSITEM_H
#define GRAPHICSITEM_H

#include <QGraphicsItem>

#include "renderer.h"

class GraphicsItem : public QGraphicsPixmapItem
{
public:
    GraphicsItem(QGraphicsItem *parent = 0) : QGraphicsPixmapItem(parent) { }
    virtual void reload() = 0;
};

class BackgroundGraphicsItem : public GraphicsItem
{
public:
    BackgroundGraphicsItem(QGraphicsItem *parent = 0) : QGraphicsPixmapItem(parent) { }
    void reload();
};

class SimpleCellGraphicsItem : public GraphicsItem
{
public:
    SimpleCellGraphicsItem(int x, int y, Renderer::Resource resource, QGraphicsItem *parent = 0);
    void reload();
private:
    const Renderer::Resource m_resource;
    const int m_x, m_y;
};

#endif // GRAPHICSITEM_H
