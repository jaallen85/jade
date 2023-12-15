// File: OdgItem.cpp
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

#include "OdgItem.h"
#include "OdgControlPoint.h"
#include "OdgGluePoint.h"
#include <QPainter>

OdgItem::OdgItem() :
    mPosition(), mFlipped(false), mRotation(0), mTransform(), mTransformInverse(),
    mControlPoints(), mGluePoints(), mSelected(false)
{
    // Nothing more to do here.
}

OdgItem::~OdgItem()
{
    clearGluePoints();
    clearControlPoints();
}

//======================================================================================================================

void OdgItem::setPosition(const QPointF& position)
{
    mPosition = position;
    updateTransform();
}

void OdgItem::setFlipped(bool flipped)
{
    mFlipped = flipped;
    updateTransform();
}

void OdgItem::setRotation(int rotation)
{
    mRotation = (rotation % 4);
    updateTransform();
}

QPointF OdgItem::position() const
{
    return mPosition;
}

bool OdgItem::isFlipped() const
{
    return mFlipped;
}

int OdgItem::rotation() const
{
    return mRotation;
}

QTransform OdgItem::transform() const
{
    return mTransform;
}

QTransform OdgItem::transformInverse() const
{
    return mTransformInverse;
}

QPointF OdgItem::mapToScene(const QPointF& position) const
{
    return mTransform.map(position);
}

QRectF OdgItem::mapToScene(const QRectF& rect) const
{
    return QRectF(mapToScene(rect.topLeft()), mapToScene(rect.bottomRight()));
}

QPolygonF OdgItem::mapToScene(const QPolygonF& polygon) const
{
    return mTransform.map(polygon);
}

QPainterPath OdgItem::mapToScene(const QPainterPath& path) const
{
    return mTransform.map(path);
}

QPointF OdgItem::mapFromScene(const QPointF& position) const
{
    return mTransformInverse.map(position);
}

QRectF OdgItem::mapFromScene(const QRectF& rect) const
{
    return QRectF(mapFromScene(rect.topLeft()), mapFromScene(rect.bottomRight()));
}

QPolygonF OdgItem::mapFromScene(const QPolygonF& polygon) const
{
    return mTransformInverse.map(polygon);
}

QPainterPath OdgItem::mapFromScene(const QPainterPath& path) const
{
    return mTransformInverse.map(path);
}

//======================================================================================================================

void OdgItem::addControlPoint(OdgControlPoint* point)
{
    if (point)
    {
        mControlPoints.append(point);
        point->mItem = this;
    }
}

void OdgItem::insertControlPoint(int index, OdgControlPoint* point)
{
    if (point)
    {
        mControlPoints.insert(index, point);
        point->mItem = this;
    }
}

void OdgItem::removeControlPoint(OdgControlPoint* point)
{
    if (point)
    {
        mControlPoints.removeAll(point);
        point->mItem = nullptr;
    }
}

void OdgItem::clearControlPoints()
{
    qDeleteAll(mControlPoints);
    mControlPoints.clear();
}

QList<OdgControlPoint*> OdgItem::controlPoints() const
{
    return mControlPoints;
}

//======================================================================================================================

void OdgItem::addGluePoint(OdgGluePoint* point)
{
    if (point)
    {
        mGluePoints.append(point);
        point->mItem = this;
    }
}

void OdgItem::insertGluePoint(int index, OdgGluePoint* point)
{
    if (point)
    {
        mGluePoints.insert(index, point);
        point->mItem = this;
    }
}

void OdgItem::removeGluePoint(OdgGluePoint* point)
{
    if (point)
    {
        mGluePoints.removeAll(point);
        point->mItem = nullptr;
    }
}

void OdgItem::clearGluePoints()
{
    qDeleteAll(mGluePoints);
    mGluePoints.clear();
}

QList<OdgGluePoint*> OdgItem::gluePoints() const
{
    return mGluePoints;
}

//======================================================================================================================

void OdgItem::setSelected(bool selected)
{
    mSelected = selected;
}

bool OdgItem::isSelected() const
{
    return mSelected;
}

//======================================================================================================================

void OdgItem::setProperty(const QString& name, const QVariant& value)
{
    // Nothing to do here
}

QVariant OdgItem::property(const QString& name) const
{
    return QVariant();
}

//======================================================================================================================

bool OdgItem::isValid() const
{
    return true;
}

//======================================================================================================================

void OdgItem::resize(OdgControlPoint* point, const QPointF& position, bool snapTo45Degrees)
{
    if (point) point->setPosition(mapFromScene(position));
}

//======================================================================================================================

