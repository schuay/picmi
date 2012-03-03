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


#include "boardmap.h"

#include <time.h>
#include <stdlib.h>
#include <vector>

static int box_count(const QList<Board::State> &data) {
    int count = 0;
    for (int i = 0; i < data.size(); i++) {
        if (data[i] != Board::Nothing) {
            count++;
        }
    }
    return count;
}

BoardMap::BoardMap(int width, int height, double box_ratio) :
    Board(width, height), m_box_count(width * height * box_ratio)
{
    genRandom();
    calcStreaks();
}

BoardMap::BoardMap(int width, int height, const QList<Board::State> &map) :
    Board(width, height), m_box_count(box_count(map))
{
    for (int i = 0; i < map.size(); i++) {
        m_state[i] = map[i];
    }
    calcStreaks();
}

void BoardMap::genRandom() {

    std::vector<int> indices;
    for (int i = 0; i < m_size; i++) {
        indices.push_back(i);
    }

    srand(time(NULL));
    for (int i = 0; i < m_box_count; i++) {
        int ind = rand() % indices.size();
        m_state[indices[ind]] = Box;
        indices.erase(indices.begin() + ind);
    }
}

bool BoardMap::isStreakFiller(enum State state) const {
    return (state == Nothing);
}

void BoardMap::calcStreaks() {
    for (int x = 0; x < m_width; x++) {
        std::vector<enum State> line = colToLine(x);
        m_col_streaks.push_back(lineToStreaks(line));
    }

    for (int y = 0; y < m_height; y++) {
        std::vector<enum State> line = rowToLine(y);
        m_row_streaks.push_back(lineToStreaks(line));
    }
}

std::vector<int> BoardMap::getRowStreak(int y) const {
    assertInbounds(0, y);
    return m_row_streaks[y];
}
std::vector<int> BoardMap::getColStreak(int x) const {
    assertInbounds(x, 0);
    return m_col_streaks[x];
}
