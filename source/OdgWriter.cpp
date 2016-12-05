/* OdgWriter.cpp
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

#include "OdgWriter.h"
#include <quazip.h>
#include <quazipfile.h>

OdgWriter::OdgWriter() { }

OdgWriter::~OdgWriter() { }

//==================================================================================================

bool OdgWriter::write(DiagramWidget* diagram, QPrinter* printer, const QString& filePath)
{
	mErrorMessage.clear();
	mContentStr.clear();
	mStylesStr.clear();
	mMetaStr.clear();
	mSettingsStr.clear();
	mManifestStr.clear();
	mItemIds.clear();
	mItemStyleIds.clear();

	mFilePath = filePath;
	mPrinter = printer;
	mDiagram = diagram;

	getDiagramPrinterInfo();

	writeContent();
	writeStyles();

	writeMeta();
	writeSettings();
	writeManifest();

	finishOdg();

	return mErrorMessage.isEmpty();
}

QString OdgWriter::errorMessage() const
{
	return mErrorMessage;
}

//==================================================================================================

void OdgWriter::getDiagramPrinterInfo()
{
	if (mPrinter->pageLayout().pageSize().definitionUnits() == QPageSize::Millimeter ||
		mPrinter->pageLayout().pageSize().definitionUnits() == QPageSize::Cicero) mDiagramUnits = "mm";
	else mDiagramUnits = "in";

	QPrinter::Unit printerUnits = (mDiagramUnits == "mm") ? QPrinter::Millimeter : QPrinter::Inch;
	QPageLayout::Unit pageLayoutUnits = (mDiagramUnits == "mm") ? QPageLayout::Millimeter : QPageLayout::Inch;

	mVisibleRect = mDiagram->sceneRect();

	qreal pageAspect = mPrinter->pageRect(printerUnits).width() / (qreal)mPrinter->pageRect(printerUnits).height();
	mDiagramScale = qMin(mPrinter->pageRect(printerUnits).width() / mVisibleRect.width(),
		mPrinter->pageRect(printerUnits).height() / mVisibleRect.height());

	if (mVisibleRect.height() * pageAspect > mVisibleRect.width())
	{
		mVisibleRect.adjust(-(mVisibleRect.height() * pageAspect - mVisibleRect.width()) / 2, 0,
			(mVisibleRect.height() * pageAspect - mVisibleRect.width()) / 2, 0);
	}
	else if (mVisibleRect.width() / pageAspect > mVisibleRect.height())
	{
		mVisibleRect.adjust(0, -(mVisibleRect.width() / pageAspect - mVisibleRect.height()) / 2,
			0, (mVisibleRect.width() / pageAspect - mVisibleRect.height()) / 2);
	}

	mDiagramTransform = QTransform();
	mDiagramTransform.translate(mPrinter->pageLayout().margins(pageLayoutUnits).left(), mPrinter->pageLayout().margins(pageLayoutUnits).top());
	mDiagramTransform.scale(mDiagramScale, mDiagramScale);
	mDiagramTransform.translate(-mVisibleRect.left(), -mVisibleRect.top());
}

//==================================================================================================

void OdgWriter::writeContent()
{
	/*<office:document-content xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0";
 xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0";
 xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0";
 xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0";
 xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0";
 xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0";
 xmlns:xlink="http://www.w3.org/1999/xlink";
 xmlns:dc="http://purl.org/dc/elements/1.1/";
 xmlns:meta="urn:oasis:names:tc:opendocument:xmlns:meta:1.0";
 xmlns:number="urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0";
 xmlns:presentation="urn:oasis:names:tc:opendocument:xmlns:presentation:1.0";
 ;
 xmlns:chart="urn:oasis:names:tc:opendocument:xmlns:chart:1.0";
 xmlns:dr3d="urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0";
 xmlns:math="http://www.w3.org/1998/Math/MathML";
 xmlns:form="urn:oasis:names:tc:opendocument:xmlns:form:1.0";
 xmlns:script="urn:oasis:names:tc:opendocument:xmlns:script:1.0";
 xmlns:ooo="http://openoffice.org/2004/office";
 xmlns:ooow="http://openoffice.org/2004/writer";
 xmlns:oooc="http://openoffice.org/2004/calc";
 xmlns:dom="http://www.w3.org/2001/xml-events";
 xmlns:xforms="http://www.w3.org/2002/xforms";
 xmlns:xsd="http://www.w3.org/2001/XMLSchema";
 xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance";
 xmlns:smil="urn:oasis:names:tc:opendocument:xmlns:smil-compatible:1.0";
 xmlns:anim="urn:oasis:names:tc:opendocument:xmlns:animation:1.0";
 xmlns:rpt="http://openoffice.org/2005/report";
 xmlns:of="urn:oasis:names:tc:opendocument:xmlns:of:1.2";
 xmlns:xhtml="http://www.w3.org/1999/xhtml";
 xmlns:grddl="http://www.w3.org/2003/g/data-view#";
 xmlns:officeooo="http://openoffice.org/2009/office";
 xmlns:tableooo="http://openoffice.org/2009/table";
 xmlns:drawooo="http://openoffice.org/2010/draw";
 xmlns:calcext="urn:org:documentfoundation:names:experimental:calc:xmlns:calcext:1.0";
 xmlns:loext="urn:org:documentfoundation:names:experimental:office:xmlns:loext:1.0";
 xmlns:field="urn:openoffice:names:experimental:ooo-ms-interop:xmlns:field:1.0;
" xmlns:formx="urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:form:1.0";
 xmlns:css3t="http://www.w3.org/TR/css3-text/" office:version="1.2">*/

	//xml.writeAttribute("", "");

	QXmlStreamWriter xml(&mContentStr);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(2);

	xml.writeStartDocument();
	xml.writeStartElement("office:document-content");
	xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
	xml.writeAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
	xml.writeAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
	xml.writeAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
	xml.writeAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
	xml.writeAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");

	xml.writeStartElement("office:scripts");
	xml.writeEndElement();

	xml.writeStartElement("office:font-face-decls");
	/*xml.writeStartElement("style:font-face");
	xml.writeAttribute("style:name", "Arial");
	xml.writeAttribute("svg:font-family", "Arial");
	xml.writeAttribute("style:font-family-generic", "swiss");
	xml.writeAttribute("style:font-pitch", "variable");
	xml.writeEndElement();*/
	extractFontDeclarations();
	xml.writeEndElement();

	xml.writeStartElement("office:automatic-styles");
	xml.writeStartElement("style:style");
	xml.writeAttribute("style:name", "Page1");
	xml.writeAttribute("style:family", "drawing-page");
	xml.writeEndElement();

	writeItemStyles(xml, mDiagram->items());

	xml.writeEndElement();

	xml.writeStartElement("office:body");
	xml.writeStartElement("office:drawing");
	xml.writeStartElement("draw:page");
	xml.writeAttribute("draw:name", "Page1");
	xml.writeAttribute("draw:style-name", "Page1");
	xml.writeAttribute("draw:master-page-name", "DefaultPage");

	writeItems(xml, mDiagram->items());

	xml.writeEndElement();
	xml.writeEndElement();
	xml.writeEndElement();

	xml.writeEndElement();
	xml.writeEndDocument();
}

