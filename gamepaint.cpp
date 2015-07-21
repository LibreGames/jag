#include "gamescene.h"
#include "gameitem.h"
#include "baseitem.h"
#include "gametools.h"
#include "gamemenu.h"
#include "gamestock.h"
#include "gameprofile.h"
#include "gamebonus.h"
#include "gamebackground.h"
#include "gamestat.h"
#include "version.h"

#include <QTimer>

void GameScene::drawBackground(QPainter *painter, const QRectF &)
{
  if (myLock)
    return;

  QPainter &p(*painter);
  p.setRenderHint(QPainter::Antialiasing);

  paintState = true;

  p.setOpacity(1);

  if (menu->isActive() && !gameProfile->isGamePaused())
  {
    // background
    p.drawPixmap(0,0, gameStock->MenuBackground);

    //QTimer::singleShot(200, this, SLOT(update()));

//    p.setOpacity(1);
    p.drawPixmap(LEFT_CENTER(gameStock->GameLogoBig.width(),WIDTH),
                 10,
                 gameStock->GameLogoBig
                 );

    //version
    p.setFont(gameStock->Font8);
    p.setPen(QPen(Qt::white));
//    p.drawText(QRect(LEFT_CENTER(gameStock->GameLogoBig.width(), WIDTH), DY(120), gameStock->GameLogoBig.width(),20),
//               Qt::AlignRight | Qt::AlignTop,
//               QString("Version %1 [Qt %2]").arg(GAME_VERSION_STR, QT_VERSION_STR));
    p.drawText(DX(10),DY(20), QString("Version %1 [Qt %2]").arg(GAME_VERSION_STR, QT_VERSION_STR));

  }
  else
  {
    // background
    p.drawPixmap(0,0,bgpicture);

    // concrete & target layer
    if (level_start)
      drawLevelStartup(p);

    else
    {
      p.setOpacity(1);
      for (int i = 0; i < rows; i++)
      {
        int rowy = row2y(i);

        for (int j = 0; j < cols; j++)
        {
          PlaceInfo &pi = data(i,j);

          if (pi.empty())
            continue;

          int colx = col2x(j);

          if (pi.hasTarget())
            p.drawPixmap(colx, rowy, pi.place & Target1 ? target1 : target2);
        }
      }

      // items & blocks layer
      for (int i = 0; i < rows; i++)
      {
        int rowy = row2y(i);

        for (int j = 0; j < cols; j++)
        {
          PlaceInfo &pi = data(i,j);

          int colx = col2x(j);

          if (pi.hasItem())
          {
            // selection
            if (pi.item->isSelected())
            {
              p.setOpacity(0.3);
              p.setBrush(Qt::yellow);
    #if QT_VERSION >= 0x040400
              p.drawRoundedRect(colx+1, rowy+1, X56-2, Y56-2, 5, 5);
    #else
              p.drawRect(colx+1, rowy+1, X56-2, Y56-2);
    #endif
            }

            // item
            p.setOpacity(pi.item->opacity());
            p.drawPixmap(item_col2x(j) + pi.item->dx(), item_row2y(i) + pi.item->dy(),
                         allItems.at(pi.item->id()));
          }

          if (pi.hasBlock())
          {
            p.setOpacity(1);
            p.drawPixmap(colx, rowy, pi.place & Block1 ? block1 : block2);
          }

          if (toolset->current())
          {
            GameTool::ToolAction state = toolset->checkItemState(i, j);
            if (state != GameTool::ToolOutOfRange)
            {
              p.setPen(QPen(Qt::white, 2, Qt::DotLine));
  //            p.setOpacity(0.5);
  //            p.drawRect(col2x(j), row2y(i), 56, 56);
              p.setOpacity(0.3);
              p.setBrush(state & GameTool::ToolActive ? Qt::blue : Qt::red);
    #if QT_VERSION >= 0x040400
              p.drawRoundedRect(colx+1, rowy+1, X56-2, Y56-2, 5, 5);
    #else
              p.drawRect(colx+1, rowy+1, X56-2, Y56-2);
    #endif
            }
          }

        }
      }

    }

    // hint layer
    drawHint(p);

    // HUD layer
    updateCounters(p);
    // tools
    toolset->updateTools(p);
    // bonus
    gameBonus->updateItems(this, p);

    // temp layer
    for (int i = 0; i < tempItems.count(); i++)
    {
      tempItems.at(i)->paint(p);
    }

    // text hint
    if (hintText.length() && !stat_active)
    {
      drawTextHint(p);
    }

    // bonus window
    if (gameBonus->isActive())
    {
      gameBonus->drawBonusMenu(this, p);
    }

    // statistics
    if (stat_active)
    {
      drawStatInfo(p);
    }

  }

  // menu
  if (menu->isActive())
  {
    if (gameProfile->isGamePaused())
    {
      p.setOpacity(1);
      p.setPen(QPen(Qt::black));
      p.setBrush(QBrush(Qt::black));

      // background
      p.setOpacity(0.7);
      p.drawRect(0, 0, WIDTH, HEIGHT);
    }

      // logo
      p.setOpacity(0.7);

      if (gameProfile->isGameStarted())
      {
        p.drawPixmap(LEFT_CENTER(gameStock->GameLogoBig.width(),WIDTH),
                     10,
                     gameStock->GameLogoBig
                     );
      }

      // user name and info
      p.setBrush(QBrush(QColor(56,56,56)));
      p.setOpacity(0.5);
#if QT_VERSION >= 0x040400
      p.drawRoundedRect(DX(1024 - 750)/2,DY(650),DX(750),DY(55), 5,5);
#else
      p.drawRect(DX(1024 - 750)/2,DY(650),DX(750),DY(55));
#endif

      p.setOpacity(0.7);
      p.setFont(gameStock->Font8);
      p.setPen(QPen(Qt::white));
      p.drawText(DX(1024 - 750)/2,DY(650), DX(150),Y20, Qt::AlignCenter, tr("Player"));
      p.drawText(DX(1024 - 800)/2+DX(200),DY(650), DX(200),Y20, Qt::AlignCenter, tr("Level Pack"));
      p.drawText(DX(1024)/2 + X100,DY(650), X100,Y20, Qt::AlignCenter, tr("Level"));
      p.drawText(DX(1024)/2 + DX(200),DY(650), DX(150),Y20, Qt::AlignCenter, tr("Score"));

      PlayerInfo *pl = gameProfile->currentPlayer();
      LevelPackInfo *lpi = pl->currentLevelPackInfo();

      p.setFont(gameStock->Font20);
      p.drawText(menu->buttonProfile, Qt::AlignCenter, QString("%1").arg(pl->name));
      p.drawText(menu->buttonLevelpack, Qt::AlignCenter, QString("%1").arg(gameProfile->levelPackName(pl->currentlevelpack)));
      int lvl = (lpi->level > max_level) ? max_level : lpi->level;
      p.drawText(DX(1024)/2 + X100,DY(670), X100,DY(30), Qt::AlignCenter, QString("%1/%2").arg(lvl).arg(max_level));
      p.drawText(DX(1024)/2 + DX(200),DY(670), DX(150),DY(30), Qt::AlignCenter, QString("%1").arg(lpi->score));

      p.setOpacity(1);
  }

  paintState = false;
}


