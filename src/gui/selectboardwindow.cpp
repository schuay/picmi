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

#include "selectboardwindow.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <QAbstractTableModel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtAlgorithms>
#include <assert.h>

#include "src/logic/elapsedtime.h"
#include "src/logic/levelloader.h"

static QString diffString(const int difficulty);

class LevelTableModel : public QAbstractTableModel
{
public:
    LevelTableModel(QList<QSharedPointer<Level> > &levels, QObject * parent = 0);

    enum Columns {
        Name,
        LevelSet,
        Difficulty,
        Size,
        Solved,
        ColumnCount /* not a real column */
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

private:
    QList<QSharedPointer<Level> > &m_levels;
};

LevelTableModel::LevelTableModel(QList<QSharedPointer<Level> > &levels, QObject *parent) :
    QAbstractTableModel(parent), m_levels(levels)
{

}

int LevelTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_levels.size();
}

int LevelTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant LevelTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::TextAlignmentRole)) {
             return QVariant();
    }

    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    QSharedPointer<Level> level= m_levels[index.row()];
    switch (index.column()) {
    case Name: return level->visibleName();
    case LevelSet: return level->levelset();
    case Difficulty: return diffString(level->difficulty());
    case Size: return QString("%1x%2").arg(level->width()).arg(level->height());
    case Solved: return (level->solved()) ? Time(level->solvedTime()).toString() : "-";
    default: assert(0);
    }

    return QVariant();
}

QVariant LevelTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case Name: return QVariant(ki18n("Level").toString());
        case LevelSet: return QVariant(ki18n("Level Set").toString());
        case Difficulty: return QVariant(ki18n("Difficulty").toString());
        case Size: return QVariant(ki18n("Size").toString());
        case Solved: return QVariant(ki18nc("board solved", "Solved").toString());
        default: assert(0);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

typedef bool (*LevelComparator)(const QSharedPointer<Level> &,
                                const QSharedPointer<Level> &);

static bool levelLessThan(const QSharedPointer<Level> &lhs,
                          const QSharedPointer<Level> &rhs) {
    return lhs->name() < rhs->name();
}

static bool levelGreaterThan(const QSharedPointer<Level> &lhs,
                             const QSharedPointer<Level> &rhs) {
    return levelLessThan(rhs, lhs);
}

static bool diffLessThan(const QSharedPointer<Level> &lhs,
                         const QSharedPointer<Level> &rhs) {
    return lhs->difficulty() < rhs->difficulty();
}

static bool diffGreaterThan(const QSharedPointer<Level> &lhs,
                            const QSharedPointer<Level> &rhs) {
    return diffLessThan(rhs, lhs);
}

static bool solvedLessThan(const QSharedPointer<Level> &lhs,
                           const QSharedPointer<Level> &rhs) {
    if (!lhs->solved() && !rhs->solved()) {
        return false;
    } else if (!lhs->solved() || !rhs->solved()) {
        return lhs->solved();
    } else {
        return (lhs->solvedTime() < rhs->solvedTime());
    }
}

static bool solvedGreaterThan(const QSharedPointer<Level> &lhs,
                              const QSharedPointer<Level> &rhs) {
    return solvedLessThan(rhs, lhs);
}

void LevelTableModel::sort(int column, Qt::SortOrder order) {
    LevelComparator cmp = NULL;
    switch (column) {
    case Name: cmp = (order == Qt::AscendingOrder) ? levelLessThan : levelGreaterThan; break;
    case Difficulty: cmp = (order == Qt::AscendingOrder) ? diffLessThan : diffGreaterThan; break;
    case Solved: cmp = (order == Qt::AscendingOrder) ? solvedLessThan : solvedGreaterThan; break;
    default: assert(0);
    }

    qStableSort(m_levels.begin(), m_levels.end(), cmp);
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() -1));
}

