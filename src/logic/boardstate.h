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


#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include <QVector>
#include <QStack>

#include "board.h"

class BoardState : public Board
{
public:
    struct LineInfo {
        int box_count;
        int cross_count;
        std::vector<int> streaks_regular;
        std::vector<int> streaks_reversed;
        std::vector<State> line;
    };

    BoardState(int width, int height);

    /* 0 <= x < m_width; 0 <= y < m_height */
    void set(int x, int y, enum State state);

    void undo();

    boost::shared_ptr<LineInfo> getRowStreak(int y) const;
    boost::shared_ptr<LineInfo> getColStreak(int x) const;

private:
    bool isStreakFiller(enum State state) const;

    /* 0 <= x < m_width; 0 <= y < m_height */
    void calcStreaks(int x, int y);
    void calcStreaks();

    boost::shared_ptr<LineInfo> lineToLineInfo(const std::vector<enum State> &line) const;

private:

    struct UndoAction {
        int x, y;
        enum State state;
    };

    QStack<UndoAction> m_undo_queue;

    QVector<boost::shared_ptr<LineInfo> > m_row_infos;
    QVector<boost::shared_ptr<LineInfo> > m_col_infos;
};

#endif // BOARDSTATE_H
