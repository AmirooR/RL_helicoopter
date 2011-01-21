#include <QtGui>
#include <QPixmap>
#include <math.h>


int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	QGraphicsScene scene;
	scene.setBackgroundBrush(Qt::black);
	scene.setSceneRect(0, 0, 600, 200);
	scene.setItemIndexMethod(QGraphicsScene::NoIndex);

	QPixmap const *pic = new QPixmap(":/images/cops.jpg");
	QGraphicsPixmapItem *helicops = new QGraphicsPixmapItem(*pic, 0, &scene);
	helicops->setPos(50, 50);
	scene.addItem(helicops);

	QGraphicsView view(&scene);
	view.setRenderHint(QPainter::Antialiasing);
	view.setCacheMode(QGraphicsView::CacheBackground);
	view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	view.setDragMode(QGraphicsView::ScrollHandDrag);
	view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Helicops!"));
	view.resize(650, 250);
	view.show();

	return app.exec();
}
