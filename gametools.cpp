#include "gametools.h"

#include "scene_if.h"
#include "gamewidget.h"
#include "gamestock.h"
#include "gamebonus.h"
#include "gameprofile.h"
#include "gamesound.h"

#include "hammertool.h"
#include "bighammertool.h"
#include "smallhammertool.h"
#include "bombtool.h"
#include "thundertool.h"
#include "clocktool.h"
#include "randomkilltool.h"
#include "mixertool.h"
#include "unblocktool.h"
#include "twintool.h"

#include "consttools.h"

ToolSet * toolset = 0;

////////////////////////////////////////////////////////////////////////////////

#define TOOL_STAGES 20

int GameTool::stage;

GameTool::GameTool(int x, int y, int score) :
    myPos(x,y),
    myScore(score),
    myToolset(0),
    myProgress(0),
    myActive(false)
{
}

GameTool::GameTool(int x, int y, int score, const QString &resource) :
    myPos(x,y),
    myScore(score),
    myToolset(0),
    myProgress(0),
    myActive(false)
{
  myPixmap = QPixmap(GameWidget::getResourcePath() + "/tools/" + resource + ".png")
             .scaled(DX(48),DY(48), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

GameTool::~GameTool()
{
}

void GameTool::setCurrent()
{
  stage = TOOL_STAGES;
}

void GameTool::advanceAndPaint(QPainter &p, int currentScore)
{
  if (!myActive)
  {
//    if (currentScore >= myScore && !myToolset->isPuzzle())
//    {
//      setActive(true);
//      animateActivation();
//      myToolset->nextToolActivated();
//    }
    return;
  }

  p.setOpacity(currentScore >= myScore ? 1 : 0.3);

  p.setPen(QPen(Qt::white));

  p.drawText(QRect(myPos.x(), myPos.y()-DY(12), DX(48), DY(20)),
             Qt::AlignHCenter | Qt::AlignTop, QString::number(myScore));

  if (myToolset->current() == this)
  {
    p.setOpacity((1.0/TOOL_STAGES) * stage);
    if (!--stage) stage = TOOL_STAGES;
  }

  p.drawPixmap(myPos, myPixmap);
}

void GameTool::animateActivation()
{
  scene->createPixmapPopup(myPos.x(), myPos.y(), -1, -1, myPixmap, 20);
  scene->createPixmapPopup(myPos.x(), myPos.y(), 1, 1, myPixmap, 20);
  scene->createPixmapPopup(myPos.x(), myPos.y(), -1, 1, myPixmap, 20);
  scene->createPixmapPopup(myPos.x(), myPos.y(), 1, -1, myPixmap, 20);
}

QString GameTool::hint() const
{
  return QString("%1: %2").arg(bonusInfo->name, bonusInfo->comment);
}

////////////////////////////////////////////////////////////////////////////////

ToolSet::ToolSet() : toolScore(0), currentTool(0)
{
  initGraphics();

  // active by default
  smallHammerTool->setActive();
  unblockTool->setActive();
}

ToolSet::~ToolSet()
{
  qDeleteAll(tools);
}

int ToolSet::bonusClock() const { return clockBonus->value(); }
int ToolSet::bonusScore() const { return scoreBonus->value(); }
int ToolSet::bonusTimer() const { return timerBonus->value(); }
int ToolSet::bonusMag() const { return magBonus->value(); }

void ToolSet::initGraphics()
{
  int magBonusVal = 0;
  int timerBonusVal = 0;
  int clockBonusVal = 0;
  int scoreBonusVal = 0;

  QList<bool> activ;

  if (tools.count())
  {
    magBonusVal = magBonus->value();
    timerBonusVal = timerBonus->value();
    clockBonusVal = clockBonus->value();
    scoreBonusVal = scoreBonus->value();

    for (int i = 0; i < tools.count(); i++)
      activ.append(tools.at(i)->isActive());

    qDeleteAll(tools);
    tools.clear();
  }

  int x1 = DX(906), x2 = DX(965);
  int y = DY(290), dy = DY(65);

  smallHammerTool = new SmallHammerTool(x1,y, 1000);

  clockTool = new ClockTool(x2,y, 1500);

  y += dy;

  mixerTool = new MixerTool(x1,y, 2000);

  unblockTool = new UnblockTool(x2,y, 2500);

  y += dy;

  hammerTool = new HammerTool(x1,y, 3000);

  bigHammerTool = new BigHammerTool(x2,y, 3500);

  y += dy;

  bombTool = new BombTool(x1,y, 4000);

  randomKillTool = new RandomKillTool(x2,y, 4500);

  y += dy;

  thunderTool = new ThunderTool(x1,y, 5000);

  twinTool = new TwinTool(x2,y, 5500);


  // permanent tools
  y += DY(100);
  magBonus = new MagBonus(x1,y, 10000);
  timerBonus = new TimerBonus(x2,y, 12000);
  y += dy;
  clockBonus = new ClockBonus(x1,y, 15000);
  scoreBonus = new ScoreBonus(x2,y, 20000);

  // order in which the tools are given
  tools.append(smallHammerTool);
  tools.append(clockTool);
  tools.append(mixerTool);
  tools.append(unblockTool);
  tools.append(hammerTool);
  tools.append(bigHammerTool);
  tools.append(bombTool);
  tools.append(randomKillTool);
  tools.append(thunderTool);
  tools.append(twinTool);

  tools.append(magBonus);
  tools.append(timerBonus);
  tools.append(clockBonus);
  tools.append(scoreBonus);

  // update bonus values
  magBonus->setValue(magBonusVal);
  timerBonus->setValue(timerBonusVal);
  clockBonus->setValue(clockBonusVal);
  scoreBonus->setValue(scoreBonusVal);

  for (int i = 0; i < tools.count(); i++)
  {
    GameTool *tool = tools.at(i);
    tool->setBase(this);

    if (i < activ.count())
      tool->setActive(activ.at(i));
  }
}

void ToolSet::readProfile(LevelPackInfo *lpi)
{
  next_tool = lpi->next_tool;
  toolScore = lpi->localScore;

  diff = lpi->diff;
  puzzle = lpi->mode;

  currentTool = 0;

  diff = lpi->diff;
  puzzle = lpi->isPuzzle();

  for (int i = 0; i < tools.count(); i++)
  {
    bool act = i < 2 ? true : (!puzzle && i < next_tool);
    tools.at(i)->setActive(act);
  }

  scoreBonus->setValue(lpi->score_bonus);
  clockBonus->setValue(lpi->time_bonus);
  timerBonus->setValue(lpi->speed_bonus);
  magBonus->setValue(lpi->speed_mag_bonus);
}

void ToolSet::writeProfile(LevelPackInfo *lpi)
{
  lpi->next_tool = next_tool;
  lpi->localScore = toolScore;

  lpi->score_bonus = scoreBonus->value();
  lpi->time_bonus = clockBonus->value();
  lpi->speed_bonus = timerBonus->value();
  lpi->speed_mag_bonus = magBonus->value();
}

void ToolSet::addScore(int score)
{
  toolScore += score;
}

void ToolSet::nextToolActivated()
{
  if (puzzle || next_tool == ALL_TOOLS)
    return;

  if (next_tool < tools.count())
  {
    GameTool *tool = tools.at(next_tool);
    tool->setActive(true);
    tool->animateActivation();
    sndEngine->playSound(GameSound::sndTool);

    next_tool++;
  }
  else
    next_tool = ALL_TOOLS;
}

void ToolSet::updateTools(QPainter &p)
{
  p.setOpacity(1);
  p.setPen(QPen(Qt::lightGray));
  p.setFont(gameStock->Font20);

  p.drawText(QRect(DX(910), DY(240), DX(1024-910-10), 40),
             Qt::AlignTop | Qt::AlignHCenter,
             QString("%1").arg(toolScore));

  p.setFont(gameStock->Font8);

  for (int i = 0; i < tools.count(); i++)
  {
    GameTool *tool = tools.at(i);
    tool->advanceAndPaint(p, toolScore);
  }
}

bool ToolSet::checkMouseActions(const QPoint &pos)
{
  for (int i = 0; i < tools.count(); i++)
  {
    GameTool *tool = tools.at(i);
    if (QRect(tool->pos(), QSize(DX(48),DY(48))).contains(pos))
    {
      if (!tool->isActive() || tool == currentTool)
        break;

      if (tool->score() <= toolScore)
      {
        currentTool = tool;
        currentTool->setCurrent();
        QApplication::setOverrideCursor(currentTool->pixmap());

        if (tool->type() != GameTool::ManualTool)
        {
          if (tool->checkToolClick())
          {
              toolScore -= tool->score();
              if (tool->inProgress())
                return true;
              else
                break;
          }
        }
        else  // manual tool
        {
        }

        myOverRow = myOverCol = -1;
        return true;
      }

      break;
    }
  }

  currentTool = 0;
  scene->setDefaultGameCursor();
  return false;
}

void ToolSet::progressCurrent()
{
  if (!(currentTool->checkToolClick()))
  {
    currentTool = 0;
    scene->setDefaultGameCursor();
  }
}

bool ToolSet::checkItemClick(int row, int col)
{
  if (!currentTool->checkItemClick(row, col))
    return false;

  toolScore -= currentTool->score();
  currentTool = 0;
  scene->setDefaultGameCursor();
  return true;
}

GameTool::ToolAction ToolSet::checkItemState(int row, int col)
{
  if (!currentTool || myOverCol == -1 || myOverRow == -1)
    return GameTool::ToolOutOfRange;

  return currentTool->checkItemState(row, col);
}

void ToolSet::checkMouseMoved(int row, int col)
{
  if (col >= 0 && col < scene->numCols() && row >= 0 && row < scene->numRows())
  {
    myOverRow = row;
    myOverCol = col;
  }
  else
    myOverRow = myOverCol = -1;
}

bool ToolSet::checkMouseHover(const QPoint &pos)
{
  for (int i = 0; i < tools.count(); i++)
  {
    GameTool *tool = tools.at(i);
    if (QRect(tool->pos(), QSize(DX(48),DY(48))).contains(pos) && tool->isActive())
    {
      scene->showHint(tool->hint());
      return true;
    }
  }

  return false;
}
