#include <QtGui>
#include <QPixmap>
#include <math.h>
#include <time.h>
#include "cops.h"
#include "copsScene.h"
#include "barrier.h"


int main(int argc, char **argv)
{
	QApplication app(argc, argv);
        //qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        qsrand(1);
        //srand(time(0));
        srand(1);

        int screenWidth = 600;
        int screenHeight = 300;
	CopsScene scene;
        scene.setBackgroundBrush(Qt::black);
        scene.setSceneRect(0, 0, screenWidth, screenHeight);
	scene.setItemIndexMethod(QGraphicsScene::NoIndex);

	Cops *helicops = new Cops();
        helicops->setPos(50, 50);
	scene.addItem(helicops);

        QGraphicsRectItem *downRect = new QGraphicsRectItem(0, 0, screenWidth, 10);
        downRect->setBrush(Qt::green);
        downRect->setPos(0,screenHeight - 10);
        scene.addItem(downRect);

        QGraphicsRectItem *upRect = new QGraphicsRectItem(0, 0, screenWidth, 10);
        upRect->setBrush(Qt::green);
        upRect->setPos(0,0);
        scene.addItem(upRect);

        Barrier *barrier = new Barrier(0, 0, 20, 70);
        barrier->setBrush(Qt::green);
        barrier->setPos(400, 100);
        scene.addItem(barrier);

	QGraphicsView view(&scene);
        view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view.setRenderHint(QPainter::Antialiasing);
	view.setCacheMode(QGraphicsView::CacheBackground);
	view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	view.setDragMode(QGraphicsView::ScrollHandDrag);
	view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Helicops!"));
        view.resize(screenWidth, screenHeight);
	view.show();

	return app.exec();
}
