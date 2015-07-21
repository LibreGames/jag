#include "mainwindow.h"
#include "levelpack.h"

static LevelPackDialog *d_levelpack = 0;

//------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.actionNew, SIGNAL(activated()), this, SLOT(fileNew()));
    connect(ui.action_Open, SIGNAL(activated()), this, SLOT(fileOpen()));
    connect(ui.action_Save, SIGNAL(activated()), this, SLOT(fileSave()));
    connect(ui.actionSave_as, SIGNAL(activated()), this, SLOT(fileSaveAs()));
    connect(ui.actionCreate_levelpack, SIGNAL(activated()), this, SLOT(fileCreateLevelPack()));
    connect(ui.actionExtract_levelpack, SIGNAL(activated()), this, SLOT(fileExtractLevelPack()));
    connect(ui.actionE_xit, SIGNAL(activated()), this, SLOT(fileExit()));
    connect(ui.actionShow_properties, SIGNAL(activated()), this, SLOT(editShowProperties()));
    connect(ui.dockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(dockShowHide(bool)));
    connect(ui.actionAbout, SIGNAL(activated()), this, SLOT(about()));

    connect(ui.rbNoTarget, SIGNAL(toggled(bool)), this, SLOT(on_property_toggled(bool)));
    connect(ui.rbTarget1, SIGNAL(toggled(bool)), this, SLOT(on_property_toggled(bool)));
    connect(ui.rbTarget2, SIGNAL(toggled(bool)), this, SLOT(on_property_toggled(bool)));
    connect(ui.rbNoBlock, SIGNAL(toggled(bool)), this, SLOT(on_property_toggled(bool)));
    connect(ui.rbBlock1, SIGNAL(toggled(bool)), this, SLOT(on_property_toggled(bool)));
    connect(ui.rbBlock2, SIGNAL(toggled(bool)), this, SLOT(on_property_toggled(bool)));

    connect(ui.tbSelect, SIGNAL(toggled(bool)), this, SLOT(on_drawButton_toggled(bool)));
    connect(ui.tbNoPlace, SIGNAL(toggled(bool)), this, SLOT(on_drawButton_toggled(bool)));
    connect(ui.tbConcrete, SIGNAL(toggled(bool)), this, SLOT(on_drawButton_toggled(bool)));
    connect(ui.tbAppear, SIGNAL(toggled(bool)), this, SLOT(on_drawButton_toggled(bool)));
    connect(ui.tbTarget1, SIGNAL(toggled(bool)), this, SLOT(on_drawButton_toggled(bool)));
    connect(ui.tbTarget2, SIGNAL(toggled(bool)), this, SLOT(on_drawButton_toggled(bool)));
    connect(ui.tbOnStart, SIGNAL(toggled(bool)), this, SLOT(on_drawButton_toggled(bool)));
    connect(ui.tbBlock1, SIGNAL(toggled(bool)), this, SLOT(on_drawButton_toggled(bool)));
    connect(ui.tbBlock2, SIGNAL(toggled(bool)), this, SLOT(on_drawButton_toggled(bool)));

    actionSelectAll = new QAction(tr("Select All"), this);
    actionSelectAll->setShortcut(QKeySequence::SelectAll);
    actionCopy = new QAction(tr("Copy"), this);
    actionCopy->setShortcut(QKeySequence::Copy);
    actionPaste = new QAction(tr("Paste"), this);
    actionPaste->setShortcut(QKeySequence::Paste);
    actionUndo = new QAction(tr("Undo"), this);
    actionUndo->setShortcut(QKeySequence::Undo);
    actionRedo = new QAction(tr("Redo"), this);
    actionRedo->setShortcut(QKeySequence::Redo);
    contextEdit = 0;
    updateEditActions();

    connect(actionSelectAll, SIGNAL(activated()), this, SLOT(editSelectAll()));
    connect(actionCopy, SIGNAL(activated()), this, SLOT(editCopy()));
    connect(actionPaste, SIGNAL(activated()), this, SLOT(editPaste()));
    connect(actionUndo, SIGNAL(activated()), this, SLOT(editUndo()));
    connect(actionRedo, SIGNAL(activated()), this, SLOT(editRedo()));

    ui.menu_Edit->addSeparator();
    ui.menu_Edit->addAction(actionUndo);
    ui.menu_Edit->addAction(actionRedo);
    ui.menu_Edit->addSeparator();
    ui.menu_Edit->addAction(actionCopy);
    ui.menu_Edit->addAction(actionPaste);
    ui.menu_Edit->addSeparator();
    ui.menu_Edit->addAction(actionSelectAll);

    ui.twLevel->setMouseTracking(true);
    connect(ui.twLevel, SIGNAL(enteredWidget()), this, SLOT(on_enteredWidget()));
    connect(ui.twLevel, SIGNAL(leavedWidget()), this, SLOT(on_leavedWidget()));
    connect(ui.twLevel, SIGNAL(mouseClicked(const QPoint &)), this, SLOT(on_mouseClicked(const QPoint &)));
    connect(ui.twLevel, SIGNAL(mouseRightClicked(const QPoint &)), this, SLOT(on_mouseRightClicked(const QPoint &)));
    connect(ui.twLevel, SIGNAL(mouseReleased()), this, SLOT(on_mouseReleased()));

    m_noupdate = false;
    m_filename = "";
    m_cursor = SelectMode;
    m_undoindex = -1;
    m_undoafterrelease = false;

    QString appDir = qApp->applicationDirPath();
    QDir::setCurrent(appDir);

    loadSettings();

    m_modified = false;
    fileNew();
}

