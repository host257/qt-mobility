#ifndef MAPBOX_H
#define MAPBOX_H

#include <qmobilityglobal.h>

#include <QWidget>
#include <QNetworkSession>

class QGraphicsScene;
class QGraphicsView;
class StatsWidget;
class QNetworkSession;

QTM_BEGIN_NAMESPACE
    class QGeoCoordinate;
    class QGraphicsGeoMap;
    class QGeoServiceProvider;
    class QGeoMappingManager;
    class QGeoMapRectangleObject;
    class QGeoMapCircleObject;
    class QGeoMapPolylineObject;
    class QGeoMapPolygonObject;
    class QGeoMapRouteObject;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class MapBox : public QWidget
{
    Q_OBJECT
private:
    QGraphicsView * m_qgv;
    QGraphicsScene * m_scene;
    QGraphicsGeoMap * m_mapWidget;

    QGeoServiceProvider *m_serviceProvider;
    QGeoMappingManager *m_mapManager;
    void createProvider();
    void createMapWidget();

    static QNetworkSession * m_session;

    QString m_providerId;
    QMap<QString,QVariant> m_parameters;

public:
    StatsWidget * m_statistics;
    explicit MapBox(QWidget *parent = 0);
    ~MapBox();

    QGraphicsGeoMap * map() { return m_mapWidget; }
    void setProvider(const QString & providerId);
    static MapBox * createOnlineMap(QWidget * parent = 0);
    static MapBox * createOfflineMap(QWidget * parent = 0);

    QGeoMapRectangleObject * addRectangle(qreal top, qreal left, qreal bottom, qreal right);
    QGeoMapRectangleObject * addRectangle(const QGeoCoordinate & topLeft, const QGeoCoordinate & bottomRight);
    QGeoMapPolylineObject * addPolyline(const QList<QGeoCoordinate> &path);
    QGeoMapPolygonObject * addPolygon(const QList<QGeoCoordinate> &path);
    QGeoMapCircleObject * addCircle(const QGeoCoordinate & center, qreal radius);
    void addRoute(const QGeoCoordinate & start, const QGeoCoordinate & end);
    void addRoute(const QList<QGeoCoordinate> & waypoints);

    static QNetworkSession * session();

    void setParameter(const QString & parameter, const QVariant & value);

    QPixmap grab();
    qreal squareError(MapBox * other);
    qreal squareError(const QPixmap & other);
    qreal squareError(const QImage & other);
    static qreal squareError(const QImage & image1, const QImage & image2);

    int countErrors(MapBox * other);
    int countErrors(const QPixmap & other);
    int countErrors(const QImage & other);
    static int countErrors(const QImage & image1, const QImage & image2);

public slots:
    void clearCache();

protected:
    virtual void resizeEvent(QResizeEvent * event);
    virtual void timerEvent(QTimerEvent * event);

private slots:
    void routeFinished();
};

#endif // MAPBOX_H