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


#include "elapsedtime.h"

QString Time::toString(QString format) const {
    const int hours = m_seconds / m_secs_per_hour;
    int i = m_seconds % m_secs_per_hour;
    const int minutes = i / m_secs_per_minute;
    const int seconds = i % m_secs_per_minute;

    return format.arg(hours).arg(QString::number(minutes), 2, QChar('0'))
            .arg(QString::number(seconds), 2, QChar('0'));
}

ElapsedTime::ElapsedTime() : m_elapsed(0), m_paused(false), m_stopped(false),
    m_next_penalty(10 * 1000), m_penalty_multiplier(2)
{
}

void ElapsedTime::addPenaltyTime() {
    m_elapsed += m_next_penalty;
    m_next_penalty *= m_penalty_multiplier;
}

void ElapsedTime::start() {
    if (m_stopped) {
        return;
    }
    m_start = QDateTime::currentDateTime();
    m_timer.start();
}

void ElapsedTime::pause(bool paused) {
    if (paused == m_paused || m_stopped) {
        return;
    }
    m_paused = paused;
    if (paused) {
        m_elapsed += m_timer.elapsed();
    } else {
        m_timer.restart();
    }
}

void ElapsedTime::stop() {
    m_elapsed += m_timer.elapsed();
    m_stopped = true;
}

QTime ElapsedTime::elapsed() const {
    QTime time(0, 0, 0, 0);
    return time.addMSecs(elapsedMSecs());
}

int ElapsedTime::elapsedMSecs() const {
    int elapsed = m_elapsed;
    if (!m_paused && !m_stopped) {
        elapsed += m_timer.elapsed();
    }
    return elapsed;
}

QDateTime ElapsedTime::starttime() const {
    return m_start;
}
