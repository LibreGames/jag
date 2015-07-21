#include "levelwidget.h"

void LevelWidget::enterEvent(QEvent *event)
{
    emit enteredWidget();
    m_clicked = false;
    m_rightclicked = false;

    QTableWidget::enterEvent(event);
}

void LevelWidget::leaveEvent(QEvent *event)
{
    emit leavedWidget();
    m_clicked = false;
    m_rightclicked = false;

    QTableWidget::leaveEvent(event);
}

void LevelWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_clicked)
        emit mouseClicked(event->pos());
    else if (m_rightclicked)
        emit mouseRightClicked(event->pos());

    QTableWidget::mouseMoveEvent(event);
}

void LevelWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_clicked = true;
        emit mouseClicked(event->pos());
    }
    else if (event->button() == Qt::RightButton) {
        m_rightclicked = true;
        emit mouseRightClicked(event->pos());
    }

    QTableWidget::mousePressEvent(event);
}

void LevelWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_clicked = false;
        emit mouseReleased();
    }
    else if (event->button() == Qt::RightButton) {
        m_rightclicked = false;
        emit mouseReleased();
    }

    QTableWidget::mouseReleaseEvent(event);
}
