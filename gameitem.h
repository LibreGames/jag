#ifndef GAMEITEM_H
#define GAMEITEM_H

#include "defines.h"


class GameItem
{
  public:
    enum ItemState { IdleState, NotAliveState, DyingState, FallingState, BornState,
                     SelectedState };

    GameItem(quint8 id);

    inline quint8 id() const { return myId; }
    inline void setId(quint8 newid) { myId = newid; }

    inline int dx() const { return myXoff; }
    inline int dy() const { return myYoff; }

    inline qreal opacity() const { return myOpacity; }
    inline void setOpacity(qreal val) { myOpacity = val; }

    void advance();

    void idle();
    inline bool isIdle() const { return myState == IdleState; }

    void die();
    inline bool isDying() const { return myState == DyingState; }

    void scheduleDeath();
    inline bool isNotAlive() const { return myState == NotAliveState; }

    void fall(int dx, int dy);
    inline bool isFalling() const { return myState == FallingState; }

    void born();
    inline bool isBorning() const { return myState == BornState; }

    void select();
    inline bool isSelected() const { return myState == SelectedState; }

  protected:

    quint8 myId;
    ItemState myState;

    int myCount;

    int myXoff, myYoff;
    int myDx, myDy;

    qreal myOpacity;
};

#endif // GAMEITEM_H
