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

#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <QSettings>
#include <QDateTime>
#include <QVector>
#include <QList>
#include <boost/shared_ptr.hpp>

#include "settings.h"

class HighScore
{
public:
    /* settings must be in array processing mode */
    HighScore(boost::shared_ptr<QSettings> settings, int index);
    HighScore(const Settings &settings, QDateTime datetime, int played_seconds);

    QDateTime datetime() const { return m_datetime; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    int playedSeconds() const { return m_played_seconds; }
    QTime played() const { return QTime(0, 0, 0).addSecs(m_played_seconds); }
    double boxDensity() const { return m_box_density; }
    bool noHintsMode() const { return m_no_hints_mode; }

    void save(boost::shared_ptr<QSettings> settings) const;

private:
    void initKeys();

    enum HighScoreType {
        DateTime = 0,
        Width,
        Height,
        PlayedSeconds,
        BoxDensity,
        NoHintsMode
    };

    QDateTime m_datetime;
    int m_width, m_height, m_played_seconds;
    double m_box_density;
    bool m_no_hints_mode;

    QVector<QString> m_keys;
};

class HighScores
{
public:
    HighScores();

    void add(boost::shared_ptr<HighScore> score);
    QList<boost::shared_ptr<HighScore> > scoresInCategory(boost::shared_ptr<HighScore> score) const;

private:
    QVector<boost::shared_ptr<HighScore> > m_scores;
    boost::shared_ptr<QSettings> m_settings;
    int m_size;
    const QString m_prefix;
};

#endif // HIGHSCORES_H
