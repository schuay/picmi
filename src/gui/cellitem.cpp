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


#include "cellitem.h"

#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

#include "renderer.h"
#include "src/constants.h"

CellItem::CellItem(int x, int y, boost::shared_ptr<Picmi> game, Scene *scene, QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent), ReloadableItem(x, y), m_game(game), m_scene(scene)
{
    setZValue(ZVALUE_CELLITEM);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);

    reload(QSize());
}

void CellItem::refresh() {
    switch(m_game->stateAt(m_x, m_y)) {
    case Board::Nothing: setPixmap(m_transparent); break;
    case Board::Box: setPixmap(m_box); break;
    case Board::Cross: setPixmap(m_cross); break;
    default: assert(0);
    }
}

void CellItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int tilesize = Renderer::instance()->getTilesize();
    setPos(m_x * tilesize, m_y * tilesize);

    m_cross = Renderer::instance()->getPixmap(Renderer::Cross);
    m_box = Renderer::instance()->getPixmap(Renderer::Box);
    m_transparent = Renderer::instance()->getPixmap(Renderer::Transparent);

    refresh();
}

void CellItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (m_dragmanager) {
        /* a second button was clicked during a drag; ignored */
        return;
    }

    m_dragmanager.reset(new DragManager(m_game, m_scene, QPoint(m_x, m_y)));
    m_dragbutton = event->button();
    switch (event->button()) {
    case Qt::LeftButton:
        m_dragmanager->init(Board::Box);
        break;
    case Qt::RightButton:
        m_dragmanager->init(Board::Cross);
        break;
    default:
        break;
    }
}

void CellItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
    m_scene->hover(m_x, m_y);
}

int CellItem::drag_offset(int pos) const {
    const int tilesize = Renderer::instance()->getTilesize();
    int offset = pos / tilesize;
    if (pos < 0) {
        offset--;
    }
    return offset;
}

void CellItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    int dx = drag_offset(event->pos().x());
    int dy = drag_offset(event->pos().y());

    if (m_game->outOfBounds(m_x + dx, m_y + dy)) {
        return;
    }

    m_dragmanager->move(m_x + dx, m_y + dy);
}

void CellItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == m_dragbutton) {
        m_dragmanager.reset();
    }
}

void CellItem::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_H:
    case Qt::Key_Left: m_scene->move(-1, 0); break;
    case Qt::Key_L:
    case Qt::Key_Right: m_scene->move(1, 0); break;
    case Qt::Key_K:
    case Qt::Key_Up: m_scene->move(0, -1); break;
    case Qt::Key_J:
    case Qt::Key_Down: m_scene->move(0, 1); break;
    case Qt::Key_Y: m_scene->move(-1, -1); break;
    case Qt::Key_U: m_scene->move(1, -1); break;
    case Qt::Key_B: m_scene->move(-1, 1); break;
    case Qt::Key_N: m_scene->move(1, 1); break;
    case Qt::Key_Space: m_scene->press(m_x, m_y, Board::Box); break;
    case Qt::Key_X: m_scene->press(m_x, m_y, Board::Cross); break;
    default: break;
    }
}
