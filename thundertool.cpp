#include "thundertool.h"
#include "gameitem.h"
#include "gamesound.h"
#include "gamebonus.h"

#include "scene_if.h"

ThunderTool::ThunderTool(int x, int y, int score) : GameTool(x,y, score, "cross")
{
  bonusInfo = new BonusInfo(1000, myPixmap, tr("Row Blaster"),
                            tr("Removes items in a row"));
}

GameTool::ToolAction ThunderTool::checkItemState(int row, int col)
{
  int tr = myToolset->toolRow(), tc = myToolset->toolCol();

  PlaceInfo &pi1 = scene->data(tr,tc);
  if (pi1.empty())
    return ToolOutOfRange;

  if (row == tr)
  {
    PlaceInfo &pi = scene->data(row,col);
    if (pi.itemCanBeHighlighted())
    {
      if (!pi1.itemCanBeHighlighted())
        return ToolInactive;

      return ToolActive;
    }
    if (!pi.empty())
      return ToolInactive;
  }

  return ToolOutOfRange;
}


bool ThunderTool::checkItemClick(int row, int col)
{
  PlaceInfo &pi = scene->data(row,col);

  if (!pi.itemCanBeHighlighted())
    return false;

  int cx = -1;
  for (int j = 0; j < scene->numCols(); j++)
  {
    PlaceInfo &pi = scene->data(row,j);

    if (pi.itemCanBeHighlighted())
    {
      scene->removeAndCountItem(row,j);

      scene->createPixmapPopup(scene->col2x(j), scene->row2y(row), 0, 5*cx, myPixmap, 5);
    }

    cx = -cx;
  }

  sndEngine->playSound(GameSound::sndRow);

  return true;
}

