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


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHBoxLayout>
#include <QGraphicsSimpleTextItem>
#include <QMessageBox>
#include <klocalizedstring.h>
#include <kstandardgameaction.h>
#include <kactioncollection.h>

#include "helpwindow.h"
#include "settingswindow.h"
#include "config.h"
#include "src/constants.h"

MainWindow::MainWindow(QWidget *parent) :
    KXmlGuiWindow(parent), m_key_size("window/size"),
    m_key_pos("window/position"), m_in_progress(false)
{
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName("picmi");

    setWindowIcon(QIcon(QString(FILEPATH) + "icon.png"));

    m_timer.setInterval(1000);

    setCentralWidget(&m_view);

    setupActions();
    restoreWindowState();

    Settings settings;
    KGameDifficulty::setLevel(settings.level());

    startGame();
}

void MainWindow::setupActions() {
    KStandardGameAction::gameNew(this, SLOT(startGame()), actionCollection());
    KStandardGameAction::highscores(this, SLOT(highscores()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());
    KStandardAction::preferences(this, SLOT(settings()), actionCollection());
    m_action_pause = KStandardGameAction::pause(this, SLOT(togglePaused(bool)), actionCollection());
    m_action_undo = KStandardGameAction::undo(this, SLOT(undo()), actionCollection());

    KGameDifficulty::init(this, this, SLOT(levelChanged(KGameDifficulty::standardLevel)), SLOT(customLevelChanged(int)));
    KGameDifficulty::setRestartOnChange(KGameDifficulty::RestartOnChange);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Easy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Medium);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Hard);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Configurable);

    setupGUI();
}

void MainWindow::levelChanged(KGameDifficulty::standardLevel level) {
    Settings settings;
    settings.setLevel(level);
    settings.qSettings()->sync();
    startGame();
}

void MainWindow::customLevelChanged(int level) {
    Q_UNUSED(level);
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

void MainWindow::startGame() {

    if (m_scene) {
        disconnect(m_scene.get(), SIGNAL(gameWon()), this, SLOT(gameWon()));
        disconnect(&m_timer, SIGNAL(timeout()), m_scene.get(), SLOT(updatePlayedTime()));
    }

    m_action_undo->setEnabled(true);
    m_action_pause->setEnabled(true);
    m_action_pause->setChecked(false);
    KGameDifficulty::setRunning(true);

    m_timer.start();
    std::shared_ptr<Settings> settings(new Settings);
    m_game.reset(new Picmi(settings));
    m_scene = m_view.createScene(m_game);

    m_view.setEnabled(true);
    m_view.setFocus();

    connect(&m_timer, SIGNAL(timeout()), m_scene.get(), SLOT(updatePlayedTime()));
    connect(m_scene.get(), SIGNAL(gameWon()), this, SLOT(gameWon()));

    m_in_progress = true;
}

std::shared_ptr<KScoreDialog> MainWindow::createScoreDialog() {
    Settings settings;

    std::shared_ptr<KScoreDialog> p(new KScoreDialog(KScoreDialog::Name | KScoreDialog::Date | KScoreDialog::Time, this));

    p->addLocalizedConfigGroupNames(KGameDifficulty::localizedLevelStrings());
    p->setConfigGroupWeights(KGameDifficulty::levelWeights());
    QPair<QByteArray, QString> group = KGameDifficulty::localizedLevelString();
    if(group.first.isEmpty()) {
        group = qMakePair(QByteArray("Custom"), i18n("Custom"));
    }
    p->setConfigGroup(group);

    p->hideField(KScoreDialog::Score);

    return p;
}

void MainWindow::gameWon() {
    KScoreDialog::FieldInfo score = m_game->endGame();
    m_view.setEnabled(false);
    m_action_pause->setEnabled(false);
    m_action_undo->setEnabled(false);
    KGameDifficulty::setRunning(false);
    m_timer.stop();
    m_in_progress = false;

    std::shared_ptr<KScoreDialog> scoreDialog = createScoreDialog();
    if (scoreDialog->addScore(score, KScoreDialog::LessIsMore | KScoreDialog::AskName) != 0) {
        scoreDialog->exec();
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

void MainWindow::about() {
    pauseGame();

    QString text = QString("Picmi %1.%2.%3\nBuilt on %4 at %5\n\nCopyright 2012 Jakob Gruber\n\n"
                           "This program is distributed in the hope that it will be useful,\n"
                           "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.")
            .arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_PATCH)
            .arg(__DATE__).arg(__TIME__);
    QMessageBox::about(this, "About Picmi", text);
}

void MainWindow::pauseGame() {
    if (m_action_pause->isChecked() || !m_in_progress) {
        return;
    }

    m_action_pause->setChecked(true);
    togglePaused(true);
}

void MainWindow::help() {
    pauseGame();

    HelpWindow w(this);
    w.exec();
}

void MainWindow::settings() {
    pauseGame();

    SettingsWindow w(this);
    w.exec();
}