//------------------------------------------
MainWindow::~MainWindow()
{
    saveSettings();
    if (d_levelpack)
        delete d_levelpack;
}

//------------------------------------------
void MainWindow::fileNew()
{
    if (m_modified) {
        QMessageBox::StandardButton mb = confirmSave();
        if (mb == QMessageBox::Yes) {
            fileSave();
            return;
        }
        else if (mb == QMessageBox::Cancel)
            return;
    }

    clearLevel();
    m_filename = "";
    setModified(false);
    initLevel(LEVEL_WIDTH, LEVEL_HEIGHT);
    clearUndo();
}

//------------------------------------------
void MainWindow::fileOpen()
{
    if (m_modified) {
        QMessageBox::StandardButton mb = confirmSave();
        if (mb == QMessageBox::Yes) {
            fileSave();
            return;
        }
        else if (mb == QMessageBox::Cancel)
            return;
    }

    QSettings settings("xlabsoft","LevelEditor");
    QString qsPath = settings.value("scnpath",".").toString();
    qsPath = checkPathForDialog(qsPath);

    QString fname = QFileDialog::getOpenFileName(this, tr("Load level"), qsPath, tr("Jag levels (*.scn)"));
    if (!fname.isEmpty()) {
        clearLevel();
        loadLevel(fname);
        m_filename = fname;
        setModified(false);
        clearUndo();

        fname.truncate((fname).lastIndexOf('/')+1);
        settings.setValue("scnpath", fname);
    }
    updateWindowTitle();
}

//------------------------------------------
void MainWindow::fileSave()
{
    if (m_filename.isEmpty()) {
        fileSaveAs();
        return;
    }
    saveLevel(m_filename);
    setModified(false);
}

//------------------------------------------
void MainWindow::fileSaveAs()
{
    QSettings settings("xlabsoft","LevelEditor");
    QString qsPath = settings.value("scnpath",".").toString();
    qsPath = checkPathForDialog(qsPath);

    QString fname = QFileDialog::getSaveFileName(this, tr("Save level"), qsPath, tr("Jag levels (*.scn)"));
    if (!fname.isEmpty()) {
        saveLevel(fname);
        m_filename = fname;
        setModified(false);

        fname.truncate((fname).lastIndexOf('/')+1);
        settings.setValue("scnpath", fname);
    }
}

//------------------------------------------
void MainWindow::fileCreateLevelPack()
{
    if (!d_levelpack)
        d_levelpack = new LevelPackDialog();

    if (d_levelpack->exec() == 1) {
        bool mix = false;
        QString fname = d_levelpack->packFileName();
        QStringList files = d_levelpack->files();
        QString author = d_levelpack->authorText();
        QString packname = d_levelpack->packNameText();
        QString comment = d_levelpack->commentText();

        if (fname.isEmpty() || files.isEmpty()) return;

        QFile f(fname);
        if (f.open(QIODevice::WriteOnly)) {
            QDataStream ds(&f);

            // format signature
            ds << QString("LPK2");

            // packname, author and comment
            ds << packname << author << comment;

            // levels in pack
            ds << quint32(files.count());

            // mix?
            if (mix)
            {
              for (int i = 0; i < files.count(); i++)
              {
                QString fs = files.takeAt(qrand() % files.count());
                files.append(fs);
              }
            }

            // levels offsets and sizes
            int base = ds.device()->pos() + files.count()*4*2;
            for (int i = 0; i < files.count(); i++) {
                QFileInfo fi(files.at(i));
                ds << quint32(base);
                ds << quint32(fi.size());
                base += fi.size();
            }

            // levels data
            for (int i = 0; i < files.count(); i++) {
                QFile flevel(files.at(i));
                // read file and write to stream
                if (flevel.open(QIODevice::ReadOnly)) {
                    QDataStream dslevel(&flevel);
                    QByteArray ba = dslevel.device()->readAll();
                    ds.device()->write(ba);
                    flevel.close();
                }
            }

            f.close();
        }

        QSettings settings("xlabsoft","LevelEditor");
        fname.truncate((fname).lastIndexOf('/')+1);
        settings.setValue("lpkpath", fname);

        settings.setValue("author", author);
        settings.setValue("packname", packname);
        settings.setValue("comment", comment);
    }
}

