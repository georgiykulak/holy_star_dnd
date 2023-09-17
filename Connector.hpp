#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include <QWidget>

class Connector : public QWidget
{
    Q_OBJECT
public:
    explicit Connector(QWidget *parent = nullptr);

    void SetPixmap(const QPixmap& pixmap);
    QPixmap GetPixmap() const;
    void DrawPixmap(QPainter& painter);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_pixmap;
};

#endif // CONNECTOR_HPP