void OdgItem::rotate(const QPointF& center)
{
    setPosition(rotatePoint(mPosition, center));
    setRotation(mRotation + 1);
}

void OdgItem::rotateBack(const QPointF& center)
{
    setPosition(rotateBackPoint(mPosition, center));
    setRotation(mRotation - 1);
}

void OdgItem::flipHorizontal(const QPointF& center)
{
    setPosition(flipPointHorizontal(mPosition, center));
    setFlipped(!mFlipped);
}

void OdgItem::flipVertical(const QPointF& center)
{
    rotate(center);
    rotate(center);
    flipHorizontal(center);
}

//======================================================================================================================

bool OdgItem::canInsertPoints() const
{
    return false;
}

bool OdgItem::canRemovePoints() const
{
    return false;
}

int OdgItem::insertPointIndex(const QPointF& position)
{
    return -1;
}

int OdgItem::removePointIndex(const QPointF& position)
{
    return -1;
}

//======================================================================================================================

void OdgItem::scaleBy(double scale)
{
    setPosition(QPointF(mPosition.x() * scale, mPosition.y() * scale));
    for(auto& controlPoint : qAsConst(mControlPoints))
        controlPoint->setPosition(QPointF(controlPoint->position().x() * scale, controlPoint->position().y() * scale));
}

void OdgItem::placeCreateEvent(const QRectF& contentRect, double grid)
{
    // Nothing to do here.
}

OdgControlPoint* OdgItem::placeResizeStartPoint() const
{
    return nullptr;
}

OdgControlPoint* OdgItem::placeResizeEndPoint() const
{
    return nullptr;
}

//======================================================================================================================

void OdgItem::updateTransform()
{
    mTransform.reset();
    mTransform.translate(mPosition.x(), mPosition.y());
    if (mFlipped) mTransform.scale(-1, 1);
    mTransform.rotate(90 * mRotation);

    mTransformInverse = mTransform.inverted();
}

//======================================================================================================================

QPainterPath OdgItem::strokePath(const QPainterPath& path, const QPen& pen) const
{
    if (path.isEmpty()) return path;

    QPainterPathStroker ps;
    ps.setWidth((pen.widthF() <= 0.0) ? 1E-6 : pen.widthF());
    ps.setCapStyle(Qt::SquareCap);
    ps.setJoinStyle(Qt::BevelJoin);
    return ps.createStroke(path);
}

//======================================================================================================================

QPointF OdgItem::snapResizeTo45Degrees(OdgControlPoint* point, const QPointF& position, OdgControlPoint* startPoint,
                                       OdgControlPoint* endPoint) const
{
    OdgControlPoint* otherPoint = nullptr;
    if (point == startPoint) otherPoint = endPoint;
    else if (point == endPoint) otherPoint = startPoint;

    if (otherPoint)
    {
        const QPointF otherPosition = mapToScene(otherPoint->position());
        const QPointF delta = position - otherPosition;

        const double angle = qAtan2(position.y() - otherPosition.y(), position.x() - otherPosition.x());
        const int targetAngleDegrees = qRound(qRadiansToDegrees(angle) / 45) * 45;
        const double targetAngleRadians = qDegreesToRadians(targetAngleDegrees);

        double length = qMax(qAbs(delta.x()), qAbs(delta.y()));
        if (qAbs(targetAngleDegrees % 90) == 45)
            length *= qSqrt(2);

        return QPointF(otherPosition.x() + length * qCos(targetAngleRadians),
                       otherPosition.y() + length * qSin(targetAngleRadians));
    }

    return position;
}

//======================================================================================================================

QPointF OdgItem::rotatePoint(const QPointF& point, const QPointF& center) const
{
    const QPointF difference = point - center;
    return QPointF(center.x() - difference.y(), center.y() + difference.x());
}

QPointF OdgItem::rotateBackPoint(const QPointF& point, const QPointF& center) const
{
    const QPointF difference = point - center;
    return QPointF(center.x() + difference.y(), center.y() - difference.x());
}

QPointF OdgItem::flipPointHorizontal(const QPointF& point, const QPointF& center) const
{
    return QPointF(2 * center.x() - point.x(), point.y());
}

QPointF OdgItem::flipPointVertical(const QPointF& point, const QPointF& center) const
{
    return QPointF(point.x(), 2 * center.y() - point.y());
}

//======================================================================================================================