//------------------------------------------
void MainWindow::fileExtractLevelPack()
{
    QSettings settings("xlabsoft","LevelEditor");
    QString qsPath = settings.value("lpkpath",".").toString();
    qsPath = checkPathForDialog(qsPath);

    QString fname = QFileDialog::getOpenFileName(this, tr("Select level pack to extract"),
                                                 qsPath, tr("Jag levelpaks (*.lpk)"));
    if (fname.isEmpty()) return;

    qsPath = settings.value("scnpath",".").toString();
    qsPath = checkPathForDialog(qsPath);

    QString path = QFileDialog::getExistingDirectory(this, tr("Select destination folder"), qsPath);
    QDir dir(path);
    path = dir.fromNativeSeparators(path);
    if (path.isEmpty() || !dir.exists(path)) return;
    if (!path.endsWith('/'))
        path += "/";

    QFile f(fname);
    if (f.open(QIODevice::ReadOnly)) {
        QDataStream ds(&f);

        // signature
        QString sig;
        ds >> sig;

        if (sig == "LPK2") {
            // author, packname and comment
            QString packname, author, comment;
            ds >> packname >> author >> comment;
        }

        if (sig == "LPK1" || sig == "LPK2") {
            // level count
            int levelcnt;
            ds >> levelcnt;

            // read levels
            int pos = ds.device()->pos();
            bool yesall = false;
            for (int i = 0; i < levelcnt; i++) {
                // offset and size
                ds.device()->seek(pos);
                int offset, size;
                ds >> offset >> size;
                pos = ds.device()->pos();

                // read level data
                ds.device()->seek(offset);
                QByteArray ba = ds.device()->read(size);

                // ask to overwrite
                QString levelname = QString("%1.scn").arg(i+1, 3, 10, QChar('0'));
                if (QFile::exists(path + levelname) && !yesall) {
                    QMessageBox::StandardButton mb = QMessageBox::question(this,
                        tr("Overwrite file?"),
                        tr("Extracting file %1 already exists.\nDo you want to overwrite it?").arg(levelname),
                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel | QMessageBox::YesAll);
                    if (mb == QMessageBox::Cancel) {
                        f.close();
                        return;
                    }
                    else if (mb == QMessageBox::No)
                        continue;

                    if (mb == QMessageBox::YesAll)
                        yesall = true;
                }

                // write to file
                QFile flevel(path + levelname);
                if (flevel.open(QIODevice::WriteOnly)) {
                    QDataStream dslevel(&flevel);
                    dslevel.device()->write(ba);
                    flevel.close();
                }
            }
        }

        f.close();
    }

    fname.truncate((fname).lastIndexOf('/')+1);
    settings.setValue("lpkpath", fname);
    settings.setValue("scnpath", path);
}

//------------------------------------------
void MainWindow::fileExit()
{
    if (m_modified) {
        QMessageBox::StandardButton mb = confirmSave();
        if (mb == QMessageBox::Yes) {
            fileSave();
            return;
        }
        else if (mb == QMessageBox::Cancel)
            return;
    }
    close();
}

//------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_modified) {
        QMessageBox::StandardButton mb = confirmSave();
        if (mb == QMessageBox::Yes) {
            fileSave();
            event->ignore();
        }
        else if (mb == QMessageBox::Cancel)
            event->ignore();
    }
}

//------------------------------------------
void MainWindow::editShowProperties()
{
    if (ui.actionShow_properties->isChecked())
        ui.dockWidget->show();
    else
        ui.dockWidget->hide();
}

//------------------------------------------
void MainWindow::editSelectAll()
{
    ui.twLevel->setUpdatesEnabled(false);

    for (int i = 0; i < LEVEL_WIDTH; i++)
        for (int j = 0; j < LEVEL_HEIGHT; j++) {
            QTableWidgetItem *twi = ui.twLevel->item(j, i);
            if (!twi) continue;
            twi->setSelected(true);
        }

    ui.twLevel->setUpdatesEnabled(true);
}

//------------------------------------------
void MainWindow::editCopy()
{
    // clear previous
    clipsel.clear();

    // copy new
    QList<QTableWidgetItem *> sel = ui.twLevel->selectedItems();
    for (int i = 0; i < sel.count(); i++) {
        QTableWidgetItem *twi = sel.at(i);
        if (twi)
            clipsel.append(LevelItem(twi->row(), twi->column(), twi->data(Qt::UserRole).toUInt()));
    }

    updateEditActions();
}

//------------------------------------------
void MainWindow::editPaste()
{
    // left upper corner to paste to
    QList<QTableWidgetItem *> sel = ui.twLevel->selectedItems();
    int row = LEVEL_HEIGHT, col = LEVEL_WIDTH;
    for (int i = 0; i < sel.count(); i++) {
        QTableWidgetItem *twi = sel.at(i);
        if (!twi) continue;
        if (twi->row() <= row && twi->column() <= col) {
            row = twi->row();
            col = twi->column();
        }
    }

    // left upper corner of clipboard selection
    int cliprow = LEVEL_HEIGHT, clipcol = LEVEL_WIDTH;
    for (int i = 0; i < clipsel.count(); i++) {
        LevelItem li = clipsel.at(i);
        if (li.row <= cliprow)
            cliprow = li.row;
        if (li.col <= clipcol)
            clipcol = li.col;
    }

    // paste
    for (int i = 0; i < clipsel.count(); i++) {
        LevelItem li = clipsel.at(i);
        int r = row + li.row - cliprow;
        int c = col + li.col - clipcol;
        if (r >= LEVEL_HEIGHT || c >= LEVEL_WIDTH) continue;

        QTableWidgetItem *to = ui.twLevel->item(r, c);
        if (!to) continue;
        to->setData(Qt::UserRole, uint(li.data));
    }
    updateLevelMap();
    setModified(true);
    saveForUndo();
}

