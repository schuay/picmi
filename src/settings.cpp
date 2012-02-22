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


#include "settings.h"

Settings::Settings() {

    m_keys << "game/width"
           << "game/height"
           << "game/box_ratio"
           << "game/prevent_mistakes"
           << "game/level";

    m_qsettings.reset(new QSettings);
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

KGameDifficulty::standardLevel Settings::level() const {
    return m_level;
}

void Settings::setWidth(int width) {
    m_width = width;
    m_qsettings->setValue(m_keys[Width], width);
}

void Settings::setHeight(int height) {
    m_height = height;
    m_qsettings->setValue(m_keys[Height], height);
}

void Settings::setBoxDensity(double box_density) {
    m_box_density = box_density;
    m_qsettings->setValue(m_keys[BoxDensity], box_density);
}

void Settings::setPreventMistakes(bool prevent_mistakes) {
    m_prevent_mistakes = prevent_mistakes;
    m_qsettings->setValue(m_keys[PreventMistakes], prevent_mistakes);
}

void Settings::setLevel(KGameDifficulty::standardLevel level) {
    m_level = level;
    m_qsettings->setValue(m_keys[Level], level);
}

void Settings::restore() {
    m_width = m_qsettings->value(m_keys[Width], 15).toInt();
    m_height = m_qsettings->value(m_keys[Height], 10).toInt();
    m_box_density = m_qsettings->value(m_keys[BoxDensity], 0.55).toDouble();
    m_prevent_mistakes = m_qsettings->value(m_keys[PreventMistakes], false).toBool();
    m_level = (KGameDifficulty::standardLevel)m_qsettings->value(m_keys[Level],
        KGameDifficulty::Medium).toInt();
}

std::shared_ptr<QSettings> Settings::qSettings() {
    return m_qsettings;
}
