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

#include <QString>

class DrawingWidget;
class DrawingItem;
/*class DrawingItemGroup;
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
class DrawingTextRectItem;*/
class QuaZip;

class VsdxWriter
{
private:
	DrawingWidget* mDrawing;

	QString mDrawingUnits;
	qreal mDrawingScale;

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

	/*void writeRectItem(QXmlStreamWriter* xml, DrawingRectItem* item);
	void writeEllipseItem(QXmlStreamWriter* xml, DrawingEllipseItem* item);
	void writeLineItem(QXmlStreamWriter* xml, DrawingLineItem* item);
	void writeCurveItem(QXmlStreamWriter* xml, DrawingCurveItem* item);
	void writePolylineItem(QXmlStreamWriter* xml, DrawingPolylineItem* item);
	void writePolygonItem(QXmlStreamWriter* xml, DrawingPolygonItem* item);
	void writeTextItem(QXmlStreamWriter* xml, DrawingTextItem* item);
	void writeTextRectItem(QXmlStreamWriter* xml, DrawingTextRectItem* item);
	void writeTextEllipseItem(QXmlStreamWriter* xml, DrawingTextEllipseItem* item);
	void writePathItem(QXmlStreamWriter* xml, DrawingPathItem* item);
	void writeGroupItem(QXmlStreamWriter* xml, DrawingItemGroup* item);
	void writeReferenceItem(QXmlStreamWriter* xml, DrawingReferenceItem* item);

	void writeArrow(QXmlStreamWriter* xml, DrawingItem* item, const DrawingArrow& arrow, const QPen& pen);

	QString colorToString(const QColor& color) const;
	QString pointsToString(const QPolygonF& polygon) const;
	QString styleToString(const QBrush& brush, const QPen& pen);
	QString styleToString(const QBrush& brush, const QPen& pen, const QFont& font, Qt::Alignment alignment);
	QString transformToString(const QPointF& position, const QTransform& transform) const;*/
};

#endif