//------------------------------------------
void MainWindow::editUndo()
{
    if (m_undoindex >= 0 && m_undoindex < undoList.count()) {
        // restore
        UndoData ud = undoList.at(m_undoindex);
        // save current state for redo
        undoList[m_undoindex] = m_lastundo;

        m_undoindex--;
        applyUndoData(ud);
    }
}

//------------------------------------------
void MainWindow::editRedo()
{
    if (m_undoindex >= -1 && m_undoindex < undoList.count() - 1) {
        m_undoindex++;
        UndoData ud = undoList.at(m_undoindex);
        undoList[m_undoindex] = m_lastundo;
        applyUndoData(ud);
    }
}

//------------------------------------------
void MainWindow::applyUndoData(UndoData &ud)
{
    ui.sbTime->blockSignals(true);
    ui.sbTime->setValue(ud.leveltime);
    ui.sbTime->blockSignals(false);

    ui.sbItems->blockSignals(true);
    ui.sbItems->setValue(ud.itemscount);
    ui.sbItems->blockSignals(false);

    for (int i = 0; i < ud.cells.count(); i++) {
        LevelItem li = ud.cells.at(i);
        QTableWidgetItem *to = ui.twLevel->item(li.row, li.col);
        if (!to) continue;
            to->setData(Qt::UserRole, uint(li.data));
    }
    updateLevelMap();
    setModified(ud.modified);

    prepareUndo();
}

//------------------------------------------
void MainWindow::dockShowHide(bool vis)
{
    ui.actionShow_properties->setChecked(vis);
}

//------------------------------------------
void MainWindow::on_cbConcrete_stateChanged(int)
{
    bool en = ui.cbConcrete->isChecked();
    ui.gbTarget->setEnabled(en);
    ui.gbBlock->setEnabled(en);
    ui.cbAppear->setEnabled(en);
    ui.cbItemOnStart->setEnabled(en);

    if (ui.cbConcrete->checkState() != Qt::PartiallyChecked)
        ui.cbConcrete->setTristate(false);

    if (!m_noupdate)
        updateSelectedCells();
}

//------------------------------------------
void MainWindow::on_property_toggled(bool)
{
    if (!m_noupdate)
        updateSelectedCells();
}

void MainWindow::on_cbAppear_stateChanged(int)
{
    if (ui.cbAppear->checkState() != Qt::PartiallyChecked)
        ui.cbAppear->setTristate(false);

    on_property_toggled(true);
}

void MainWindow::on_cbItemOnStart_stateChanged(int)
{
    if (ui.cbItemOnStart->checkState() != Qt::PartiallyChecked)
        ui.cbItemOnStart->setTristate(false);

    on_property_toggled(true);
}

//------------------------------------------
void MainWindow::on_sbTime_valueChanged(int)
{
    setModified(true);
    saveForUndo();
}

void MainWindow::on_sbItems_valueChanged(int)
{
    setModified(true);
    saveForUndo();
}

//------------------------------------------
void MainWindow::on_drawButton_toggled(bool)
{
    if (ui.tbSelect->isChecked())
        m_cursor = SelectMode;
    else if (ui.tbNoPlace->isChecked())
        m_cursor = NoPlace;
    else if (ui.tbConcrete->isChecked())
        m_cursor = PlaceConcrete;
    else if (ui.tbAppear->isChecked())
        m_cursor = ItemAppear;
    else if (ui.tbTarget1->isChecked())
        m_cursor = Target1;
    else if (ui.tbTarget2->isChecked())
        m_cursor = Target2;
    else if (ui.tbOnStart->isChecked())
        m_cursor = ItemStart;
    else if (ui.tbBlock1->isChecked())
        m_cursor = Block1;
    else if (ui.tbBlock2->isChecked())
        m_cursor = Block2;

    ui.gbProperties->setEnabled(m_cursor == SelectMode);
}

//------------------------------------------
void MainWindow::on_mouseClicked(const QPoint &pos)
{
    if (m_cursor == SelectMode) return;

    QTableWidgetItem *twi = ui.twLevel->itemAt(pos);
    if (twi) {
        if (!twi->isSelected()) {
            QList<QTableWidgetItem *> sel = ui.twLevel->selectedItems();
            for (int i = 0; i < sel.count(); i++) {
                QTableWidgetItem *twisel = sel.at(i);
                if (!twisel) continue;
                twisel->setSelected(false);
            }
            twi->setSelected(true);
        }
        if (m_cursor != NoPlace)
            ui.cbConcrete->setCheckState(Qt::Checked);

        switch (m_cursor) {
        case NoPlace: ui.cbConcrete->setCheckState(Qt::Unchecked); break;
        case PlaceConcrete: {
            ui.rbNoTarget->setChecked(true);
            ui.rbNoBlock->setChecked(true);
            ui.cbAppear->setCheckState(Qt::Unchecked);
            ui.cbItemOnStart->setCheckState(Qt::Unchecked);
            break;
        }
        case Target1: ui.rbTarget1->setChecked(true); break;
        case Target2: ui.rbTarget2->setChecked(true); break;
        case Block1: ui.rbBlock1->setChecked(true); break;
        case Block2: ui.rbBlock2->setChecked(true); break;
        case ItemAppear: ui.cbAppear->setCheckState(Qt::Checked); break;
        case ItemStart: ui.cbItemOnStart->setCheckState(Qt::Checked); break;
        case SelectMode: break;
        }
    }
}

