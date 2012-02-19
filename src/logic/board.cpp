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


#include "board.h"

Board::Board(int width, int height) : m_width(width), m_height(height), m_size(width * height)
{
    m_state.reserve(m_size);

    for (int i = 0; i < m_size; i++) {
        m_state[i] = Nothing;
    }
}

bool Board::outOfBounds(int x, int y) const {
    return (x < 0 || x >= m_width || y < 0 || y >= m_height);
}

void Board::assertInbounds(int x, int y) const {
    if (outOfBounds(x, y)) {
        throw OutOfBoundsException();
    }
}

Board::State Board::get(int x, int y) const {
    assertInbounds(x, y);
    return m_state[xy_to_i(x, y)];
}

int Board::xy_to_i(int x, int y) const {
    return y * m_width + x;
}

int Board::i_to_x(int i) const {
    return i % m_width;
}

int Board::i_to_y(int i) const {
    return i / m_width;
}

std::vector<Board::State> Board::colToLine(int x) const {
    std::vector<Board::State> line;
    for (int y = 0; y < m_height; y++) {
        line.push_back(get(x, y));
    }
    return line;
}

std::vector<Board::State> Board::rowToLine(int y) const {
    std::vector<Board::State> line;
    for (int x = 0; x < m_width; x++) {
        line.push_back(get(x, y));
    }
    return line;
}

std::vector<int> Board::lineToStreaks(const std::vector<Board::State> &line) const {

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
