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


#include "settings.h"

Settings::Settings() {

    m_keys << "game/width"
           << "game/height"
           << "game/box_ratio"
           << "game/prevent_mistakes"
           << "game/level"
           << "game/custom_bg_enabled"
           << "game/custom_bg_path"
           << "game/font_color_solved"
           << "game/font_color_unsolved";

    m_qsettings = QSharedPointer<QSettings>(new QSettings);
    restore();
}

int Settings::height() const {
    return m_height;
}

int Settings::width() const {
    return m_width;
}

double Settings::boxDensity() const {
    return m_box_density;
}

bool Settings::preventMistakes() const {
    return m_prevent_mistakes;
}

#ifdef HAVE_KGDIFFICULTY
KgDifficultyLevel::StandardLevel Settings::level() const {
#else
KGameDifficulty::standardLevel Settings::level() const {
#endif
    return m_level;
}

bool Settings::customBgEnabled() const
{
    return m_custom_bg_enabled;
}

QString Settings::customBgPath() const
{
    return m_custom_bg_path;
}

QString Settings::fontColorSolved() const
{
    return m_font_color_solved;
}

QString Settings::fontColorUnsolved() const
{
    return m_font_color_unsolved;
}

void Settings::setWidth(int width) {
    m_width = width;
    setValue(Width, width);
}

void Settings::setHeight(int height) {
    m_height = height;
    setValue(Height, height);
}

void Settings::setBoxDensity(double box_density) {
    m_box_density = box_density;
    setValue(BoxDensity, box_density);
}

void Settings::setPreventMistakes(bool prevent_mistakes) {
    m_prevent_mistakes = prevent_mistakes;
    setValue(PreventMistakes, prevent_mistakes);
}

#ifdef HAVE_KGDIFFICULTY
void Settings::setLevel(KgDifficultyLevel::StandardLevel level) {
#else
void Settings::setLevel(KGameDifficulty::standardLevel level) {
#endif
    m_level = level;
    setValue(Level, level);
}

void Settings::setCustomBgEnabled(bool enabled)
{
    m_custom_bg_enabled = enabled;
    setValue(CustomBgEnabled, enabled);
}

void Settings::setCustomBgPath(const QString &path)
{
    m_custom_bg_path = path;
    setValue(CustomBgPath, path);
}

void Settings::setFontColorSolved(const QString &color)
{
    m_font_color_solved = color;
    setValue(FontColorSolved, color);
}

void Settings::setFontColorUnsolved(const QString &color)
{
    m_font_color_unsolved = color;
    setValue(FontColorUnsolved, color);
}

void Settings::restore() {
    m_width = m_qsettings->value(m_keys[Width], 15).toInt();
    m_height = m_qsettings->value(m_keys[Height], 10).toInt();
    m_box_density = m_qsettings->value(m_keys[BoxDensity], 0.55).toDouble();
    m_prevent_mistakes = m_qsettings->value(m_keys[PreventMistakes], false).toBool();
#ifdef HAVE_KGDIFFICULTY
    m_level = (KgDifficultyLevel::StandardLevel)m_qsettings->value(m_keys[Level],
        KgDifficultyLevel::Medium).toInt();
#else
    m_level = (KGameDifficulty::standardLevel)m_qsettings->value(m_keys[Level],
        KGameDifficulty::Medium).toInt();
#endif
    m_custom_bg_enabled = m_qsettings->value(m_keys[CustomBgEnabled], false).toBool();
    m_custom_bg_path = m_qsettings->value(m_keys[CustomBgPath], "").toString();
    m_font_color_solved = m_qsettings->value(m_keys[FontColorSolved], "#555555").toString();
    m_font_color_unsolved = m_qsettings->value(m_keys[FontColorUnsolved], "#000000").toString();
}

void Settings::setValue(SettingsType type, const QVariant &value)
{
    m_qsettings->setValue(m_keys[type], value);
    emit settingChanged(type);
}

void Settings::sync()
{
    m_qsettings->sync();
}

Settings *Settings::instance()
{
    static Settings settings;
    return &settings;
}
