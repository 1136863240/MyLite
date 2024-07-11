#ifndef HOBUTTON_H
#define HOBUTTON_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QEvent>
#include <QMouseEvent>

class HoButton : public QWidget
{
    Q_OBJECT
public:
    explicit HoButton(QWidget *parent = nullptr);

    void setPosSize(int x, int y, int width, int height);
    void setRadius(qreal all);
    void setRadius(qreal topLeft, qreal topRight, qreal bottomLeft, qreal bottomRight);
    void setText(QString text);
    void setTextColor(QColor textColor);
    void setTextFont(QFont font);
    void setBorder(int weight, QColor color);
    void setBackgroundColor(QBrush brush);
    void setId(int id);

    int getX();
    int getY();
    int getWidth();
    int getHeight();
    qreal getTopLeftRadius();
    qreal getTopRightRadius();
    qreal getBottomRightRadius();
    qreal getBottomLeftRadius();
    QString getText();
    QPen getBorderPen();
    QFont getTextFont();
    QColor getTextColor();
    QBrush getBackgroundBrush();
    int getId();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEnterEvent *);
    void leaveEvent(QEvent *);

signals:
    void onClick(int id);
    void onMouseEnter(int id);
    void onMouseLeave(int id);

private:
    int x;
    int y;
    int width;
    int height;
    int id;
    qreal topLeftRadius;
    qreal topRightRadius;
    qreal bottomRightRadius;
    qreal bottomLeftRadius;
    QString text;
    QPen borderPen;
    QFont textFont;
    QColor textColor;
    QBrush backgroundBrush;
    bool isMousePress;
};

#endif // HOBUTTON_H
