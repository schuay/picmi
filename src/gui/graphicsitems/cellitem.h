/* *************************************************************************
 *  Copyright 2012 Jakob Gruber <jakob.gruber@gmail.com>                   *
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


#ifndef CELLITEM_H
#define CELLITEM_H

#include <QGraphicsPixmapItem>

#include "src/gui/scene.h"
#include "src/logic/picmi.h"
#include "reloadableitem.h"

class Scene;
class DragManager;

class CellItem : public QGraphicsPixmapItem, public ReloadableItem
{
public:
    CellItem(int x, int y, std::shared_ptr<Picmi> game, QGraphicsItem *parent = 0);

    /* updates displayed pixmap according to current cell state */
    virtual void refresh();

    virtual void reload(const QSize &size);

protected:
    virtual int getTilesize() const = 0;
    virtual QPixmap getPixmap() const = 0;

    const std::shared_ptr<Picmi> m_game;
};

class GameCellItem : public CellItem
{
public:
    /* creates the item with field coordinates (x,y) and the specified
      game and scene */
    GameCellItem(int x, int y, std::shared_ptr<Picmi> game, Scene *scene, QGraphicsItem *parent = 0);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    virtual int getTilesize() const;
    virtual QPixmap getPixmap() const;

private:

    int drag_offset(int pos) const;

private:
    Scene *m_scene;
    std::shared_ptr<DragManager> m_dragmanager;
    Qt::MouseButton m_dragbutton;
};

#endif // CELLITEM_H
