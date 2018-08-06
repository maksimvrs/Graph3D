#include "graph3d.hpp"

using namespace QtDataVisualization;


const int sampleCountX = 50;
const int sampleCountZ = 50;

Graph3D::Graph3D(Q3DSurface *surface,
                 double (*function)(double x, double y))
    : m_graph(surface)
    , function(function)
{
    m_graph->setAxisX(new QValue3DAxis);
    m_graph->setAxisY(new QValue3DAxis);
    m_graph->setAxisZ(new QValue3DAxis);

    m_dataProxy = new QSurfaceDataProxy();
    m_dataSeries = new QSurface3DSeries(m_dataProxy);

    m_dataSeries->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);
    m_dataSeries->setFlatShadingEnabled(true);

    m_graph->axisX()->setLabelFormat("%.2f");
    m_graph->axisY()->setLabelFormat("%.2f");
    m_graph->axisZ()->setLabelFormat("%.2f");
    m_graph->axisX()->setRange(float(-m_range / 2 + m_center.x()), float(m_range / 2 + m_center.x()));
    m_graph->axisZ()->setRange(float(-m_range / 2 + m_center.y()), float(m_range / 2 + m_center.y()));
    m_graph->axisX()->setLabelAutoRotation(90);
    m_graph->axisY()->setLabelAutoRotation(90);
    m_graph->axisZ()->setLabelAutoRotation(90);

    calculateStep();

    connect(m_graph->axisX(), SIGNAL(maxChanged(float)),
            this, SLOT(calculateStep()));
    connect(m_graph->axisX(), SIGNAL(minChanged(float)),
            this, SLOT(calculateStep()));
    connect(m_graph->axisZ(), SIGNAL(maxChanged(float)),
            this, SLOT(calculateStep()));
    connect(m_graph->axisZ(), SIGNAL(minChanged(float)),
            this, SLOT(calculateStep()));

    connect(m_graph, SIGNAL(selectedSeriesChanged(QSurface3DSeries*)),
            this, SLOT(setSelectedSeries(QSurface3DSeries*)));
}

Graph3D::~Graph3D()
{
    delete m_dataSeries;
    delete m_dataProxy;
    delete m_graph;
}

void Graph3D::setMinXValue(double min)
{
    double oldMin = double(m_graph->axisX()->min());
    m_graph->axisX()->setRange(float(min), m_graph->axisX()->max());
    m_range = double(qAbs(m_graph->axisX()->max() - m_graph->axisX()->min()));
    m_center += QPointF((min - oldMin) / 2, 0);
}

void Graph3D::setMinYValue(double min)
{
    double oldMin = double(m_graph->axisZ()->min());
    m_graph->axisZ()->setRange(float(min), m_graph->axisZ()->max());
    m_range = double(qAbs(m_graph->axisZ()->max() - m_graph->axisZ()->min()));
    m_center += QPointF((min - oldMin) / 2, 0);
}

double Graph3D::getMinXValue()
{
    return double(m_graph->axisX()->min());
}

double Graph3D::getMinYValue()
{
    return double(m_graph->axisZ()->min());
}

void Graph3D::setMaxXValue(double max)
{
    double oldMax = double(m_graph->axisX()->max());
    m_graph->axisX()->setRange(m_graph->axisX()->min(), float(max));
    m_range = double(qAbs(m_graph->axisX()->max() - m_graph->axisX()->min()));
    m_center += QPointF(0, (max - oldMax) / 2);
}

void Graph3D::setMaxYValue(double max)
{
    double oldMax = double(m_graph->axisZ()->max());
    m_graph->axisZ()->setRange(m_graph->axisZ()->min(), float(max));
    m_range = double(qAbs(m_graph->axisZ()->max() - m_graph->axisZ()->min()));
    m_center += QPointF(0, (max - oldMax) / 2);
}

double Graph3D::getMaxXValue()
{
    return double(m_graph->axisX()->max());
}

double Graph3D::getMaxYValue()
{
    return double(m_graph->axisZ()->max());
}

