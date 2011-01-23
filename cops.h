#ifndef COPS_H
#define COPS_H

#include <QGraphicsItem>
#include <QObject>
#include <QPixmap>

#define HELICOPS 65537
//! [0]
class Cops : public QObject, public QGraphicsItem
{
	Q_OBJECT

	public:

		enum { Type = UserType + 1 };

		int type() const
		{
			// Enable the use of qgraphicsitem_cast with this item.
			return Type;
		}


		Cops();

		QRectF boundingRect() const;
		QPainterPath shape() const;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
				QWidget *widget);

	protected:
		void timerEvent(QTimerEvent *event);
		void mousePressEvent(QGraphicsSceneMouseEvent *event);

	private:
		qreal angle;
		qreal speed;
		qreal mouseEyeDirection;
		QColor color;
		QPixmap *pic;
};

#endif
