#include "qdeclarativelandmarkmodel_p.h"
#include <QTimer>

#ifdef QDECLARATIVE_LANDMARK_DEBUG
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

QDeclarativeLandmarkAbstractModel::QDeclarativeLandmarkAbstractModel(QObject *parent) :
        QAbstractListModel(parent), m_manager(0),
        m_componentCompleted(false), m_updatePending(false), m_autoUpdate(true),
        m_limit(-1), m_offset(-1)
{
}

QDeclarativeLandmarkAbstractModel::~QDeclarativeLandmarkAbstractModel()
{
    delete m_manager;
}

void QDeclarativeLandmarkAbstractModel::componentComplete()
{
    m_componentCompleted = true;
    if (!m_manager) {
        m_manager = new QLandmarkManager();
        connectManager();
    }
    if (m_autoUpdate)
        scheduleUpdate();
}

void QDeclarativeLandmarkAbstractModel::connectManager()
{
    if (!m_manager)
        return;
    connect(m_manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)),
            this, SLOT(categoriesChanged(QList<QLandmarkCategoryId>)));
    connect(m_manager, SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)),
            this, SLOT(categoriesChanged(QList<QLandmarkCategoryId>)));
    connect(m_manager, SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)),
            this, SLOT(categoriesChanged(QList<QLandmarkCategoryId>)));
    connect(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)),
            this, SLOT(landmarksChanged(QList<QLandmarkId>)));
    connect(m_manager, SIGNAL(landmarksChanged(QList<QLandmarkId>)),
            this, SLOT(landmarksChanged(QList<QLandmarkId>)));
    connect(m_manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)),
            this, SLOT(landmarksChanged(QList<QLandmarkId>)));
    connect(m_manager, SIGNAL(dataChanged()),
            this, SLOT(dataChanged()));
}

void QDeclarativeLandmarkAbstractModel::categoriesChanged(const QList<QLandmarkCategoryId>& ids)
{
    if (m_autoUpdate)
        update();
    emit modelChanged();
}

void QDeclarativeLandmarkAbstractModel::landmarksChanged(const QList<QLandmarkId>& ids)
{
    if (m_autoUpdate)
        update();
    emit modelChanged();
}

void QDeclarativeLandmarkAbstractModel::dataChanged()
{
    if (m_autoUpdate)
        update();
    emit modelChanged();
}

void QDeclarativeLandmarkAbstractModel::setAutoUpdate(bool autoUpdate)
{
    if (autoUpdate == m_autoUpdate)
        return;
    m_autoUpdate = autoUpdate;
    emit autoUpdateChanged();
}

bool QDeclarativeLandmarkAbstractModel::autoUpdate() const
{
    return m_autoUpdate;
}

void QDeclarativeLandmarkAbstractModel::scheduleUpdate()
{
    if (!m_componentCompleted || m_updatePending)
        return;
    m_updatePending = true; // Disallow possible duplicate request triggering
    QMetaObject::invokeMethod(this, "startUpdate", Qt::QueuedConnection);
}

QString QDeclarativeLandmarkAbstractModel::error() const
{
    return m_error;
}

// This is purely for testing purposes
void QDeclarativeLandmarkAbstractModel::setDbFileName(QString fileName)
{
    if (fileName == m_dbFileName)
        return;
    m_dbFileName = fileName;
    if (m_manager) {
        cancelUpdate();
        delete m_manager;
    }

    QMap<QString, QString> map;
    map["filename"] = m_dbFileName;
    m_manager = new QLandmarkManager("com.nokia.qt.landmarks.engines.sqlite", map);
    connectManager();
}

void QDeclarativeLandmarkAbstractModel::update()
{
    scheduleUpdate();
}

int QDeclarativeLandmarkAbstractModel::limit()
{
    return m_limit;
}

void QDeclarativeLandmarkAbstractModel::setLimit(int limit)
{
    if (limit == m_limit)
        return;
    m_limit = limit;
    emit limitChanged();
}

