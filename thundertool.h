#ifndef THUNDERTOOL_H
#define THUNDERTOOL_H

#include "gametools.h"

class ThunderTool : public GameTool
{
  Q_OBJECT
public:
  ThunderTool(int x, int y, int score);
  virtual ~ThunderTool() {}

  ToolAction checkItemState(int row, int col);
  bool checkItemClick(int row, int col);
};

#endif // THUNDERTOOL_H
