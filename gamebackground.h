#ifndef GAMEBACKGROUND_H
#define GAMEBACKGROUND_H

#include <QPainter>

struct PlayerInfo;
struct BgPrivate;

class GameBackground
{
public:
    GameBackground();

    void readProfile(PlayerInfo *pl);

    void draw(QPainter &p, int frame, const QRect &r);

private:
    BgPrivate *priv;
};

extern GameBackground *gameBackground;

#endif // GAMEBACKGROUND_H
