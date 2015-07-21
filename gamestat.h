#ifndef GAMESTAT_H
#define GAMESTAT_H

#include <QRect>
#include <QList>

struct StatInfo
{
  StatInfo();

  void initGraphics();

  int code;

  int level_time;
  int level_score;
  int level_targets_all, level_targets_left;

  class GameTool *tool;

  QRect buttonOk, buttonCancel;
  QList<QRect> buttons;
};

#endif // GAMESTAT_H