void OdgWriter::writeStyles()
{
	/*<office:document-styles
		xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0";
xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0";
xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0";
xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0";
xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0";
xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0";
xmlns:xlink="http://www.w3.org/1999/xlink";
xmlns:dc="http://purl.org/dc/elements/1.1/";
xmlns:meta="urn:oasis:names:tc:opendocument:xmlns:meta:1.0";
xmlns:number="urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0";
xmlns:presentation="urn:oasis:names:tc:opendocument:xmlns:presentation:1.0";
xmlns:svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0";
 xmlns:chart="urn:oasis:names:tc:opendocument:xmlns:chart:1.0";
 xmlns:dr3d="urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0";
 xmlns:math="http://www.w3.org/1998/Math/MathML";
 xmlns:form="urn:oasis:names:tc:opendocument:xmlns:form:1.0";
 xmlns:script="urn:oasis:names:tc:opendocument:xmlns:script:1.0";
 xmlns:ooo="http://openoffice.org/2004/office";
 xmlns:ooow="http://openoffice.org/2004/writer";
 xmlns:oooc="http://openoffice.org/2004/calc";
 xmlns:dom="http://www.w3.org/2001/xml-events";
 xmlns:smil="urn:oasis:names:tc:opendocument:xmlns:smil-compatible:1.0";
 xmlns:anim="urn:oasis:names:tc:opendocument:xmlns:animation:1.0;
" xmlns:rpt="http://openoffice.org/2005/report";
 xmlns:of="urn:oasis:names:tc:opendocument:xmlns:of:1.2";
 xmlns:xhtml="http://www.w3.org/1999/xhtml";
 xmlns:grddl="http://www.w3.org/2003/g/data-view#";
 xmlns:officeooo="http://openoffice.org/2009/office";
 xmlns:tableooo="http://openoffice.org/2009/table";
 xmlns:drawooo="http://openoffice.org/2010/draw";
 xmlns:calcext="urn:org:documentfoundation:names:experimental:calc:xmlns:calcext:1.0";
 xmlns:loext="urn:org:documentfoundation:names:experimental:office:xmlns:loext:1.0";
 xmlns:field="urn:openoffice:names:experimental:ooo-ms-interop:xmlns:field:1.0";
 xmlns:css3t="http://www.w3.org/TR/css3-text/" office:version="1.2">*/

	//xml.writeAttribute("", "");

	QXmlStreamWriter xml(&mStylesStr);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(2);

	xml.writeStartDocument();
	xml.writeStartElement("office:document-styles");
	xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
	xml.writeAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
	xml.writeAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
	xml.writeAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
	xml.writeAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");

	xml.writeStartElement("office:font-face-decls");
	xml.writeEndElement();

	xml.writeStartElement("office:styles");
	// various arrow styles

	// Dash styles
	xml.writeStartElement("draw:stroke-dash");
	xml.writeAttribute("draw:name", "dashed");
	xml.writeAttribute("draw:style", "round");
	xml.writeAttribute("draw:dots1", "1");
	xml.writeAttribute("draw:dots1-length", "300%");
	xml.writeAttribute("draw:dots2", "0");
	xml.writeAttribute("draw:dots2-length", "0%");
	xml.writeAttribute("draw:distance", "200%");
	xml.writeEndElement();

	xml.writeStartElement("draw:stroke-dash");
	xml.writeAttribute("draw:name", "dotted");
	xml.writeAttribute("draw:style", "round");
	xml.writeAttribute("draw:dots1", "1");
	xml.writeAttribute("draw:dots1-length", "100%");
	xml.writeAttribute("draw:dots2", "0");
	xml.writeAttribute("draw:dots2-length", "0%");
	xml.writeAttribute("draw:distance", "200%");
	xml.writeEndElement();

	xml.writeStartElement("draw:stroke-dash");
	xml.writeAttribute("draw:name", "dash-dotted");
	xml.writeAttribute("draw:style", "round");
	xml.writeAttribute("draw:dots1", "1");
	xml.writeAttribute("draw:dots1-length", "300%");
	xml.writeAttribute("draw:dots2", "1");
	xml.writeAttribute("draw:dots2-length", "100%");
	xml.writeAttribute("draw:distance", "200%");
	xml.writeEndElement();

	xml.writeStartElement("draw:stroke-dash");
	xml.writeAttribute("draw:name", "dash-dot-dotted");
	xml.writeAttribute("draw:style", "round");
	xml.writeAttribute("draw:dots1", "1");
	xml.writeAttribute("draw:dots1-length", "300%");
	xml.writeAttribute("draw:dots2", "2");
	xml.writeAttribute("draw:dots2-length", "100%");
	xml.writeAttribute("draw:distance", "200%");
	xml.writeEndElement();

	xml.writeEndElement();

	xml.writeStartElement("office:automatic-styles");
	xml.writeStartElement("style:page-layout");
	xml.writeAttribute("style:name", "DefaultPageLayout");
	xml.writeStartElement("style:page-layout-properties");

	if (mDiagramUnits == "mm")
	{
		QSizeF pageSize = mPrinter->pageLayout().pageSize().size(QPageSize::Millimeter);
		QMarginsF margins = mPrinter->pageLayout().margins(QPageLayout::Millimeter);
		qreal pageWidth = 0, pageHeight = 0;

		if (mPrinter->orientation() == QPrinter::Landscape)
		{
			pageWidth = qMax(pageSize.width(), pageSize.height());
			pageHeight = qMin(pageSize.width(), pageSize.height());
		}

		xml.writeAttribute("fo:page-width", QString::number(pageWidth) + "mm");
		xml.writeAttribute("fo:page-height", QString::number(pageHeight) + "mm");

		if (pageWidth > pageHeight)
			xml.writeAttribute("style:print-orientation", "landscape");
		else
			xml.writeAttribute("style:print-orientation", "portrait");

		xml.writeAttribute("fo:margin-top", QString::number(margins.top()) + "mm");
		xml.writeAttribute("fo:margin-bottom", QString::number(margins.bottom()) + "mm");
		xml.writeAttribute("fo:margin-left", QString::number(margins.left()) + "mm");
		xml.writeAttribute("fo:margin-right", QString::number(margins.right()) + "mm");
	}
	else
	{
		QSizeF pageSize = mPrinter->pageLayout().pageSize().size(QPageSize::Inch);
		QMarginsF margins = mPrinter->pageLayout().margins(QPageLayout::Inch);
		qreal pageWidth = 0, pageHeight = 0;

		if (mPrinter->orientation() == QPrinter::Landscape)
		{
			pageWidth = qMax(pageSize.width(), pageSize.height());
			pageHeight = qMin(pageSize.width(), pageSize.height());
		}

		xml.writeAttribute("fo:page-width", QString::number(pageWidth) + "in");
		xml.writeAttribute("fo:page-height", QString::number(pageHeight) + "in");

		if (pageWidth > pageHeight)
			xml.writeAttribute("style:print-orientation", "landscape");
		else
			xml.writeAttribute("style:print-orientation", "portrait");

		xml.writeAttribute("fo:margin-top", QString::number(margins.top()) + "in");
		xml.writeAttribute("fo:margin-bottom", QString::number(margins.bottom()) + "in");
		xml.writeAttribute("fo:margin-left", QString::number(margins.left()) + "in");
		xml.writeAttribute("fo:margin-right", QString::number(margins.right()) + "in");
	}

	xml.writeEndElement();
	xml.writeEndElement();
	xml.writeStartElement("style:style");
	xml.writeAttribute("style:name", "DefaultPageStyle");
	xml.writeAttribute("style:family", "drawing-page");

	// Page style
	xml.writeStartElement("style:drawing-page-properties");
	xml.writeAttribute("draw:background-size", "full");

	QColor backgroundColor = mDiagram->backgroundBrush().color();
	if (backgroundColor.alpha() > 0)
	{
		xml.writeAttribute("draw:fill", "solid");
		xml.writeAttribute("draw:fill-color", colorToHexString(backgroundColor));
		xml.writeAttribute("draw:opacity", QString::number(backgroundColor.alphaF() * 100, 'f', 1) + "%");
	}
	else xml.writeAttribute("draw:fill", "none");

	xml.writeEndElement();
	xml.writeEndElement();
	xml.writeEndElement();

	xml.writeStartElement("office:master-styles");
	xml.writeStartElement("style:master-page");
	xml.writeAttribute("style:name", "DefaultPage");
	xml.writeAttribute("style:page-layout-name", "DefaultPageLayout");
	xml.writeAttribute("draw:style-name", "DefaultPageStyle");
	xml.writeEndElement();
	xml.writeEndElement();

	xml.writeEndElement();
	xml.writeEndDocument();
}