int QDeclarativeLandmarkAbstractModel::offset()
{
    return m_offset;
}

void QDeclarativeLandmarkAbstractModel::setOffset(int offset)
{
    if (offset == m_offset)
        return;
    m_offset = offset;
    emit offsetChanged();
}

QDeclarativeLandmarkModel::QDeclarativeLandmarkModel(QObject *parent) :
        QDeclarativeLandmarkAbstractModel(parent),
        m_filter(0), m_fetchRequest(0), m_sortingOrder(0), m_sortOrder(AscendingOrder), m_sortKey(DefaultSort)
{
    // Establish role names so that they can be queried from this model
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(NameRole, "name");
    roleNames.insert(PhoneNumberRole, "phoneNumber");
    roleNames.insert(DescriptionRole, "description");
    roleNames.insert(RadiusRole, "radius");
    roleNames.insert(IconSourceRole, "iconSource");
    roleNames.insert(UrlRole, "url");
    roleNames.insert(LatitudeRole, "latitude");
    roleNames.insert(LongitudeRole, "longitude");
    setRoleNames(roleNames);
}

QDeclarativeLandmarkModel::~QDeclarativeLandmarkModel()
{
    delete m_fetchRequest;
    delete m_sortingOrder;
}

// When the parent is valid it means that rowCount is returning the number of children of parent.
int QDeclarativeLandmarkModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_landmarks.count();
}

// Returns the stored under the given role for the item referred to by the index.
QVariant QDeclarativeLandmarkModel::data(const QModelIndex &index, int role) const
{
    QLandmark landmark = m_landmarks.value(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return landmark.name();
        case NameRole:
            return landmark.name();
        case PhoneNumberRole:
            return landmark.phoneNumber();
        case DescriptionRole:
            return landmark.description();
        case RadiusRole:
            return landmark.radius();
        case IconSourceRole:
            return landmark.iconUrl();
        case UrlRole:
            return landmark.url();
        case LatitudeRole:
            return landmark.coordinate().latitude();
        case LongitudeRole:
            return landmark.coordinate().longitude();
    }
    return QVariant();
}

QDeclarativeLandmarkFilterBase* QDeclarativeLandmarkModel::filter()
{
    return m_filter;
}

void QDeclarativeLandmarkModel::setFilter(QDeclarativeLandmarkFilterBase* filter)
{
    if (filter == m_filter)
        return;
    m_filter = filter;
    scheduleUpdate();
    emit filterChanged();
}

void QDeclarativeLandmarkModel::startUpdate()
{
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug("QDeclarativeLandmarkModel::startUpdate()");
#endif
    if (!m_manager)
        return;
    // Clear any previous updates and request new
    cancelUpdate();
    m_fetchRequest = new QLandmarkFetchRequest(m_manager, this);
    QObject::connect(m_fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)), this, SLOT(fetchRequestStateChanged(QLandmarkAbstractRequest::State)));
    // Set filters, orders, and limits
    if (m_filter) {
        QLandmarkFilter* filter = m_filter->filter();
        if (filter)
            m_fetchRequest->setFilter(*filter);
    }
    setFetchRange();
    setFetchOrder();
    m_fetchRequest->start();
    m_updatePending = false; // Allow requesting updates again
}

void QDeclarativeLandmarkModel::cancelUpdate()
{
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug("QDeclarativeLandmarkModel::cancelUpdate()");
#endif
    if (m_fetchRequest) {
        delete m_fetchRequest;
        m_fetchRequest = 0;
    }
    if (m_sortingOrder) {
        delete m_sortingOrder;
        m_sortingOrder = 0;
    }
}

int QDeclarativeLandmarkModel::count()
{
    return m_landmarks.count();
}

