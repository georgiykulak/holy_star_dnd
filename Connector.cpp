#include "Connector.hpp"

#include <QPainter>

Connector::Connector(QWidget *parent)
    : QWidget{parent}
{
    auto size = QSize(12, 12);
    setMinimumSize(size);
    setMaximumSize(size);
    m_pixmap = QPixmap(size);
    m_pixmap.fill(QColor(Qt::transparent));
}

void Connector::SetPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

QPixmap Connector::GetPixmap() const
{
    return m_pixmap;
}

void Connector::DrawPixmap(QPainter &painter)
{
    Qt::GlobalColor color = Qt::darkBlue;
    QPen mPen;
    mPen.setColor(color);
    mPen.setWidth(2);
    painter.setPen(mPen);
    painter.setBrush(color);
    painter.drawEllipse(1, 1, 10, 10);
}

void Connector::paintEvent(QPaintEvent *event)
{
    (void)event;
    QPainter painter(this);
    DrawPixmap(painter);
}