void GameScene::updateCounters(QPainter &p)
{
  // global counters
  p.setPen(QPen(Qt::white));
  p.setOpacity(1);

  p.setFont(gameStock->Font20);
  p.drawText(X910,DY(50), QString("%1").arg(score, 7, 10, QChar('0')));
  p.drawText(X910,DY(150), QString::number(level));
  p.setPen(QPen(time < 30 ? Qt::red : time < 60 ? Qt::yellow : Qt::white));
  p.drawText(X910,DY(100), QString("%1:%2").arg(time/60, 2, 10, QChar('0')).arg(time%60, 2, 10, QChar('0')));

  p.setPen(QPen(bonus > 1 ? Qt::white : Qt::gray));
  p.drawText(X910,DY(200), QString("x%1").arg(bonus));
  if (bonus_time)
  {
    p.setPen(QPen(bonus_time > 7 ? Qt::green : bonus_time > 3 ? Qt::yellow : Qt::red));
    p.drawText(DX(970),DY(180),DX(46),DY(20),
                      Qt::AlignVCenter | Qt::AlignRight,
                      QString("%1").arg(bonus_time, 2, 10, QChar('0')));
  }
}

void GameScene::drawHint(QPainter &p)
{
  static const QColor colors[] =
  { Qt::blue, Qt::yellow, Qt::red, Qt::green, Qt::yellow, Qt::cyan, Qt::magenta };
  static int idx1 = qrand() % 7, idx2 = qrand() % 7;

  if (hintMove && availFrom.x() >= 0 && availTo.x() >= 0)
  {
    p.setPen(QPen(colors[idx1], 4));
    p.setBrush(colors[idx2]);
    p.setOpacity(hintOpacity);
    hintOpacity += hintDelta;
    if (hintOpacity >= 0.5 || hintOpacity <= 0)
    {
      hintDelta = -hintDelta;
      if (hintDelta > 0)
      {
        idx1 = qrand() % 7;
        idx2 = qrand() % 7;
      }
    }

#if QT_VERSION >= 0x040400
    p.drawRoundedRect(col2x(availFrom.x())+1, row2y(availFrom.y())+1, X56-2, Y56-2, 5, 5);
    p.drawRoundedRect(col2x(availTo.x())+1, row2y(availTo.y())+1, X56-2, Y56-2, 5, 5);
#else
    p.drawRect(col2x(availFrom.x())+1, row2y(availFrom.y())+1, X56-2, Y56-2);
    p.drawRect(col2x(availTo.x())+1, row2y(availTo.y())+1, X56-2, Y56-2);
#endif
  }
}