//------------------------------------------
void MainWindow::on_mouseRightClicked(const QPoint &pos)
{
    if (m_cursor == SelectMode) {
        on_twLevel_customContextMenuRequested(pos);
        return;
    }

    QTableWidgetItem *twi = ui.twLevel->itemAt(pos);
    if (twi) {
        if (!twi->isSelected()) {
            QList<QTableWidgetItem *> sel = ui.twLevel->selectedItems();
            for (int i = 0; i < sel.count(); i++) {
                QTableWidgetItem *twisel = sel.at(i);
                if (!twisel) continue;
                twisel->setSelected(false);
            }
            twi->setSelected(true);
        }

        switch (m_cursor) {
        case NoPlace: ui.cbConcrete->setCheckState(Qt::Unchecked); break;
        case PlaceConcrete: {
            ui.rbNoTarget->setChecked(true);
            ui.rbNoBlock->setChecked(true);
            ui.cbAppear->setCheckState(Qt::Unchecked);
            ui.cbItemOnStart->setCheckState(Qt::Unchecked);
            break;
        }
        case Target1:
            if (ui.rbTarget1->isChecked())
                ui.rbNoTarget->setChecked(true);
            break;
        case Target2:
            if (ui.rbTarget2->isChecked())
                ui.rbNoTarget->setChecked(true);
            break;
        case Block1:
            if (ui.rbBlock1->isChecked())
                ui.rbNoBlock->setChecked(true);
            break;
        case Block2:
            if (ui.rbBlock2->isChecked())
                ui.rbNoBlock->setChecked(true);
            break;
        case ItemAppear: ui.cbAppear->setCheckState(Qt::Unchecked); break;
        case ItemStart: ui.cbItemOnStart->setCheckState(Qt::Unchecked); break;
        case SelectMode: break;
        }
    }
}

//------------------------------------------
void MainWindow::on_mouseReleased()
{
    if (m_undoafterrelease) {
        saveForUndo();
        m_undoafterrelease = false;
    }
}

//------------------------------------------
void MainWindow::on_twLevel_itemSelectionChanged()
{
    QList<QTableWidgetItem *> sel = ui.twLevel->selectedItems();

    ui.gbDraw->setEnabled(sel.count() <= 1);

    uint ordata = 0, anddata  = 0xFFFFFFFF;
    for (int i = 0; i < sel.count(); i++) {
        QTableWidgetItem *twi = sel.at(i);
        if (!twi) continue;
        uint data = twi->data(Qt::UserRole).toUInt();
        ordata |= data;
        anddata &= data;
    }

    Qt::CheckState cCon = anddata & PlaceConcrete ? Qt::Checked : (ordata & PlaceConcrete ? Qt::PartiallyChecked : Qt::Unchecked);
    Qt::CheckState cTarget1 = anddata & Target1 ? Qt::Checked : (ordata & Target1 ? Qt::PartiallyChecked : Qt::Unchecked);
    Qt::CheckState cTarget2 = anddata & Target2 ? Qt::Checked : (ordata & Target2 ? Qt::PartiallyChecked : Qt::Unchecked);
    Qt::CheckState cBlock1 = anddata & Block1 ? Qt::Checked : (ordata & Block1 ? Qt::PartiallyChecked : Qt::Unchecked);
    Qt::CheckState cBlock2 = anddata & Block2 ? Qt::Checked : (ordata & Block2 ? Qt::PartiallyChecked : Qt::Unchecked);
    Qt::CheckState cStart = anddata & ItemStart ? Qt::Checked : (ordata & ItemStart ? Qt::PartiallyChecked : Qt::Unchecked);
    Qt::CheckState cAppear = anddata & ItemAppear ? Qt::Checked : (ordata & ItemAppear ? Qt::PartiallyChecked : Qt::Unchecked);

    m_noupdate = true;
    ui.rbNoTarget->setAutoExclusive(false);
    ui.rbTarget1->setAutoExclusive(false);
    ui.rbTarget2->setAutoExclusive(false);
    ui.rbNoTarget->setChecked(cTarget1 == Qt::Unchecked && cTarget2 == Qt::Unchecked);
    ui.rbTarget1->setChecked(cTarget1 == Qt::Checked && cTarget2 == Qt::Unchecked);
    ui.rbTarget2->setChecked(cTarget1 == Qt::Unchecked && cTarget2 == Qt::Checked);
    ui.rbNoTarget->setAutoExclusive(true);
    ui.rbTarget1->setAutoExclusive(true);
    ui.rbTarget2->setAutoExclusive(true);

    ui.rbNoBlock->setAutoExclusive(false);
    ui.rbBlock1->setAutoExclusive(false);
    ui.rbBlock2->setAutoExclusive(false);
    ui.rbNoBlock->setChecked(cBlock1 == Qt::Unchecked && cBlock2 == Qt::Unchecked);
    ui.rbBlock1->setChecked(cBlock1 == Qt::Checked && cBlock2 == Qt::Unchecked);
    ui.rbBlock2->setChecked(cBlock1 == Qt::Unchecked && cBlock2 == Qt::Checked);
    ui.rbNoBlock->setAutoExclusive(true);
    ui.rbBlock1->setAutoExclusive(true);
    ui.rbBlock2->setAutoExclusive(true);

    ui.cbConcrete->setCheckState(cCon);
    ui.cbAppear->setCheckState(cAppear);
    ui.cbItemOnStart->setCheckState(cStart);
    m_noupdate = false;
}

