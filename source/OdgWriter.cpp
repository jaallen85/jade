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
#include <QtPrintSupport>
#include <quazip.h>
#include <quazipfile.h>

OdgWriter::OdgWriter() 
{
	mDiagram = nullptr;
	mPrinter = nullptr;
	mDiagramScale = 1.0;
}

OdgWriter::~OdgWriter() { }

//==================================================================================================

bool OdgWriter::write(DiagramWidget* diagram, QPrinter* printer, const QString& filePath)
{
	mFilePath = filePath;
	mPrinter = printer;
	mDiagram = diagram;

	mErrorMessage.clear();

	analyzeDiagram();
	analyzeItemStyles();
	
	writeOdg(writeContent(), writeStyles(), writeMeta(), writeSettings(), writeManifest());

	return mErrorMessage.isEmpty();
}

QString OdgWriter::errorMessage() const
{
	return mErrorMessage;
}

//==================================================================================================

void OdgWriter::analyzeDiagram()
{
	// Transform (diagram to ODG scene)
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
	
void OdgWriter::analyzeItemStyles()
{
	// Item styles
	mItemStyles.clear();
	findItemStyles(mDiagram->items(), mItemStyles);
	
	mDashStyles.clear();
	mFontDecls.clear();
	clearArrowStyles();
	for(auto styleIter = mItemStyles.begin(); styleIter != mItemStyles.end(); styleIter++)
	{
		// Dash styles
		if ((*styleIter)->hasValue(DrawingItemStyle::PenStyle))
		{
			Qt::PenStyle penStyle = (Qt::PenStyle)(*styleIter)->value(DrawingItemStyle::PenStyle).toUInt();
			if (penStyle != Qt::NoPen && penStyle != Qt::SolidLine &&
				!mDashStyles.contains(penStyle)) mDashStyles.append(penStyle);
		}
		
		// Font declarations
		if ((*styleIter)->hasValue(DrawingItemStyle::FontName))
		{
			QString fontName = (*styleIter)->value(DrawingItemStyle::FontName).toString();
			if (!mFontDecls.contains(fontName)) mFontDecls.append(fontName);
		}
		
		// Arrow styles
		if ((*styleIter)->hasValue(DrawingItemStyle::StartArrowStyle) && 
			(*styleIter)->hasValue(DrawingItemStyle::StartArrowSize) && 
			(*styleIter)->hasValue(DrawingItemStyle::PenWidth))
		{
			DrawingItemStyle::ArrowStyle arrowStyle = (DrawingItemStyle::ArrowStyle)
				(*styleIter)->value(DrawingItemStyle::StartArrowStyle).toUInt();
			qreal arrowSize = (*styleIter)->value(DrawingItemStyle::StartArrowSize).toReal();
			qreal penWidth = (*styleIter)->value(DrawingItemStyle::PenWidth).toReal();
			QString styleName = arrowStyleName(arrowStyle, arrowSize, penWidth);
			
			if (arrowStyle != DrawingItemStyle::ArrowNone && !containsArrowStyle(arrowStyle, arrowSize, penWidth)) 
				addArrowStyle(arrowStyle, arrowSize, penWidth);
		}
		
		if ((*styleIter)->hasValue(DrawingItemStyle::EndArrowStyle) && 
			(*styleIter)->hasValue(DrawingItemStyle::EndArrowSize) && 
			(*styleIter)->hasValue(DrawingItemStyle::PenWidth))
		{
			DrawingItemStyle::ArrowStyle arrowStyle = (DrawingItemStyle::ArrowStyle)
				(*styleIter)->value(DrawingItemStyle::EndArrowStyle).toUInt();
			qreal arrowSize = (*styleIter)->value(DrawingItemStyle::EndArrowSize).toReal();
			qreal penWidth = (*styleIter)->value(DrawingItemStyle::PenWidth).toReal();
			QString styleName = arrowStyleName(arrowStyle, arrowSize, penWidth);
			
			if (arrowStyle != DrawingItemStyle::ArrowNone && !containsArrowStyle(arrowStyle, arrowSize, penWidth)) 
				addArrowStyle(arrowStyle, arrowSize, penWidth);
		}
	}
}

void OdgWriter::findItemStyles(const QList<DrawingItem*>& items, QList<DrawingItemStyle*>& itemStyles)
{
	DrawingItemStyle* style;
	DrawingItemGroup* groupItem;
	
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		style = (*itemIter)->style();
		if (!itemStyles.contains(style)) itemStyles.append(style);
		
		groupItem = dynamic_cast<DrawingItemGroup*>(*itemIter);
		if (groupItem) findItemStyles(groupItem->items(), itemStyles);
	}
}

