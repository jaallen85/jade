/* OdgWriter.h
 *
 * Copyright (C) 2013-2016 Jason Allen
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

#ifndef ODGWRITER_H
#define ODGWRITER_H

#include <DiagramWidget.h>
#include <QtPrintSupport>

class QuaZip;

class OdgWriter
{
private:
	QString mErrorMessage;

	DiagramWidget* mDiagram;
	QPrinter* mPrinter;
	QString mFilePath;

	QString mContentStr;
	QString mStylesStr;

	QString mMetaStr;
	QString mSettingsStr;
	QString mManifestStr;

	QRectF mVisibleRect;
	qreal mDiagramScale;
	QString mDiagramUnits;
	QTransform mDiagramTransform;

	QHash<DrawingItem*,QString> mItemIds;
	QHash<DrawingItemStyle*,QString> mItemStyleIds;

public:
	OdgWriter();
	~OdgWriter();

	bool write(DiagramWidget* diagram, QPrinter* printer, const QString& filePath);
	QString errorMessage() const;

private:
	void getDiagramPrinterInfo();

	void writeContent();
	void writeStyles();

	void writeManifest();
	void writeMeta();
	void writeSettings();

	void finishOdg();
	void createFileInZip(QuaZip* zip, const QString& path, const QString& content);

	void writeItemStyles(QXmlStreamWriter& xml, const QList<DrawingItem*>& items);
	void writeItems(QXmlStreamWriter& xml, const QList<DrawingItem*>& items);

	void writeLineItem(QXmlStreamWriter& xml, DrawingLineItem* item);
	void writeArcItem(QXmlStreamWriter& xml, DrawingArcItem* item);
	void writePolylineItem(QXmlStreamWriter& xml, DrawingPolylineItem* item);
	void writeCurveItem(QXmlStreamWriter& xml, DrawingCurveItem* item);
	void writeRectItem(QXmlStreamWriter& xml, DrawingRectItem* item);
	void writeEllipseItem(QXmlStreamWriter& xml, DrawingEllipseItem* item);
	void writePolygonItem(QXmlStreamWriter& xml, DrawingPolygonItem* item);
	void writeTextItem(QXmlStreamWriter& xml, DrawingTextItem* item);
	void writeTextRectItem(QXmlStreamWriter& xml, DrawingTextRectItem* item);
	void writeTextEllipseItem(QXmlStreamWriter& xml, DrawingTextEllipseItem* item);
	void writeTextPolygonItem(QXmlStreamWriter& xml, DrawingTextPolygonItem* item);
	void writePathItem(QXmlStreamWriter& xml, DrawingPathItem* item);
	void writeItemGroup(QXmlStreamWriter& xml, DrawingItemGroup* item);

	void writeItemStyle(QXmlStreamWriter& xml, DrawingItemStyle* style);

	QString arrowToPathString(DrawingItemStyle::ArrowStyle arrowStyle, qreal arrowSize, qreal penWidth) const;
	QString colorToHexString(const QColor& color) const;
	QString penStyleToString(Qt::PenStyle style) const;
	QString penDashStyleToString(Qt::PenStyle style) const;
	QString penCapStyleToString(Qt::PenCapStyle style) const;
	QString penJoinStyleToString(Qt::PenJoinStyle style) const;
	QString pointsToString(const QPolygonF& points) const;
	QString transformToString(const QPointF& pos, qreal rotation, bool flipped) const;
};

#endif
