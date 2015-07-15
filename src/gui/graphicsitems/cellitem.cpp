/* *************************************************************************
 *  Copyright 2015 Jakob Gruber <jakob.gruber@gmail.com>                   *
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
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
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
    QPoint m_prev_pos;
    Scene *m_scene;
    Board::State m_before, m_after, m_request;
    DragManager::DragDirection m_direction;
    bool m_initialized;
};

DragManager::DragManager(QSharedPointer<Picmi> game, Scene *scene, QPoint start) :
    m_game(game), m_start(start), m_prev_pos(start), m_scene(scene), m_initialized(false)
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
    const QPoint curr_pos = normCoordinates(x, y);
    if (curr_pos == m_prev_pos) {
        return;
    }

    /* When moving the mouse quickly, it can happen that subsequent calls to move()
     * skip a tile. Ensure that all tiles between m_prev_pos and curr_pos are processed. */

    QPoint step = curr_pos - m_prev_pos;
    assert(step.x() == 0 || step.y() == 0);
    step /= qMax(qAbs(step.x()), qAbs(step.y()));
    assert(qAbs(step.x()) == 1 || qAbs(step.y()) == 1);

    for (QPoint i = m_prev_pos + step; ; i += step) {
        const Board::State current = m_game->stateAt(i.x(), i.y());
        if (current == m_before && current != m_after && m_initialized) {
            m_scene->press(i.x(), i.y(), m_request);
        } else {
            m_scene->hover(i.x(), i.y());
        }

        if (i == curr_pos) {
            break;
        }
    }

    m_prev_pos = curr_pos;
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
    CellItem(x, y, game, parent), m_scene(scene), m_state(Board::Nothing)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);

    m_anim = createAnimation();

    reload(QSize());
}

QAbstractAnimation *GameCellItem::createAnimation() {
    QParallelAnimationGroup *anim_group = new QParallelAnimationGroup(this);

    QPropertyAnimation *anim = new QPropertyAnimation(this, "opacity");
    anim->setDuration(150);
    anim->setStartValue(0.1);
    anim->setEndValue(1.0);
    anim_group->addAnimation(anim);

    anim = new QPropertyAnimation(this, "scale");
    anim->setDuration(150);
    anim->setStartValue(0.3);
    anim->setEndValue(1.0);
    anim_group->addAnimation(anim);

    return anim_group;
}

void GameCellItem::refresh() {
    CellItem::refresh();

    /* Only start animation when the cell state has changed. */

    const Board::State curr_state = m_game->stateAt(m_x, m_y);
    if (curr_state == m_state) {
        return;
    }

    m_state = curr_state;
    m_anim->start();
}

void GameCellItem::reload(const QSize &size) {
    /* Reset the transformation origin point for our scaling animation. */
    const int tilesize = getTilesize();
    setTransformOriginPoint(tilesize / 2, tilesize / 2);

    CellItem::reload(size);

    /* Save the original scene position - setTransformOriginPoint() together
     * with scaling modify it. */
    m_sceneorigin = scenePos();
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

QPoint GameCellItem::sceneToGame(const QPointF &p) const {
    const QPointF pf = (p - m_sceneorigin) / Renderer::instance()->getTilesize();
    return QPoint(m_x + pf.x(), m_y + pf.y());
}

void GameCellItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (!m_dragmanager) {
        return;
    }

    const QPoint p = sceneToGame(event->scenePos());
    if (m_game->outOfBounds(p.x(), p.y())) {
        return;
    }

    m_dragmanager->move(p.x(), p.y());
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