//==================================================================================================

QString OdgWriter::writeContent()
{
	QString contentStr;
	
	QXmlStreamWriter xml(&contentStr);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(2);

	xml.writeStartDocument();
	xml.writeStartElement("office:document-content");
	xml.writeAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
	xml.writeAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
	xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
	xml.writeAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
	xml.writeAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
	xml.writeAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");

	xml.writeStartElement("office:scripts");
	xml.writeEndElement();

	xml.writeStartElement("office:font-face-decls");
	writeFontDeclarations(xml);
	xml.writeEndElement();

	xml.writeStartElement("office:automatic-styles");
	xml.writeStartElement("style:style");
	xml.writeAttribute("style:name", "Page1");
	xml.writeAttribute("style:family", "drawing-page");
	xml.writeEndElement();
	writeItemStyles(xml);
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
	
	return contentStr;
}

QString OdgWriter::writeStyles()
{
	QString stylesStr;
	
	QXmlStreamWriter xml(&stylesStr);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(2);

	xml.writeStartDocument();
	xml.writeStartElement("office:document-styles");
	xml.writeAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
	xml.writeAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
	xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
	xml.writeAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
	xml.writeAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");

	xml.writeStartElement("office:font-face-decls");
	xml.writeEndElement();

	xml.writeStartElement("office:styles");
	writeArrowStyles(xml);
	writeDashStyles(xml);
	xml.writeEndElement();

	xml.writeStartElement("office:automatic-styles");
	writeDefaultPageStyle(xml);
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
	
	return stylesStr;
}

QString OdgWriter::writeMeta()
{
	QString metaStr;
	
	QXmlStreamWriter xml(&metaStr);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(2);

	xml.writeStartDocument();
	xml.writeStartElement("office:document-meta");
	xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");

	xml.writeStartElement("office:meta");
	xml.writeEndElement();

	xml.writeEndElement();
	xml.writeEndDocument();
	
	return metaStr;
}

QString OdgWriter::writeSettings()
{
	QString settingsStr;
	
	QXmlStreamWriter xml(&settingsStr);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(2);

	xml.writeStartDocument();
	xml.writeStartElement("office:document-settings");
	xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");

	xml.writeStartElement("office:settings");
	xml.writeEndElement();

	xml.writeEndElement();
	xml.writeEndDocument();
	
	return settingsStr;
}

QString OdgWriter::writeManifest()
{
	QString manifestStr;
	
	QXmlStreamWriter xml(&manifestStr);
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
	
	return manifestStr;
}

void OdgWriter::writeOdg(const QString& contentStr, const QString& stylesStr, const QString& metaStr, 
	const QString& settingsStr, const QString& manifestStr)
{
	QuaZip odgFile(mFilePath);

	if (odgFile.open(QuaZip::mdCreate))
	{
		createFileInZip(&odgFile, "mimetype", "application/vnd.oasis.opendocument.graphics");
		createFileInZip(&odgFile, "META-INF/manifest.xml", manifestStr);
		createFileInZip(&odgFile, "content.xml", contentStr);
		createFileInZip(&odgFile, "meta.xml", metaStr);
		createFileInZip(&odgFile, "settings.xml", settingsStr);
		createFileInZip(&odgFile, "styles.xml", stylesStr);

		odgFile.close();
	}
	else mErrorMessage = "Error creating file: " + mFilePath;
}

