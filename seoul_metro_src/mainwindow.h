#pragma once
#include <QMainWindow>
#include <QEvent>
#include <QWheelEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QMap>
#include <QVector>
#include <QTimer>
#include <QLabel>
#include <QListWidget>
#include <QDockWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QStatusBar>

// ─────────────────────────────────────────────
// Data structures
// ─────────────────────────────────────────────

struct ArrivalInfo {
    QString lineNo;
    QString destination;
    int     minutesLeft;   // minutes until arrival
    QString trainId;
};

struct Station {
    QString id;
    QString nameKo;
    QString nameEn;
    QVector<QString> lines;   // e.g. {"1","2"}
    double  x, y;             // scene coordinates
};

struct Edge {
    QString fromId;
    QString toId;
    QString lineNo;
    int     travelTimeSec;
};

// ─────────────────────────────────────────────
// Custom QGraphicsItem for a station vertex
// ─────────────────────────────────────────────
class StationItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    explicit StationItem(const Station &st, QGraphicsItem *parent = nullptr);

    QString stationId() const { return m_id; }

signals:
    void clicked(const QString &stationId);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    QString m_id;
    QColor  m_baseColor;
};

// ─────────────────────────────────────────────
// Main Window
// ─────────────────────────────────────────────
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onStationClicked(const QString &stationId);
    void refreshArrivals();
    void onSearchTextChanged(const QString &text);
    void onSearchActivated(const QString &text);
    void onLineFilterChanged(const QString &line);

    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    // Build data
    void buildStationData();
    void buildEdgeData();

    // Build scene
    void buildScene();
    void drawEdges();
    void drawStations();
    void drawLegend();

    // UI helpers
    void setupDock();
    void updateInfoPanel(const QString &stationId);
    QVector<ArrivalInfo> generateArrivals(const QString &stationId);
    QColor lineColor(const QString &lineNo) const;
    QString lineColorName(const QString &lineNo) const;

    // Graph / BFS
    QVector<QString> shortestPath(const QString &fromId, const QString &toId);

    // Data
    QMap<QString, Station> m_stations;
    QVector<Edge>          m_edges;

    // Scene
    QGraphicsScene *m_scene = nullptr;
    QGraphicsView  *m_view  = nullptr;
    QMap<QString, StationItem *> m_items;

    // Dock / info panel
    QDockWidget  *m_dock       = nullptr;
    QLabel       *m_stationLbl = nullptr;
    QLabel       *m_linesLbl   = nullptr;
    QListWidget  *m_arrivalList= nullptr;
    QTimer       *m_refreshTimer = nullptr;
    QString       m_selectedStation;

    // Search bar
    QLineEdit  *m_searchEdit   = nullptr;
    QComboBox  *m_lineFilter   = nullptr;

    // Status
    QLabel *m_statusLbl = nullptr;
};
