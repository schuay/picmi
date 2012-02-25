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
    /* creates the item with field coordinates (x,y) and the specified
      game and scene */
    CellItem(int x, int y, std::shared_ptr<Picmi> game, Scene *scene, QGraphicsItem *parent = 0);

    /* updates displayed pixmap according to current cell state */
    void refresh();

    void reload(const QSize &size);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:

    int drag_offset(int pos) const;

private:

    const std::shared_ptr<Picmi> m_game;
    Scene *m_scene;
    std::shared_ptr<DragManager> m_dragmanager;
    Qt::MouseButton m_dragbutton;
    QPixmap m_cross;
    QPixmap m_box;
    QPixmap m_transparent;
};

#endif // CELLITEM_H