//==================================================================================================

void OdgWriter::writeMeta()
{
	QXmlStreamWriter xml(&mMetaStr);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(2);

	xml.writeStartDocument();
	xml.writeStartElement("office:document-meta");
	xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");

	xml.writeStartElement("office:meta");
	xml.writeEndElement();

	xml.writeEndElement();
	xml.writeEndDocument();
}

void OdgWriter::writeSettings()
{
	QXmlStreamWriter xml(&mSettingsStr);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(2);

	xml.writeStartDocument();
	xml.writeStartElement("office:document-settings");
	xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");

	xml.writeStartElement("office:settings");
	xml.writeEndElement();

	xml.writeEndElement();
	xml.writeEndDocument();
}

void OdgWriter::writeManifest()
{
	QXmlStreamWriter xml(&mManifestStr);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(2);

	xml.writeStartDocument();
	xml.writeStartElement("manifest:manifest");
	xml.writeAttribute("xmlns:manifest", "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0");
	xml.writeAttribute("manifest:version", "1.2");

	xml.writeStartElement("manifest:file-entry");
	xml.writeAttribute("manifest:full-path", "/");
	xml.writeAttribute("manifest:version", "1.2");
	xml.writeAttribute("manifest:media-type", "application/vnd.oasis.opendocument.graphics");
	xml.writeEndElement();

	xml.writeStartElement("manifest:file-entry");
	xml.writeAttribute("manifest:full-path", "content.xml");
	xml.writeAttribute("manifest:media-type", "text/xml");
	xml.writeEndElement();

	xml.writeStartElement("manifest:file-entry");
	xml.writeAttribute("manifest:full-path", "meta.xml");
	xml.writeAttribute("manifest:media-type", "text/xml");
	xml.writeEndElement();

	xml.writeStartElement("manifest:file-entry");
	xml.writeAttribute("manifest:full-path", "settings.xml");
	xml.writeAttribute("manifest:media-type", "text/xml");
	xml.writeEndElement();

	xml.writeStartElement("manifest:file-entry");
	xml.writeAttribute("manifest:full-path", "styles.xml");
	xml.writeAttribute("manifest:media-type", "text/xml");
	xml.writeEndElement();

	xml.writeEndElement();
	xml.writeEndDocument();
}

