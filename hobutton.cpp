#include "hobutton.h"

HoButton::HoButton(QWidget *parent)
    : QWidget{parent}
{
    this->width = 0;
    this->height = 0;
    this->id = 0;
    this->topLeftRadius = 0;
    this->topRightRadius = 0;
    this->bottomLeftRadius = 0;
    this->bottomRightRadius = 0;
    this->borderPen.setWidth(0);
    this->borderPen.setColor(QColor(255, 255, 255));
    this->isMousePress = false;
}

void HoButton::setPosSize(int x, int y, int width, int height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->setGeometry(x, y, width, height);
}

void HoButton::setRadius(qreal all) {
    this->topLeftRadius = all;
    this->topRightRadius = all;
    this->bottomLeftRadius = all;
    this->bottomRightRadius = all;
}

void HoButton::setRadius(qreal topLeft, qreal topRight, qreal bottomLeft, qreal bottomRight) {
    this->topLeftRadius = topLeft;
    this->topRightRadius = topRight;
    this->bottomLeftRadius = bottomLeft;
    this->bottomRightRadius = bottomRight;
}

void HoButton::setText(QString text) {
    this->text = text;
}

void HoButton::setTextColor(QColor textColor) {
    this->textColor = textColor;
}

void HoButton::setTextFont(QFont font) {
    this->textFont = font;
}

void HoButton::setBorder(int weight, QColor color) {
    this->borderPen.setWidth(weight);
    this->borderPen.setColor(color);
}

void HoButton::setBackgroundColor(QBrush brush) {
    this->backgroundBrush = brush;
}

void HoButton::setId(int id) {
    this->id = id;
}

int HoButton::getX() {
    return this->x;
}

int HoButton::getY() {
    return this->y;
}

int HoButton::getWidth() {
    return this->width;
}

int HoButton::getHeight() {
    return this->height;
}

qreal HoButton::getTopLeftRadius() {
    return this->topLeftRadius;
}

qreal HoButton::getTopRightRadius() {
    return this->topRightRadius;
}

qreal HoButton::getBottomRightRadius() {
    return this->bottomRightRadius;
}

qreal HoButton::getBottomLeftRadius() {
    return this->bottomLeftRadius;
}

QString HoButton::getText() {
    return this->text;
}

QPen HoButton::getBorderPen() {
    return this->borderPen;
}

QFont HoButton::getTextFont() {
    return this->textFont;
}

QColor HoButton::getTextColor() {
    return this->textColor;
}

QBrush HoButton::getBackgroundBrush() {
    return this->backgroundBrush;
}

int HoButton::getId() {
    return this->id;
}

void HoButton::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 定义矩形的位置和大小
    QRectF rect(0, 0, this->width, this->height);
    // 创建QPainterPath对象
    QPainterPath path;
    path.moveTo(rect.topLeft() + QPointF(this->topLeftRadius, 0));
    path.lineTo(rect.topRight() - QPointF(this->topRightRadius, 0));
    path.arcTo(QRectF(rect.topRight() - QPointF(this->topRightRadius * 2, 0), QSizeF(this->topRightRadius * 2, this->topRightRadius * 2)), 90, -90);
    path.lineTo(rect.bottomRight() - QPointF(0, this->bottomRightRadius));
    path.arcTo(QRectF(rect.bottomRight() - QPointF(this->bottomRightRadius * 2, this->bottomRightRadius * 2), QSizeF(this->bottomRightRadius * 2, this->bottomRightRadius * 2)), 0, -90);
    path.lineTo(rect.bottomLeft() + QPointF(this->bottomLeftRadius, 0));
    path.arcTo(QRectF(rect.bottomLeft() - QPointF(0, this->bottomLeftRadius * 2), QSizeF(this->bottomLeftRadius * 2, this->bottomLeftRadius * 2)), 270, -90);
    path.lineTo(rect.topLeft() + QPointF(0, this->topLeftRadius));
    path.arcTo(QRectF(rect.topLeft(), QSizeF(this->topLeftRadius * 2, this->topLeftRadius * 2)), 180, -90);
    // 关闭路径
    path.closeSubpath();
    // 设置画笔和画刷
    painter.setPen(this->borderPen);
    painter.setBrush(this->backgroundBrush);
    // 绘制路径
    painter.drawPath(path);

    // 设置画笔
    painter.setPen(this->textColor);
    // 设置字体
    painter.setFont(this->textFont);
    // 创建裁剪区域
    QRectF clipRect = rect.adjusted(5, 5, -5, -5); // 调整裁剪区域，避免文本被边框覆盖
    // 设置裁剪区域
    painter.setClipRect(clipRect);
    // 绘制文本
    painter.drawText(clipRect, Qt::AlignCenter, this->text);
}

void HoButton::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->isMousePress = true;
    }
}

void HoButton::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && this->isMousePress) {
        this->isMousePress = false;
        emit this->onClick(this->id);
    }
}

void HoButton::enterEvent(QEnterEvent *) {
    emit this->onMouseEnter(this->id);
}

void HoButton::leaveEvent(QEvent *) {
    emit this->onMouseLeave(this->id);
}