double OdgItem::distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const
{
    // Let A = line.p1(), B = line.p2(), and C = point
    const double dotAbBc = (line.x2() - line.x1()) * (point.x() - line.x2()) + (line.y2() - line.y1()) * (point.y() - line.y2());
    const double dotBaAc = (line.x1() - line.x2()) * (point.x() - line.x1()) + (line.y1() - line.y2()) * (point.y() - line.y1());
    const double crossABC = (line.x2() - line.x1()) * (point.y() - line.y1()) - (line.y2() - line.y1()) * (point.x() - line.x1());
    const double distanceAB = qSqrt((line.x2() - line.x1()) * (line.x2() - line.x1()) + (line.y2() - line.y1()) * (line.y2() - line.y1()));
    const double distanceAC = qSqrt((point.x() - line.x1()) * (point.x() - line.x1()) + (point.y() - line.y1()) * (point.y() - line.y1()));
    const double distanceBC = qSqrt((point.x() - line.x2()) * (point.x() - line.x2()) + (point.y() - line.y2()) * (point.y() - line.y2()));

    if (distanceAB != 0)
    {
        if (dotAbBc > 0) return distanceBC;
        if (dotBaAc > 0) return distanceAC;
        return qAbs(crossABC) / distanceAB;
    }

    return 1.0E12;
}

OdgControlPoint* OdgItem::pointNearest(const QPointF& position) const
{
    if (!mControlPoints.isEmpty())
    {
        OdgControlPoint* nearestPoint = mControlPoints.first();

        QPointF vec = nearestPoint->position() - position;
        double minimumDistanceSquared = vec.x() * vec.x() + vec.y() * vec.y();
        double distanceSquared = 0;

        for(auto& controlPoint : mControlPoints)
        {
            vec = controlPoint->position() - position;
            distanceSquared = vec.x() * vec.x() + vec.y() * vec.y();
            if (distanceSquared < minimumDistanceSquared)
            {
                nearestPoint = controlPoint;
                minimumDistanceSquared = distanceSquared;
            }
        }

        return nearestPoint;
    }

    return nullptr;
}

//======================================================================================================================

QRectF OdgItem::drawText(QPainter& painter, const QPointF& anchorPoint, const QFont& font, Qt::Alignment alignment,
                         const QSizeF& padding, const QBrush& brush, const QString& caption)
{
    QRectF textRect;

    if (isValid())
    {
        QRectF scaledTextRect;
        double scaleFactor = 1.0;

        calculateTextRect(anchorPoint, font, alignment, padding, caption, textRect, scaledTextRect, scaleFactor);

        QFont scaledFont = font;
        scaledFont.setPointSizeF(scaledFont.pointSizeF() * scaleFactor * 72 / 96);

        painter.scale(1 / scaleFactor, 1 / scaleFactor);
        painter.setBrush(QBrush(Qt::transparent));
        painter.setPen(QPen(brush, 0.0));
        painter.setFont(scaledFont);
        painter.drawText(scaledTextRect, alignment, caption);
        painter.scale(scaleFactor, scaleFactor);
    }

    return textRect;
}

void OdgItem::calculateTextRect(const QPointF& anchorPoint, const QFont& font, Qt::Alignment alignment,
                                const QSizeF& padding, const QString& caption, QRectF& itemTextRect,
                                QRectF& scaledTextRect, double& scaleFactor) const
{
    if (isValid())
    {
        scaleFactor = calculateTextScaleFactor(font);

        QFont scaledFont = font;
        scaledFont.setPointSizeF(scaledFont.pointSizeF() * scaleFactor * 72 / 96);

        // Determine text width and height
        QFontMetricsF scaledFontMetrics(scaledFont);
        double scaledTextWidth = 0, scaledTextHeight = 0;
        const QStringList lines = caption.split("\n");
        for(auto& line : lines)
        {
            scaledTextWidth = qMax(scaledTextWidth, scaledFontMetrics.boundingRect(line).width());
            scaledTextHeight += scaledFontMetrics.lineSpacing();
        }
        scaledTextHeight -= scaledFontMetrics.leading();

        // Determine text left and top
        double scaledTextLeft = anchorPoint.x() * scaleFactor;
        double scaledTextTop = anchorPoint.y() * scaleFactor;

        if (alignment & Qt::AlignHCenter)
            scaledTextLeft -= scaledTextWidth / 2;
        else if (alignment & Qt::AlignRight)
            scaledTextLeft -= scaledTextWidth;
        if (alignment & Qt::AlignVCenter)
            scaledTextTop -= scaledTextHeight / 2;
        else if (alignment & Qt::AlignBottom)
            scaledTextTop -= scaledTextHeight;

        if (alignment & Qt::AlignLeft)
            scaledTextLeft += padding.width() * scaleFactor;
        else if (alignment & Qt::AlignRight)
            scaledTextLeft -= padding.width() * scaleFactor;
        if (alignment & Qt::AlignTop)
            scaledTextTop += padding.height() * scaleFactor;
        else if (alignment & Qt::AlignBottom)
            scaledTextTop -= padding.height() * scaleFactor;

        // Scale the text rect to item coordinates
        double itemTextWidth = scaledTextWidth / scaleFactor;
        double itemTextHeight = scaledTextHeight / scaleFactor;
        double itemTextLeft = anchorPoint.x();
        double itemTextTop = anchorPoint.y();

        if (alignment & Qt::AlignHCenter)
            itemTextLeft -= itemTextWidth / 2;
        else if (alignment & Qt::AlignRight)
            itemTextLeft -= itemTextWidth;
        if (alignment & Qt::AlignVCenter)
            itemTextTop -= itemTextHeight / 2;
        else if (alignment & Qt::AlignBottom)
            itemTextTop -= itemTextHeight;

        if (alignment & Qt::AlignLeft)
            itemTextLeft += padding.width();
        else if (alignment & Qt::AlignRight)
            itemTextLeft -= padding.width();
        if (alignment & Qt::AlignTop)
            itemTextTop += padding.height();
        else if (alignment & Qt::AlignBottom)
            itemTextTop -= padding.height();

        itemTextRect = QRectF(itemTextLeft, itemTextTop, itemTextWidth, itemTextHeight);
        scaledTextRect = QRectF(scaledTextLeft, scaledTextTop, scaledTextWidth, scaledTextHeight);
    }
}