//==================================================================================================

void OdgWriter::finishOdg()
{
	QuaZip odgFile(mFilePath);

	if (odgFile.open(QuaZip::mdCreate))
	{
		createFileInZip(&odgFile, "mimetype", "application/vnd.oasis.opendocument.graphics");
		createFileInZip(&odgFile, "META-INF/manifest.xml", mManifestStr);
		createFileInZip(&odgFile, "content.xml", mContentStr);
		createFileInZip(&odgFile, "meta.xml", mMetaStr);
		createFileInZip(&odgFile, "settings.xml", mSettingsStr);
		createFileInZip(&odgFile, "styles.xml", mStylesStr);

		odgFile.close();
	}
	else mErrorMessage = "Error creating file: " + mFilePath;
}

void OdgWriter::createFileInZip(QuaZip* zip, const QString& path, const QString& content)
{
	QuaZipFile outputFile(zip);

	// mimetype
	if (outputFile.open(QIODevice::WriteOnly, QuaZipNewInfo(path)))
	{
		QTextStream outputStream(&outputFile);
		outputStream << content;
		outputStream.flush();
		outputFile.close();
	}
	else mErrorMessage = "Error creating " + path + " in file: " + zip->getZipName();
}

//==================================================================================================

void OdgWriter::writeItemStyles(QXmlStreamWriter& xml, const QList<DrawingItem*>& items)
{
	int index = 0;
	QString itemId;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		itemId = QString::number(index+1);
		if (itemId.size() < 5) itemId = "item" + QString(5 - itemId.size(), '0') + itemId;
		else itemId = "item" + itemId;

		mItemIds[*itemIter] = itemId;
		mItemStyleIds[(*itemIter)->style()] = itemId + "_style";

		writeItemStyle(xml, (*itemIter)->style());

		index++;
	}
}

void OdgWriter::writeItems(QXmlStreamWriter& xml, const QList<DrawingItem*>& items)
{
	int index = 0;
	QString itemId;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		DrawingLineItem* lineItem = dynamic_cast<DrawingLineItem*>(*itemIter);
		DrawingArcItem* arcItem = dynamic_cast<DrawingArcItem*>(*itemIter);
		DrawingPolylineItem* polylineItem = dynamic_cast<DrawingPolylineItem*>(*itemIter);
		DrawingCurveItem* curveItem = dynamic_cast<DrawingCurveItem*>(*itemIter);
		DrawingRectItem* rectItem = dynamic_cast<DrawingRectItem*>(*itemIter);
		DrawingEllipseItem* ellipseItem = dynamic_cast<DrawingEllipseItem*>(*itemIter);
		DrawingPolygonItem* polygonItem = dynamic_cast<DrawingPolygonItem*>(*itemIter);
		DrawingTextItem* textItem = dynamic_cast<DrawingTextItem*>(*itemIter);
		DrawingTextRectItem* textRectItem = dynamic_cast<DrawingTextRectItem*>(*itemIter);
		DrawingTextEllipseItem* textEllipseItem = dynamic_cast<DrawingTextEllipseItem*>(*itemIter);
		DrawingTextPolygonItem* textPolygonItem = dynamic_cast<DrawingTextPolygonItem*>(*itemIter);
		DrawingPathItem* pathItem = dynamic_cast<DrawingPathItem*>(*itemIter);
		DrawingItemGroup* groupItem = dynamic_cast<DrawingItemGroup*>(*itemIter);

		if (lineItem) writeLineItem(xml, lineItem);
		else if (arcItem) writeArcItem(xml, arcItem);
		else if (polylineItem) writePolylineItem(xml, polylineItem);
		else if (curveItem) writeCurveItem(xml, curveItem);
		else if (rectItem) writeRectItem(xml, rectItem);
		else if (ellipseItem) writeEllipseItem(xml, ellipseItem);
		else if (polygonItem) writePolygonItem(xml, polygonItem);
		else if (textItem) writeTextItem(xml, textItem);
		else if (textRectItem) writeTextRectItem(xml, textRectItem);
		else if (textEllipseItem) writeTextEllipseItem(xml, textEllipseItem);
		else if (textPolygonItem) writeTextPolygonItem(xml, textPolygonItem);
		else if (pathItem) writePathItem(xml, pathItem);
		else if (groupItem) writeItemGroup(xml, groupItem);

		index++;
	}
}

