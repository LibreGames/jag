#include "unblocktool.h"
#include "gameitem.h"
#include "gamesound.h"
#include "gamebonus.h"

#include "scene_if.h"

UnblockTool::UnblockTool(int x, int y, int score) : GameTool(x, y, score, "unblock")
{
  bonusInfo = new BonusInfo(1000, myPixmap, tr("Unblocker"),
                            tr("Removes single block from item"));
}

GameTool::ToolAction UnblockTool::checkItemState(int row, int col)
{
  if (row == myToolset->toolRow() && col == myToolset->toolCol())
  {
    PlaceInfo &pi = scene->data(row,col);
    if (pi.itemCanBeHighlighted() && pi.hasBlock())
      return ToolActive;
    if (!pi.empty())
      return ToolInactive;
  }

  return ToolOutOfRange;
}

bool UnblockTool::checkItemClick(int row, int col)
{
  PlaceInfo &pi = scene->data(row,col);
  if (pi.itemCanBeHighlighted() && pi.hasBlock())
  {
    // remove block
    scene->removeBlock(pi,row,col);

    scene->createPixmapPopup(scene->col2x(col), scene->row2y(row), 0, 5, myPixmap, 5);
    scene->createPixmapPopup(scene->col2x(col), scene->row2y(row), 0, -5, myPixmap, 5);

//    sndEngine->playSound(GameSound::sndUnblock);

    return true;
  }

  return false;
}
