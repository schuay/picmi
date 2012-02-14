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


#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    setFixedSize(400, 300);

    connect(ui->gameSizeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(gameSizeChanged(int)));

    restoreSettings();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::gameSizeChanged(int size) {
    switch (size) {
    case 0: /* small */
        setValues(10, 10, 0.55, true); enableControls(false); break;
    case 1: /* medium */
        setValues(10, 15, 0.55, true); enableControls(false); break;
    case 2: /* large */
        setValues(15, 15, 0.55, true); enableControls(false); break;
    case 3: /* custom */
         enableControls(true); break;
    default: assert(0);
    }
}

void SettingsWindow::enableControls(bool enabled) {
    ui->heightSpinBox->setEnabled(enabled);
    ui->widthSpinBox->setEnabled(enabled);
    ui->densitySlider->setEnabled(enabled);
    ui->noHintsModeCheckBox->setEnabled(enabled);
}

void SettingsWindow::setValues(int height, int width, double density, bool no_hints_mode) {
    ui->heightSpinBox->setValue(height);
    ui->widthSpinBox->setValue(width);
    ui->densitySlider->setValue(density * 100.0);
    ui->noHintsModeCheckBox->setChecked(no_hints_mode);
}

void SettingsWindow::restoreSettings() {
    setValues(m_settings.height(), m_settings.width(),
              m_settings.boxDensity() * 100.0, m_settings.noHintsMode());
    ui->gameSizeComboBox->setCurrentIndex(m_settings.size());
    gameSizeChanged(m_settings.size());
}

void SettingsWindow::saveSettings() {
    m_settings.setHeight(ui->heightSpinBox->value());
    m_settings.setWidth(ui->widthSpinBox->value());
    m_settings.setBoxDensity(ui->densitySlider->value() / 100.0);
    m_settings.setNoHintsMode(ui->noHintsModeCheckBox->isChecked());
    m_settings.setSize((Settings::GameSize)ui->gameSizeComboBox->currentIndex());

    boost::shared_ptr<QSettings> qsettings = m_settings.qSettings();
    qsettings->sync();
}

void SettingsWindow::accept() {
    saveSettings();
    QDialog::accept();
}
