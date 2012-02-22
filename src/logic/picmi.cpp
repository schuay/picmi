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


#include "picmi.h"

#include <assert.h>

class IOHandler
{
public:
    IOHandler(BoardMap *map, BoardState *state, ElapsedTime *timer) : m_map(map), m_state(state), m_timer(timer) { }
    virtual ~IOHandler() { }

    void set(int x, int y, Board::State state);

protected:
    virtual void setCross(int x, int y);
    virtual void setBox(int x, int y) = 0;

    BoardMap *m_map;
    BoardState *m_state;
    ElapsedTime *m_timer;
};

void IOHandler::set(int x, int y, Board::State state) {
    switch (state) {
    case Board::Cross: setCross(x, y); break;
    case Board::Box: setBox(x, y); break;
    default: assert(0);
    }
}

void IOHandler::setCross(int x, int y) {
    switch (m_state->get(x, y)) {
    case Board::Cross: m_state->set(x, y, Board::Nothing); break;
    case Board::Box: break;
    case Board::Nothing: m_state->set(x, y, Board::Cross); break;
    default: assert(0);
    }
}

class IOHandlerNoHints : public IOHandler
{
public:
    IOHandlerNoHints(BoardMap *map, BoardState *state, ElapsedTime *timer) : IOHandler(map, state, timer) { }

protected:
    void setBox(int x, int y);
};


void IOHandlerNoHints::setBox(int x, int y) {
    switch (m_state->get(x, y)) {
    case Board::Cross: break;
    case Board::Box: m_state->set(x, y, Board::Nothing); break;
    case Board::Nothing: m_state->set(x, y, Board::Box); break;
    default: assert(0);
    }
}

class IOHandlerHints : public IOHandler
{
public:
    IOHandlerHints(BoardMap *map, BoardState *state, ElapsedTime *timer) : IOHandler(map, state, timer) { }

protected:
    void setBox(int x, int y);
};


void IOHandlerHints::setBox(int x, int y) {
    switch (m_state->get(x, y)) {
    case Board::Cross: break;
    case Board::Box: m_state->set(x, y, Board::Nothing); break;
    case Board::Nothing:
        if (m_map->get(x, y) == Board::Box) {
            m_state->set(x, y, Board::Box);
        } else {
            m_state->set(x, y, Board::Cross);
            m_timer->addPenaltyTime();
        }
        break;
    default: assert(0);
    }
}

Picmi::Picmi(std::shared_ptr<Settings> settings)
{
    m_settings = settings;
    m_map.reset(new BoardMap(settings->width(), settings->height(),
                             settings->boxDensity()));
    m_state.reset(new BoardState(settings->width(), settings->height()));

    if (settings->noHintsMode()) {
        m_io_handler.reset(new IOHandlerNoHints(m_map.get(), m_state.get(), &m_timer));
    } else {
        m_io_handler.reset(new IOHandlerHints(m_map.get(), m_state.get(), &m_timer));
    }

    m_timer.start();
}

bool Picmi::won() const {
    /* detect a win by comparing streaks.
       the reason we don't use the raw map and state is because we can't guarantee
       that our generated puzzles have exactly one solution, but we can work around
       this by ending the game once all streaks are marked solved. */

    for (int x = 0; x < width(); x++) {
        std::vector<std::shared_ptr<struct StreakElement> > streak = getColStreak(x);
        for (int i = 0; i < (int)streak.size(); i++) {
            if (!streak[i]->solved) {
                return false;
            }
        }
    }

    for (int y = 0; y < height(); y++) {
        std::vector<std::shared_ptr<struct StreakElement> > streak = getRowStreak(y);
        for (int i = 0; i < (int)streak.size(); i++) {
            if (!streak[i]->solved) {
                return false;
            }
        }
    }

    return true;
}

void Picmi::undo() {
    m_state->undo();
}

KScoreDialog::FieldInfo Picmi::endGame() {
    m_timer.stop();

    KScoreDialog::FieldInfo score;
    score[KScoreDialog::Score].setNum(m_timer.elapsedSecs());
    score[KScoreDialog::Time] = Time(m_timer.elapsedSecs()).toString();
    score[KScoreDialog::Date] = m_timer.startDate().toString();

    return score;
}

int Picmi::height() const {
    return m_settings->height();
}

int Picmi::width() const {
    return m_settings->width();
}

bool Picmi::outOfBounds(int x, int y) const {
    return m_map->outOfBounds(x, y);
}

void Picmi::setPaused(bool paused) {
    m_timer.pause(paused);
}

int Picmi::elapsedSecs() const {
    return m_timer.elapsedSecs();
}

std::vector<std::shared_ptr<Picmi::StreakElement> > Picmi::newStreak(const std::vector<int> &map) const {
    std::vector<std::shared_ptr<Picmi::StreakElement> > streak;
    for (int i = 0; i < (int)map.size(); i++) {
        std::shared_ptr<Picmi::StreakElement> element(new Picmi::StreakElement);
        element->value = map[i];
        element->solved = false;
        streak.push_back(element);
    }
    return streak;
}

std::vector<std::shared_ptr<Picmi::StreakElement> > Picmi::processStreak(
        const std::vector<int> &map, std::shared_ptr<BoardState::LineInfo> state) const {

    const bool line_complete = (state->box_count + state->cross_count == (int)state->line.size());
    std::vector<std::shared_ptr<Picmi::StreakElement> > streak = newStreak(map);

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

std::vector<std::shared_ptr<Picmi::StreakElement> > Picmi::getRowStreak(int y) const {
    std::vector<int> map_streak = m_map->getRowStreak(y);
    std::shared_ptr<BoardState::LineInfo> state_streak = m_state->getRowStreak(y);

    return processStreak(map_streak, state_streak);
}

std::vector<std::shared_ptr<Picmi::StreakElement> > Picmi::getColStreak(int x) const {
    std::vector<int> map_streak = m_map->getColStreak(x);
    std::shared_ptr<BoardState::LineInfo> state_streak = m_state->getColStreak(x);

    return processStreak(map_streak, state_streak);
}

void Picmi::setState(int x, int y, Board::State state) {
    m_io_handler->set(x, y, state);
}