//------------------------------------------
void MainWindow::on_twLevel_customContextMenuRequested(const QPoint &pos)
{
    if (m_cursor != SelectMode) {
        ui.tbSelect->click();
        setEditCursor(SelectMode);
        return;
    }

    if (contextEdit)
        delete contextEdit;
    contextEdit = new QMenu(0);

    contextEdit->addAction(actionUndo);
    contextEdit->addAction(actionRedo);
    contextEdit->addSeparator();
    contextEdit->addAction(actionCopy);
    contextEdit->addAction(actionPaste);
    contextEdit->addSeparator();
    contextEdit->addAction(actionSelectAll);

    contextEdit->popup(ui.twLevel->mapToGlobal(pos));
}

//------------------------------------------
void MainWindow::initLevel(int w, int h)
{
    ui.twLevel->setUpdatesEnabled(false);

    ui.twLevel->setColumnCount(w);
    ui.twLevel->setRowCount(h);

    for (int i = 0; i < w; i++)
        for (int j = 0; j < h; j++) {
            QTableWidgetItem *twi = ui.twLevel->item(j, i);
            if (!twi) {
                twi = new QTableWidgetItem(QIcon(),"");
                twi->setData(Qt::UserRole, uint(NoPlace));
                twi->setIcon(QIcon(cellPixmap(uint(NoPlace))));
                twi->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                ui.twLevel->setItem(j, i, twi);
            }
        }

    ui.twLevel->setUpdatesEnabled(true);
}

//------------------------------------------
void MainWindow::clearLevel()
{
    ui.twLevel->setUpdatesEnabled(false);
    ui.twLevel->setColumnCount(0);
    ui.twLevel->setRowCount(0);
    ui.twLevel->setUpdatesEnabled(true);
}

//------------------------------------------
void MainWindow::updateLevelMap()
{
    ui.twLevel->setUpdatesEnabled(false);

    for (int i = 0; i < LEVEL_WIDTH; i++)
        for (int j = 0; j < LEVEL_HEIGHT; j++) {
            QTableWidgetItem *twi = ui.twLevel->item(j, i);
            if (!twi) continue;
            uint data = twi->data(Qt::UserRole).toUInt();
            twi->setIcon(QIcon(cellPixmap(data)));
        }

    ui.twLevel->setUpdatesEnabled(true);
}

//------------------------------------------
void MainWindow::updateSelectedCells()
{
    ui.twLevel->setUpdatesEnabled(false);

    QList<QTableWidgetItem *> sel = ui.twLevel->selectedItems();
    for (int i = 0; i < sel.count(); i++) {
        QTableWidgetItem *twi = sel.at(i);
        if (!twi) continue;
        uint data = twi->data(Qt::UserRole).toUInt();

        if (ui.cbConcrete->checkState() == Qt::Checked)
            data |= PlaceConcrete;
        else if (ui.cbConcrete->checkState() == Qt::Unchecked)
            data &= ~PlaceConcrete;

        if (ui.rbNoTarget->isChecked())
            data &= ~Targets;
        else if (ui.rbTarget1->isChecked()) {
            data |= Target1;
            data &= ~Target2;
        }
        else if (ui.rbTarget2->isChecked()) {
            data |= Target2;
            data &= ~Target1;
        }

        if (ui.rbNoBlock->isChecked())
            data &= ~Blocks;
        else if (ui.rbBlock1->isChecked()) {
            data |= Block1;
            data &= ~Block2;
        }
        else if (ui.rbBlock2->isChecked()) {
            data |= Block2;
            data &= ~Block1;
        }

        if (ui.cbAppear->checkState() == Qt::Checked)
            data |= ItemAppear;
        else if (ui.cbAppear->checkState() == Qt::Unchecked)
            data &= ~ItemAppear;

        if (ui.cbItemOnStart->checkState() == Qt::Checked)
            data |= ItemStart;
        else if (ui.cbItemOnStart->checkState() == Qt::Unchecked)
            data &= ~ItemStart;

        twi->setData(Qt::UserRole, data);
        twi->setIcon(QIcon(cellPixmap(data)));
    }

    ui.twLevel->setUpdatesEnabled(true);

    setModified(true);

    if (m_cursor == SelectMode)
        saveForUndo();
    else if (ui.twLevel->isMousePressed())
        m_undoafterrelease = true;
}

