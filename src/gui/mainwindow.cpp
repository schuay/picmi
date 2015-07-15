/* *************************************************************************
 *  Copyright 2015 Jakob Gruber <jakob.gruber@gmail.com>                   *
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

#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardGameAction>
#include <KToggleFullScreenAction>
#include <QCoreApplication>
#include <QGraphicsSimpleTextItem>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QPointer>
#include <QPushButton>
#include <QStatusBar>

#include "src/constants.h"
#include "src/logic/levelloader.h"
#include "src/logic/picmi.h"
#include "selectboardwindow.h"
#include "settingswindow.h"
#include "scene.h"

MainWindow::MainWindow(QWidget *parent) :
    KXmlGuiWindow(parent),
    m_key_pos("window/position"), m_in_progress(false), m_mode(Random)
{
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
    KStandardGameAction::restart(this, SLOT(restartGame()), actionCollection());
    KStandardGameAction::highscores(this, SLOT(highscores()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());
    KStandardAction::preferences(this, SLOT(settings()), actionCollection());
    KStandardAction::fullScreen(this, SLOT(toggleFullscreen(bool)), this, actionCollection());
    m_action_pause = KStandardGameAction::pause(this, SLOT(togglePaused(bool)), actionCollection());
    m_action_undo = KStandardGameAction::undo(this, SLOT(undo()), actionCollection());
    m_action_hint = KStandardGameAction::hint(this, SLOT(hint()), actionCollection());
    m_action_solve = KStandardGameAction::solve(this, SLOT(solve()), actionCollection());

    /* Prevent the default hint shortcut from overwriting our HJKL vim-like control mapping. */
    actionCollection()->setDefaultShortcut(m_action_hint, QKeySequence(Qt::CTRL + Qt::Key_I));

    m_action_save_state = actionCollection()->addAction("save-position");
    m_action_save_state->setText(i18n("Save Position"));
    m_action_save_state->setIcon(QIcon::fromTheme("list-add"));
    actionCollection()->setDefaultShortcut(m_action_save_state, QKeySequence(Qt::CTRL + Qt::Key_S));
    connect(m_action_save_state, SIGNAL(triggered()), this, SLOT(saveState()));

    m_action_load_state = actionCollection()->addAction("load-position");
    m_action_load_state->setText(i18n("Load Position"));
    m_action_load_state->setIcon(QIcon::fromTheme("view-refresh"));
    actionCollection()->setDefaultShortcut(m_action_load_state, QKeySequence(Qt::CTRL + Qt::Key_L));
    connect(m_action_load_state, SIGNAL(triggered()), this, SLOT(loadState()));

    m_status_time = new QLabel;
    m_status_time->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_status_position = new QLabel;
    m_status_position->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_new_game = new QPushButton;
    m_new_game->setText(i18n("New Game"));
    m_new_game->setVisible(false);
    connect(m_new_game, SIGNAL(clicked(bool)), this, SLOT(startRandomGame()));

    m_load_game = new QPushButton;
    m_load_game->setText(i18n("Load New Game"));
    m_load_game->setVisible(false);
    connect(m_load_game, SIGNAL(clicked(bool)), this, SLOT(loadBoard()));

    this->statusBar()->addWidget(m_new_game, 0);
    this->statusBar()->addWidget(m_load_game, 0);
    this->statusBar()->addWidget(m_status_position, 1);
    this->statusBar()->addWidget(m_status_time, 1);

    Kg::difficulty()->addStandardLevel(KgDifficultyLevel::Easy);
    Kg::difficulty()->addStandardLevel(KgDifficultyLevel::Medium, true);
    Kg::difficulty()->addStandardLevel(KgDifficultyLevel::Hard);

    KgDifficultyLevel *configurable = new KgDifficultyLevel(90, QByteArray("Custom"), i18nc("custom difficulty", "Custom"));
    Kg::difficulty()->addLevel(configurable);

    KgDifficultyGUI::init(this);
    connect(Kg::difficulty(), SIGNAL(currentLevelChanged(const KgDifficultyLevel*)), this,
            SLOT(levelChanged(const KgDifficultyLevel*)));

    /* Disable the toolbar configuration menu entry.
     * The default size is used at first start up. */
    setupGUI(QSize(700, 560), Keys | StatusBar | Save | Create);
}

void MainWindow::loadBoard() {
    QPointer<SelectBoardWindow> w(new SelectBoardWindow(this));
    if (w->exec() == QDialog::Accepted) {
        startPresetGame(w->selectedBoard());
    }
    delete w;
}

void MainWindow::levelChanged(const KgDifficultyLevel* level) {
    Settings::instance()->setLevel(level->standardLevel());
    Settings::instance()->sync();
    startRandomGame();
}

void MainWindow::toggleFullscreen(bool full_screen) {
    KToggleFullScreenAction::setFullScreen(this, full_screen);
}

void MainWindow::undoStackSizeChanged(int size)
{
    m_action_undo->setEnabled(size != 0);
}

void MainWindow::saveStackSizeChanged(int size)
{
    m_action_load_state->setEnabled(size != 0);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveWindowState();
    KXmlGuiWindow::closeEvent(event);
}

void MainWindow::saveWindowState() {
    QSettings settings;
    settings.setValue(m_key_pos, pos());
    settings.sync();
}

void MainWindow::restoreWindowState() {
    QSettings settings;
    QPoint p = settings.value(m_key_pos, pos()).toPoint();

    move(p);
}

