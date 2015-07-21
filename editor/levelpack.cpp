#include "levelpack.h"
#include <QtGui>

const int STACK_LEVELS = 0;
const int STACK_INFO = 1;
const int STACK_OUTPUT = 2;

//------------------------------------------
LevelPackDialog::LevelPackDialog(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
}

//------------------------------------------
int LevelPackDialog::exec()
{
    m_result = 0;
    ui.lwLevels->clear();

    QSettings settings("xlabsoft","LevelEditor");
    author = settings.value("author","XlabSoft").toString();
    packname = settings.value("packname","My Jag Pack").toString();
    comment = settings.value("comment","Created by jag level editor").toString();
    ui.leAuthor->setText(author);
    ui.lePackName->setText(packname);
    ui.leComment->setText(comment);

    ui.stackedWidget->setCurrentIndex(STACK_LEVELS);
    updateNavigation();

    QDialog::exec();

    return m_result;
}

//------------------------------------------
void LevelPackDialog::updateNavigation()
{
    switch (ui.stackedWidget->currentIndex()) {
        case STACK_LEVELS: {
            ui.bNext->setText(tr("Next >>"));
            ui.bPrev->setEnabled(false);
            break;
        }
        case STACK_INFO: {
            ui.bNext->setText(tr("Next >>"));
            ui.bPrev->setEnabled(true);
            break;
        }
        case STACK_OUTPUT: {
            ui.bNext->setText(tr("Create"));
            ui.bPrev->setEnabled(true);
            break;
        }
    }
}

//------------------------------------------
void LevelPackDialog::updateLevelsGroup()
{
    ui.gbLevels->setTitle(tr("Levels list (%1 levels)").arg(ui.lwLevels->count()));
}

//------------------------------------------
QString LevelPackDialog::checkPathForDialog(const QString &path)
{
    QString qs = path;
    QDir dir(path);
    while (!dir.exists(qs)) {
        if (qs.endsWith(":/") || qs.contains('/') == 0) break;
        qs.truncate(qs.lastIndexOf('/'));
        qs.truncate(qs.lastIndexOf('/')+1);
    }
    return (qs);
}

//------------------------------------------
void LevelPackDialog::on_bNext_clicked()
{
    switch (ui.stackedWidget->currentIndex()) {
        case STACK_LEVELS: {
            if (ui.lwLevels->count() > 0)
                ui.stackedWidget->setCurrentIndex(STACK_INFO);
            break;
        }
        case STACK_INFO: {
            ui.stackedWidget->setCurrentIndex(STACK_OUTPUT);
            break;
        }
        case STACK_OUTPUT: {
            if (!ui.lePackFile->text().isEmpty()) {
                m_result = 1;
                close();
            }
            else
                QMessageBox::information(0, tr("File name required"),
                               tr("Please select file name to save level pack."),
                               QMessageBox::Close);
            break;
        }
    }
    updateNavigation();
}

//------------------------------------------
void LevelPackDialog::on_bPrev_clicked()
{
    switch (ui.stackedWidget->currentIndex()) {
        case STACK_INFO: {
            ui.stackedWidget->setCurrentIndex(STACK_LEVELS);
            break;
        }
        case STACK_OUTPUT: {
            ui.stackedWidget->setCurrentIndex(STACK_INFO);
            break;
        }
    }
    updateNavigation();
}

//------------------------------------------
void LevelPackDialog::on_bBrowse_clicked()
{
    QSettings settings("xlabsoft","LevelEditor");
    QString qsPath = settings.value("scnpath",".").toString();
    qsPath = checkPathForDialog(qsPath);

    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select levels to add to level pack"),
                                                      qsPath, tr("Jag levels (*.scn)"));
    if (files.isEmpty()) return;

    files.sort();
    ui.lwLevels->addItems(files);
    updateLevelsGroup();
}

//------------------------------------------
void LevelPackDialog::on_bUp_clicked()
{
    QList<QListWidgetItem *> sel = ui.lwLevels->selectedItems();
    for (int i = 0; i < sel.count(); i++) {
        int row = ui.lwLevels->row(sel.at(i));
        if (row > 0) {
            QListWidgetItem *lwi = ui.lwLevels->takeItem(row);
            ui.lwLevels->insertItem(row - 1, lwi);
        }
    }
}

//------------------------------------------
void LevelPackDialog::on_bDown_clicked()
{
    QList<QListWidgetItem *> sel = ui.lwLevels->selectedItems();
    for (int i = 0; i < sel.count(); i++) {
        int row = ui.lwLevels->row(sel.at(i));
        if (row < ui.lwLevels->count() - 1) {
            QListWidgetItem *lwi = ui.lwLevels->takeItem(row);
            ui.lwLevels->insertItem(row + 1, lwi);
        }
    }
}

//------------------------------------------
void LevelPackDialog::on_bRemove_clicked()
{
    QList<QListWidgetItem *> sel = ui.lwLevels->selectedItems();
    for (int i = 0; i < sel.count(); i++) {
        int row = ui.lwLevels->row(sel.at(i));
        QListWidgetItem *lwi = ui.lwLevels->takeItem(row);
        if (lwi)
            delete lwi;
    }
    updateLevelsGroup();
}

//------------------------------------------
void LevelPackDialog::on_bClear_clicked()
{
    ui.lwLevels->clear();
    updateLevelsGroup();
}

//------------------------------------------
void LevelPackDialog::on_bBrowsePackFile_clicked()
{
    QSettings settings("xlabsoft","LevelEditor");
    QString qsPath = settings.value("lpkpath",".").toString();
    qsPath = checkPathForDialog(qsPath);
    QString fname = QFileDialog::getSaveFileName(this, tr("Select level pack name"),
                                                 qsPath, tr("Jag levelpaks (*.lpk)"));
    if (!fname.isEmpty())
        ui.lePackFile->setText(fname);
}

//------------------------------------------
QStringList LevelPackDialog::files()
{
    QStringList sl;
    for (int i = 0; i < ui.lwLevels->count(); i++)
        sl.append(ui.lwLevels->item(i)->text());

    return sl;
}