//------------------------------------------
void MainWindow::loadSettings()
{
    QSettings settings("xlabsoft","LevelEditor");

    move(settings.value("X", 200).toUInt(), settings.value("Y", 100).toUInt());
    resize(settings.value("W", 770).toUInt(), settings.value("H", 892).toUInt());

    QFile f("config.dat");
    if (f.open(QIODevice::ReadOnly)) {
        QDataStream ds(&f);
        QByteArray ba;
        ds >> ba;
        restoreState(ba);
        f.close();
    }
}

//------------------------------------------
void MainWindow::saveSettings()
{
    QSettings settings("xlabsoft","LevelEditor");

    settings.setValue("X", x());
    settings.setValue("Y", y());
    settings.setValue("W", width());
    settings.setValue("H", height());

    QFile f("config.dat");
    if (f.open(QIODevice::WriteOnly)) {
        QDataStream ds(&f);
        ds << saveState();
        f.close();
    }
}

//------------------------------------------
void MainWindow::saveLevel(const QString &path)
{
    QFile f(path);
    if (f.open(QIODevice::WriteOnly)) {
        QDataStream ds(&f);

        // format signature
        ds << QString("SCN1");

        // items
        ds << quint32(ui.sbItems->value());

        // time
        ds << quint32(ui.sbTime->value());

        // level dimensions
        quint32 rows = ui.twLevel->rowCount(), cols = ui.twLevel->columnCount();
        ds << rows << cols;

        // cells
        for (uint i = 0; i < rows; i++)
            for (uint j = 0; j < cols; j++) {
                quint32 data = ui.twLevel->item(i, j)->data(Qt::UserRole).toUInt();
                if (!(data & PlaceConcrete))
                    data = NoPlace;
                ds << data;
            }

        f.close();
    }
}

//------------------------------------------
void MainWindow::loadLevel(const QString &path)
{
    QFile f(path);
    if (f.open(QIODevice::ReadOnly)) {
        QDataStream ds(&f);

        QString sig;
        // format signature
        ds >> sig;
        if (sig == "SCN1") {
            quint32 items, time, rows, cols;

            // items
            ds >> items;
            ui.sbItems->setValue(items);

            //time
            ds >> time;
            ui.sbTime->setValue(time);

            // level dimensions
            ds >> rows >> cols;
//            initLevel(cols, rows);
            initLevel(LEVEL_WIDTH, LEVEL_HEIGHT);

            // cells
            for (uint i = 0; i < rows; i++)
                for (uint j = 0; j < cols; j++) {
                    quint32 data;
                    ds >> data;
                    QTableWidgetItem *twi = ui.twLevel->item(i, j);
                    if (!twi)
                        twi = new QTableWidgetItem(QIcon(),"");
                    twi->setData(Qt::UserRole, uint(data));
                    twi->setIcon(QIcon(cellPixmap(uint(data))));
                    ui.twLevel->setItem(i, j, twi);
                }
        }
        f.close();
    }
}

//------------------------------------------
QPixmap MainWindow::concretePixmap()
{
    return QPixmap(":/images/concrete.png");
}

//------------------------------------------
QPixmap MainWindow::noplacePixmap()
{
    return QPixmap(":/images/noplace.png");
}

//------------------------------------------
QPixmap MainWindow::target1Pixmap()
{
    return QPixmap(":/images/target1.png");
}

//------------------------------------------
QPixmap MainWindow::target2Pixmap()
{
    return QPixmap(":/images/target2.png");
}

//------------------------------------------
QPixmap MainWindow::block1Pixmap()
{
    return QPixmap(":/images/block1.png");
}

//------------------------------------------
QPixmap MainWindow::block2Pixmap()
{
    return QPixmap(":/images/block2.png");
}

//------------------------------------------
QPixmap MainWindow::appearPixmap()
{
    return QPixmap(":/images/appear.png");
}

//------------------------------------------
QPixmap MainWindow::onstartPixmap()
{
    return QPixmap(":/images/onstart.png");
}

//------------------------------------------
QPixmap MainWindow::cellPixmap(uint data)
{
    QPixmap pix(concretePixmap());
    if (!(data & PlaceConcrete)) {
        pix = noplacePixmap();
    }
    else {
        QPainter p;
        p.begin(&pix);

        if (data & Target2)
            p.drawPixmap(0, 0, 56, 56, target2Pixmap());
        else if (data & Target1)
            p.drawPixmap(0, 0, 56, 56, target1Pixmap());

        if (data & ItemStart)
            p.drawPixmap(0, 0, 56, 56, onstartPixmap());

        if (data & Block2)
            p.drawPixmap(0, 0, 56, 56, block2Pixmap());
        else if (data & Block1)
            p.drawPixmap(0, 0, 56, 56, block1Pixmap());

        if (data & ItemAppear)
            p.drawPixmap(0, 0, 56, 56, appearPixmap());

        p.end();
    }
    return pix;
}

//------------------------------------------
void MainWindow::setModified(bool m)
{
    m_modified = m;
    updateWindowTitle();
}

//------------------------------------------
void MainWindow::updateWindowTitle()
{
    QString title = tr("JAG Level Editor v0.2.1 - ");
    title += (m_filename.isEmpty() ? tr("noname.scn") : m_filename);
    if (m_modified)
        title += "*";
    setWindowTitle(title);
    updateStatistics();
}

