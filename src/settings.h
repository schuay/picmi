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


#ifndef SETTINGS_H
#define SETTINGS_H

#include "config.h"
#include <QSettings>
#include <QVector>
#include <QSharedPointer>

#ifdef HAVE_KGDIFFICULTY
#include <kgdifficulty.h>
#else
#include <kgamedifficulty.h>
#endif

class Settings
{
public:
    int width() const;
    int height() const;
    double boxDensity() const;
    bool preventMistakes() const;
#ifdef HAVE_KGDIFFICULTY
    KgDifficultyLevel::StandardLevel level() const;
#else
    KGameDifficulty::standardLevel level() const;
#endif
    bool customBgEnabled() const;
    QString customBgPath() const;
    QString fontColorSolved() const;
    QString fontColorUnsolved() const;

    void setWidth(int width);
    void setHeight(int height);
    void setBoxDensity(double box_density);
    void setPreventMistakes(bool prevent_mistakes);
#ifdef HAVE_KGDIFFICULTY
    void setLevel(KgDifficultyLevel::StandardLevel level);
#else
    void setLevel(KGameDifficulty::standardLevel level);
#endif
    void setCustomBgEnabled(bool enabled);
    void setCustomBgPath(const QString &path);
    void setFontColorSolved(const QString &color);
    void setFontColorUnsolved(const QString & color);

    void sync();

    static Settings *instance();

private:
    Settings();
    Q_DISABLE_COPY(Settings)

    void restore();

    enum SettingsType {
        Width = 0,
        Height,
        BoxDensity,
        PreventMistakes,
        Level,
        CustomBgEnabled,
        CustomBgPath,
        FontColorSolved,
        FontColorUnsolved
    };

    QVector<QString> m_keys;

    int m_width, m_height;
    double m_box_density;
    bool m_prevent_mistakes;
#ifdef HAVE_KGDIFFICULTY
    KgDifficultyLevel::StandardLevel m_level;
#else
    KGameDifficulty::standardLevel m_level;
#endif
    bool m_custom_bg_enabled;
    QString m_custom_bg_path;
    QString m_font_color_unsolved, m_font_color_solved;

    QSharedPointer<QSettings> m_qsettings;
};

#endif // SETTINGS_H