////////////////////////////////////////////////////////////////////////////////

void GameScene::drawLevelStartup(QPainter &p)
{
  static int cnt_init = rows*cols/4;
  static int cnt_inc = 4;
  static int cnt = cnt_init;
  static int c2 = cols/2-1;
  //static int r2 = rows/2-1;

  for (int i = 0; i < rows; i++)
  {
    int xoff = -c2*cnt*cnt_inc;
    int rowy = row2y(i);

    for (int j = 0; j < cols; j++)
    {
      if (i >= ls_y && j >= ls_x)
        goto _brak;

      PlaceInfo &pi = data(i,j);

      if (!pi.empty())
      {
        int colx = col2x(j)+xoff;

        p.setOpacity(0.8);
        p.drawPixmap(colx, rowy, concrete);

        if (pi.hasTarget()) {
          p.setOpacity(1);
          p.drawPixmap(colx, rowy, pi.place & Target1 ? target1 : target2);
        }

        if (pi.hasItem())
        {
          p.setOpacity(pi.item->opacity());
          p.drawPixmap(item_col2x(j)+xoff + pi.item->dx(), item_row2y(i) + pi.item->dy(),
                         allItems.at(pi.item->id()));
        }

        if (pi.hasBlock())
        {
          p.setOpacity(1);
          p.drawPixmap(colx, rowy, pi.place & Block1 ? block1 : block2);
        }
      }

      xoff+=cnt*cnt_inc;
    }
  }

_brak:

  if (cnt) cnt--;

  ls_x += 4;
  if (ls_x >= cols)
  {
    ls_x = 0;
    if (++ls_y >= rows)
    {
      ls_y = 0;
      cnt = cnt_init;
      level_start = false;
      timeTimer->start();
      enableInput();

      // force draw concretes on bg
      drawConcretesOnBackground();
      return;
    }
  }
}

void GameScene::drawConcretesOnBackground()
{
    QPainter p(&bgpicture);

    p.setOpacity(0.8);
    for (int i = 0; i < rows; i++)
    {
        int rowy = row2y(i);

        for (int j = 0; j < cols; j++)
        {
          PlaceInfo &pi = data(i,j);

          if (pi.empty())
            continue;

          int colx = col2x(j);

          p.drawPixmap(colx, rowy, concrete);
        }
    }
}

void GameScene::drawHUDonBackground()
{
    QPainter p(&bgpicture);

    p.setOpacity(0.3);
    p.setPen(QPen(Qt::white, 2));
    p.setBrush(Qt::black);
#if QT_VERSION >= 0x040400
    p.drawRoundedRect(X900-1,0+1,DX(1024-900),DY(210), 5,5);
    p.drawRoundedRect(X900-1,DY(240)-1,DX(1024-900),DY(768-240), 5,5);
    p.drawRoundedRect(0+1,DY(710)-1,DX(860),DY(58), 5,5);
#else
    p.drawRect(900-1,0+1,1024-900,210);
    p.drawRect(900-1,240-1,1024-900,768-240);
    p.drawRect(0+1,710-1,860,58);
#endif

  p.setPen(QPen(Qt::white));
  p.setOpacity(1);

  p.setFont(gameStock->Font8);
  p.drawText(X910,DY(10), X100,Y20, Qt::AlignLeft, tr("Score"));
  p.drawText(X910,DY(60), X100,Y20, Qt::AlignLeft, tr("Time"));
  p.drawText(X910,DY(110), X100,Y20, Qt::AlignLeft, tr("Level"));
  p.drawText(X910,DY(160), X100,Y20, Qt::AlignLeft, tr("Bonus"));

  p.setFont(gameStock->Font12);
  p.drawText(X900,DY(600), DX(1024-900),DY(30), Qt::AlignCenter, tr("Bonus"));
}