void QDeclarativeLandmarkModel::setFetchRange()
{
    if (!m_fetchRequest || ((m_limit <= 0) && (m_offset <= 0)))
        return;
    if (m_limit > 0)
        m_fetchRequest->setLimit(m_limit);
    if ((m_offset > 0))
        m_fetchRequest->setOffset(m_offset);
}

void QDeclarativeLandmarkModel::setFetchOrder()
{
    if (!m_fetchRequest)
        return;
    if (m_sortingOrder) {
        // Arguably existing could be recycled but would introduce
        // just complex logic. Perhaps a later optimization.
        delete m_sortingOrder;
        m_sortingOrder = 0;
    }
    switch (m_sortKey) {
        case DefaultSort:
            m_sortingOrder = new QLandmarkSortOrder();
            break;
        case NameSort:
            m_sortingOrder = new QLandmarkNameSort();
            break;
        default:
            return;

    }
    m_sortingOrder->setDirection((Qt::SortOrder)m_sortOrder);
    m_fetchRequest->setSorting(*m_sortingOrder);
}

void QDeclarativeLandmarkModel::convertLandmarksToDeclarative()
{
    foreach(const QLandmark& landmark, m_landmarks) {
        if (!m_landmarkMap.contains(landmark.landmarkId().localId())) {
            QDeclarativeLandmark* declarativeLandmark = new QDeclarativeLandmark(this);
            declarativeLandmark->setLandmark(landmark);
            m_landmarkMap.insert(landmark.landmarkId().localId(), declarativeLandmark);
        } else {
            // The landmark exists already, update it
            m_landmarkMap.value(landmark.landmarkId().localId())->setLandmark(landmark);
        }
    }
}

QDeclarativeLandmarkModel::SortKey QDeclarativeLandmarkModel::sortBy() const
{
    return m_sortKey;
}

void QDeclarativeLandmarkModel::setSortBy(QDeclarativeLandmarkModel::SortKey key)
{
    if (key == m_sortKey)
        return;
    m_sortKey = key;
    emit sortByChanged();
}

QDeclarativeLandmarkModel::SortOrder QDeclarativeLandmarkModel::sortOrder() const
{
    return m_sortOrder;
}

void QDeclarativeLandmarkModel::setSortOrder(QDeclarativeLandmarkModel::SortOrder order)
{
    if (order == m_sortOrder)
        return;
    m_sortOrder = order;
    emit sortOrderChanged();
}

Q_INVOKABLE QList<QDeclarativeLandmark*> QDeclarativeLandmarkModel::landmarks() const
{
    return m_landmarkMap.values();
}

void QDeclarativeLandmarkModel::fetchRequestStateChanged(QLandmarkAbstractRequest::State state)
{
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug() << "QDeclarativeLandmarkModel::Fetch request finished with state: " << state;
    if (m_fetchRequest) {
        qDebug() << "and related request error code is: " << m_fetchRequest->errorString();
        qDebug() << "and received this many landmarks:" << m_fetchRequest->landmarks().count();
    }
#endif
    if (!m_fetchRequest || state != QLandmarkAbstractRequest::FinishedState)
        return;

    if (m_fetchRequest->error() == QLandmarkManager::NoError) {
        // Later improvement item is to make udpate incremental by connecting to resultsAvailable() -function.
        beginInsertRows(QModelIndex(), 0, m_landmarks.count()); // TODO check if this shuold be amount of received landmarks
        int oldCount = m_landmarks.count();
        m_landmarks = m_fetchRequest->landmarks();
        endInsertRows();
        if (oldCount != m_landmarks.count())
            emit countChanged();
    } else if (m_error != m_fetchRequest->errorString()) {
        m_error = m_fetchRequest->errorString();
        emit errorChanged();
    }
    // Convert into declarative classes --> possible to return landmarks in a list in QML
    convertLandmarksToDeclarative();
}

#include "moc_qdeclarativelandmarkmodel_p.cpp"

QTM_END_NAMESPACE