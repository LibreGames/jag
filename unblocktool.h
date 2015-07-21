#ifndef UNBLOCKTOOL_H
#define UNBLOCKTOOL_H

#include "gametools.h"

class UnblockTool : public GameTool
{
  Q_OBJECT
public:
  UnblockTool(int x, int y, int score);
  virtual ~UnblockTool() {}

  virtual ToolAction checkItemState(int row, int col);
  virtual bool checkItemClick(int row, int col);
};

#endif // UNBLOCKTOOL_H
