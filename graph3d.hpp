#ifndef GRAPH3D_HPP
#define GRAPH3D_HPP

#include <QObject>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtMath>

#include <limits>

using namespace QtDataVisualization;

class Graph3D : public QObject
{
    Q_OBJECT
public:
    explicit Graph3D(Q3DSurface *surface,
                     double (*function)(double x, double y) = nullptr);
    ~Graph3D();

    void plot();

    double setPosition(QPointF position);

    void setMinXValue(double min);
    void setMinYValue(double min);

    double getMinXValue();
    double getMinYValue();

    void setMaxXValue(double max);
    void setMaxYValue(double max);

    double getMaxXValue();
    double getMaxYValue();

    void setRange(double range);
    void setCenter(QPointF center);

signals:

private slots:
    void calculateStep();

private:
    Q3DSurface *m_graph;
    QSurfaceDataProxy *m_dataProxy;
    QSurface3DSeries *m_dataSeries;

    double (*function)(double x, double y);

    double m_range = 20;
    QPointF m_center = {0, 0};
    double m_maxY = std::numeric_limits<double>::min();
    double m_minY = std::numeric_limits<double>::max();

    double m_stepX;
    double m_stepZ;

    void maxGradient();
    void minGradient();
};

#endif // GRAPH3D_HPP
