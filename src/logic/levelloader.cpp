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

#include "levelloader.h"

#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QSettings>

#include "src/systemexception.h"
#include "config.h"

Level::Level() : m_solved(false), m_solved_time(0) { }

QString Level::key() const {
    return QString("preset_scores/%1_%2").arg(m_levelset, m_name);
}

void Level::writeSettings(int seconds) {
    QSettings settings;
    QString k = key();

    settings.setValue(k, seconds);
    settings.sync();
}

void Level::readSettings() {
    QSettings settings;
    QString k = key();

    if (settings.contains(k)) {
        m_solved = true;
        m_solved_time = settings.value(k).toInt();
    }
}

void Level::setSolved(int seconds) {
    if (m_solved_time > 0 && seconds >= m_solved_time) {
        return;
    }
    m_solved = true;
    m_solved_time = seconds;
    writeSettings(seconds);
}

QList<std::shared_ptr<Level> > LevelLoader::load() {
    QList<QString> paths;
    paths << QString("levels/") << QString(FILEPATH "levels/");

    QList<std::shared_ptr<Level> > list;

    for (int i = 0; i < paths.size(); i++) {
        QDir dir(paths[i]);
        if (!dir.exists()) {
            continue;
        }

        QStringList files = dir.entryList(QStringList("*.xml"));

        for (int j = 0; j < files.size(); j++) {
            LevelLoader loader(paths[i] + files[j]);
            list.append(loader.loadLevels());
        }

        break;
    }

    return list;
}

LevelLoader::LevelLoader(const QString &filename)
{
    setLevelset(filename);
}

void LevelLoader::setLevelset(const QString& filename)
{
    m_levelset.reset(new QDomDocument());

    QFile file(filename);
    if (!file.open( QIODevice::ReadOnly)) {
        throw SystemException(QString("Can't open file %1").arg(filename));
    }

    QString errorString;
    int errorLine;
    int errorColumn;
    bool success = m_levelset->setContent( &file, false, &errorString, &errorLine, &errorColumn);

    file.close();
    if (!success) {
        throw SystemException(QString("Can't read levelset from %1 \nError: %2 in Line %3, Column %4")
                              .arg(filename, errorString).arg(errorLine).arg(errorColumn));
    }
}

QList<std::shared_ptr<Level> > LevelLoader::loadLevels() {
    QDomElement levels = m_levelset->documentElement();

    if (!levels.hasAttribute("name")) {
        throw SystemException("No levelset name specified");
    }
    m_levelsetname = levels.attribute("name");

    QList<std::shared_ptr<Level> > l;
    QDomNodeList childNodes = levels.childNodes();
    for (int i = 0; i < childNodes.size(); i++) {
        l.append(loadLevel(childNodes.at(i).toElement()));
    }
    return l;
}

std::shared_ptr<Level> LevelLoader::loadLevel(const QDomElement &node) const {
    if (node.isNull() || node.tagName() != "board") {
        throw SystemException("Unexpected level node");
    }

    if (!node.hasAttribute("name") || !node.hasAttribute("author") || !node.hasAttribute("rows")
            || !node.hasAttribute("columns") || !node.hasAttribute("difficulty")) {
        throw SystemException("Level node missing attribute.");
    }

    std::shared_ptr<Level> p(new Level);
    p->m_name = node.attribute("name");
    p->m_author = node.attribute("author");
    p->m_levelset = m_levelsetname;
    p->m_difficulty = node.attribute("difficulty").toInt();
    p->m_width = node.attribute("columns").toInt();
    p->m_height = node.attribute("rows").toInt();

    QDomNodeList childNodes = node.childNodes();
    int i;
    for (i = 0; i < childNodes.size(); i++) {
        QList<Board::State> l = loadRow(childNodes.at(i).toElement());
        if (l.size() != p->width()) {
            throw SystemException("Invalid column count");
        }
        p->m_map.append(l);
    }

    if (i != p->height()) {
        throw SystemException("Invalid row count");
    }

    p->readSettings();

    return p;
}

static Board::State charToState(const QChar &c) {
    switch (c.toAscii()) {
    case '-': return Board::Nothing;
    case '1': return Board::Box;
    default: throw SystemException("Invalid char in level definition");
    }
}

QList<Board::State> LevelLoader::loadRow(const QDomElement &node) const {
    if (node.isNull() || node.tagName() != "row") {
        throw SystemException("Unexpected row node");
    }

    QString text = node.text();
    QList<Board::State> list;
    foreach (const QChar &c, text) {
        Board::State s = charToState(c);
        list.append(s);
    }

    return list;
}