void MainWindow::undo() {
    QPoint p = m_game->undo();
    m_scene->refresh(p);
}

void MainWindow::hint()
{
    QPoint p = m_game->hint();

    if (m_in_progress) {
        m_scene->refresh(p);
        m_scene->hover(p.x(), p.y());
    }
}

void MainWindow::solve()
{
    m_game->solve();
}

void MainWindow::saveState() {
    m_game->saveState();
    updatePositions();
}

void MainWindow::loadState() {
    m_game->loadState();
    m_scene->refresh();
}

void MainWindow::startRandomGame() {
    m_game = QSharedPointer<Picmi>(new Picmi());
    m_mode = Random;

    startGame();
}

void MainWindow::restartGame()
{
    m_game = QSharedPointer<Picmi>(new Picmi(m_game->getBoardMap()));

    startGame();
}

void MainWindow::startPresetGame(QSharedPointer<Level> board) {
    QSharedPointer<BoardMap> p(new BoardMap(board->width(), board->height(), board->map()));
    m_game = QSharedPointer<Picmi>(new Picmi(p));
    m_mode = Preset;
    m_current_level = board;

    startGame();
}

void MainWindow::startGame() {

    if (m_scene) {
        disconnect(&m_timer, &QTimer::timeout, this, &MainWindow::updatePlayedTime);
    }

    m_action_undo->setEnabled(false);
    m_action_hint->setEnabled(true);
    m_action_solve->setEnabled(true);
    m_action_save_state->setEnabled(true);
    m_action_load_state->setEnabled(false);
    m_action_pause->setEnabled(true);
    m_action_pause->setChecked(false);
    m_status_position->setVisible(true);
    m_load_game->setVisible(false);
    m_new_game->setVisible(false);
    Kg::difficulty()->setGameRunning(true);

    m_timer.start();
    m_scene = m_view.createScene(m_game);
    updatePlayedTime();
    updatePositions();

    m_view.setEnabled(true);
    m_view.setFocus();
    m_view.setPaused(false);

    connect(&m_timer, &QTimer::timeout, this, &MainWindow::updatePlayedTime);
    connect(m_game.data(), SIGNAL(stateChanged()), this, SLOT(updatePositions()));
    connect(m_game.data(), SIGNAL(gameCompleted()), this, SLOT(gameCompleted()));
    connect(m_game.data(), SIGNAL(gameWon()), this, SLOT(gameWon()));
    connect(m_game.data(), SIGNAL(undoStackSizeChanged(int)), this, SLOT(undoStackSizeChanged(int)));
    connect(m_game.data(), SIGNAL(saveStackSizeChanged(int)), this, SLOT(saveStackSizeChanged(int)));

    m_in_progress = true;
}

void MainWindow::updatePlayedTime() {
    m_status_time->setText(i18n("Elapsed time: %1",
                                Time(m_game->elapsedSecs()).toString()));
}

void MainWindow::updatePositions() {
    m_status_position->setText(i18n("Actions since last saved position: %1",
                                    m_game->currentStateAge()));
}

QSharedPointer<KScoreDialog> MainWindow::createScoreDialog() {
    QSharedPointer<KScoreDialog> p(new KScoreDialog(KScoreDialog::Name | KScoreDialog::Date | KScoreDialog::Time));

    p->initFromDifficulty(Kg::difficulty());
    p->hideField(KScoreDialog::Score);

    return p;
}

void MainWindow::gameWon() {
    KScoreDialog::FieldInfo score = m_game->endGame();
    bool notified = false;
    m_status_position->setVisible(false);
    if (m_mode == Random) {
        m_new_game->setVisible(true);
        if (Kg::difficultyLevel() != KgDifficultyLevel::Custom) {
            QSharedPointer<KScoreDialog> scoreDialog = createScoreDialog();
            if (scoreDialog->addScore(score, KScoreDialog::LessIsMore | KScoreDialog::AskName) != 0) {
                scoreDialog->exec();
                notified = true;
            }
        }
    } else if (m_mode == Preset) {
        m_load_game->setVisible(true);
        m_current_level->setSolved(m_game->elapsedSecs());
    }

    /* Ensure that the user gets some kind of feedback about solving the board. */
    if (!notified) {
        KMessageBox::information(this, i18n("Board Solved!"),
                                 i18n("Congratulations, you've solved this board!"));
    }

    m_view.setFocus();
}

void MainWindow::gameCompleted() {
    m_view.setEnabled(false);
    m_action_pause->setEnabled(false);
    m_action_solve->setEnabled(false);
    m_action_hint->setEnabled(false);
    m_action_undo->setEnabled(false);
    m_action_save_state->setEnabled(false);
    m_action_load_state->setEnabled(false);
    Kg::difficulty()->setGameRunning(false);
    m_timer.stop();
    m_in_progress = false;
}

void MainWindow::highscores() {
    pauseGame();

    QSharedPointer<KScoreDialog> scoreDialog = createScoreDialog();
    scoreDialog->exec();

    m_view.setFocus();
}

void MainWindow::togglePaused(bool paused) {
    m_view.setPaused(paused);
    m_action_undo->setEnabled(!paused);
    m_action_hint->setEnabled(!paused);
    m_action_solve->setEnabled(!paused);
    m_action_save_state->setEnabled(!paused);
    m_action_load_state->setEnabled(!paused);

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

    QPointer<SettingsWindow> w(new SettingsWindow(this));
    w->exec();
    delete w;
}
