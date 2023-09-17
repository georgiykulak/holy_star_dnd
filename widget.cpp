#include "widget.h"
#include "Connector.hpp"
#include "ui_widget.h"

#include <QDebug>
#include <QPainter>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QMimeData>

#include <cmath>

Widget::Widget(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    QPainter painter(this);
    initStartPoints(painter);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *event)
{
    (void)event;
    QPainter painter(this);
    QPen mPen;

    mPen.setColor(Qt::black);
    mPen.setWidth(2);
    painter.setPen(mPen);
    painter.drawLine(m_currentLine);
    for (const auto& line : m_lines)
    {
        painter.drawLine(line);
    }

    drawStarCircle(painter);
}

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-oph-connector")
        && event->source() == this)
    {
        QByteArray itemData = event->mimeData()->data("application/x-oph-connector");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint startPosition;
        QPoint offset;
        dataStream >> pixmap >> startPosition >> offset;

        m_currentLine = QLine(startPosition + offset, event->pos());
        update();

        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void Widget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-oph-connector")
        && event->source() == this)
    {
        QByteArray itemData = event->mimeData()->data("application/x-oph-connector");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint startPosition;
        QPoint offset;
        dataStream >> pixmap >> startPosition >> offset;

        m_currentLine = QLine(startPosition + offset, event->pos());
        update();

        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void Widget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-oph-connector")
        && event->source() == this)
    {
        QByteArray itemData = event->mimeData()->data("application/x-oph-connector");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint startPosition;
        QPoint offset;
        dataStream >> pixmap >> startPosition >> offset;

        m_currentLine = {};
        update();
        QWidget* child = childAt(event->pos());
        if (child)
        {
            Connector* connector = qobject_cast<Connector*>(child);
            if (connector)
            {
                m_currentLine = {};
                m_lines.push_back(QLine(startPosition + offset, event->pos()));
                update();

                event->setDropAction(Qt::CopyAction);
                event->accept();

                return;
            }
        }

        event->ignore();
    }
    else
    {
        event->ignore();
    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    QWidget* child = childAt(event->pos());
    if (!child)
    {
        if (event->button() == Qt::RightButton || event->button() == Qt::MiddleButton)
        {
            Connector* connector = new Connector(this);
            QPixmap pixmap = connector->GetPixmap();
            QPainter painter(&pixmap);

            connector->DrawPixmap(painter);
            connector->SetPixmap(pixmap);
            connector->move(event->pos());
            connector->update();
            connector->show();
            connector->setAttribute(Qt::WA_DeleteOnClose);
        }

        return;
    }

    Connector* connector = qobject_cast<Connector*>(child);
    if (connector && event->button() == Qt::LeftButton)
    {
        QPixmap pixmap = connector->GetPixmap();
        QPoint startPosition = connector->pos();
        QPoint offset = event->pos() - startPosition;

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << pixmap << startPosition << offset;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-oph-connector", itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(offset);

        QPixmap tempPixmap = pixmap;
        QPainter painter;
        painter.begin(&tempPixmap);
        painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 50));
        painter.end();

        connector->SetPixmap(tempPixmap);

        if (drag->exec(Qt::CopyAction) == Qt::CopyAction)
        {
            connector->show();
            connector->SetPixmap(pixmap);
        }
    }
}

void Widget::initStartPoints(QPainter& painter)
{
    float r = 149;
    QPointF offset(397, 243);

    for (int k = 0; k < 5; ++k)
    {
        // {(r * cos(2πk/5+π/2), r * sin(2πk/5+π/2)) | k=0,...,4}
        QPointF pos(r * std::cos(2 * 3.14 * float(k) / 5 + 3.14 / 2),
                    r * std::sin(2 * 3.14 * float(k) / 5 + 3.14 / 2));

        Connector* connector = new Connector(this);
        QPixmap pixmap = connector->GetPixmap();

        connector->DrawPixmap(painter);
        connector->SetPixmap(pixmap);
        connector->move(QPoint(int(pos.x() + offset.x()),
                               int(pos.y() + offset.y())));
        connector->update();
        connector->show();
        connector->setAttribute(Qt::WA_DeleteOnClose);
    }
}

void Widget::drawStarCircle(QPainter& painter)
{
    painter.drawEllipse(250, 100, 300, 300);
}
