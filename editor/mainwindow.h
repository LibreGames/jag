#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "ui_mainwindow.h"
#include "levelwidget.h"

#include "defines.h"

const int LEVEL_WIDTH = 15;
const int LEVEL_HEIGHT = 12;

struct LevelItem
{
    LevelItem(int r, int c, uint d) : row(r), col(c), data(d) {}

    int row, col;
    uint data;
};

struct UndoData
{
    QList<LevelItem> cells;
    int leveltime, itemscount;
    bool modified;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_cbConcrete_stateChanged(int);
    void on_property_toggled(bool);
    void on_cbAppear_stateChanged(int);
    void on_cbItemOnStart_stateChanged(int);
    void on_sbTime_valueChanged(int);
    void on_sbItems_valueChanged(int);

    void on_drawButton_toggled(bool);

    void on_twLevel_itemSelectionChanged();
    void on_twLevel_customContextMenuRequested(const QPoint &);

    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileCreateLevelPack();
    void fileExtractLevelPack();
    void fileExit();
    void editShowProperties();
    void editSelectAll();
    void editCopy();
    void editPaste();
    void editUndo();
    void editRedo();
    void dockShowHide(bool);
    void about();

    void on_enteredWidget()  { if (m_cursor != SelectMode) setEditCursor(m_cursor); }
    void on_leavedWidget()   { if (m_cursor != SelectMode) setEditCursor(SelectMode); }
    void on_mouseClicked(const QPoint &);
    void on_mouseRightClicked(const QPoint &);
    void on_mouseReleased();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void initLevel(int, int);
    void clearLevel();
    void updateLevelMap();
    void updateSelectedCells();
    void updateWindowTitle();
    void updateStatistics();
    void updateEditActions();
    void setEditCursor(quint32 cursor);
    void setModified(bool m);

    void clearUndo();
    void prepareUndo();
    void saveForUndo();
    void applyUndoData(UndoData &ud);
    bool isUndoDuplicate();

    QString checkPathForDialog(const QString &path);
    QMessageBox::StandardButton confirmSave();

    void loadSettings();
    void saveSettings();
    void saveLevel(const QString &path);
    void loadLevel(const QString &path);

    QPixmap concretePixmap();
    QPixmap noplacePixmap();
    QPixmap target1Pixmap();
    QPixmap target2Pixmap();
    QPixmap block1Pixmap();
    QPixmap block2Pixmap();
    QPixmap appearPixmap();
    QPixmap onstartPixmap();
    QPixmap cellPixmap(uint data);

    QAction *actionSelectAll;
    QAction *actionCopy;
    QAction *actionPaste;
    QAction *actionUndo;
    QAction *actionRedo;
    QMenu *contextEdit;

    QList<LevelItem> clipsel;
    QList<UndoData> undoList;

    UndoData m_lastundo;
    int m_undoindex;

    Ui::MainWindow ui;
    bool m_modified, m_noupdate, m_undoafterrelease;
    QString m_filename;
    quint32 m_cursor;
};

#endif // MAINWINDOW_H
