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

#include <algorithm>

/* 0 <= x < m_width; 0 <= y < m_height; returns a row/col as a sequence of states */
static QVector<Board::State> colToLine(const QSharedPointer<Board> &board, int x);
static QVector<Board::State> rowToLine(const QSharedPointer<Board> &board, int y);

/* takes a sequence of states and returns streaks */
static QVector<Streak> lineToStreaks(
        const QVector<Board::State> &line, Board::State filler);

static QVector<QSharedPointer<Streaks::StreakElement> > newStreak(const QVector<Streak> &map);
static QVector<QSharedPointer<Streaks::StreakElement> > processStreak(
        const QVector<Streak> &map, QSharedPointer<LineInfo> state);


LineInfo::LineInfo(const QVector<Board::State> &l)
{
    streaks_regular = lineToStreaks(l, Board::Cross);

    QVector<Board::State> line_reversed(l);
    std::reverse(line_reversed.begin(), line_reversed.end());
    streaks_reversed = lineToStreaks(line_reversed, Board::Cross);

    /* Fix begin and end indices of reversed streaks. */

    for (int i = 0; i < streaks_reversed.size(); i++) {
        streaks_reversed[i].begin = l.size() - streaks_reversed[i].begin - 1;
        streaks_reversed[i].end   = l.size() - streaks_reversed[i].end - 1;
        qSwap(streaks_reversed[i].begin, streaks_reversed[i].end);
    }
}


static QVector<Board::State> colToLine(const QSharedPointer<Board> &board,
                                       int x)
{
    QVector<Board::State> line;
    for (int y = 0; y < board->height(); y++) {
        line.push_back(board->get(x, y));
    }
    return line;
}


static QVector<Board::State> rowToLine(const QSharedPointer<Board> &board,
                                int y)
{
    QVector<Board::State> line;
    for (int x = 0; x < board->width(); x++) {
        line.push_back(board->get(x, y));
    }
    return line;
}

/**
 * Streaks are generated using a simple state machine. Either we're in a filler
 * section (Nothing for map streaks, Cross for state streaks), or we're in a Box
 * streak. A final possibility is that we're done processing.
 */

enum {
    S_FILLER,
    S_STREAK,
    S_END
};

static QVector<Streak> lineToStreaks(const QVector<Board::State> &line,
                                  Board::State filler)
{
    Streak s;
    QVector<Streak> streaks;
    int state = S_FILLER;

    for (int i = 0; i < line.size(); i++) {
        const Board::State t = line[i];

        switch (state) {
        case S_FILLER:
            if (t == Board::Box) {
                s.begin = i;
                s.count = 0;
                state = S_STREAK;
            } else if (t == filler) {
                /* Nothing. */
            } else {
                state = S_END;
            }
            break;
        case S_STREAK:
            if (t == Board::Box) {
                /* Nothing. */
            } else {
                s.end = i - 1;
                s.count = s.end - s.begin + 1;
                streaks.append(s);
                state = (t == filler) ? S_FILLER : S_END;
            }
            break;
        case S_END:
        default:
            return streaks;
        }
    }

    if (state == S_STREAK) {
        s.end = line.size() - 1;
        s.count = s.end - s.begin + 1;
        streaks.append(s);
    }

    return streaks;
}

static QVector<QSharedPointer<Streaks::StreakElement> > processStreak(
        const QVector<Streak> &map, QSharedPointer<LineInfo> state)
{
    QVector<QSharedPointer<Streaks::StreakElement> > streak = newStreak(map);

    /* TODO */

    return streak;
}

static QVector<QSharedPointer<Streaks::StreakElement> > newStreak(const QVector<Streak> &map)
{
    QVector<QSharedPointer<Streaks::StreakElement> > streak;
    for (int i = 0; i < (int)map.size(); i++) {
        QSharedPointer<Streaks::StreakElement> element(new Streaks::StreakElement);
        element->value = map[i].count;
        element->solved = false;
        streak.push_back(element);
    }

    return streak;
}


Streaks::Streaks(QSharedPointer<BoardMap> map, QSharedPointer<BoardState> state)
    : m_map(map), m_state(state)
{
    calcMapStreaks();
    update();
}

void Streaks::calcMapStreaks() {
    for (int x = 0; x < m_map->width(); x++) {
        QVector<Board::State> line = colToLine(m_map, x);
        m_map_col_streaks.push_back(lineToStreaks(line, Board::Nothing));
    }

    for (int y = 0; y < m_map->height(); y++) {
        QVector<Board::State> line = rowToLine(m_map, y);
        m_map_row_streaks.push_back(lineToStreaks(line, Board::Nothing));
    }
}

void Streaks::update(int x, int y) {
    m_state_col_streaks[x] = QSharedPointer<LineInfo>(new LineInfo(colToLine(m_state, x)));
    m_state_row_streaks[y] = QSharedPointer<LineInfo>(new LineInfo(rowToLine(m_state, y)));
}

void Streaks::update() {
    m_state_col_streaks.clear();
    for (int x = 0; x < m_state->width(); x++) {
        QSharedPointer<LineInfo> line(new LineInfo(colToLine(m_state, x)));
        m_state_col_streaks.push_back(line);
    }

    m_state_row_streaks.clear();
    for (int y = 0; y < m_state->height(); y++) {
        QSharedPointer<LineInfo> line(new LineInfo(rowToLine(m_state, y)));
        m_state_row_streaks.push_back(line);
    }
}

QVector<QSharedPointer<Streaks::StreakElement> > Streaks::getRowStreak(int y) const {
    return processStreak(m_map_row_streaks[y], m_state_row_streaks[y]);
}

QVector<QSharedPointer<Streaks::StreakElement> > Streaks::getColStreak(int x) const {
    return processStreak(m_map_col_streaks[x], m_state_col_streaks[x]);
}
