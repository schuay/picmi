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


#include "boardstate.h"

#include <algorithm>

BoardState::BoardState(int width, int height) : Board(width, height)
{
    calcStreaks();
}

void BoardState::set(int x, int y, State state) {
    assertInbounds(x, y);

    const int i = xy_to_i(x, y);

    UndoAction undo;
    undo.x = x;
    undo.y = y;
    undo.state = m_state[i];
    m_undo_queue.push(undo);

    m_state[i] = state;
    calcStreaks(x, y);
}

void BoardState::undo() {
    if (m_undo_queue.empty()) {
        return;
    }

    UndoAction undo = m_undo_queue.pop();
    m_state[xy_to_i(undo.x, undo.y)] = undo.state;
    calcStreaks(undo.x, undo.y);
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

boost::shared_ptr<BoardState::LineInfo> BoardState::lineToLineInfo(const std::vector<enum State> &line) const {
    boost::shared_ptr<LineInfo> lineinfo(new LineInfo);
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

boost::shared_ptr<BoardState::LineInfo> BoardState::getRowStreak(int y) const {
    assertInbounds(0, y);
    return m_row_infos[y];
}

boost::shared_ptr<BoardState::LineInfo> BoardState::getColStreak(int x) const {
    assertInbounds(x, 0);
    return m_col_infos[x];
}
