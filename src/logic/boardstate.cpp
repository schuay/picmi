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


#include "boardstate.h"

#include <algorithm>
#include <assert.h>

BoardState::BoardState(int width, int height) : Board(width, height), m_box_count(0)
{
}

void BoardState::set(int x, int y, State state) {
    assertInbounds(x, y);

    const int i = xy_to_i(x, y);

    if (m_state[i] == state) {
        return;
    }

    UndoAction undo;
    undo.x = x;
    undo.y = y;
    undo.state = m_state[i];
    m_undo_queue.push(undo);

    updateBoxCount(m_state[i], state);
    m_state[i] = state;
}

void BoardState::updateBoxCount(Board::State prev, Board::State next) {
    if (prev == Board::Box && next != Board::Box) {
        m_box_count--;
    } else if (prev != Board::Box && next == Board::Box) {
        m_box_count++;
    }
}

QPoint BoardState::undo() {
    if (m_undo_queue.empty()) {
        return QPoint();
    }

    UndoAction undo = m_undo_queue.pop();

    const int i = xy_to_i(undo.x, undo.y);

    updateBoxCount(m_state[i], undo.state);
    m_state[i] = undo.state;

    /* if we undo past a saved state, remove it */

    if (!m_saved_states.isEmpty()
            && m_undo_queue.size() < m_saved_states.top()) {
        (void)m_saved_states.pop();
    }

    return QPoint(undo.x, undo.y);
}

void BoardState::saveState() {

    /* a saved state consists only of the size of
       the undo queue to rewind to to restore a specific state */

    int size = m_undo_queue.size();
    if (!m_saved_states.isEmpty() && m_saved_states.top() == size) {
        return;
    }
    m_saved_states.push(size);
}

void BoardState::loadState() {

    if (m_saved_states.isEmpty()) {
        return;
    }

    /* rewind the undo queue until its size equals
       the saved queue size */

    int size = m_saved_states.pop();
    assert(size <= m_undo_queue.size());

    while (size < m_undo_queue.size()) {
        (void)undo();
    }
}

int BoardState::currentStateAge() const {
    if (m_saved_states.isEmpty()) {
        return m_undo_queue.size();
    }
    return m_undo_queue.size() - m_saved_states.top();
}