//==================================================================================================

void OdgWriter::writeLineItem(QXmlStreamWriter& xml, DrawingLineItem* item)
{
	xml.writeStartElement("draw:line");

	xml.writeAttribute("draw:transform", transformToString(mDiagramTransform.map(item->pos()), item->rotation(), item->isFlipped()));

	QLineF line = item->line();
	xml.writeAttribute("svg:x1", QString::number(line.x1() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y1", QString::number(line.y1() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:x2", QString::number(line.x2() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y2", QString::number(line.y2() * mDiagramScale) + mDiagramUnits);

	if (mItemStyleIds.contains(item->style()))
		xml.writeAttribute("draw:style-name", mItemStyleIds[item->style()]);

	xml.writeEndElement();
}

void OdgWriter::writeArcItem(QXmlStreamWriter& xml, DrawingArcItem* item)
{
	xml.writeStartElement("draw:path");

	xml.writeAttribute("draw:transform", transformToString(mDiagramTransform.map(item->pos()), item->rotation(), item->isFlipped()));

	QLineF line = item->arc();
	QRectF rect = QRectF(line.p1() * mDiagramScale, line.p2() * mDiagramScale).normalized();

	xml.writeAttribute("svg:x", QString::number(rect.left()) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top()) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width()) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height()) + mDiagramUnits);

	xml.writeAttribute("svg:viewBox", QString::number(rect.left()) + " " + QString::number(rect.top()) + " " +
		QString::number(rect.width()) + " " + QString::number(rect.height()));

	xml.writeAttribute("svg:d", "M " + QString::number(line.x1() * mDiagramScale) + " " + QString::number(line.y1() * mDiagramScale) + " " +
		"A " + QString::number(rect.width()) + " " + QString::number(rect.height()) + " 0 0 0 " +
		QString::number(line.x2() * mDiagramScale) + " " + QString::number(line.y2() * mDiagramScale));

	if (mItemStyleIds.contains(item->style()))
		xml.writeAttribute("draw:style-name", mItemStyleIds[item->style()]);

	xml.writeEndElement();
}

void OdgWriter::writePolylineItem(QXmlStreamWriter& xml, DrawingPolylineItem* item)
{
	xml.writeStartElement("draw:polyline");

	xml.writeAttribute("draw:transform", transformToString(mDiagramTransform.map(item->pos()), item->rotation(), item->isFlipped()));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->pos() * mDiagramScale);
	QRectF rect = polygon.boundingRect();

	xml.writeAttribute("svg:x", QString::number(rect.left()) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top()) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width()) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height()) + mDiagramUnits);

	xml.writeAttribute("svg:viewBox", QString::number(rect.left()) + " " + QString::number(rect.top()) + " " +
		QString::number(rect.width()) + " " + QString::number(rect.height()));

	if (!polygon.isEmpty()) xml.writeAttribute("draw:points", pointsToString(polygon));

	if (mItemStyleIds.contains(item->style()))
		xml.writeAttribute("draw:style-name", mItemStyleIds[item->style()]);

	xml.writeEndElement();
}

void OdgWriter::writeCurveItem(QXmlStreamWriter& xml, DrawingCurveItem* item)
{
	xml.writeStartElement("draw:path");

	xml.writeAttribute("draw:transform", transformToString(mDiagramTransform.map(item->pos()), item->rotation(), item->isFlipped()));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->pos() * mDiagramScale);
	QRectF rect = polygon.boundingRect();

	xml.writeAttribute("svg:x", QString::number(rect.left()) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top()) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width()) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height()) + mDiagramUnits);

	xml.writeAttribute("svg:viewBox", QString::number(rect.left()) + " " + QString::number(rect.top()) + " " +
		QString::number(rect.width()) + " " + QString::number(rect.height()));

	xml.writeAttribute("svg:d", "M " + QString::number(item->curveStartPos().x() * mDiagramScale) + " " + QString::number(item->curveStartPos().y() * mDiagramScale) + " " +
		"C " + QString::number(item->curveStartControlPos().x() * mDiagramScale) + " " + QString::number(item->curveStartControlPos().y() * mDiagramScale) + " " +
		QString::number(item->curveEndControlPos().x() * mDiagramScale) + " " + QString::number(item->curveEndControlPos().y() * mDiagramScale) + " " +
		QString::number(item->curveEndPos().x() * mDiagramScale) + " " + QString::number(item->curveEndPos().y() * mDiagramScale));

	if (mItemStyleIds.contains(item->style()))
		xml.writeAttribute("draw:style-name", mItemStyleIds[item->style()]);

	xml.writeEndElement();
}

void OdgWriter::writeRectItem(QXmlStreamWriter& xml, DrawingRectItem* item)
{
	xml.writeStartElement("draw:rect");

	xml.writeAttribute("draw:transform", transformToString(mDiagramTransform.map(item->pos()), item->rotation(), item->isFlipped()));

	QRectF rect = item->rect();
	xml.writeAttribute("svg:x", QString::number(rect.left() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height() * mDiagramScale) + mDiagramUnits);

	if (item->cornerRadiusX() != 0)	xml.writeAttribute("draw:corner-radius", QString::number(item->cornerRadiusX() * mDiagramScale) + mDiagramUnits);

	if (mItemStyleIds.contains(item->style()))
		xml.writeAttribute("draw:style-name", mItemStyleIds[item->style()]);

	xml.writeEndElement();
}

