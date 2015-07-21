#include "gamescene.h"
#include "gameprofile.h"
#include "gamestock.h"
#include "gamestat.h"
#include "gamebonus.h"
#include "gametools.h"
#include "gamesound.h"

StatInfo::StatInfo()
{
  tool = 0;

  initGraphics();
}

void StatInfo::initGraphics()
{
  buttons.clear();

  buttonOk = QRect(DX(450),DY(600),DX(300),DY(50));
  buttonCancel = QRect(DX(100),DY(600),DX(300),DY(50));

  buttons << buttonOk << buttonCancel;
}

void GameScene::showStatictics(STAT_CODE code)
{
  stat->tool = 0;
  stat->code = code;
  stat_active = true;

  advanceTimer->start();
  enableInput();

  if (code == STAT_LEVEL_WON || code == STAT_GAME_WON)
  {
    QTimer::singleShot(1000, this, SLOT(bonusTimeLeft()));

    LevelPackInfo *lpi = gameProfile->currentPlayer()->currentLevelPackInfo();
    lpi->localScore += time * BONUS_FOR_SECOND;
    lpi->score += time * BONUS_FOR_SECOND;
//    lpi->next_tool = toolset->activeToolIndex(lpi->score);

    // every 2nd level give a bonus
    if ((level-1) % 2 == 1 && !lpi->isPuzzle())
    {
      toolset->nextToolActivated();
      int idx = toolset->nextToolIndex();
      lpi->next_tool = idx;
      if (idx != ALL_TOOLS)
        stat->tool = toolset->tool(idx-1);
    }
    return;
  }
}

void GameScene::bonusTimeLeft()
{
  if (!stat_active)
    return;

  score += BONUS_FOR_SECOND;
  toolset->addScore(BONUS_FOR_SECOND);

  sndEngine->playSound(GameSound::sndBonus);

  if (time > 0) {
    --time;
    QTimer::singleShot(10, this, SLOT(bonusTimeLeft()));
  }
}

void GameScene::checkStatMouseActions(const QPoint &pos)
{
  if (stat->buttonCancel.contains(pos))
  {
    exitToMainMenu();
  }
  else
  if (stat->buttonOk.contains(pos))
  {
    runNextLevel();
  }
}

void GameScene::drawStatInfo(QPainter &p)
{
  drawTransRect(p, QRect(0+1,0+1,DX(860)-1,DY(700)-1), Qt::white, Qt::black, 0.9);

  p.setOpacity(1);

  p.setFont(gameStock->Font40);


  const int y30 = DY(30);

  if (stat->code == STAT_LEVEL_WON)
  {
    p.drawText(0,0,DX(860),DY(80), Qt::AlignHCenter | Qt::AlignBottom,
               tr("Level %1 complete!").arg(level));

    p.setFont(gameStock->Font20);

    const int x1 = DX(200), x2 = DX(300), x3 = DX(600);
    const int y1 = DY(150), y2 = DY(250), y3 = DY(350);

    p.drawPixmap(x1,y1, gameStock->Clock);
    p.drawText(x3,y1+y30, QString("%1:%2").arg(time/60, 2, 10, QChar('0')).arg(time%60, 2, 10, QChar('0')));

    p.drawPixmap(x1,y2, gameStock->Score);
    p.drawText(x3,y2+y30, QString::number(/* stat->level_score */ score));

    p.drawPixmap(x1,y3, gameStock->Items.at(0));
    p.drawText(x3,y3+y30, QString::number(gameBonus->collectedOnLevel()));

    p.setPen(QPen(Qt::yellow));
    p.drawText(x2,y1+y30, tr("Time left:"));
    p.drawText(x2,y2+y30, tr("Score:"));
    p.drawText(x2,y3+y30, tr("Items collected:"));

    if (stat->tool)
    {
      p.setPen(QPen(Qt::white));
      p.drawText(QRect(0,DY(500),DX(900),y30), Qt::AlignCenter,
                 tr("You got new %1: %2")
                 .arg(stat->tool->type() == GameTool::ConstTool ? tr("bonus") : tr("tool"))
                 .arg(stat->tool->info()->name)
                 );
      p.setFont(gameStock->Font12);
      p.drawText(QRect(0,DY(530),DX(900),y30), Qt::AlignCenter,
                 stat->tool->info()->comment
                 );
    }
  }
  else if (stat->code == STAT_LEVEL_FAIL) // level failed
  {
    p.drawText(0,0,DX(860),DY(80), Qt::AlignHCenter | Qt::AlignBottom,
               tr("Level %1 failed!").arg(level));

    int percent = 0;
    if (stat->level_targets_all > 0)
        percent = (stat->level_targets_all - stat->level_targets_left)*100/stat->level_targets_all;
    p.setFont(gameStock->Font20);
    p.drawText(0,DY(300),DX(860),DY(50), Qt::AlignCenter,
               tr("You have completed %1% of level %2.").arg(percent).arg(level));
    p.drawText(0,DY(350),DX(860),DY(50), Qt::AlignCenter,
               tr("Next time you'll be more lucky!"));
  }
  else { // game won
    p.drawText(0,0,DX(860),DY(80), Qt::AlignHCenter | Qt::AlignBottom,
               tr("Congratulations!"));

    p.setFont(gameStock->Font20);

    p.drawText(0,DY(100),DX(860),DY(50), Qt::AlignCenter,
               tr("You have completed all the levels and won the game!"));

    const int x1 = DX(200), x2 = DX(300), x3 = DX(600);
    const int y2 = DY(300), y3 = DY(400);

    p.drawPixmap(x1,y2, gameStock->Score);
    p.drawText(x3,y2+y30, QString::number(score));

    p.drawPixmap(x1,y3, gameStock->Items.at(0));
    p.drawText(x3,y3+y30, QString::number(gameBonus->collectedTotal()));

    p.setPen(QPen(Qt::yellow));
    p.drawText(x2,y2+y30, tr("Total score:"));
    p.drawText(x2,y3+y30, tr("Items collected:"));
  }

  // ok button
  if (stat->code != STAT_GAME_WON)
  {
    drawTransRect(p, stat->buttonOk, Qt::white, Qt::darkGray,
                  stat->buttonOk.contains(cursorPosition()) ? 0.3 : 0.1
                  );

    p.setOpacity(1);
    p.setPen(QPen(Qt::white));
    p.setFont(gameStock->Font20);
    p.drawText(stat->buttonOk, Qt::AlignCenter, stat->code == STAT_LEVEL_WON ? tr("Continue") : tr("Try Again"));
  }

  // cancel button
  stat->buttonCancel = (stat->code == STAT_GAME_WON) ?
                       QRect(DX(280),DY(600),DX(300),DY(50)) :
                       QRect(DX(100),DY(600),DX(300),DY(50));

  drawTransRect(p, stat->buttonCancel, Qt::white, Qt::darkGray,
                stat->buttonCancel.contains(cursorPosition()) ? 0.3 : 0.1
                );

  p.setOpacity(1);
  p.setPen(QPen(Qt::white));
  p.setFont(gameStock->Font20);
  p.drawText(stat->buttonCancel, Qt::AlignCenter, tr("Exit to Menu"));
}
