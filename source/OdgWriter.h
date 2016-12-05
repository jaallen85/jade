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

class QPrinter;
class QuaZip;

class OdgWriter
{
private:
	struct ArrowStyle
	{
		DrawingItemStyle::ArrowStyle arrowStyle;
		qreal arrowSize;
		qreal penWidth;
	};
	
	DiagramWidget* mDiagram;
	QPrinter* mPrinter;
	QString mFilePath;

	QString mErrorMessage;

	// Internal variables
	QRectF mVisibleRect;
	qreal mDiagramScale;
	QString mDiagramUnits;
	QTransform mDiagramTransform;
	
	QList<DrawingItemStyle*> mItemStyles;
	QStringList mFontDecls;
	QList<Qt::PenStyle> mDashStyles;
	QList<ArrowStyle> mArrowStyles;
	
public:
	OdgWriter();
	~OdgWriter();

	bool write(DiagramWidget* diagram, QPrinter* printer, const QString& filePath);
	QString errorMessage() const;
	
private:
	void analyzeDiagram();
	void analyzeItemStyles();
	void findItemStyles(const QList<DrawingItem*>& items, QList<DrawingItemStyle*>& itemStyles);
	
	QString writeContent();
	QString writeStyles();
	QString writeMeta();
	QString writeSettings();
	QString writeManifest();
	void writeOdg(const QString& contentStr, const QString& stylesStr, const QString& metaStr, 
		const QString& settingsStr, const QString& manifestStr);
	void createFileInZip(QuaZip* zip, const QString& path, const QString& content);
		
private:
	void writeDefaultPageStyle(QXmlStreamWriter& xml);
	void writeFontDeclarations(QXmlStreamWriter& xml);
	void writeArrowStyles(QXmlStreamWriter& xml);
	void writeDashStyles(QXmlStreamWriter& xml);

	void writeItemStyles(QXmlStreamWriter& xml);
	void writeItemStyle(QXmlStreamWriter& xml, DrawingItemStyle* style);
	void writeItemParagraphStyle(QXmlStreamWriter& xml, DrawingItemStyle* style);
	QString itemStyleName(DrawingItemStyle* style);
	
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
	
private:
	QString fontStyleName(const QString& fontName) const;
	QString dashStyleName(Qt::PenStyle penStyle) const;
	
	void addArrowStyle(DrawingItemStyle::ArrowStyle arrowStyle, qreal arrowSize, qreal penWidth);
	void clearArrowStyles();
	bool containsArrowStyle(DrawingItemStyle::ArrowStyle arrowStyle, qreal arrowSize, qreal penWidth) const;
	QString arrowStyleName(DrawingItemStyle::ArrowStyle arrowStyle, qreal arrowSize, qreal penWidth) const;
	QString arrowStylePath(DrawingItemStyle::ArrowStyle arrowStyle, qreal arrowSize, qreal penWidth, QRectF& viewBox) const;
	bool arrowStyleCentered(DrawingItemStyle::ArrowStyle arrowStyle) const;

	QString colorToHexString(const QColor& color) const;
	QString penStyleToString(Qt::PenStyle style) const;
	QString penCapStyleToString(Qt::PenCapStyle style) const;
	QString penJoinStyleToString(Qt::PenJoinStyle style) const;
	QString pointsToString(const QPolygonF& points) const;
	QString transformToString(const QPointF& pos, qreal rotation, bool flipped) const;
};

#endif
