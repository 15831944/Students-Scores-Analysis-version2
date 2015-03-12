#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H

#include <QAbstractItemView>
#include <QFont>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QRect>
#include <QSize>
#include <QPoint>
#include <QWidget>
#include <QPainter>
#include <QPen>

QT_BEGIN_NAMESPACE
class QRubberBand;
QT_END_NAMESPACE

class DiagramView : public QAbstractItemView
{
    Q_OBJECT

public:
    DiagramView(QWidget *parent = 0);

    QRect visualRect(const QModelIndex &index) const;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
    QModelIndex indexAt(const QPoint &point) const;

protected slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

protected:
    bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event);
    QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction,
                           Qt::KeyboardModifiers modifiers);

    int horizontalOffset() const;
    int verticalOffset() const;

    bool isIndexHidden(const QModelIndex &index) const;

    void setSelection(const QRect&, QItemSelectionModel::SelectionFlags command);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void scrollContentsBy(int dx, int dy);

    QRegion visualRegionForSelection(const QItemSelection &selection) const;

private:
    QRect itemRect(const QModelIndex &item) const;
    QRegion itemRegion(const QModelIndex &index) const;
    int rows(const QModelIndex &index = QModelIndex()) const;
    void updateGeometries();

    QList<qreal> xList;//x��������
    QList<qreal> yDrawList;//��y��������
    QList<qreal> yWaList;//WA��y��������
    QList<qreal> yWbList;//WB��x��������
    int Counter;//��¼3�����ߵĵڼ�����

    int margin;
    int totalSize;
    int leftspace;
    int diagramSize;
    int diagramWidth;
    int diagramHeight;
    double heightrate;
    double widthrate;
    int validItems;
    double totalValue;
    QPoint origin;
    QRubberBand *rubberBand;
};

#endif // DIAGRAMVIEW_H
