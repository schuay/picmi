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

#include "highscorewindow.h"
#include "helpwindow.h"
#include "settingswindow.h"
#include "config.h"
#include "src/constants.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_key_size("window/size"), m_key_pos("window/position"), m_in_progress(false)
{
    ui->setupUi(this);

    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName("picmi");

    setWindowIcon(QIcon(QString(FILEPATH) + "icon.png"));

    /* hide the status bar for now until we find a use for it */
    ui->statusBar->hide();

    restoreWindowState();

    m_scores.reset(new HighScores);

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(startGame()));
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(help()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(settings()));
    connect(ui->actionPause, SIGNAL(triggered(bool)), this, SLOT(togglePaused(bool)));
    connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(undo()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionHigh_Scores, SIGNAL(triggered()), this, SLOT(highscores()));

    m_timer.setInterval(1000);

    startGame();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveWindowState();
    QMainWindow::closeEvent(event);
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

    ui->actionUndo->setEnabled(true);
    ui->actionPause->setEnabled(true);
    ui->actionPause->setChecked(false);

    m_timer.start();
    std::shared_ptr<Settings> settings(new Settings());
    m_game.reset(new Picmi(settings));
    m_scene = ui->graphicsView->createScene(m_game);

    ui->graphicsView->setEnabled(true);
    ui->graphicsView->setFocus();
    ui->statusBar->clearMessage();

    connect(&m_timer, SIGNAL(timeout()), m_scene.get(), SLOT(updatePlayedTime()));
    connect(m_scene.get(), SIGNAL(gameWon()), this, SLOT(gameWon()));

    m_in_progress = true;
}

void MainWindow::gameWon() {
    std::shared_ptr<HighScore> score = m_game->endGame();
    m_scores->add(score);
    ui->graphicsView->setEnabled(false);
    ui->actionPause->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    m_timer.stop();
    m_in_progress = false;

    HighScoreWindow w(m_scores, score, this);
    w.exec();

    ui->graphicsView->setFocus();
}

void MainWindow::highscores() {
    pauseGame();

    Settings s;
    HighScoreWindow w(m_scores, s, this);
    w.exec();

    ui->graphicsView->setFocus();
}

void MainWindow::togglePaused(bool paused) {
    ui->graphicsView->setPaused(paused);
    ui->actionUndo->setEnabled(!paused);

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
    if (ui->actionPause->isChecked() || !m_in_progress) {
        return;
    }

    ui->actionPause->setChecked(true);
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

MainWindow::~MainWindow()
{
    delete ui;
}
