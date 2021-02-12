// VsdxWriter.h
// Copyright (C) 2021  Jason Allen
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

#ifndef VSDXWRITER_H
#define VSDXWRITER_H

#include <DrawingArrow.h>
#include <QBrush>
#include <QList>
#include <QPen>
#include <QPointF>
#include <QString>

class DrawingWidget;
class DrawingItem;
class DrawingItemGroup;
class DrawingCurveItem;
class DrawingEllipseItem;
class DrawingLineItem;
class DrawingPathItem;
class DrawingPolygonItem;
class DrawingPolylineItem;
class DrawingRectItem;
class DrawingReferenceItem;
class DrawingTextItem;
class DrawingTextEllipseItem;
class DrawingTextRectItem;
class QuaZip;

class VsdxWriter
{
private:
	DrawingWidget* mDrawing;

	QString mDrawingUnits;
	qreal mDrawingScale;
	qreal mPageMargin;

	int mShapeIndex;
	int mShapeDepth;

public:
	VsdxWriter(DrawingWidget* drawing);
	~VsdxWriter();

	bool save(const QString& filePath);

private:
	bool createFileInZip(QuaZip* zip, const QString& path, const QString& content);
	QString writeContentTypes();
	QString writeRels();
	QString writeApp();
	QString writeCore();
	QString writeCustom();
	QString writePagesRels();
	QString writePages();
	QString writePage1();
	QString writeDocumentRels();
	QString writeDocument();
	QString writeWindows();

	QString writeItems(const QList<DrawingItem*>& items);
	QString writeRectItem(DrawingRectItem* item);
	QString writeEllipseItem(DrawingEllipseItem* item);
	QString writeLineItem(DrawingLineItem* item);
	QString writeCurveItem(DrawingCurveItem* item);
	QString writePolylineItem(DrawingPolylineItem* item);
	QString writePolygonItem(DrawingPolygonItem* item);
	QString writeTextItem(DrawingTextItem* item);
	QString writeTextRectItem(DrawingTextRectItem* item);
	QString writeTextEllipseItem(DrawingTextEllipseItem* item);
	QString writePathItem(DrawingPathItem* item);
	QString writeGroupItem(DrawingItemGroup* item);
	QString writeReferenceItem(DrawingReferenceItem* item);

	QString writePositionAndSize(const QPointF& position, const QRectF& boundingRect, const QTransform& transform, QSizeF* shapeSize = nullptr);
	QString writePositionAndSizeForLine(const QPointF& begin, const QPointF& end, QSizeF* shapeSize = nullptr);

	QString writeStyle(const QBrush& brush, const QPen& pen);
	QString writeStyle(const QBrush& brush, const QPen& pen, const QBrush& textBrush, const QFont& font, Qt::Alignment alignment);
	QString writeArrow(const DrawingArrow& arrow, const QPen& pen, bool startArrow);

	QPointF mapFromScene(const QPointF& position) const;

	QString colorToString(const QColor& color) const;
};

#endif
