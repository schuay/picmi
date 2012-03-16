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


#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include <QPoint>
#include <QVector>
#include <QStack>
#include <memory>

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

    /* initializes an empty field of given width and height,
      and creates initial (empty) streaks.
      width, height > 0 */
    BoardState(int width, int height);

    /* 0 <= x < m_width; 0 <= y < m_height
      sets point (x, y) to state */
    void set(int x, int y, enum State state);

    /* undo the last stored action and return the changed coordinate.
       if none are left, nothing occurs and QPoint() is returned*/
    QPoint undo();

    /* gets the specified row/column streak. these can be different after
      each player action.
      (x, y) is inbounds */
    std::shared_ptr<LineInfo> getRowStreak(int y) const;
    std::shared_ptr<LineInfo> getColStreak(int x) const;

    /* returns the count of player-set boxes */
    int boxCount() const { return m_box_count; }

private:
    bool isStreakFiller(enum State state) const;

    /* 0 <= x < m_width; 0 <= y < m_height */
    void calcStreaks(int x, int y);
    void calcStreaks();

    void calcBoxCount();

    std::shared_ptr<LineInfo> lineToLineInfo(const std::vector<enum State> &line) const;

private:

    struct UndoAction {
        int x, y;
        enum State state;
    };

    QStack<UndoAction> m_undo_queue;

    QVector<std::shared_ptr<LineInfo> > m_row_infos;
    QVector<std::shared_ptr<LineInfo> > m_col_infos;

    int m_box_count;
};

#endif // BOARDSTATE_H
