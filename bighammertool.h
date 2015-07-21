#ifndef BIGHAMMERTOOL_H
#define BIGHAMMERTOOL_H

#include "gametools.h"

class BigHammerTool : public GameTool
{
  Q_OBJECT
public:
  BigHammerTool(int x, int y, int score);
  virtual ~BigHammerTool() {}

  ToolAction checkItemState(int row, int col);
  bool checkItemClick(int row, int col);
};

#endif // BIGHAMMERTOOL_H