void OdgWriter::createFileInZip(QuaZip* zip, const QString& path, const QString& content)
{
	QuaZipFile outputFile(zip);

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

void OdgWriter::writeDefaultPageStyle(QXmlStreamWriter& xml)
{
	// Page layout
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

	// Page style
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
}

void OdgWriter::writeFontDeclarations(QXmlStreamWriter& xml)
{
	
}

void OdgWriter::writeArrowStyles(QXmlStreamWriter& xml)
{
	
}

void OdgWriter::writeDashStyles(QXmlStreamWriter& xml)
{
	// todo: only add styles actually used by document

	/*xml.writeStartElement("draw:stroke-dash");
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
	xml.writeEndElement();*/
}

//==================================================================================================

void OdgWriter::writeItemStyles(QXmlStreamWriter& xml)
{
	for(auto styleIter = mItemStyles.begin(); styleIter != mItemStyles.end(); styleIter++)
		writeItemStyle(xml, *styleIter);
}

void OdgWriter::writeItemStyle(QXmlStreamWriter& xml, DrawingItemStyle* style)
{
	// Graphic style
	xml.writeStartElement("style:style");
	xml.writeAttribute("style:name", itemStyleName(style));
	xml.writeAttribute("style:family", "graphic");

	xml.writeStartElement("style:graphic-properties");

	// Pen
	if (style->hasValue(DrawingItemStyle::PenStyle))
	{
		Qt::PenStyle penStyle = (Qt::PenStyle)style->value(DrawingItemStyle::PenStyle).toUInt();
		xml.writeAttribute("draw:stroke", penStyleToString(penStyle));
		if (penStyle == Qt::DashLine || penStyle == Qt::DotLine || penStyle == Qt::DashDotLine || penStyle == Qt::DashDotDotLine)
			xml.writeAttribute("draw:stroke-dash", dashStyleName(penStyle));
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
	if (style->hasValue(DrawingItemStyle::StartArrowStyle) && style->hasValue(DrawingItemStyle::StartArrowSize) &&
		style->hasValue(DrawingItemStyle::PenWidth))
	{
		DrawingItemStyle::ArrowStyle arrowStyle = (DrawingItemStyle::ArrowStyle)style->value(DrawingItemStyle::StartArrowStyle).toUInt();
		qreal arrowSize = style->value(DrawingItemStyle::EndArrowSize).toReal() * mDiagramScale;
		qreal penWidth = style->value(DrawingItemStyle::PenWidth).toReal() * mDiagramScale;

		xml.writeAttribute("draw:marker-start", arrowStyleName(arrowStyle, arrowSize, penWidth));
		xml.writeAttribute("draw:marker-start-center", arrowStyleCentered(arrowStyle) ? "true" : "false");
		xml.writeAttribute("draw:marker-start-width", QString::number(arrowSize) + mDiagramUnits);
	}

	if (style->hasValue(DrawingItemStyle::EndArrowStyle) && style->hasValue(DrawingItemStyle::EndArrowSize) &&
		style->hasValue(DrawingItemStyle::PenWidth))
	{
		DrawingItemStyle::ArrowStyle arrowStyle = (DrawingItemStyle::ArrowStyle)style->value(DrawingItemStyle::EndArrowStyle).toUInt();
		qreal arrowSize = style->value(DrawingItemStyle::EndArrowSize).toReal() * mDiagramScale;
		qreal penWidth = style->value(DrawingItemStyle::PenWidth).toReal() * mDiagramScale;

		xml.writeAttribute("draw:marker-end", arrowStyleName(arrowStyle, arrowSize, penWidth));
		xml.writeAttribute("draw:marker-end-center", arrowStyleCentered(arrowStyle) ? "true" : "false");
		xml.writeAttribute("draw:marker-end-width", QString::number(arrowSize) + mDiagramUnits);
	}

	xml.writeEndElement();

	writeItemParagraphStyle(xml, style);

	xml.writeEndElement();

	// Paragraph style
	xml.writeStartElement("style:style");
	xml.writeAttribute("style:name", itemStyleName(style) + "_paragraph");
	xml.writeAttribute("style:family", "paragraph");
	writeItemParagraphStyle(xml, style);
	xml.writeEndElement();
}

void OdgWriter::writeItemParagraphStyle(QXmlStreamWriter& xml, DrawingItemStyle* style)
{
	xml.writeStartElement("style:paragraph-properties");
	xml.writeEndElement();

	xml.writeStartElement("style:text-properties");

	if (style->hasValue(DrawingItemStyle::FontName))
		xml.writeAttribute("style:font-name", fontStyleName(style->value(DrawingItemStyle::FontName).toString()));

	if (style->hasValue(DrawingItemStyle::FontSize))
		xml.writeAttribute("fo:font-size", QString::number(style->value(DrawingItemStyle::FontSize).toReal() * mDiagramScale * 96) + "pt");

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

	xml.writeEndElement();
}

QString OdgWriter::itemStyleName(DrawingItemStyle* style)
{
	QString name = mItemStyles.indexOf(style) + 1;
	if (name.size() < 4) name = "style" + QString(4 - name.size(), '0') + name;
	return name;
}

//==================================================================================================

void OdgWriter::writeItems(QXmlStreamWriter& xml, const QList<DrawingItem*>& items)
{
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
	}
}

void OdgWriter::writeLineItem(QXmlStreamWriter& xml, DrawingLineItem* item)
{
	xml.writeStartElement("draw:line");

	xml.writeAttribute("draw:transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	QLineF line = item->line();
	xml.writeAttribute("svg:x1", QString::number(line.x1() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y1", QString::number(line.y1() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:x2", QString::number(line.x2() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y2", QString::number(line.y2() * mDiagramScale) + mDiagramUnits);

	xml.writeAttribute("draw:style-name", itemStyleName(item->style()));

	xml.writeEndElement();
}

void OdgWriter::writeArcItem(QXmlStreamWriter& xml, DrawingArcItem* item)
{
	xml.writeStartElement("draw:path");

	xml.writeAttribute("draw:transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

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

	xml.writeAttribute("draw:style-name", itemStyleName(item->style()));

	xml.writeEndElement();
}

void OdgWriter::writePolylineItem(QXmlStreamWriter& xml, DrawingPolylineItem* item)
{
	xml.writeStartElement("draw:polyline");

	xml.writeAttribute("draw:transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

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

	xml.writeAttribute("draw:style-name", itemStyleName(item->style()));

	xml.writeEndElement();
}

void OdgWriter::writeCurveItem(QXmlStreamWriter& xml, DrawingCurveItem* item)
{
	xml.writeStartElement("draw:path");

	xml.writeAttribute("draw:transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

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

	xml.writeAttribute("draw:style-name", itemStyleName(item->style()));

	xml.writeEndElement();
}

void OdgWriter::writeRectItem(QXmlStreamWriter& xml, DrawingRectItem* item)
{
	xml.writeStartElement("draw:rect");

	xml.writeAttribute("draw:transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	QRectF rect = item->rect();
	xml.writeAttribute("svg:x", QString::number(rect.left() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height() * mDiagramScale) + mDiagramUnits);

	if (item->cornerRadiusX() != 0)	
	{
		xml.writeAttribute("draw:corner-radius", 
			QString::number(item->cornerRadiusX() * mDiagramScale) + mDiagramUnits);
	}

	xml.writeAttribute("draw:style-name", itemStyleName(item->style()));

	xml.writeEndElement();
}

void OdgWriter::writeEllipseItem(QXmlStreamWriter& xml, DrawingEllipseItem* item)
{
	xml.writeStartElement("draw:ellipse");

	xml.writeAttribute("draw:transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	QRectF rect = item->ellipse();
	xml.writeAttribute("svg:x", QString::number(rect.left() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height() * mDiagramScale) + mDiagramUnits);

	xml.writeAttribute("draw:style-name", itemStyleName(item->style()));

	xml.writeEndElement();
}

void OdgWriter::writePolygonItem(QXmlStreamWriter& xml, DrawingPolygonItem* item)
{
	xml.writeStartElement("draw:polygon");

	xml.writeAttribute("draw:transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

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

	xml.writeAttribute("draw:style-name", itemStyleName(item->style()));

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

	xml.writeAttribute("draw:transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	QRectF rect = item->rect();
	xml.writeAttribute("svg:x", QString::number(rect.left() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height() * mDiagramScale) + mDiagramUnits);

	if (item->cornerRadiusX() != 0)	
	{
		xml.writeAttribute("draw:corner-radius", 
			QString::number(item->cornerRadiusX() * mDiagramScale) + mDiagramUnits);
	}

	xml.writeAttribute("draw:style-name", itemStyleName(item->style()));

	xml.writeStartElement("text:p");
	xml.writeAttribute("text:style-name", itemStyleName(item->style()) + "_paragraph");
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

QString OdgWriter::fontStyleName(const QString& fontName) const
{
	return fontName;
}

QString OdgWriter::dashStyleName(Qt::PenStyle penStyle) const
{
	QString str;

	switch (penStyle)
	{
	case Qt::DashLine: str = "Dashed"; break;
	case Qt::DotLine: str = "Dotted"; break;
	case Qt::DashDotLine: str = "DashDotted"; break;
	case Qt::DashDotDotLine: str = "DashDotDotted"; break;
	}

	return str;
}

//==================================================================================================

void OdgWriter::addArrowStyle(DrawingItemStyle::ArrowStyle arrowStyle, qreal arrowSize, qreal penWidth)
{
	ArrowStyle newStyle;
	newStyle.arrowStyle = arrowStyle;
	newStyle.arrowSize = arrowSize;
	newStyle.penWidth = penWidth;
	mArrowStyles.append(newStyle);
}

void OdgWriter::clearArrowStyles()
{
	mArrowStyles.clear();
}

bool OdgWriter::containsArrowStyle(DrawingItemStyle::ArrowStyle arrowStyle, qreal arrowSize, qreal penWidth) const
{
	bool found = false;
	
	for(auto styleIter = mArrowStyles.begin(); !found && styleIter != mArrowStyles.end(); styleIter++)
	{
		found = (styleIter->arrowStyle == arrowStyle && styleIter->arrowSize == arrowSize && 
			styleIter->penWidth == penWidth);
	}
	
	return found;
}

QString OdgWriter::arrowStyleName(DrawingItemStyle::ArrowStyle arrowStyle, qreal arrowSize, qreal penWidth) const
{
	QString name;
	
	switch (arrowStyle)
	{
	case DrawingItemStyle::ArrowNormal: name = "Normal"; break;
	case DrawingItemStyle::ArrowReverse: name = "Reverse"; break;
	case DrawingItemStyle::ArrowTriangle: name = "Triangle"; break;
	case DrawingItemStyle::ArrowTriangleFilled: name = "TriangleFilled"; break;
	case DrawingItemStyle::ArrowConcave: name = "Concave"; break;
	case DrawingItemStyle::ArrowConcaveFilled: name = "ConcaveFilled"; break;
	case DrawingItemStyle::ArrowCircle: name = "Circle"; break;
	case DrawingItemStyle::ArrowCircleFilled: name = "CircleFilled"; break;
	case DrawingItemStyle::ArrowDiamond: name = "Diamond"; break;
	case DrawingItemStyle::ArrowDiamondFilled: name = "DiamondFilled"; break;
	case DrawingItemStyle::ArrowHarpoon: name = "Harpoon"; break;
	case DrawingItemStyle::ArrowHarpoonMirrored: name = "HarpoonMirrored"; break;
	case DrawingItemStyle::ArrowX: name = "X"; break;
	default: break;
	}
	
	if (!name.isEmpty())
		name = name + "_" + QString::number(arrowSize) + "_" + QString::number(penWidth);
	
	return name;
}

/*QString OdgWriter::arrowStylePath(DrawingItemStyle::ArrowStyle arrowStyle, qreal arrowSize, 
	qreal penWidth, QRectF& viewBox) const
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
}*/

bool OdgWriter::arrowStyleCentered(DrawingItemStyle::ArrowStyle arrowStyle) const
{
	return (arrowStyle == DrawingItemStyle::ArrowCircle ||
		arrowStyle == DrawingItemStyle::ArrowCircleFilled ||
		arrowStyle == DrawingItemStyle::ArrowDiamond ||
		arrowStyle == DrawingItemStyle::ArrowDiamondFilled ||
		arrowStyle == DrawingItemStyle::ArrowX);
}

//==================================================================================================

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
	QPointF mappedPos = mDiagramTransform.map(pos);
	
	QString str = "translate(" + QString::number(mappedPos.x()) + mDiagramUnits + "," + 
		QString::number(mappedPos.y()) + mDiagramUnits + ")";
	str += (rotation != 0) ? " rotate(" + QString::number(rotation) + ")" : "";
	str += (flipped) ? " scale(-1.0,1.0)" : "";
	return str;
}
