#ifndef COPS_SCENE_H
#define COPS_SCENE_H

#include <QGraphicsScene>

//! [0]
class CopsScene :  public QGraphicsScene
{
	private:
		enum MouseStateEnum
						{	
							CLICKED = 1,
							RELEASED = 2
						};
		MouseStateEnum mouseState;
public:
    CopsScene();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void timerEvent(QTimerEvent *);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

};
//! [0]

#endif

