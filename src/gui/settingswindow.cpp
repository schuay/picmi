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
#include <kcolordialog.h>
#include <kfiledialog.h>
#include <klocalizedstring.h>

static inline QString toStylesheet(const QString &color)
{
    return QString("QLabel { color : %1 }").arg(color);
}

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    restoreSettings();

    connect(ui->bgToolButton, SIGNAL(clicked()), this, SLOT(bgToolButtonClicked()));
    connect(ui->fontColorSolvedPushButton, SIGNAL(clicked()), this, SLOT(selectSolvedColor()));
    connect(ui->fontColorUnsolvedPushButton, SIGNAL(clicked()), this, SLOT(selectSolvedColor()));
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::selectSolvedColor()
{
    QColor color(m_font_color_solved);
    if (KColorDialog::getColor(color, this) != QDialog::Accepted) {
        return;
    }
    m_font_color_solved = color.name();
    ui->fontColorSolvedLabel->setStyleSheet(toStylesheet(m_font_color_solved));
}

void SettingsWindow::selectUnsolvedColor()
{
    QColor color(m_font_color_unsolved);
    if (KColorDialog::getColor(color, this) != QDialog::Accepted) {
        return;
    }
    m_font_color_unsolved = color.name();
    ui->fontColorUnsolvedLabel->setStyleSheet(toStylesheet(m_font_color_unsolved));
}

void SettingsWindow::bgToolButtonClicked()
{
    QString fileName = KFileDialog::getOpenFileName(KUrl(),
                                                    i18n("*.png *.jpg|Image Files (*.png *.jpg)"),
                                                    this, i18n("Select Background"));

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
    m_font_color_solved = m_settings.fontColorSolved();
    m_font_color_unsolved = m_settings.fontColorUnsolved();
    ui->fontColorUnsolvedLabel->setStyleSheet(toStylesheet(m_font_color_unsolved));
    ui->fontColorSolvedLabel->setStyleSheet(toStylesheet(m_font_color_solved));
}

void SettingsWindow::saveSettings() {
    m_settings.setHeight(ui->heightSpinBox->value());
    m_settings.setWidth(ui->widthSpinBox->value());
    m_settings.setBoxDensity(ui->densitySlider->value() / 100.0);
    m_settings.setPreventMistakes(ui->preventMistakesCheckBox->isChecked());
    m_settings.setCustomBgEnabled(ui->bgCustomRadioButton->isChecked());
    m_settings.setCustomBgPath(ui->bgLineEdit->text());
    m_settings.setFontColorSolved(m_font_color_solved);
    m_settings.setFontColorUnsolved(m_font_color_unsolved);

    std::shared_ptr<QSettings> qsettings = m_settings.qSettings();
    qsettings->sync();
}

void SettingsWindow::accept() {
    saveSettings();
    QDialog::accept();
}
