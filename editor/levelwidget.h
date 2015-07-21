#ifndef LEVELWIDGET_H
#define LEVELWIDGET_H

#include <QtGui>

class LevelWidget: public QTableWidget
{
    Q_OBJECT

public:
    LevelWidget(QWidget *parent = 0)
        : QTableWidget(parent), m_clicked(false), m_rightclicked(false) {}

    bool isMousePressed()  { return m_clicked || m_rightclicked; }

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void enteredWidget();
    void leavedWidget();
    void mouseClicked(const QPoint &);
    void mouseRightClicked(const QPoint &);
    void mouseReleased();

private:
    bool m_clicked, m_rightclicked;
};

#endif // LEVELWIDGET_H
