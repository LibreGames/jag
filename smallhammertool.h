#ifndef SMALLHAMMER_H
#define SMALLHAMMER_H

#include "gametools.h"

class SmallHammerTool : public GameTool
{
  Q_OBJECT
public:
  SmallHammerTool(int x, int y, int score);
  virtual ~SmallHammerTool() {}

  ToolAction checkItemState(int row, int col);
  bool checkItemClick(int row, int col);
};

#endif // SMALLHAMMER_H
