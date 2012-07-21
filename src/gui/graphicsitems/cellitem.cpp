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


#include "cellitem.h"

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <assert.h>

#include "src/constants.h"
#include "src/gui/renderer.h"
#include "src/gui/scene.h"

class DragManager
{
public:
    DragManager(QSharedPointer<Picmi> game, Scene *scene, QPoint start);

    void init(Board::State state);
    void move(int x, int y);

private:
    QPoint normCoordinates(int x, int y);

private:

    enum DragDirection {
        Horizontal,
        Vertical,
        Undefined
    };

    const QSharedPointer<Picmi> m_game;
    const QPoint m_start;
    Scene *m_scene;
    Board::State m_before, m_after, m_request;
    DragManager::DragDirection m_direction;
    bool m_initialized;
};

DragManager::DragManager(QSharedPointer<Picmi> game, Scene *scene, QPoint start) :
    m_game(game), m_start(start), m_scene(scene), m_initialized(false)
{
    m_direction = Undefined;
}

void DragManager::init(Board::State state) {
    m_before = m_game->stateAt(m_start.x(), m_start.y());
    m_request = state;
    m_scene->press(m_start.x(), m_start.y(), state);
    m_after = m_game->stateAt(m_start.x(), m_start.y());
    m_initialized = true;
}

void DragManager::move(int x, int y) {
    QPoint normed = normCoordinates(x, y);
    Board::State current = m_game->stateAt(normed.x(), normed.y());
    if (current == m_before && current != m_after && m_initialized) {
        m_scene->press(normed.x(), normed.y(), m_request);
    } else {
        m_scene->hover(normed.x(), normed.y());
    }
}

QPoint DragManager::normCoordinates(int x, int y) {
    if (m_direction == Undefined) {
        int abs_dx = abs(m_start.x() - x);
        int abs_dy = abs(m_start.y() - y);

        if (abs_dx == 0 && abs_dy == 0) {
            return m_start;
        }

        if (abs_dx > abs_dy) {
            m_direction = Horizontal;
        } else {
            m_direction = Vertical;
        }
    }

    switch (m_direction) {
    case Horizontal: return QPoint(x, m_start.y());
    case Vertical: return QPoint(m_start.x(), y);
    default: assert(0);
    }

    return QPoint();
}

CellItem::CellItem(int x, int y, QSharedPointer<Picmi> game, QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent), ReloadableItem(x, y), m_game(game)
{
    setZValue(ZVALUE_CELLITEM);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
}

void CellItem::refresh() {
    setPixmap(getPixmap());
}

void CellItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int tilesize = getTilesize();
    setPos(m_x * tilesize, m_y * tilesize);

    refresh();
}

OverviewCellItem::OverviewCellItem(int x, int y, QSharedPointer<Picmi> game, QGraphicsItem *parent) :
    CellItem(x, y, game, parent)
{
    setEnabled(false);
    reload(QSize());
}

QPixmap OverviewCellItem::getPixmap() const {
    switch(m_game->stateAt(m_x, m_y)) {
    case Board::Nothing: return Renderer::instance()->getPixmap(Renderer::Transparent);
    case Board::Box: return Renderer::instance()->getPixmap(Renderer::OverviewBox);
    case Board::Cross: return Renderer::instance()->getPixmap(Renderer::OverviewCross);
    default: assert(0);
    }

    throw OutOfBoundsException();
}

int OverviewCellItem::getTilesize() const {
    return Renderer::instance()->getOverviewTilesize();
}

GameCellItem::GameCellItem(int x, int y, QSharedPointer<Picmi> game, Scene *scene, QGraphicsItem *parent) :
    CellItem(x, y, game, parent), m_scene(scene)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);

    reload(QSize());
}

QPixmap GameCellItem::getPixmap() const {
    switch(m_game->stateAt(m_x, m_y)) {
    case Board::Nothing: return Renderer::instance()->getPixmap(Renderer::Transparent);
    case Board::Box: return Renderer::instance()->getPixmap(Renderer::Box);
    case Board::Cross: return Renderer::instance()->getPixmap(Renderer::Cross);
    default: assert(0);
    }

    throw OutOfBoundsException();
}

int GameCellItem::getTilesize() const {
    return Renderer::instance()->getTilesize();
}

void GameCellItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (m_dragmanager) {
        /* a second button was clicked during a drag; ignored */
        return;
    }

    m_dragmanager = QSharedPointer<DragManager>(new DragManager(m_game, m_scene, QPoint(m_x, m_y)));
    m_dragbutton = event->button();
    switch (m_dragbutton) {
    case Qt::LeftButton: m_dragmanager->init(Board::Box); break;
    case Qt::RightButton: m_dragmanager->init(Board::Cross); break;
    default: break;    /* for example, middle mouse button */
    }
}

void GameCellItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
    m_scene->hover(m_x, m_y);
}

int GameCellItem::drag_offset(int pos) const {
    const int tilesize = Renderer::instance()->getTilesize();
    int offset = pos / tilesize;
    if (pos < 0) {
        offset--;
    }
    return offset;
}

void GameCellItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (!m_dragmanager) {
        return;
    }

    int dx = drag_offset(event->pos().x());
    int dy = drag_offset(event->pos().y());

    if (m_game->outOfBounds(m_x + dx, m_y + dy)) {
        return;
    }

    m_dragmanager->move(m_x + dx, m_y + dy);
}

void GameCellItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == m_dragbutton) {
        m_dragmanager.clear();
    }
}

void GameCellItem::keyPressEvent(QKeyEvent *event) {
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