double OdgItem::calculateTextScaleFactor(const QFont& font) const
{
    if (font.pointSizeF() < 10)
    {
        // For very small point sizes (<~0.01), QPainter.drawText gives off the following error:
        // QWindowsFontEngineDirectWrite::addGlyphsToPath: GetGlyphRunOutline failed (The operation completed
        // successfully.)
        // We assume that point sizes in the range 10 to 100 are handled well, so scale the font size to within
        // this range.
        return qPow(10, qCeil(2 - std::log10(font.pointSizeF())));
    }

    return 1.0;
}

//======================================================================================================================

QList<OdgItem*> OdgItem::copyItems(const QList<OdgItem*>& items)
{
    QList<OdgItem*> copiedItems;

    // Copy the items
    for(auto& item : items) copiedItems.append(item->copy());

    // Maintain connections to other items in this list
    const int numberOfItems = items.size();
    OdgItem* item = nullptr;
    QList<OdgGluePoint*> gluePoints;
    int gluePointIndex = 0, numberOfGluePoints = 0;
    OdgGluePoint* gluePoint = nullptr;
    QList<OdgControlPoint*> controlPoints;
    int controlPointIndex = 0, numberOfControlPoints = 0;
    OdgControlPoint* controlPoint = nullptr;
    int controlPointItemIndex = 0, controlPointItemPointIndex = 0;

    OdgItem* copiedItem = nullptr;
    OdgGluePoint* copiedItemGluePoint = nullptr;
    OdgItem* copiedTargetItem = nullptr;
    OdgControlPoint* copiedTargetItemControlPoint = nullptr;

    for(int itemIndex = 0; itemIndex < numberOfItems; itemIndex++)
    {
        item = items.at(itemIndex);
        gluePoints = item->gluePoints();
        numberOfGluePoints = gluePoints.size();
        for(gluePointIndex = 0; gluePointIndex < numberOfGluePoints; gluePointIndex++)
        {
            gluePoint = gluePoints.at(gluePointIndex);

            controlPoints = gluePoint->connections();
            numberOfControlPoints = controlPoints.size();
            for(controlPointIndex = 0; controlPointIndex < numberOfControlPoints; controlPointIndex++)
            {
                controlPoint = controlPoints.at(controlPointIndex);

                if (items.contains(controlPoint->item()))
                {
                    // There is a connection here that must be maintained in the copied items
                    controlPointItemIndex = items.indexOf(controlPoint->item());
                    controlPointItemPointIndex = controlPoint->item()->controlPoints().indexOf(controlPoint);

                    copiedItem = copiedItems.at(itemIndex);
                    copiedItemGluePoint = copiedItem->gluePoints().value(gluePointIndex);
                    copiedTargetItem = copiedItems.at(controlPointItemIndex);
                    copiedTargetItemControlPoint = copiedTargetItem->controlPoints().value(controlPointItemPointIndex);

                    copiedTargetItemControlPoint->connect(copiedItemGluePoint);
                }
            }
        }
    }

	return copiedItems;
}
