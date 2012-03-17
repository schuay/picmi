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
#include <QDebug>
#include <kglobal.h>
#include <kstandarddirs.h>

#include "src/systemexception.h"
#include "config.h"

class LevelList : public QList<std::shared_ptr<Level> >
{
public:
    LevelList() : QList<std::shared_ptr<Level> >() { }
    void append(const QList<std::shared_ptr<Level> > &t);
private:
    bool containsLevel(std::shared_ptr<Level> level) const;
};

void LevelList::append(const QList<std::shared_ptr<Level> > &t) {
    for (int i = 0; i < t.size(); i++) {
        std::shared_ptr<Level> level = t[i];
        if (!containsLevel(level)) {
            QList::append(level);
        }
    }
}

bool LevelList::containsLevel(std::shared_ptr<Level> level) const {
    for (int i = 0; i < size(); i++) {
        if (at(i)->name() == level->name() && at(i)->author() == level->author()) {
            return true;
        }
    }
    return false;
}

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
    const QString prefix = "levels/";
    QList<QString> paths;
    paths << QString(prefix)
          << QString(FILEPATH "/" + prefix)
          << KGlobal::dirs()->findResourceDir("appdata", prefix) + prefix;

    LevelList list;

    for (int i = 0; i < paths.size(); i++) {
        QDir dir(paths[i]);
        if (!dir.exists()) {
            continue;
        }

        QStringList files = dir.entryList(QStringList("*.xml"));

        for (int j = 0; j < files.size(); j++) {
            LevelLoader loader(dir.absoluteFilePath(files[j]));
            list.append(loader.loadLevels());
        }
    }

    return list;
}

LevelLoader::LevelLoader(const QString &filename) :
    m_filename(filename), m_valid(true)
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
        qDebug() << QString("Can't read levelset from %1 \nError: %2 in Line %3, Column %4")
                              .arg(filename, errorString).arg(errorLine).arg(errorColumn);
        m_valid = false;
    }
}

QList<std::shared_ptr<Level> > LevelLoader::loadLevels() {
    QList<std::shared_ptr<Level> > l;

    if (!m_valid) {
        return l;
    }

    QDomElement levels = m_levelset->documentElement();
    if (!levels.hasAttribute("name")) {
        qDebug() << "Loading level failed: no levelset name specified";
        return l;
    }
    m_levelsetname = levels.attribute("name");

    QDomNodeList childNodes = levels.childNodes();
    for (int i = 0; i < childNodes.size(); i++) {
        try {
            l.append(loadLevel(childNodes.at(i).toElement()));
        } catch (const SystemException &e) {
            qDebug() << "Loading level failed: " << e.what();
        }
    }
    return l;
}

std::shared_ptr<Level> LevelLoader::loadLevel(const QDomElement &node) const {
    if (node.isNull() || node.tagName() != "board") {
        throw SystemException("Unexpected level node");
    }

    if (!node.hasAttribute("name") || !node.hasAttribute("author")
            || !node.hasAttribute("difficulty")) {
        throw SystemException("Level node missing attribute.");
    }

    std::shared_ptr<Level> p(new Level);
    p->m_name = node.attribute("name");
    p->m_author = node.attribute("author");
    p->m_levelset = m_levelsetname;
    p->m_difficulty = node.attribute("difficulty").toInt();

    QDomNodeList childNodes = node.childNodes();

    if (childNodes.isEmpty()) {
        throw SystemException("Empty level definition.");
    }

    const QString tag_name = childNodes.at(0).toElement().tagName();
    if (tag_name == "row") {
        int i;
        QList<Board::State> l;
        for (i = 0; i < childNodes.size(); i++) {
            l = loadRow(childNodes.at(i).toElement());
            p->m_map.append(l);
        }
        p->m_width = l.size();
        p->m_height = i;
    } else if (tag_name == "xpm") {
        QImage xpm = openXPM(childNodes.at(0).toElement());
        p->m_map = loadXPM(xpm);
        p->m_width = xpm.width();
        p->m_height = xpm.height();
    }

    if (p->m_map.size() != p->height() * p->width()) {
        throw SystemException("Invalid board size");
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

QImage LevelLoader::openXPM(const QDomElement &node) const {
    if (node.isNull() || node.tagName() != "xpm") {
        throw SystemException("Unexpected row node");
    }

    QFileInfo file(m_filename);
    QString filepath = file.absolutePath() + "/" + node.text();

    QImage xpm(filepath);

    if (xpm.isNull()) {
        throw SystemException(QString("Could not load %1").arg(filepath));
    }

    return xpm;
}

QList<Board::State> LevelLoader::loadXPM(const QImage &xpm) const {
    QList<Board::State> list;
    for (int y = 0; y < xpm.height(); y++) {
        for (int x = 0; x < xpm.width(); x++) {
            QRgb pix = xpm.pixel(x, y);
            list.append((pix == 0) ? Board::Nothing : Board::Box);
        }
    }

    return list;
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