void OdgWriter::writeEllipseItem(QXmlStreamWriter& xml, DrawingEllipseItem* item)
{
	xml.writeStartElement("draw:ellipse");

	xml.writeAttribute("draw:transform", transformToString(mDiagramTransform.map(item->pos()), item->rotation(), item->isFlipped()));

	QRectF rect = item->ellipse();
	xml.writeAttribute("svg:x", QString::number(rect.left() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height() * mDiagramScale) + mDiagramUnits);

	if (mItemStyleIds.contains(item->style()))
		xml.writeAttribute("draw:style-name", mItemStyleIds[item->style()]);

	xml.writeEndElement();
}

void OdgWriter::writePolygonItem(QXmlStreamWriter& xml, DrawingPolygonItem* item)
{
	xml.writeStartElement("draw:polygon");

	xml.writeAttribute("draw:transform", transformToString(mDiagramTransform.map(item->pos()), item->rotation(), item->isFlipped()));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->pos() * mDiagramScale);
	QRectF rect = polygon.boundingRect();

	xml.writeAttribute("svg:x", QString::number(rect.left()) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top()) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width()) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height()) + mDiagramUnits);

	xml.writeAttribute("svg:viewBox", QString::number(rect.left()) + " " + QString::number(rect.top()) + " " +
		QString::number(rect.width()) + " " + QString::number(rect.height()));

	if (!polygon.isEmpty()) xml.writeAttribute("draw:points", pointsToString(polygon));

	if (mItemStyleIds.contains(item->style()))
		xml.writeAttribute("draw:style-name", mItemStyleIds[item->style()]);

	xml.writeEndElement();
}

void OdgWriter::writeTextItem(QXmlStreamWriter& xml, DrawingTextItem* item)
{
	/*writeStartElement("text");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	writeItemStyle(item->style());

	writeCharacters(item->caption());

	writeEndElement();*/
}

void OdgWriter::writeTextRectItem(QXmlStreamWriter& xml, DrawingTextRectItem* item)
{
	xml.writeStartElement("draw:rect");

	xml.writeAttribute("draw:transform", transformToString(mDiagramTransform.map(item->pos()), item->rotation(), item->isFlipped()));

	QRectF rect = item->rect();
	xml.writeAttribute("svg:x", QString::number(rect.left() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height() * mDiagramScale) + mDiagramUnits);

	if (item->cornerRadiusX() != 0)	xml.writeAttribute("draw:corner-radius", QString::number(item->cornerRadiusX() * mDiagramScale) + mDiagramUnits);

	if (mItemStyleIds.contains(item->style()))
		xml.writeAttribute("draw:style-name", mItemStyleIds[item->style()]);

	xml.writeStartElement("text:p");
	xml.writeAttribute("text:style-name", mItemStyleIds[item->style()] + "_paragraph");
	xml.writeCharacters(item->caption());
	xml.writeEndElement();

	xml.writeEndElement();
}

void OdgWriter::writeTextEllipseItem(QXmlStreamWriter& xml, DrawingTextEllipseItem* item)
{
	/*writeStartElement("text-ellipse");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	QRectF rect = item->ellipse();
	writeAttribute("left", QString::number(rect.left()));
	writeAttribute("top", QString::number(rect.top()));
	writeAttribute("width", QString::number(rect.width()));
	writeAttribute("height", QString::number(rect.height()));

	writeItemStyle(item->style());

	writeCharacters(item->caption());

	writeEndElement();*/
}

void OdgWriter::writeTextPolygonItem(QXmlStreamWriter& xml, DrawingTextPolygonItem* item)
{
	/*writeStartElement("text-polygon");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->pos());
	if (!polygon.isEmpty()) writeAttribute("points", pointsToString(polygon));

	writeItemStyle(item->style());

	writeCharacters(item->caption());

	writeEndElement();*/
}

void OdgWriter::writePathItem(QXmlStreamWriter& xml, DrawingPathItem* item)
{
	/*writeStartElement("path");

	writeAttribute("name", item->name());

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	QRectF rect = item->rect();
	writeAttribute("left", QString::number(rect.left()));
	writeAttribute("top", QString::number(rect.top()));
	writeAttribute("width", QString::number(rect.width()));
	writeAttribute("height", QString::number(rect.height()));

	writeItemStyle(item->style());

	QRectF pathRect = item->pathRect();
	writeAttribute("view-left", QString::number(pathRect.left()));
	writeAttribute("view-top", QString::number(pathRect.top()));
	writeAttribute("view-width", QString::number(pathRect.width()));
	writeAttribute("view-height", QString::number(pathRect.height()));

	writeAttribute("d", pathToString(item->path()));

	QString glueStr = pointsToString(item->connectionPoints());
	if (!glueStr.isEmpty()) writeAttribute("glue-points", glueStr);

	writeEndElement();*/
}

void OdgWriter::writeItemGroup(QXmlStreamWriter& xml, DrawingItemGroup* item)
{
	/*writeStartElement("group");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	writeItemElements(item->items());

	writeEndElement();*/
}

//==================================================================================================

