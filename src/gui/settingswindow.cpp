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


#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <assert.h>
#include <klocalizedstring.h>
#include <QFileDialog>

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    setFixedSize(400, 300);

    restoreSettings();

    connect(ui->bgToolButton, SIGNAL(clicked()), this, SLOT(bgToolButtonClicked()));
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::bgToolButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
         i18n("Select Background"), "", i18n("Image Files (*.png *.jpg)"));

    if (fileName.isNull()) {
        return;
    }

    ui->bgLineEdit->setText(fileName);
}

void SettingsWindow::restoreSettings() {
    ui->heightSpinBox->setValue(m_settings.height());
    ui->widthSpinBox->setValue(m_settings.width());
    ui->densitySlider->setValue(m_settings.boxDensity() * 100.0);
    ui->preventMistakesCheckBox->setChecked(m_settings.preventMistakes());
    ui->bgCustomRadioButton->setChecked(m_settings.customBgEnabled());
    ui->bgLineEdit->setText(m_settings.customBgPath());
}

void SettingsWindow::saveSettings() {
    m_settings.setHeight(ui->heightSpinBox->value());
    m_settings.setWidth(ui->widthSpinBox->value());
    m_settings.setBoxDensity(ui->densitySlider->value() / 100.0);
    m_settings.setPreventMistakes(ui->preventMistakesCheckBox->isChecked());
    m_settings.setCustomBgEnabled(ui->bgCustomRadioButton->isChecked());
    m_settings.setCustomBgPath(ui->bgLineEdit->text());

    std::shared_ptr<QSettings> qsettings = m_settings.qSettings();
    qsettings->sync();
}

void SettingsWindow::accept() {
    saveSettings();
    QDialog::accept();
}
