#include <QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtGui/QScreen>

#include "graph3d.hpp"

double func(double x, double y)
{
    return sin(x) * cos(y);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Q3DSurface *graph = new Q3DSurface();
    Graph3D *graphManager = new Graph3D(graph, &func);
    QWidget *window = QWidget::createWindowContainer(graph);
    QSize screenSize = graph->screen()->size();
    window->setMinimumSize(QSize(screenSize.width() / 2, int(screenSize.height() / 1.6)));
    window->setMaximumSize(screenSize);
    window->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    window->setFocusPolicy(Qt::StrongFocus);
    graphManager->plot();
    window->show();

    return a.exec();
}
