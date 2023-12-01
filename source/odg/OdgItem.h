// File: OdgItem.h
// Copyright (C) 2023  Jason Allen
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ODGITEM_H
#define ODGITEM_H

#include <QList>
#include <QPainterPath>
#include <QTransform>

class QPainter;
class OdgControlPoint;
class OdgGluePoint;

class OdgItem
{
private:
    QPointF mPosition;
    bool mFlipped;
    int mRotation;
    QTransform mTransform, mTransformInverse;

    QList<OdgControlPoint*> mControlPoints;
    QList<OdgGluePoint*> mGluePoints;

    bool mSelected;

public:
    OdgItem();
    virtual ~OdgItem();

    void setPosition(const QPointF& position);
    void setFlipped(bool flipped);
    void setRotation(int rotation);
    QPointF position() const;
    bool isFlipped() const;
    int rotation() const;
    QTransform transform() const;
    QTransform transformInverse() const;
    QPointF mapToScene(const QPointF& position) const;
    QRectF mapToScene(const QRectF& rect) const;
    QPolygonF mapToScene(const QPolygonF& polygon) const;
    QPainterPath mapToScene(const QPainterPath& path) const;
    QPointF mapFromScene(const QPointF& position) const;
    QRectF mapFromScene(const QRectF& rect) const;
    QPolygonF mapFromScene(const QPolygonF& polygon) const;
    QPainterPath mapFromScene(const QPainterPath& path) const;

    void addControlPoint(OdgControlPoint* point);
    void insertControlPoint(int index, OdgControlPoint* point);
    void removeControlPoint(OdgControlPoint* point);
    void clearControlPoints();
    QList<OdgControlPoint*> controlPoints() const;

    void addGluePoint(OdgGluePoint* point);
    void insertGluePoint(int index, OdgGluePoint* point);
    void removeGluePoint(OdgGluePoint* point);
    void clearGluePoints();
    QList<OdgGluePoint*> gluePoints() const;

    void setSelected(bool selected);
    bool isSelected() const;

    virtual QRectF boundingRect() const = 0;
    virtual QPainterPath shape() const = 0;
    virtual bool isValid() const;

    virtual void paint(QPainter& painter) = 0;

    virtual void scaleBy(double scale);

protected:
    QPainterPath strokePath(const QPainterPath& path, const QPen& pen) const;

    QRectF drawText(QPainter& painter, const QPointF& anchorPoint, const QFont& font, Qt::Alignment alignment,
                    const QSizeF& padding, const QBrush& brush, const QString& caption);
    void calculateTextRect(const QPointF& anchorPoint, const QFont& font, Qt::Alignment alignment,
                           const QSizeF& padding, const QString& caption, QRectF& itemTextRect,
                           QRectF& scaledTextRect, double& scaleFactor) const;
    double calculateTextScaleFactor(const QFont& font) const;

private:
    void updateTransform();
};

#endif
