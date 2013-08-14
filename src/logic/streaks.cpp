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

QVector<Streaks::StreakPrivate>
Streaks::lineToStreaks(const QVector<Board::State> &line,
                       Board::State filler)
{
    StreakPrivate s;
    QVector<StreakPrivate> streaks;
    int state = S_FILLER;

    for (int i = 0; i < line.size(); i++) {
        const Board::State t = line[i];

        switch (state) {
        case S_FILLER:
            if (t == Board::Box) {
                s.begin = i;
                s.value = 0;
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
                s.value = s.end - s.begin + 1;
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
        s.value = s.end - s.begin + 1;
        streaks.append(s);
    }

    return streaks;
}

QVector<Streaks::Streak>
Streaks::processStreak(const QVector<StreakPrivate> &map,
                       const QVector<Board::State> &l)
{        
    QVector<Streaks::Streak> streak = newStreak(map);
    QVector<StreakPrivate *> assocs(map.size(), NULL);

    /* Create the state streaks. */

    QVector<StreakPrivate> streaks_regular = lineToStreaks(l, Board::Cross);

    QVector<Board::State> line_reversed(l);
    std::reverse(line_reversed.begin(), line_reversed.end());
    QVector<StreakPrivate> streaks_reversed = lineToStreaks(line_reversed, Board::Cross);

    /* Fix begin and end indices of reversed streaks. */

    for (int i = 0; i < streaks_reversed.size(); i++) {
        streaks_reversed[i].begin = l.size() - streaks_reversed[i].begin - 1;
        streaks_reversed[i].end   = l.size() - streaks_reversed[i].end - 1;
        qSwap(streaks_reversed[i].begin, streaks_reversed[i].end);
    }

    if (streaks_regular.size() > map.size() || streaks_reversed.size() > map.size()) {
        return streak;
    }

    /* The concept of this check is fairly simple, and consists of two passes:
     * 1. Compare and match the regular state streaks to map streaks.
     * 2. Compare and match the reverse state streaks to map streaks.
     * A streak is solved, iff it is matched with exactly one streak (reverse
     * or regular), or it is matched with two and their begin and end indices match.
     */

    for (int i = 0; i < streaks_regular.size(); i++) {
        streak[i].solved = (streak[i].value == streaks_regular[i].value);
        assocs[i] = &streaks_regular[i];
    }

    for (int i = 0; i < streaks_reversed.size(); i++) {
        const int ix = map.size() - i - 1;

        streak[ix].solved = (streak[ix].value == streaks_reversed[i].value);

        if (assocs[ix] != NULL) {
            const bool range_matches = (assocs[ix]->begin == streaks_reversed[i].begin &&
                                        assocs[ix]->end   == streaks_reversed[i].end);
            streak[ix].solved &= range_matches;
        }
    }

    return streak;
}

QVector<Streaks::Streak>
Streaks::newStreak(const QVector<StreakPrivate> &map)
{
    QVector<Streaks::Streak> streak;
    for (int i = 0; i < (int)map.size(); i++) {
        Streaks::Streak element;
        element.value = map[i].value;
        element.solved = false;
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
    m_state_col_streaks[x] = processStreak(m_map_col_streaks[x], colToLine(m_state, x));
    m_state_row_streaks[y] = processStreak(m_map_row_streaks[y], rowToLine(m_state, y));
}

void Streaks::update() {
    m_state_col_streaks.clear();
    for (int x = 0; x < m_state->width(); x++) {
        m_state_col_streaks.push_back(processStreak(m_map_col_streaks[x], colToLine(m_state, x)));
    }

    m_state_row_streaks.clear();
    for (int y = 0; y < m_state->height(); y++) {
        m_state_row_streaks.push_back(processStreak(m_map_row_streaks[y], rowToLine(m_state, y)));
    }
}

QVector<Streaks::Streak> Streaks::getRowStreak(int y) const {
    return m_state_row_streaks[y];
}

QVector<Streaks::Streak> Streaks::getColStreak(int x) const {
    return m_state_col_streaks[x];
}
