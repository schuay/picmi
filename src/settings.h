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


#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVector>
#include <memory>

class Settings
{
public:
    Settings();

    enum GameSize {
        Small,
        Medium,
        Large,
        Custom
    };

    int width() const;
    int height() const;
    double boxDensity() const;
    bool noHintsMode() const;
    enum GameSize size() const;
    QString sizeString() const;

    void setWidth(int width);
    void setHeight(int height);
    void setBoxDensity(double box_density);
    void setNoHintsMode(bool no_hints_mode);
    void setSize(enum GameSize size);

    std::shared_ptr<QSettings> qSettings();

private:
    void restore();

    enum SettingsType {
        Width = 0,
        Height,
        BoxDensity,
        NoHintsMode,
        Size
    };

    QVector<QString> m_keys;

    int m_width, m_height;
    double m_box_density;
    bool m_no_hints_mode;
    Settings::GameSize m_size;

    std::shared_ptr<QSettings> m_qsettings;
};

#endif // SETTINGS_H