void OdgWriter::writeItemStyle(QXmlStreamWriter& xml, DrawingItemStyle* style)
{
	if (style && mItemStyleIds.keys().contains(style))
	{
		xml.writeStartElement("style:style");
		xml.writeAttribute("style:name", mItemStyleIds[style]);
		xml.writeAttribute("style:family", "graphic");

		xml.writeStartElement("style:graphic-properties");

		// Pen
		if (style->hasValue(DrawingItemStyle::PenStyle))
		{
			Qt::PenStyle penStyle = (Qt::PenStyle)style->value(DrawingItemStyle::PenStyle).toUInt();
			xml.writeAttribute("draw:stroke", penStyleToString(penStyle));
			if (penStyle == Qt::DashLine || penStyle == Qt::DotLine || penStyle == Qt::DashDotLine || penStyle == Qt::DashDotDotLine)
				xml.writeAttribute("draw:stroke-dash", penDashStyleToString(penStyle));
		}

		if (style->hasValue(DrawingItemStyle::PenWidth))
			xml.writeAttribute("svg:stroke-width", QString::number(style->value(DrawingItemStyle::PenWidth).toReal() * mDiagramScale) + mDiagramUnits);

		if (style->hasValue(DrawingItemStyle::PenColor))
			xml.writeAttribute("svg:stroke-color", colorToHexString(style->value(DrawingItemStyle::PenColor).value<QColor>()));

		if (style->hasValue(DrawingItemStyle::PenOpacity))
			xml.writeAttribute("svg:stroke-opacity", QString::number((int)(style->value(DrawingItemStyle::PenOpacity).toReal() * 100 + 0.5)) + "%");

		if (style->hasValue(DrawingItemStyle::PenCapStyle))
			xml.writeAttribute("svg:stroke-linecap", penCapStyleToString((Qt::PenCapStyle)style->value(DrawingItemStyle::PenCapStyle).toUInt()));

		if (style->hasValue(DrawingItemStyle::PenJoinStyle))
			xml.writeAttribute("draw:stroke-linejoin", penJoinStyleToString((Qt::PenJoinStyle)style->value(DrawingItemStyle::PenJoinStyle).toUInt()));

		// Brush
		xml.writeAttribute("draw:fill", "solid");

		if (style->hasValue(DrawingItemStyle::BrushColor))
			xml.writeAttribute("draw:fill-color", colorToHexString(style->value(DrawingItemStyle::BrushColor).value<QColor>()));

		if (style->hasValue(DrawingItemStyle::BrushOpacity))
			xml.writeAttribute("draw:opacity", QString::number((int)(style->value(DrawingItemStyle::BrushOpacity).toReal() * 100 + 0.5)) + "%");


		// Arrow
		/*if (style->hasValue(DrawingItemStyle::StartArrowStyle))
		{
			DrawingItemStyle::ArrowStyle arrow = (DrawingItemStyle::ArrowStyle)style->value(DrawingItemStyle::StartArrowStyle).toUInt();
			xml.writeAttribute("draw:marker-start", );
			xml.writeAttribute("draw:marker-start-center", );
		}

		if (style->hasValue(DrawingItemStyle::StartArrowSize))
			xml.writeAttribute("draw:marker-start-width", QString::number(style->value(DrawingItemStyle::StartArrowSize).toReal() * mDiagramScale) + mDiagramUnits);

		if (style->hasValue(DrawingItemStyle::EndArrowStyle))
		{
			DrawingItemStyle::ArrowStyle arrow = (DrawingItemStyle::ArrowStyle)style->value(DrawingItemStyle::EndArrowStyle).toUInt();
			xml.writeAttribute("draw:marker-end", );
			xml.writeAttribute("draw:marker-end-center", );
		}

		if (style->hasValue(DrawingItemStyle::EndArrowSize))
			xml.writeAttribute("draw:marker-end-width", QString::number(style->value(DrawingItemStyle::EndArrowSize).toReal() * mDiagramScale) + mDiagramUnits);*/

		xml.writeEndElement();

		xml.writeStartElement("style:paragraph-properties");
		xml.writeEndElement();

		xml.writeStartElement("style:text-properties");

		// Font
		if (style->hasValue(DrawingItemStyle::FontName))
			xml.writeAttribute("style:font-name", style->value(DrawingItemStyle::FontName).toString());

		if (style->hasValue(DrawingItemStyle::FontSize))
			xml.writeAttribute("fo:font-size", QString::number(style->value(DrawingItemStyle::FontSize).toReal() * mDiagramScale * 96) + "pt");

		xml.writeEndElement();

		xml.writeEndElement();


		xml.writeStartElement("style:style");
		xml.writeAttribute("style:name", mItemStyleIds[style] + "_paragraph");
		xml.writeAttribute("style:family", "paragraph");
		xml.writeStartElement("style:paragraph-properties");
		xml.writeEndElement();
		xml.writeStartElement("style:text-properties");

		// Font
		if (style->hasValue(DrawingItemStyle::FontName))
			xml.writeAttribute("style:font-name", style->value(DrawingItemStyle::FontName).toString());

		if (style->hasValue(DrawingItemStyle::FontSize))
			xml.writeAttribute("fo:font-size", QString::number(style->value(DrawingItemStyle::FontSize).toReal() * mDiagramScale * 96) + "pt");

		xml.writeEndElement();
		xml.writeEndElement();
	}

	/*
	// Font
	if (style->hasValue(DrawingItemStyle::FontName))
		writeAttribute("font-name", style->value(DrawingItemStyle::FontName).toString());

	if (style->hasValue(DrawingItemStyle::FontSize))
		writeAttribute("font-size", QString::number(style->value(DrawingItemStyle::FontSize).toReal()));

	if (style->hasValue(DrawingItemStyle::FontBold))
	{
		bool fontStyle = style->value(DrawingItemStyle::FontBold).toBool();
		if (fontStyle) writeAttribute("font-bold", "true");
	}

	if (style->hasValue(DrawingItemStyle::FontItalic))
	{
		bool fontStyle = style->value(DrawingItemStyle::FontItalic).toBool();
		if (fontStyle) writeAttribute("font-italic", "true");
	}

	if (style->hasValue(DrawingItemStyle::FontUnderline))
	{
		bool fontStyle = style->value(DrawingItemStyle::FontUnderline).toBool();
		if (fontStyle) writeAttribute("font-underline", "true");
	}

	if (style->hasValue(DrawingItemStyle::FontStrikeThrough))
	{
		bool fontStyle = style->value(DrawingItemStyle::FontStrikeThrough).toBool();
		if (fontStyle) writeAttribute("font-strike-through", "true");
	}

	if (style->hasValue(DrawingItemStyle::TextHorizontalAlignment))
	{
		Qt::Alignment textAlign = ((Qt::Alignment)style->value(DrawingItemStyle::TextHorizontalAlignment).toUInt() & Qt::AlignHorizontal_Mask);
		if (textAlign != Qt::AlignHCenter) writeAttribute("text-alignment-horizontal", alignmentToString(textAlign));
	}

	if (style->hasValue(DrawingItemStyle::TextVerticalAlignment))
	{
		Qt::Alignment textAlign = ((Qt::Alignment)style->value(DrawingItemStyle::TextVerticalAlignment).toUInt() & Qt::AlignVertical_Mask);
		if (textAlign != Qt::AlignVCenter) writeAttribute("text-alignment-vertical", alignmentToString(textAlign));
	}

	if (style->hasValue(DrawingItemStyle::TextColor))
		writeAttribute("text-color", colorToString(style->value(DrawingItemStyle::TextColor).value<QColor>()));

	if (style->hasValue(DrawingItemStyle::TextOpacity))
	{
		qreal opacity = style->value(DrawingItemStyle::TextOpacity).toReal();
		if (opacity != 1.0) writeAttribute("text-opacity", QString::number(opacity));
	}*/
}

