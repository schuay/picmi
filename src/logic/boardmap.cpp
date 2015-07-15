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


#include "boardmap.h"

#include <qglobal.h>
#include <QTime>
#include <QVector>

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
}

BoardMap::BoardMap(int width, int height, const QList<Board::State> &map) :
    Board(width, height), m_box_count(box_count(map))
{
    for (int i = 0; i < map.size(); i++) {
        m_state[i] = map[i];
    }
}

void BoardMap::genRandom() {
    /* To maintain a uniformly random selection of k elements:
     * element i enters the selection with probability k/i. */

    qsrand(QTime::currentTime().msec());

    QVector<int> indices(m_box_count);
    for (int i = 0; i < m_size; i++) {
        if (i < m_box_count) {
            indices[i] = i;
            continue;
        }

        if (qrand() % i <= m_box_count) {
            indices[qrand() % indices.size()] = i;
        }
    }

    for (int i = 0; i < indices.size(); i++) {
        m_state[indices[i]] = Box;
    }
}
