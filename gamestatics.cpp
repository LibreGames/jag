#include "scene_if.h"
#include "baseitem.h"

#include "gamestock.h"
#include "gamesound.h"
#include "gamebonus.h"
#include "gametools.h"

#include <QApplication>

IScene::IScene() :
    time(0),
    bonus(0), bonus_time(0),
    targets(0),
    max_level(0)
{
  scene = this;
}

IScene * scene = 0;

////////////////////////////////////////////////////////////////////////////////

void IScene::createScorePopup(int x, int y, int score)
{
  createStaticPopup(QRect(x,y,X56,Y56),
                  QString::number(score),
                  Qt::AlignCenter,
                  gameStock->Font12,
                  Qt::white,
                  1,
                  0, 20,
                  0, -1);
}

void IScene::createPixmapPopup(int x, int y, int dx, int dy, const QPixmap &pm, int steps)
{
  PixmapItem *bi = new PixmapItem(x,y,dx,dy,pm,steps);
  tempItems.append(bi);
}

void IScene::createStaticPopup(QRect rect, const QString &text, int textFlags,
                       const QFont &font, QColor color,
                       qreal opacity, int staysteps, int steps,
                       int dx, int dy)
{
  TextItem *ti = new TextItem(rect,
                              text,
                              textFlags,
                              font,
                              color,
                              opacity,
                              staysteps,
                              steps,
                              dx, dy);
  tempItems.append(ti);
}

////////////////////////////////////////////////////////////////////////////////

void IScene::removeBlock(PlaceInfo &pi, int row, int col)
{
  bool b1 = true;

  if (pi.place & Block2)
  {
    b1 = false;
    pi.place = (pi.place & ~Block2) | Block1;
  }
  else
  {
    pi.place = (pi.place & ~Block1);
  }

  sndEngine->playSound(GameSound::sndUnblock);

  // create ghost
  createPixmapPopup(col2x(col), row2y(row), (qrand() % 3)-1, 10 + (qrand() % 5)-2,
                              block1,
                              50);
}

void IScene::removeTarget(PlaceInfo &pi, int row, int col)
{
  bool t1 = true;

  if (pi.place & Target2)
  {
    t1 = false;
    pi.place = (pi.place & ~Target2) | Target1;
  }
  else
    pi.place = (pi.place & ~Target1);

//  sndEngine->playSound(GameSound::sndTarget);

  // create ghost
  createPixmapPopup(col2x(col), row2y(row), (qrand() % 3)-1, 10 + (qrand() % 5)-2,
                              t1 ? target1: target2,
                              50);

//  // update bonus
//  bonus++;
//  bonus_time = 10 + gameBonus->bonusSpeed();
//  bonusTimer->start();
}

void IScene::removeAndCountItem(int row, int col)
{
  PlaceInfo &pi = data(row,col);

  // it was already scheduled and counted
  if (pi.isProcessed())
    return;

  pi.setProcessed();

  bool remove = true;
  int upscore = 0;

  if (pi.hasBlock())
  {
    remove = false;

    removeBlock(pi, row, col);
  }
  else
  if (pi.hasTarget())
  {
    targets--;

    // score for target
    upscore = TARGET_SCORE + toolset->bonusScore();

    removeTarget(pi, row, col);
  }
  else
  if (!pi.item->isNotAlive())   // do not count again
  {
    upscore = ITEM_SCORE;  // + toolset->bonusScore();
  }

  if (upscore)
  {
    // increase score
    upscore *= bonus;

    score += upscore;
    toolset->addScore(upscore);

    // add score popup
    createScorePopup(col2x(col), row2y(row), upscore);
  }

  if (remove && !pi.item->isNotAlive()) // do not remove again
  {
    gameBonus->addItemScore(pi.item->id(), ITEM_SCORE);

    pi.item->scheduleDeath();
  }
}

void IScene::removeAndCountItemOnly(int row, int col)
{
  PlaceInfo &pi = data(row,col);

  // it was already scheduled and counted
  if (pi.isProcessed())
    return;

  pi.setProcessed();

  if (!pi.item->isNotAlive())   // do not count again
  {
    // increase score
    int upscore = ITEM_SCORE; // + toolset->bonusScore();

    score += upscore;
    toolset->addScore(upscore);

    // add score popup
    createScorePopup(col2x(col), row2y(row), upscore);

    gameBonus->addItemScore(pi.item->id(), ITEM_SCORE);

    pi.item->scheduleDeath();
  }
}

////////////////////////////////////////////////////////////////////////////////

void IScene::addTime(int timeAdd)
{
  time += timeAdd;

  createStaticPopup(QRect(DX(975), DY(75), DX(100), DY(30)),
                  QString("+%1").arg(timeAdd),
                  Qt::AlignLeft | Qt::AlignTop,
                  gameStock->Font20,
                  Qt::yellow,
                  1,
                  0, 20,
                  0, -1
                  );
}

////////////////////////////////////////////////////////////////////////////////

void IScene::setDefaultGameCursor()
{
  QApplication::setOverrideCursor(gameStock->GameCursor);
}

void IScene::restoreCursor()
{
    lastCursor = QApplication::overrideCursor() ? (* QApplication::overrideCursor()) : QCursor();
    while (QApplication::overrideCursor())
        QApplication::restoreOverrideCursor();

    setDefaultGameCursor();
}

////////////////////////////////////////////////////////////////////////////////

void IScene::drawTransRect(QPainter &p, const QRect &r, QColor borderColor, QColor bgColor, qreal op)
{
    p.setOpacity(op);
    p.setPen(QPen(borderColor, 2));
    p.setBrush(bgColor);
#if QT_VERSION >= 0x040400
    p.drawRoundedRect(r, 5,5);
#else
    p.drawRect(r);
#endif
}

void IScene::drawTextHint(QPainter &p)
{
  p.setOpacity(1);
  p.setPen(QPen(Qt::white));
  p.setFont(gameStock->Font12);
  p.drawText(0,DY(690),DX(860),DY(30), Qt::AlignRight, hintText);
}
