#include "bighammertool.h"
#include "gameitem.h"
#include "gamesound.h"
#include "gamebonus.h"

#include "scene_if.h"

BigHammerTool::BigHammerTool(int x, int y, int score) : GameTool(x, y, score, "big_star")
{
  bonusInfo = new BonusInfo(1000, myPixmap, tr("Super Star"),
                            tr("Acts like a star but twice powerful (2 actions at a time)"));
}

GameTool::ToolAction BigHammerTool::checkItemState(int row, int col)
{
  if (row == myToolset->toolRow() && col == myToolset->toolCol())
  {
    PlaceInfo &pi = scene->data(row,col);
    if (pi.itemCanBeHighlighted())
      return ToolActive;
    if (!pi.empty())
      return ToolInactive;
  }

  return ToolOutOfRange;
}

bool BigHammerTool::checkItemClick(int row, int col)
{
  PlaceInfo &pi = scene->data(row,col);
  if (pi.itemCanBeHighlighted())
  {
    // this only schedules a removal
    scene->removeAndCountItem(row,col);

    scene->createPixmapPopup(scene->col2x(col), scene->row2y(row), 0, 5, myPixmap, 5);
    scene->createPixmapPopup(scene->col2x(col), scene->row2y(row), 0, -5, myPixmap, 5);

    // and 2nd hit
    if (pi.itemCanBeHighlighted() || pi.item->isNotAlive())
    {
      // set item to unprocessed
      pi.setUnprocessed();
      scene->removeAndCountItem(row,col);
    }

    sndEngine->playSound(GameSound::sndBigHammer);

    return true;
  }

  return false;
}
