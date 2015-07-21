#include "gamestock.h"
#include "defines.h"

#include "scaler.h"

GameStock * gameStock = 0;

GameStock::GameStock()
{
  initGraphics();
}

void GameStock::initGraphics()
{
  Font8 = QFont("helvetica", qMax(DY(8), 8));
  Font12 = QFont("helvetica", DY(12));
  Font20 = QFont("helvetica", DY(20));
  QFontMetrics fm20(Font20);
  if (fm20.boundingRect("0000000").width() > DY(120))
      Font20.setPointSize(DY(16));
  Font40 = QFont("helvetica", DY(40));
  Font60 = QFont("helvetica", DY(60));

  GameCursor = QCursor(QPixmap(":/images/cursor_wand.png")
                       .scaled(DX(32),DY(32), Qt::IgnoreAspectRatio, Qt::SmoothTransformation), 0, 0);

  GameLogoBig = QPixmap(":/images/logo.png");
  MenuBackground = QPixmap(":/images/bg_menu.jpg")
                   .scaled(WIDTH,HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  Concrete = QPixmap(":/images/con1.png").scaled(X56,Y56, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  Target1 = QPixmap(":/images/target1.png").scaled(X56,Y56, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  Target2 = QPixmap(":/images/target2.png").scaled(X56,Y56, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  Block1 = QPixmap(":/images/block1.png").scaled(X56,Y56, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  Block2 = QPixmap(":/images/block2.png").scaled(X56,Y56, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  Clock = QPixmap(":/images/clock.png").scaled(X48,Y48, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  Score = QPixmap(":/images/score.png").scaled(X48,Y48, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  Items.clear();
  for (int i = 0; i < MAX_ITEMS_COUNT; i++)
      Items.append(QPixmap(QString(":/images/item%1.png").arg(i+1)));
}