//==================================================================================================

QString OdgWriter::arrowToPathString(DrawingItemStyle::ArrowStyle arrowStyle, qreal arrowSize, qreal penWidth) const
{
	QString pathString;

	switch (arrowStyle)
	{
	case DrawingItemStyle::ArrowNormal:
		break;
	case DrawingItemStyle::ArrowReverse:
		break;
	case DrawingItemStyle::ArrowTriangle:
		break;
	case DrawingItemStyle::ArrowTriangleFilled:
		break;
	case DrawingItemStyle::ArrowConcave:
		break;
	case DrawingItemStyle::ArrowConcaveFilled:
		break;
	case DrawingItemStyle::ArrowCircle:
		break;
	case DrawingItemStyle::ArrowCircleFilled:
		break;
	case DrawingItemStyle::ArrowDiamond:
		break;
	case DrawingItemStyle::ArrowDiamondFilled:
		break;
	case DrawingItemStyle::ArrowHarpoon:
		break;
	case DrawingItemStyle::ArrowHarpoonMirrored:
		break;
	case DrawingItemStyle::ArrowX:
		break;
	default:
		break;
	}

	return pathString;
}

QString OdgWriter::colorToHexString(const QColor& color) const
{
	QString str = "#";

	if (color.red() < 16) str += "0";
	str += QString::number(color.red(), 16).toUpper();
	if (color.green() < 16) str += "0";
	str += QString::number(color.green(), 16).toUpper();
	if (color.blue() < 16) str += "0";
	str += QString::number(color.blue(), 16).toUpper();

	return str;
}

QString OdgWriter::penStyleToString(Qt::PenStyle style) const
{
	QString str;

	switch (style)
	{
	case Qt::NoPen: str = "none"; break;
	case Qt::DashLine:
	case Qt::DotLine:
	case Qt::DashDotLine:
	case Qt::DashDotDotLine: str = "dash"; break;
	default: str = "solid"; break;
	}

	return str;
}

QString OdgWriter::penDashStyleToString(Qt::PenStyle style) const
{
	QString str;

	switch (style)
	{
	case Qt::DashLine: str = "dashed"; break;
	case Qt::DotLine: str = "dotted"; break;
	case Qt::DashDotLine: str = "dash-dotted"; break;
	case Qt::DashDotDotLine: str = "dash-dot-dotted"; break;
	}

	return str;
}

QString OdgWriter::penCapStyleToString(Qt::PenCapStyle style) const
{
	QString str;

	switch (style)
	{
	case Qt::FlatCap: str = "butt"; break;
	case Qt::SquareCap: str = "square"; break;
	default: str = "round"; break;
	}

	return str;
}

QString OdgWriter::penJoinStyleToString(Qt::PenJoinStyle style) const
{
	QString str;

	switch (style)
	{
	case Qt::SvgMiterJoin:
	case Qt::MiterJoin: str = "miter"; break;
	case Qt::BevelJoin: str = "bevel"; break;
	default: str = "round"; break;
	}

	return str;
}

QString OdgWriter::pointsToString(const QPolygonF& points) const
{
	QString pointsStr;

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		pointsStr += QString::number((*pointIter).x()) + "," + QString::number((*pointIter).y()) + " ";

	return pointsStr.trimmed();
}

QString OdgWriter::transformToString(const QPointF& pos, qreal rotation, bool flipped) const
{
	QString str = "translate(" + QString::number(pos.x()) + mDiagramUnits + "," + QString::number(pos.y()) + mDiagramUnits + ")";
	str += (rotation != 0) ? " rotate(" + QString::number(rotation) + ")" : "";
	str += (flipped) ? " scale(-1.0,1.0)" : "";
	return str;
}
