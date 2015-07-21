#ifndef SCENE_IF_H
#define SCENE_IF_H

#include <QList>
#include <QPainter>
#include <QCursor>

#include "gameitem.h"
#include "scaler.h"

#define MAX_COLS 16
#define MAX_ROWS 16

class BaseItem;

struct PlaceInfo
{
  PlaceInfo() : item(0), place(NoPlace)
  {
  }

  inline bool hasItem() const { return item; }
  inline bool empty() const { return !(place & PlaceConcrete); }
  inline bool freePlaceForItem() const { return !item && (place & PlaceConcrete); }
  inline bool itemCanBeAppeared() const { return freePlaceForItem() && (place & ItemAppear); }
  inline bool itemCanBeCreated() const { return freePlaceForItem() && (place & ItemStart); }
  inline bool itemCanBeMoved() const { return item && !(place & Blocks) && (item->isIdle() || item->isSelected()); }
  inline bool itemCanBeHighlighted() const { return item && (item->isIdle() || item->isSelected()); }

  inline bool hasTarget() const { return place & Targets; }
  inline bool hasBlock() const { return place & Blocks; }

  inline bool isProcessed() const { return place & Processed; }
  inline void setProcessed() { place |= Processed; }
  inline void setUnprocessed() { place &= ~Processed; }

  GameItem *item;
  quint32 place;
};


class IScene
{
public:
    IScene();
    ~IScene() {}

    void createPixmapPopup(int x, int y, int dx, int dy, const QPixmap &pm, int steps);
    void createScorePopup(int x, int y, int score);
    void createStaticPopup(QRect rect, const QString &text, int textFlags,
                           const QFont &font, QColor color = Qt::white,
                           qreal opacity = 1, int staysteps = 0, int steps = 10,
                           int dx = 0, int dy = 0);

    inline void showHint(const QString &hint) {hintText = hint;}
    inline void removeHint() { if (!hintText.isEmpty()) hintText = ""; }

    void drawTransRect(QPainter &p, const QRect &r,
                       QColor borderColor = Qt::white, QColor bgColor = Qt::black,
                       qreal opacity = 0.3);

    inline int col2x(int val) const     { return val*X56+DX(xoff); }
    inline int row2y(int val) const     { return val*Y56+DY(yoff); }
    inline int item_col2x(int val) const     { return val*X56+DX(xoff+4); }
    inline int item_row2y(int val) const     { return val*Y56+DY(yoff+4); }

    inline const PlaceInfo* data() const { return field; }
    inline PlaceInfo& data(int r, int c) { return field[(r << 4) | c]; }
    inline int numRows() const { return rows; }
    inline int numCols() const { return cols; }

    inline const QList<int>& gameItems() { return activeItems; }
    inline const QPixmap& itemPixmap(int id) const { return allItems.at(id); }
    inline int randomItemId() { return activeItems.at(qrand() % activeItems.count()); }

    void removeAndCountItem(int row, int col);
    void removeAndCountItemOnly(int row, int col);  // only for small hammer
    void removeBlock(PlaceInfo &pi, int row, int col);
    void removeTarget(PlaceInfo &pi, int row, int col);

    void addTime(int time);

    void setDefaultGameCursor();
    void restoreCursor();

protected:
    void drawTextHint(QPainter &p);

    QList<BaseItem*> tempItems;

    QCursor lastCursor;

    PlaceInfo field[MAX_ROWS*MAX_COLS];
    int rows, cols;

    int xoff, yoff;

    int time;
    int bonus, bonus_time;
    int targets;
    int score;
    int level, max_level;

    QPixmap concrete;
    QPixmap block1, block2;
    QPixmap target1, target2;
    QPixmap bgpicture;
    QList<int> activeItems;

    QList<QPixmap> allItems;

    QString hintText;
};

extern IScene * scene;

#endif // SCENE_IF_H
