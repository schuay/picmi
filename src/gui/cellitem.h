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


#ifndef CELLITEM_H
#define CELLITEM_H

#include <QGraphicsPixmapItem>
#include <boost/shared_ptr.hpp>

#include "scene.h"
#include "src/logic/picmi.h"
#include "dragmanager.h"
#include "reloadableitem.h"

class Scene;
class DragManager;

class CellItem : public QGraphicsPixmapItem, public ReloadableItem
{
public:
    CellItem(int x, int y, boost::shared_ptr<Picmi> game, Scene *scene, QGraphicsItem *parent = 0);

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

    const boost::shared_ptr<Picmi> m_game;
    Scene *m_scene;
    boost::shared_ptr<DragManager> m_dragmanager;
    Qt::MouseButton m_dragbutton;
    QPixmap m_cross;
    QPixmap m_box;
    QPixmap m_transparent;
};

#endif // CELLITEM_H
