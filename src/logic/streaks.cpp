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

Streaks::Streaks(std::shared_ptr<BoardMap> map, std::shared_ptr<BoardState> state)
    : m_map(map), m_state(state)
{
    calcMapStreaks();
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

std::vector<Board::State> Streaks::colToLine(
        const std::shared_ptr<Board> &board, int x) {
    std::vector<Board::State> line;
    for (int y = 0; y < board->height(); y++) {
        line.push_back(board->get(x, y));
    }
    return line;
}

std::vector<Board::State> Streaks::rowToLine(
        const std::shared_ptr<Board> &board, int y) {
    std::vector<Board::State> line;
    for (int x = 0; x < board->width(); x++) {
        line.push_back(board->get(x, y));
    }
    return line;
}

std::vector<int> Streaks::lineToStreaks(
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
