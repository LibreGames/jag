#include "smallhammertool.h"
#include "gameitem.h"
#include "gamesound.h"
#include "gamebonus.h"

#include "scene_if.h"

SmallHammerTool::SmallHammerTool(int x, int y, int score) : GameTool(x, y, score, "hammer")
{
  bonusInfo = new BonusInfo(1000, myPixmap, tr("Hammer"),
                            tr("Removes only unblocked items (no effect on block & target)"));
}

GameTool::ToolAction SmallHammerTool::checkItemState(int row, int col)
{
  if (row == myToolset->toolRow() && col == myToolset->toolCol())
  {
    PlaceInfo &pi = scene->data(row,col);
    if (pi.itemCanBeHighlighted() && !pi.hasBlock())
      return ToolActive;
    if (!pi.empty())
      return ToolInactive;
  }

  return ToolOutOfRange;
}

bool SmallHammerTool::checkItemClick(int row, int col)
{
  PlaceInfo &pi = scene->data(row,col);
  if (pi.itemCanBeHighlighted() && !pi.hasBlock() && !pi.item->isNotAlive())
  {
    // this only schedules a removal
    scene->removeAndCountItemOnly(row,col);
    //scene->removeAndCountItem(row,col);

    scene->createPixmapPopup(scene->col2x(col), scene->row2y(row), 0, 5, myPixmap, 5);
    scene->createPixmapPopup(scene->col2x(col), scene->row2y(row), 0, -5, myPixmap, 5);

    sndEngine->playSound(GameSound::sndSmallHammer);

    return true;
  }

  return false;
}
