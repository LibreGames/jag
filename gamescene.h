#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QDebug>

#include <QGraphicsScene>
#include <QGLWidget>
#include <QDesktopWidget>
#include <QSet>
#include <QTime>
#include <QCloseEvent>
#include <QDir>

#include "defines.h"
#include "gameitem.h"
#include "scene_if.h"

class BaseItem;
class MenuWidget;
class ConfirmDialog;
class GameTool;
class ToolSet;
class GameBonus;


class GameScene : public QGraphicsScene, virtual public IScene
{
  Q_OBJECT

  friend class GameWidget;

public:
    enum MoveState { MS_IDLE, MS_SELECTED, MS_MOVING, MS_RETURN };

    GameScene(QWidget *parent);
    virtual ~GameScene();

    void initGame();
    bool initLevel(int level);
    void initTheme();

    inline const QPoint cursorPosition() const { return cursorPos.toPoint(); }

    inline MenuWidget* gameMenu() const { return menu; }

    // internals
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent (QKeyEvent *keyEvent );

    // timers
    void disableTimersButAdvance();
    void disableTimers();
    void enableTimers();

    void wait(int ms);

    bool confirmExit();

public slots:
    void pauseGame();
    void continueGame();
    void stopGame();

protected slots:
    void nextCycle();
    void checkMouseActions();

    void executeMovement();
    bool checkItemMovement();

    void hintAvailableMoves();

    void countTime();
    void countBonusTime();
    void bonusTimeLeft();

    void levelWon();
    void levelFailed();

    void initProfile();
    void initGraphics();

    void on_menuNew();
    void on_menuContinue();
    void on_menuExit();
    void on_menuPauseBack();
    void on_menuRestartLevel();
    void on_menuAbandonGame();
    void on_menuThemeChanged();
    void on_menuGameStart();
    void on_menuLevelPack();

protected:
    virtual void drawBackground(QPainter *painter, const QRectF &);
    void updateCounters(QPainter &p);
    void drawHint(QPainter &p);
    void drawLevelStartup(QPainter &p);
    void drawHUDonBackground();
    void drawConcretesOnBackground();

    enum STAT_CODE { STAT_LEVEL_WON, STAT_LEVEL_FAIL, STAT_GAME_WON };
    void showStatictics(STAT_CODE code);
    void drawStatInfo(QPainter &p);
    void checkStatMouseActions(const QPoint &pos);

    bool loadLevel(int index);
    bool loadScene(const QByteArray &ba);

    void selectActiveItems(int items);

    bool fillEmptyFreeRows();
    GameItem* generateGameItem(const QSet<int> &skip = QSet<int>());

    void refillItems(int deep);
    bool checkAvailableMoves();
    bool checkAvailItemsInRow(struct PlaceAvailInfo *pl, int row, int from, int to);
    bool checkAvailItemsInColumn(struct PlaceAvailInfo *pl, int col, int from, int to);

    bool checkRemoveItems();
    bool checkRemoveItemsInRow(int row, int from, int to, bool remove=true);
    bool checkRemoveItemsInColumn(int col, int from, int to, bool remove=true);
    void removeItemsColumn(int row, int col, int cnt);
    void removeItemsRow(int row, int col, int cnt);
    void removeScheduledItems();

    bool checkFallItems();
    void scheduleItemMove(int row1, int col1, int row2, int col2);

    void unsetCurrentItem();

    void advanceAndWait(int cycles);

    void advanceTempItems();

    void runNextLevel();
    void exitToMainMenu();

    void disableInput() { inputDisabled = true; }
    void enableInput() { inputDisabled = false; }

    QString getSchemePath() const;

    void loadRandomBackground();

    volatile bool myLock;
    volatile bool paintState;
    volatile bool inputDisabled;

    GameItem *currentItem;
    int currentRow, currentCol;
    int targetRow, targetCol;
    QPointF lastClickPos, cursorPos;
    MoveState moveState;

    QPoint availFrom, availTo;
    bool hintMove;
    qreal hintOpacity, hintDelta;

    QTimer *advanceTimer, *timeTimer, *bonusTimer, *hintTimer;
    int frameCount;

    MenuWidget *menu;
    ConfirmDialog *dconfirm;

    struct StatInfo *stat;
    bool stat_active;

    bool level_start;
    int ls_x, ls_y;
};



//#define DATA(r,c) data[(r)*cols+(c)]



#endif // GAMESCENE_H
