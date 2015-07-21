#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QtGui>


class GameScene;

extern QString resourcePath;

class GameWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GameWidget(QWidget *parent = 0);
    ~GameWidget();

    static const QString& getResourcePath() { return resourcePath; }

public slots:
    void setVideoMode();
    void playRandomMusic();


protected:
    void closeEvent(QCloseEvent *event);

private:
    void drawBackground ( QPainter * painter, const QRectF & rect );
    void keyPressEvent ( QKeyEvent * keyEvent );
    void focusOutEvent ( QFocusEvent * event );

    GameScene *scene;
};

#endif // GAMEWIDGET_H
