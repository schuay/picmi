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


#include "dragmanager.h"

DragManager::DragManager(boost::shared_ptr<Picmi> game, Scene *scene, QPoint start) :
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
}
