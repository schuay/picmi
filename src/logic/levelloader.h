/*
    Copyright 2011 Julian Helfferich <julian.helfferich@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <QList>
#include <QString>
#include <memory>
#include <src/logic/board.h>

class QDomDocument;
class QDomElement;
class LevelLoader;

class Level
{
    friend class LevelLoader;
public:
    Level();

    QString name() const { return m_name; }
    QString author() const { return m_author; }
    QString levelset() const { return m_levelset; }
    int difficulty() const { return m_difficulty; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    QList<Board::State> map() const { return m_map; }

    QString visibleName() const { if (solved()) return name(); return "???"; }
    bool solved() const { return m_solved; }
    int solvedTime() const { return m_solved_time; }
    void setSolved(int seconds);

private:
    void readSettings(); /* needs to be called by loader when done constructing */
    void writeSettings(int seconds);
    QString key() const;

    QString m_name, m_author, m_levelset;
    int m_difficulty;
    int m_width, m_height;
    QList<Board::State> m_map;
    bool m_solved;
    int m_solved_time;
};

class LevelLoader
{
public:
    LevelLoader(const QString &filename);

    QList<std::shared_ptr<Level> > loadLevels();
    static QList<std::shared_ptr<Level> > load();

private:
    void setLevelset(const QString& levelname);
    std::shared_ptr<Level> loadLevel(const QDomElement &node) const;
    QList<Board::State> loadRow(const QDomElement &node) const;

    std::shared_ptr<QDomDocument> m_levelset;
    QString m_levelsetname;
};

#endif // LEVELLOADER_H