//------------------------------------------
void MainWindow::updateStatistics()
{
    int playable = 0, t1 = 0, t2 = 0, b1 = 0, b2 = 0;

    for (int i = 0; i < LEVEL_WIDTH; i++)
        for (int j = 0; j < LEVEL_HEIGHT; j++) {
            QTableWidgetItem *twi = ui.twLevel->item(j, i);
            if (!twi) continue;
            uint data = twi->data(Qt::UserRole).toUInt();
            if (data & PlaceConcrete) playable++;
            if (data & Target1) t1++;
            if (data & Target2) t2++;
            if (data & Block1) b1++;
            if (data & Block2) b2++;
        }

    ui.lStatistics->setText(tr("Playable cells: <b>%1 of %2</b><br>Targets: <b>%3 (%4 + %5)</b><br>Blocks: <b>%6 (%7 + %8)</b>").
                            arg(playable).arg(LEVEL_WIDTH*LEVEL_HEIGHT).arg(t1+t2).arg(t1).arg(t2).arg(b1+b2).arg(b1).arg(b2));
}

//------------------------------------------
void MainWindow::updateEditActions()
{
    actionPaste->setEnabled(clipsel.count() > 0);
    actionUndo->setEnabled(undoList.count() > 0 && m_undoindex >= 0);
    actionRedo->setEnabled(m_undoindex < (undoList.count() - 1) && m_undoindex >= -1);
}

//------------------------------------------
void MainWindow::clearUndo()
{
    undoList.clear();
    m_undoindex = -1;

    // prepare current state for the next undo
    prepareUndo();
}

//------------------------------------------
void MainWindow::prepareUndo()
{
    m_lastundo.cells.clear();

    m_lastundo.leveltime = ui.sbTime->value();
    m_lastundo.itemscount = ui.sbItems->value();
    m_lastundo.modified = m_modified;

    for (int i = 0; i < LEVEL_WIDTH; i++)
        for (int j = 0; j < LEVEL_HEIGHT; j++) {
            QTableWidgetItem *twi = ui.twLevel->item(j, i);
            if (!twi) continue;
            uint data = twi->data(Qt::UserRole).toUInt();
            LevelItem li(j, i, data);
            m_lastundo.cells.append(li);
        }

    updateEditActions();
}

//------------------------------------------
void MainWindow::saveForUndo()
{
    // if we did some undos
    while (m_undoindex < undoList.count() - 1 && undoList.count() > 0)
        undoList.removeLast();

    // avoid duplicating states
    if (!isUndoDuplicate()) {
        undoList.append(m_lastundo);
        m_undoindex = undoList.count() - 1;

        // prepare current state for the next undo
        prepareUndo();
    }

    updateStatistics();
}

//------------------------------------------
bool MainWindow::isUndoDuplicate()
{
    bool dup = false;
    if (m_undoindex >= 0) {
        dup = true;
        if (m_lastundo.itemscount != ui.sbItems->value())
            dup = false;
        if (m_lastundo.leveltime != ui.sbTime->value())
            dup = false;

        for (int i = 0; i < m_lastundo.cells.count(); i++) {
            QTableWidgetItem *twi = ui.twLevel->item(m_lastundo.cells.at(i).row, m_lastundo.cells.at(i).col);
            if (!twi) continue;
            uint data = twi->data(Qt::UserRole).toUInt();
            if (m_lastundo.cells.at(i).data != data) {
                dup = false;
                break;
            }
        }
    }
    return dup;
}

//------------------------------------------
QString MainWindow::checkPathForDialog(const QString &path)
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
QMessageBox::StandardButton MainWindow::confirmSave()
{
    return QMessageBox::question(this,
           tr("Save current level?"),
           tr("The current level is changed.\nAny unsaved data will be lost.\nDo you want to save the level?"),
           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
}

//------------------------------------------
void MainWindow::setEditCursor(quint32 cursor)
{
    QPixmap pix;
    switch (cursor) {
        case NoPlace: pix = noplacePixmap(); break;
        case PlaceConcrete: pix = concretePixmap(); break;
        case Target1: pix = target1Pixmap(); break;
        case Target2: pix = target2Pixmap(); break;
        case Block1: pix = block1Pixmap(); break;
        case Block2: pix = block2Pixmap(); break;
        case ItemStart: pix = onstartPixmap(); break;
        case ItemAppear: pix = appearPixmap(); break;
        case SelectMode: break;
    }

    while (QApplication::overrideCursor())
        QApplication::restoreOverrideCursor();

    if (cursor != SelectMode)
        QApplication::setOverrideCursor(QCursor(pix.scaled(32,32, Qt::IgnoreAspectRatio,
                                                           Qt::SmoothTransformation), 0, 0));

    ui.twLevel->setSelectionMode(cursor != SelectMode ?
                                 QAbstractItemView::SingleSelection :
                                 QAbstractItemView::ExtendedSelection);

    ui.gbProperties->setEnabled(m_cursor == SelectMode);
}

//------------------------------------------
void MainWindow::about()
{
    QMessageBox::information(0,
        tr("About Level Editor"),
        tr("<center>Level Editor for JAG by<br>XlabSoft and Industrial InfoSystems.<br>This software is licensed under LGPL license.</center>"),
        QMessageBox::Close);
}