SelectBoardWindow::SelectBoardWindow(QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    setWindowTitle(i18n("Level Selection"));
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SelectBoardWindow::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SelectBoardWindow::reject);
    mainLayout->addWidget(buttonBox);

    QWidget *mainWidget = new QWidget(this);
    ui = new Ui::LevelSelectUi;
    ui->setupUi(mainWidget);
    mainLayout->addWidget(mainWidget);
    mainLayout->addWidget(buttonBox);

    m_levels = LevelLoader::load();
    m_model = QSharedPointer<LevelTableModel>(new LevelTableModel(m_levels));

    ui->tableView->setUpdatesEnabled(false);
    ui->tableView->setModel(m_model.data());

    /* Hide all columns except Level, Difficulty and Solved. */

    for (int i = 0; i < m_model->columnCount(); i++) {
        ui->tableView->hideColumn(i);
    }
    ui->tableView->showColumn(LevelTableModel::Name);
    ui->tableView->showColumn(LevelTableModel::Difficulty);
    ui->tableView->showColumn(LevelTableModel::Solved);

    ui->tableView->sortByColumn(LevelTableModel::Name, Qt::AscendingOrder);
    ui->tableView->sortByColumn(LevelTableModel::Solved, Qt::DescendingOrder);
    ui->tableView->sortByColumn(LevelTableModel::Difficulty, Qt::AscendingOrder);

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();

    if (m_levels.empty()) {
        okButton->setEnabled(false);
    } else {
        resetSelection();
        connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(selectedLevelChanged(QModelIndex,QModelIndex)));
        connect(m_model.data(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(levelDataChanged(QModelIndex,QModelIndex)));
        updateDetails(m_levels[0]);
    }

    ui->tableView->setUpdatesEnabled(true);
}

void SelectBoardWindow::showEvent(QShowEvent *event) {
    updateDetails(selectedBoard());
    QDialog::showEvent(event);
}

void SelectBoardWindow::resizeEvent(QResizeEvent *event) {
    updateDetails(selectedBoard());
    QDialog::resizeEvent(event);
}

SelectBoardWindow::~SelectBoardWindow() {
    delete ui;
}

void SelectBoardWindow::selectedLevelChanged(const QModelIndex &current, const QModelIndex &previous) {
    Q_UNUSED(previous);
    updateDetails(m_levels[current.row()]);
}

void SelectBoardWindow::levelDataChanged(const QModelIndex &topLeft, const
                                         QModelIndex &bottomRight) {
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    resetSelection();
    updateDetails(selectedBoard());
}

static QString diffString(const int difficulty) {
    if (difficulty < 0) {
        return diffString(0);
    } else if (difficulty > 7) {
        return diffString(7);
    }

    switch (difficulty) {
    case 0: return ki18n("Ridiculously Easy").toString();
    case 1: return ki18n("Very Easy").toString();
    case 2: return ki18n("Easy").toString();
    case 3: return ki18n("Medium").toString();
    case 4: return ki18n("Hard").toString();
    case 5: return ki18n("Very Hard").toString();
    case 6: return ki18n("Extremely Hard").toString();
    case 7: return ki18n("Impossible").toString();
    default: throw OutOfBoundsException();
    }
}

void SelectBoardWindow::updateDetails(QSharedPointer<Level> level) {
    ui->labelName->setText(i18n("Name: %1", level->visibleName()));
    ui->labelAuthor->setText(i18n("Author: %1", level->author()));
    ui->labelSize->setText(i18n("Size: %1x%2", level->width(), level->height()));
    ui->labelDifficulty->setText(i18n("Difficulty: %1", diffString(level->difficulty())));
    if (level->solved()) {
        ui->labelSolved->setText(i18nc("board solve time", "Solved: %1",
                                       Time(level->solvedTime()).toString()));
        QPixmap scaled = level->preview().scaled(ui->labelImage->size(),
                                                 Qt::KeepAspectRatio,
                                                 Qt::FastTransformation);
        ui->labelImage->setPixmap(scaled);
    } else {
        ui->labelSolved->setText(i18nc("board not solved yet", "Solved: -"));
        ui->labelImage->setText("?");
    }
}

void SelectBoardWindow::resetSelection() {
    QModelIndex index = m_model->index(0, 0);
    QItemSelectionModel::SelectionFlags flags =
            QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows;
    ui->tableView->selectionModel()->select(index, flags);
}

QSharedPointer<Level> SelectBoardWindow::selectedBoard() const {
    int index = ui->tableView->selectionModel()->selectedIndexes().at(0).row();
    return m_levels[index];
}
