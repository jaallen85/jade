/* VsdxWriter.h
 *
 * Copyright (C) 2013-2017 Jason Allen
 *
 * This file is part of the jade application.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef VSDXWRITER_H
#define VSDXWRITER_H

#include <DiagramWidget.h>

class QPrinter;
class QuaZip;

class VsdxWriter
{
private:
	DiagramWidget* mDiagram;
	QPrinter* mPrinter;
	QString mFilePath;

	QString mErrorMessage;
	
	// Internal variables
	QString mDiagramUnits;
	qreal mDiagramScale;
	QPointF mDiagramTranslate;
	qreal mVsdxWidth;
	qreal mVsdxHeight;
	qreal mVsdxMargin;

public:
	VsdxWriter();
	~VsdxWriter();

	bool write(DiagramWidget* diagram, QPrinter* printer, const QString& filePath);
	QString errorMessage() const;

private:
	void writeVsdx();
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
	void createFileInZip(QuaZip* zip, const QString& path, const QString& content);

	QString writeItems(const QList<DrawingItem*>& items);
	QString writeLineItem(DrawingLineItem* item, int& index);
	QString writeArcItem(DrawingArcItem* item, int& index);
	QString writePolylineItem(DrawingPolylineItem* item, int& index);
	QString writeCurveItem(DrawingCurveItem* item, int& index);
	QString writeCurveItem(DrawingItem* item, const QPointF& curveStartPoint, const QPointF& curveStartControlPoint,
		const QPointF& curveEndControlPoint, const QPointF& curveEndPoint, int& index);
	QString writeRectItem(DrawingRectItem* item, int& index);
	QString writeEllipseItem(DrawingEllipseItem* item, int& index);
	QString writePolygonItem(DrawingPolygonItem* item, int& index);
	QString writeTextItem(DrawingTextItem* item, int& index);
	QString writeTextRectItem(DrawingTextRectItem* item, int& index);
	QString writeTextEllipseItem(DrawingTextEllipseItem* item, int& index);
	QString writeTextPolygonItem(DrawingTextPolygonItem* item, int& index);
	QString writePathItem(DrawingPathItem* item, int& index);
	QString writeItemGroup(DrawingItemGroup* item, int& index);

	QString writeItemStyle(DrawingItemStyle* style);

	QPointF mapFromScene(const QPointF& pos) const;
	QRectF mapFromScene(const QRectF& rect) const;
	QPolygonF mapFromScene(const QPolygonF& poly) const;
	QString colorToHexString(const QColor& color) const;
};

#endif
