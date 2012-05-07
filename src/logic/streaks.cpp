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

#include "streaks.h"

/* 0 <= x < m_width; 0 <= y < m_height; returns a row/col as a sequence of states */
static std::vector<Board::State> colToLine(const std::shared_ptr<Board> &board, int x);
static std::vector<Board::State> rowToLine(const std::shared_ptr<Board> &board, int y);

/* takes a sequence of states and returns streaks */
static std::vector<int> lineToStreaks(
        const std::vector<Board::State> &line, Board::State filler);

static std::vector<std::shared_ptr<Streaks::StreakElement> > newStreak(const std::vector<int> &map);
static std::vector<std::shared_ptr<Streaks::StreakElement> > processStreak(
        const std::vector<int> &map, std::shared_ptr<LineInfo> state);



struct LineInfo {
    LineInfo(const std::vector<Board::State> &l) {
        memset(this, 0, sizeof(struct LineInfo));

        line = l;
        streaks_regular = lineToStreaks(l, Board::Cross);

        std::vector<Board::State> line_reversed(l);
        std::reverse(line_reversed.begin(), line_reversed.end());
        streaks_reversed = lineToStreaks(line_reversed, Board::Cross);

        for (int i = 0; i < (int)l.size(); i++) {
            if (l[i] == Board::Box) {
                box_count++;
            } else if (l[i] == Board::Cross) {
                cross_count++;
            }
        }
    }

    int box_count;
    int cross_count;
    std::vector<int> streaks_regular;
    std::vector<int> streaks_reversed;
    std::vector<Board::State> line;
};


std::vector<Board::State> colToLine(
        const std::shared_ptr<Board> &board, int x) {
    std::vector<Board::State> line;
    for (int y = 0; y < board->height(); y++) {
        line.push_back(board->get(x, y));
    }
    return line;
}

std::vector<Board::State> rowToLine(
        const std::shared_ptr<Board> &board, int y) {
    std::vector<Board::State> line;
    for (int x = 0; x < board->width(); x++) {
        line.push_back(board->get(x, y));
    }
    return line;
}

std::vector<int> lineToStreaks(
        const std::vector<Board::State> &line, Board::State filler) {
    int len = 0;
    std::vector<int> streaks;

    for (unsigned int i = 0; i < line.size(); i++) {
        if (line[i] == Board::Box) {
            len++;
        } else if (line[i] == filler) {
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

std::vector<std::shared_ptr<Streaks::StreakElement> > processStreak(
        const std::vector<int> &map, std::shared_ptr<LineInfo> state) {

    const bool line_complete = (state->box_count + state->cross_count == (int)state->line.size());
    std::vector<std::shared_ptr<Streaks::StreakElement> > streak = newStreak(map);

    /* line is not completely filled, so state and state_reversed are disjoint. */
    if (!line_complete && (state->streaks_regular.size() + state->streaks_reversed.size() > map.size())) {
        return streak;
    }

    bool solved = (map.size() == state->streaks_regular.size());
    int upper_bound = qMin(map.size(), state->streaks_regular.size());
    for (int i = 0; i < upper_bound; i++) {
        if (map[i] != state->streaks_regular[i]) {
            solved = false;
            break;
        }
        streak[i]->solved = true;
    }

    /* if the line has been filled completely with either crosses or boxes, we already have enough information
       here */

    if (line_complete) {
        switch (solved) {
        case false: return newStreak(map);
        case true: return streak;
        }
    }

    upper_bound = qMin(map.size(), state->streaks_reversed.size());
    for (int i = 0; i < upper_bound; i++) {
        /* streak is "reversed" -> process state streak from front, map streak from back */
        int index = map.size() - 1 - i;
        if (map[index] != state->streaks_reversed[i]) {
            break;
        }
        streak[index]->solved = true;
    }

    bool fully_solved = true;
    int actual_box_count = 0;
    for (int i = 0; i < (int)streak.size(); i++) {
        if (!streak[i]->solved) {
            fully_solved = false;
            break;
        }
        actual_box_count += streak[i]->value;
    }
    if (fully_solved && actual_box_count != state->box_count) {
        return newStreak(map);
    }

    return streak;
}

std::vector<std::shared_ptr<Streaks::StreakElement> > newStreak(const std::vector<int> &map) {
    std::vector<std::shared_ptr<Streaks::StreakElement> > streak;
    for (int i = 0; i < (int)map.size(); i++) {
        std::shared_ptr<Streaks::StreakElement> element(new Streaks::StreakElement);
        element->value = map[i];
        element->solved = false;
        streak.push_back(element);
    }
    return streak;
}


Streaks::Streaks(std::shared_ptr<BoardMap> map, std::shared_ptr<BoardState> state)
    : m_map(map), m_state(state)
{
    calcMapStreaks();
    update();
}

void Streaks::calcMapStreaks() {
    for (int x = 0; x < m_map->width(); x++) {
        std::vector<Board::State> line = colToLine(m_map, x);
        m_map_col_streaks.push_back(lineToStreaks(line, Board::Nothing));
    }

    for (int y = 0; y < m_map->height(); y++) {
        std::vector<Board::State> line = rowToLine(m_map, y);
        m_map_row_streaks.push_back(lineToStreaks(line, Board::Nothing));
    }
}

void Streaks::update(int x, int y) {
    m_state_col_streaks[x] = std::shared_ptr<LineInfo>(new LineInfo(colToLine(m_state, x)));
    m_state_row_streaks[y] = std::shared_ptr<LineInfo>(new LineInfo(rowToLine(m_state, y)));
}

void Streaks::update() {
    m_state_col_streaks.clear();
    for (int x = 0; x < m_state->width(); x++) {
        std::shared_ptr<LineInfo> line(new LineInfo(colToLine(m_state, x)));
        m_state_col_streaks.push_back(line);
    }

    m_state_row_streaks.clear();
    for (int y = 0; y < m_state->height(); y++) {
        std::shared_ptr<LineInfo> line(new LineInfo(rowToLine(m_state, y)));
        m_state_row_streaks.push_back(line);
    }
}

std::vector<std::shared_ptr<Streaks::StreakElement> > Streaks::getRowStreak(int y) const {
    return processStreak(m_map_row_streaks[y], m_state_row_streaks[y]);
}

std::vector<std::shared_ptr<Streaks::StreakElement> > Streaks::getColStreak(int x) const {
    return processStreak(m_map_col_streaks[x], m_state_col_streaks[x]);
}
