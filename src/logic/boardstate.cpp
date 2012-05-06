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
    calcStreaks();
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
    calcStreaks(x, y);
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
    calcStreaks(undo.x, undo.y);

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

bool BoardState::isStreakFiller(enum State state) const {
    return (state == Cross);
}

void BoardState::calcStreaks(int x, int y) {
    assertInbounds(x, y);

    std::vector<enum State> line = colToLine(x);
    m_col_infos[x] = lineToLineInfo(line);

    line = rowToLine(y);
    m_row_infos[y] = lineToLineInfo(line);
}

void BoardState::calcStreaks() {
    m_col_infos.clear();
    for (int x = 0; x < m_width; x++) {
        std::vector<enum State> line = colToLine(x);
        m_col_infos.push_back(lineToLineInfo(line));
    }

    m_row_infos.clear();
    for (int y = 0; y < m_height; y++) {
        std::vector<enum State> line = rowToLine(y);
        m_row_infos.push_back(lineToLineInfo(line));
    }
}

std::shared_ptr<BoardState::LineInfo> BoardState::lineToLineInfo(const std::vector<enum State> &line) const {
    std::shared_ptr<LineInfo> lineinfo(new LineInfo);
    memset(lineinfo.get(), 0, sizeof(struct LineInfo));

    lineinfo->line = line;
    lineinfo->streaks_regular = lineToStreaks(line);

    std::vector<enum State> line_reversed(line);
    std::reverse(line_reversed.begin(), line_reversed.end());
    lineinfo->streaks_reversed = lineToStreaks(line_reversed);

    for (int i = 0; i < (int)line.size(); i++) {
        if (line[i] == Box) {
            lineinfo->box_count++;
        } else if (line[i] == Cross) {
            lineinfo->cross_count++;
        }
    }

    return lineinfo;
}

std::shared_ptr<BoardState::LineInfo> BoardState::getRowStreak(int y) const {
    assertInbounds(0, y);
    return m_row_infos[y];
}

std::shared_ptr<BoardState::LineInfo> BoardState::getColStreak(int x) const {
    assertInbounds(x, 0);
    return m_col_infos[x];
}

std::vector<Board::State> BoardState::colToLine(int x) const {
    std::vector<Board::State> line;
    for (int y = 0; y < m_height; y++) {
        line.push_back(get(x, y));
    }
    return line;
}

std::vector<Board::State> BoardState::rowToLine(int y) const {
    std::vector<Board::State> line;
    for (int x = 0; x < m_width; x++) {
        line.push_back(get(x, y));
    }
    return line;
}

std::vector<int> BoardState::lineToStreaks(const std::vector<Board::State> &line) const {

    int len = 0;
    std::vector<int> streaks;

    for (unsigned int i = 0; i < line.size(); i++) {
        if (line[i] == Box) {
            len++;
        } else if (isStreakFiller(line[i])) {
            if (len > 0) {
                streaks.push_back(len);
                len = 0;
            }
        } else {
            break;
        }
    }

    if (len > 0) {
        streaks.push_back(len);
        len = 0;
    }

    return streaks;
}