void Graph3D::setRange(double range)
{
    float offset = float(range - m_range) / 2;
    m_graph->axisX()->setRange(m_graph->axisX()->min() - offset, m_graph->axisX()->max() + offset);
    m_graph->axisZ()->setRange(m_graph->axisZ()->min() - offset, m_graph->axisZ()->max() + offset);
}

void Graph3D::setCenter(QPointF center)
{
    QPointF offset = center - m_center;
    m_graph->axisX()->setRange(m_graph->axisX()->min() + float(offset.x()), m_graph->axisX()->max() + float(offset.x()));
    m_graph->axisZ()->setRange(m_graph->axisZ()->min() + float(offset.y()), m_graph->axisZ()->max() + float(offset.y()));
}

void Graph3D::calculateStep()
{
    m_stepX = (double(m_graph->axisX()->max()) - double(m_graph->axisX()->min())) / (sampleCountX - 1);
    m_stepZ = (double(m_graph->axisX()->max()) - double(m_graph->axisX()->min())) / (sampleCountZ - 1);
}

void Graph3D::maxGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkGreen);
    gr.setColorAt(0.5, Qt::yellow);
    gr.setColorAt(0.8, Qt::red);
    gr.setColorAt(1.0, Qt::darkRed);

    if (!m_graph->seriesList().isEmpty()) {
        m_graph->seriesList().at(0)->setBaseGradient(gr);
        m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    }
}

void Graph3D::minGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkRed);
    gr.setColorAt(0.2, Qt::red);
    gr.setColorAt(0.5, Qt::yellow);
    gr.setColorAt(1.0, Qt::darkGreen);

    if (!m_graph->seriesList().isEmpty()) {
        m_graph->seriesList().at(0)->setBaseGradient(gr);
        m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    }
}

void Graph3D::plot()
{
    QSurfaceDataArray *dataArray = new QSurfaceDataArray;
    dataArray->reserve(sampleCountZ);
    for (int i = 0 ; i < sampleCountZ ; i++) {
        QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX);
        double z = qMin(double(m_graph->axisX()->max()), (i * m_stepZ + double(m_graph->axisX()->min())));
        int index = 0;
        for (int j = 0; j < sampleCountX; j++) {
            double x = qMin(double(m_graph->axisX()->max()), (j * m_stepX + double(m_graph->axisX()->min())));
            double y = function(x, z);
            if (y > m_maxY)
                m_maxY = y;
            if (y < m_minY)
                m_minY = y;
            (*newRow)[index++].setPosition(QVector3D(float(x), float(y), float(z)));
        }
        *dataArray << newRow;
    }
    m_graph->axisY()->setRange(float(m_minY), float(m_maxY));
    m_dataProxy->resetArray(dataArray);
    m_graph->clearSelection();
    m_graph->addSeries(m_dataSeries);
    maxGradient();
}

void Graph3D::setSelectedSeries(QSurface3DSeries *series)
{
    setGridPosition(series->selectedPoint());
    connect(series, SIGNAL(selectedPointChanged(const QPoint&)),
            this, SLOT(setGridPosition(const QPoint&)));
}

void Graph3D::setGridPosition(const QPoint &position)
{
    double y = qMin(double(m_graph->axisX()->max()), (position.x() * m_stepX + double(m_graph->axisX()->min())));
    double x = qMin(double(m_graph->axisZ()->max()), (position.y() * m_stepZ + double(m_graph->axisZ()->min())));
    m_position = QVector3D(float(x), float(function(x, y)), float(y));
}

double Graph3D::setPosition(QPointF position)
{
    int x = qRound((position.x() - double(m_graph->axisX()->min())) / m_stepX);
    int z = qRound((position.y() - double(m_graph->axisZ()->min())) / m_stepZ);
    if (m_graph->selectedSeries()) {
        m_graph->selectedSeries()->setSelectedPoint(QPoint(x,  z));
        m_dataSeries->setSelectedPoint(QPoint(x,  z));
    }
    else {
        // Error
    }
    m_position = QVector3D(float(position.x()), float(function(position.x(), position.y())), float(position.y()));
    return function(position.x(), position.y());
}

QVector3D Graph3D::getPosition()
{
    return m_position;
}

void Graph3D::setPositionSlot(QPointF position)
{
    setPosition(position);
}
