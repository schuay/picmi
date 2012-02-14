#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <QSettings>
#include <QDateTime>
#include <QVector>
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

private:
    QVector<boost::shared_ptr<HighScore> > m_scores;
    boost::shared_ptr<QSettings> m_settings;
    int m_size;
    const QString m_prefix;
};

#endif // HIGHSCORES_H
