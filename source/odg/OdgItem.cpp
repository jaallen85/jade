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
    mRotation = rotation;
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
    if (point) mControlPoints.append(point);
}

void OdgItem::insertControlPoint(int index, OdgControlPoint* point)
{
    if (point) mControlPoints.insert(index, point);
}

void OdgItem::removeControlPoint(OdgControlPoint* point)
{
    if (point) mControlPoints.removeAll(point);
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
    if (point) mGluePoints.append(point);
}

void OdgItem::insertGluePoint(int index, OdgGluePoint* point)
{
    if (point) mGluePoints.insert(index, point);
}

void OdgItem::removeGluePoint(OdgGluePoint* point)
{
    if (point) mGluePoints.removeAll(point);
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

bool OdgItem::isValid() const
{
    return true;
}

//======================================================================================================================

void OdgItem::scaleBy(double scale)
{
    setPosition(QPointF(mPosition.x() * scale, mPosition.y() * scale));
    for(auto& controlPoint : qAsConst(mControlPoints))
        controlPoint->setPosition(QPointF(controlPoint->position().x() * scale, controlPoint->position().y() * scale));
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

QRectF OdgItem::drawText(QPainter& painter, const QPointF& anchorPoint, const QFont& font, Qt::Alignment alignment,
                         const QSizeF& padding, const QBrush& brush, const QString& caption)
{
    QRectF textRect;

    if (isValid())
    {
        QRectF scaledTextRect;
        double scaleFactor = 1;

        calculateTextRect(anchorPoint, font, alignment, padding, caption, textRect, scaledTextRect, scaleFactor);

        QFont scaledFont = font;
        scaledFont.setPointSizeF(scaledFont.pointSizeF() * scaleFactor);

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
        scaledFont.setPointSizeF(scaledFont.pointSizeF() * scaleFactor);

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

void OdgItem::updateTransform()
{
    mTransform.reset();
    mTransform.translate(mPosition.x(), mPosition.y());
    if (mFlipped) mTransform.scale(-1, 1);
    mTransform.rotate(90 * mRotation);

    mTransformInverse = mTransform.inverted();
}
