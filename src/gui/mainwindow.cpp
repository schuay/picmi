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


#include "mainwindow.h"

#include <QHBoxLayout>
#include <QGraphicsSimpleTextItem>
#include <QMessageBox>
#include <QCoreApplication>
#include <klocalizedstring.h>
#include <kstandardgameaction.h>
#include <kactioncollection.h>
#include <kstatusbar.h>

#include "settingswindow.h"
#include "config.h"
#include "src/constants.h"
#include "src/logic/levelloader.h"
#include "selectboardwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    KXmlGuiWindow(parent), m_key_size("window/size"),
    m_key_pos("window/position"), m_statusbar_time_id(1),
    m_in_progress(false), m_mode(Random)
{
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName("picmi");

    m_timer.setInterval(500);

    setCentralWidget(&m_view);

    setupActions();
    restoreWindowState();

    startRandomGame();
}

void MainWindow::setupActions() {
    KStandardGameAction::gameNew(this, SLOT(startRandomGame()), actionCollection());
    KStandardGameAction::load(this, SLOT(loadBoard()), actionCollection());
    KStandardGameAction::highscores(this, SLOT(highscores()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());
    KStandardAction::preferences(this, SLOT(settings()), actionCollection());
    m_action_pause = KStandardGameAction::pause(this, SLOT(togglePaused(bool)), actionCollection());
    m_action_undo = KStandardGameAction::undo(this, SLOT(undo()), actionCollection());

    this->statusBar()->insertPermanentItem("", m_statusbar_time_id);

    Kg::difficulty()->addStandardLevel(KgDifficultyLevel::Easy);
    Kg::difficulty()->addStandardLevel(KgDifficultyLevel::Medium, true);
    Kg::difficulty()->addStandardLevel(KgDifficultyLevel::Hard);

    KgDifficultyLevel *configurable = new KgDifficultyLevel(90, QByteArray("Custom"), i18n("Custom"));
    Kg::difficulty()->addLevel(configurable);

    KgDifficultyGUI::init(this);
    connect(Kg::difficulty(), SIGNAL(currentLevelChanged(const KgDifficultyLevel*)), this,
            SLOT(levelChanged(const KgDifficultyLevel*)));

    setupGUI();
}

void MainWindow::loadBoard() {
    SelectBoardWindow w(this);
    if (w.exec() == QDialog::Accepted) {
        startPresetGame(w.selectedBoard());
    }
}

void MainWindow::levelChanged(const KgDifficultyLevel* level) {
    Settings settings;
    settings.setLevel(level->standardLevel());
    settings.qSettings()->sync();
    startRandomGame();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveWindowState();
    KXmlGuiWindow::closeEvent(event);
}

void MainWindow::saveWindowState() {
    QSettings settings;
    settings.setValue(m_key_size, size());
    settings.setValue(m_key_pos, pos());
    settings.sync();
}

void MainWindow::restoreWindowState() {
    QSettings settings;
    QSize s = settings.value(m_key_size, size()).toSize();
    QPoint p = settings.value(m_key_pos, pos()).toPoint();

    resize(s);
    move(p);
}

void MainWindow::undo() {
    m_game->undo();
    m_scene->refresh();
}

void MainWindow::startRandomGame() {
    std::shared_ptr<Settings> settings(new Settings);
    m_game.reset(new Picmi(settings));
    m_mode = Random;

    startGame();
}

void MainWindow::startPresetGame(std::shared_ptr<Level> board) {
    std::shared_ptr<BoardMap> p(new BoardMap(board->width(), board->height(), board->map()));
    m_game.reset(new Picmi(p));
    m_mode = Preset;
    m_current_level = board;

    startGame();
}

void MainWindow::startGame() {

    if (m_scene) {
        disconnect(m_scene.get(), SIGNAL(gameWon()), this, SLOT(gameWon()));
        disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(updatePlayedTime()));
    }

    m_action_undo->setEnabled(true);
    m_action_pause->setEnabled(true);
    m_action_pause->setChecked(false);
    Kg::difficulty()->setGameRunning(true);

    m_timer.start();
    m_scene = m_view.createScene(m_game);
    updatePlayedTime();

    m_view.setEnabled(true);
    m_view.setFocus();
    m_view.setPaused(false);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updatePlayedTime()));
    connect(m_scene.get(), SIGNAL(gameWon()), this, SLOT(gameWon()));

    m_in_progress = true;
}

void MainWindow::updatePlayedTime() {
    this->statusBar()->changeItem(
                i18n("Elapsed time") + QString(": %1").arg(Time(m_game->elapsedSecs()).toString()),
                m_statusbar_time_id);
}

std::shared_ptr<KScoreDialog> MainWindow::createScoreDialog() {
    std::shared_ptr<KScoreDialog> p(new KScoreDialog(KScoreDialog::Name | KScoreDialog::Date | KScoreDialog::Time, this));

    p->initFromDifficulty(Kg::difficulty());
    p->hideField(KScoreDialog::Score);

    return p;
}

void MainWindow::gameWon() {
    KScoreDialog::FieldInfo score = m_game->endGame();
    m_view.setEnabled(false);
    m_action_pause->setEnabled(false);
    m_action_undo->setEnabled(false);
    Kg::difficulty()->setGameRunning(false);
    m_timer.stop();
    m_in_progress = false;

    if (m_mode == Random && Kg::difficultyLevel() != KgDifficultyLevel::Custom) {
        std::shared_ptr<KScoreDialog> scoreDialog = createScoreDialog();
        if (scoreDialog->addScore(score, KScoreDialog::LessIsMore | KScoreDialog::AskName) != 0) {
            scoreDialog->exec();
        }
    } else if (m_mode == Preset) {
        m_current_level->setSolved(m_game->elapsedSecs());
    }

    m_view.setFocus();
}

void MainWindow::highscores() {
    pauseGame();

    std::shared_ptr<KScoreDialog> scoreDialog = createScoreDialog();
    scoreDialog->exec();

    m_view.setFocus();
}

void MainWindow::togglePaused(bool paused) {
    m_view.setPaused(paused);
    m_action_undo->setEnabled(!paused);

    if (paused) {
        m_timer.stop();
    } else {
        m_timer.start();
    }
}

void MainWindow::pauseGame() {
    if (m_action_pause->isChecked() || !m_in_progress) {
        return;
    }

    m_action_pause->setChecked(true);
    togglePaused(true);
}

void MainWindow::settings() {
    pauseGame();

    SettingsWindow w(this);
    w.exec();
}
