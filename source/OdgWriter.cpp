/* OdgWriter.cpp
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
	if (mPrinter->pageLayout().pageSize().definitionUnits() == QPageSize::Millimeter ||
		mPrinter->pageLayout().pageSize().definitionUnits() == QPageSize::Cicero) mDiagramUnits = "mm";
	else mDiagramUnits = "in";

	QPrinter::Unit printerUnits = (mDiagramUnits == "mm") ? QPrinter::Millimeter : QPrinter::Inch;
	QPageLayout::Unit pageLayoutUnits = (mDiagramUnits == "mm") ? QPageLayout::Millimeter : QPageLayout::Inch;

	mVisibleRect = mDiagram->scene()->sceneRect();

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
	mItemStyles.clear();
	findItemStyles(mDiagram->scene()->items(), mItemStyles);
	
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
	writeItems(xml, mDiagram->scene()->items());
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

	QColor backgroundColor = mDiagram->scene()->backgroundBrush().color();
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
	for(auto fontIter = mFontDecls.begin(); fontIter != mFontDecls.end(); fontIter++)
	{
		QFont font(*fontIter);
		QFontInfo fontInfo(font);
		QString fontGeneric;

		switch (fontInfo.styleHint())
		{
		case QFont::SansSerif:
			fontGeneric = "swiss";
			break;
		case QFont::Serif:
			fontGeneric = "roman";
			break;
		case QFont::TypeWriter:
		case QFont::Monospace:
			fontGeneric = "script";
			break;
		case QFont::Decorative:
			fontGeneric = "decorative";
			break;
		case QFont::System:
			fontGeneric = "system";
			break;
		default:
			fontGeneric = "";
			break;
		}

		xml.writeStartElement("style:font-face");
		xml.writeAttribute("style:name", fontStyleName(*fontIter));
		xml.writeAttribute("svg:font-family", *fontIter);
		if (!fontGeneric.isEmpty()) xml.writeAttribute("style:font-family-generic", fontGeneric);
		xml.writeAttribute("style:font-pitch", fontInfo.fixedPitch() ? "fixed" : "variable");
		xml.writeEndElement();
	}
}

void OdgWriter::writeArrowStyles(QXmlStreamWriter& xml)
{
	for(auto arrowIter = mArrowStyles.begin(); arrowIter != mArrowStyles.end(); arrowIter++)
	{
		QRectF viewBox;
		QString path = arrowStylePath(arrowIter->arrowStyle, arrowIter->arrowSize, arrowIter->penWidth, viewBox);

		if (!path.isEmpty() && viewBox.isValid())
		{
			xml.writeStartElement("draw:marker");
			xml.writeAttribute("draw:name", arrowStyleName(arrowIter->arrowStyle, arrowIter->arrowSize, arrowIter->penWidth));
			xml.writeAttribute("svg:viewBox", QString::number(viewBox.left()) + " " + QString::number(viewBox.top()) + " " +
				QString::number(viewBox.width()) + " " + QString::number(viewBox.height()));
			xml.writeAttribute("svg:d", path);
			xml.writeEndElement();
		}
	}
}

void OdgWriter::writeDashStyles(QXmlStreamWriter& xml)
{
	for(auto dashIter = mDashStyles.begin(); dashIter != mDashStyles.end(); dashIter++)
	{
		switch (*dashIter)
		{
		case Qt::DashLine:
			xml.writeStartElement("draw:stroke-dash");
			xml.writeAttribute("draw:name", dashStyleName(*dashIter));
			xml.writeAttribute("draw:style", "round");
			xml.writeAttribute("draw:dots1", "1");
			xml.writeAttribute("draw:dots1-length", "300%");
			xml.writeAttribute("draw:dots2", "0");
			xml.writeAttribute("draw:dots2-length", "0%");
			xml.writeAttribute("draw:distance", "200%");
			xml.writeEndElement();
			break;
		case Qt::DotLine:
			xml.writeStartElement("draw:stroke-dash");
			xml.writeAttribute("draw:name", dashStyleName(*dashIter));
			xml.writeAttribute("draw:style", "round");
			xml.writeAttribute("draw:dots1", "1");
			xml.writeAttribute("draw:dots1-length", "100%");
			xml.writeAttribute("draw:dots2", "0");
			xml.writeAttribute("draw:dots2-length", "0%");
			xml.writeAttribute("draw:distance", "200%");
			xml.writeEndElement();
			break;
		case Qt::DashDotLine:
			xml.writeStartElement("draw:stroke-dash");
			xml.writeAttribute("draw:name", dashStyleName(*dashIter));
			xml.writeAttribute("draw:style", "round");
			xml.writeAttribute("draw:dots1", "1");
			xml.writeAttribute("draw:dots1-length", "300%");
			xml.writeAttribute("draw:dots2", "1");
			xml.writeAttribute("draw:dots2-length", "100%");
			xml.writeAttribute("draw:distance", "200%");
			xml.writeEndElement();
			break;
		case Qt::DashDotDotLine:
			xml.writeStartElement("draw:stroke-dash");
			xml.writeAttribute("draw:name", dashStyleName(*dashIter));
			xml.writeAttribute("draw:style", "round");
			xml.writeAttribute("draw:dots1", "1");
			xml.writeAttribute("draw:dots1-length", "300%");
			xml.writeAttribute("draw:dots2", "2");
			xml.writeAttribute("draw:dots2-length", "100%");
			xml.writeAttribute("draw:distance", "200%");
			xml.writeEndElement();
			break;
		default:
			break;
		}
	}
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
	else if (style->hasValue(DrawingItemStyle::FontName))
		xml.writeAttribute("draw:stroke", "none");

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
	if (style->hasValue(DrawingItemStyle::BrushColor))
	{
		xml.writeAttribute("draw:fill", "solid");
		xml.writeAttribute("draw:fill-color", colorToHexString(style->value(DrawingItemStyle::BrushColor).value<QColor>()));
	}
	else if (style->hasValue(DrawingItemStyle::FontName))
		xml.writeAttribute("draw:fill", "none");

	if (style->hasValue(DrawingItemStyle::BrushOpacity))
		xml.writeAttribute("draw:opacity", QString::number((int)(style->value(DrawingItemStyle::BrushOpacity).toReal() * 100 + 0.5)) + "%");


	// Arrow
	if (style->hasValue(DrawingItemStyle::StartArrowStyle) && style->hasValue(DrawingItemStyle::StartArrowSize) &&
		style->hasValue(DrawingItemStyle::PenWidth))
	{
		DrawingItemStyle::ArrowStyle arrowStyle = (DrawingItemStyle::ArrowStyle)style->value(DrawingItemStyle::StartArrowStyle).toUInt();
		qreal arrowSize = style->value(DrawingItemStyle::StartArrowSize).toReal();
		qreal penWidth = style->value(DrawingItemStyle::PenWidth).toReal();

		xml.writeAttribute("draw:marker-start", arrowStyleName(arrowStyle, arrowSize, penWidth));
		xml.writeAttribute("draw:marker-start-center", arrowStyleCentered(arrowStyle) ? "0.5" : "0.0");
		xml.writeAttribute("draw:marker-start-width", QString::number((arrowSize + penWidth) * mDiagramScale) + mDiagramUnits);
	}

	if (style->hasValue(DrawingItemStyle::EndArrowStyle) && style->hasValue(DrawingItemStyle::EndArrowSize) &&
		style->hasValue(DrawingItemStyle::PenWidth))
	{
		DrawingItemStyle::ArrowStyle arrowStyle = (DrawingItemStyle::ArrowStyle)style->value(DrawingItemStyle::EndArrowStyle).toUInt();
		qreal arrowSize = style->value(DrawingItemStyle::EndArrowSize).toReal();
		qreal penWidth = style->value(DrawingItemStyle::PenWidth).toReal();

		xml.writeAttribute("draw:marker-end", arrowStyleName(arrowStyle, arrowSize, penWidth));
		xml.writeAttribute("draw:marker-end-center", arrowStyleCentered(arrowStyle) ? "0.5" : "0.0");
		xml.writeAttribute("draw:marker-end-width", QString::number((arrowSize + penWidth) * mDiagramScale) + mDiagramUnits);
	}

	// Text Alignment
	if (style->hasValue(DrawingItemStyle::TextVerticalAlignment))
		xml.writeAttribute("draw:textarea-vertical-align", alignmentToString((Qt::Alignment)style->value(DrawingItemStyle::TextVerticalAlignment).toUInt()));
	else if (style->hasValue(DrawingItemStyle::FontName))
		xml.writeAttribute("draw:textarea-vertical-align", "middle");

	if (style->hasValue(DrawingItemStyle::FontName))
	{
		xml.writeAttribute("fo:padding-left", "0" + mDiagramUnits);
		xml.writeAttribute("fo:padding-top", "0" + mDiagramUnits);
		xml.writeAttribute("fo:padding-right", "0" + mDiagramUnits);
		xml.writeAttribute("fo:padding-bottom", "0" + mDiagramUnits);
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

	if (style->hasValue(DrawingItemStyle::TextHorizontalAlignment))
		xml.writeAttribute("fo:text-align", alignmentToString((Qt::Alignment)style->value(DrawingItemStyle::TextHorizontalAlignment).toUInt()));
	else if (style->hasValue(DrawingItemStyle::FontName))
		xml.writeAttribute("fo:text-align", "center");

	//if (style->hasValue(DrawingItemStyle::TextVerticalAlignment))
	//	xml.writeAttribute("style:vertical-align", alignmentToString((Qt::Alignment)style->value(DrawingItemStyle::TextVerticalAlignment).toUInt()));
	//else if (style->hasValue(DrawingItemStyle::FontName))
	//	xml.writeAttribute("style:vertical-align", "middle");

	xml.writeEndElement();

	xml.writeStartElement("style:text-properties");

	if (style->hasValue(DrawingItemStyle::FontName))
		xml.writeAttribute("style:font-name", fontStyleName(style->value(DrawingItemStyle::FontName).toString()));

	if (style->hasValue(DrawingItemStyle::FontSize))
		xml.writeAttribute("fo:font-size", QString::number(style->value(DrawingItemStyle::FontSize).toReal() * mDiagramScale * 96) + "pt");

	if (style->hasValue(DrawingItemStyle::FontBold))
		xml.writeAttribute("fo:font-weight", style->value(DrawingItemStyle::FontBold).toBool() ? "bold" : "normal");

	if (style->hasValue(DrawingItemStyle::FontItalic))
		xml.writeAttribute("fo:font-style", style->value(DrawingItemStyle::FontItalic).toBool() ? "italic" : "normal");

	if (style->hasValue(DrawingItemStyle::FontUnderline))
		xml.writeAttribute("style:text-underline-style", style->value(DrawingItemStyle::FontUnderline).toBool() ? "solid" : "none");

	if (style->hasValue(DrawingItemStyle::FontStrikeThrough))
		xml.writeAttribute("style:text-line-through-style", style->value(DrawingItemStyle::FontStrikeThrough).toBool() ? "solid" : "none");

	if (style->hasValue(DrawingItemStyle::TextColor))
		xml.writeAttribute("fo:color", colorToHexString(style->value(DrawingItemStyle::TextColor).value<QColor>()));

	xml.writeEndElement();
}

QString OdgWriter::itemStyleName(DrawingItemStyle* style)
{
	QString name = QString::number(mItemStyles.indexOf(style) + 1);
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

	xml.writeAttribute("draw:transform", transformToString(item));

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

	xml.writeAttribute("draw:transform", transformToString(item));

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

	xml.writeAttribute("draw:transform", transformToString(item));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->position() * mDiagramScale);
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

	xml.writeAttribute("draw:transform", transformToString(item));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->position() * mDiagramScale);
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

	xml.writeAttribute("draw:transform", transformToString(item));

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

	xml.writeAttribute("draw:transform", transformToString(item));

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

	xml.writeAttribute("draw:transform", transformToString(item));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->position() * mDiagramScale);
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
	xml.writeStartElement("draw:rect");

	xml.writeAttribute("draw:transform", transformToString(item));

	QRectF rect = item->boundingRect().normalized();
	xml.writeAttribute("svg:x", QString::number(rect.left() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height() * mDiagramScale) + mDiagramUnits);

	xml.writeAttribute("draw:style-name", itemStyleName(item->style()));

	xml.writeStartElement("text:p");
	xml.writeAttribute("text:style-name", itemStyleName(item->style()) + "_paragraph");
	xml.writeCharacters(item->caption());
	xml.writeEndElement();

	xml.writeEndElement();
}

void OdgWriter::writeTextRectItem(QXmlStreamWriter& xml, DrawingTextRectItem* item)
{
	xml.writeStartElement("draw:rect");

	xml.writeAttribute("draw:transform", transformToString(item));

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
	xml.writeStartElement("draw:ellipse");

	xml.writeAttribute("draw:transform", transformToString(item));

	QRectF rect = item->ellipse();
	xml.writeAttribute("svg:x", QString::number(rect.left() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height() * mDiagramScale) + mDiagramUnits);

	xml.writeAttribute("draw:style-name", itemStyleName(item->style()));

	xml.writeStartElement("text:p");
	xml.writeAttribute("text:style-name", itemStyleName(item->style()) + "_paragraph");
	xml.writeCharacters(item->caption());
	xml.writeEndElement();

	xml.writeEndElement();
}

void OdgWriter::writeTextPolygonItem(QXmlStreamWriter& xml, DrawingTextPolygonItem* item)
{
	xml.writeStartElement("draw:polygon");

	xml.writeAttribute("draw:transform", transformToString(item));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->position() * mDiagramScale);
	QRectF rect = polygon.boundingRect();

	xml.writeAttribute("svg:x", QString::number(rect.left()) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top()) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width()) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height()) + mDiagramUnits);

	xml.writeAttribute("svg:viewBox", QString::number(rect.left()) + " " + QString::number(rect.top()) + " " +
		QString::number(rect.width()) + " " + QString::number(rect.height()));

	if (!polygon.isEmpty()) xml.writeAttribute("draw:points", pointsToString(polygon));

	xml.writeAttribute("draw:style-name", itemStyleName(item->style()));

	xml.writeStartElement("text:p");
	xml.writeAttribute("text:style-name", itemStyleName(item->style()) + "_paragraph");
	xml.writeCharacters(item->caption());
	xml.writeEndElement();

	xml.writeEndElement();
}

void OdgWriter::writePathItem(QXmlStreamWriter& xml, DrawingPathItem* item)
{
	xml.writeStartElement("draw:path");

	xml.writeAttribute("draw:transform", transformToString(item));

	QRectF rect = item->rect();
	xml.writeAttribute("svg:x", QString::number(rect.left() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:y", QString::number(rect.top() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:width", QString::number(rect.width() * mDiagramScale) + mDiagramUnits);
	xml.writeAttribute("svg:height", QString::number(rect.height() * mDiagramScale) + mDiagramUnits);

	xml.writeAttribute("draw:style-name", itemStyleName(item->style()));

	QRectF pathRect = item->pathRect();
	xml.writeAttribute("svg:viewBox", QString::number(pathRect.left()) + " " + QString::number(pathRect.top()) + " " +
		QString::number(pathRect.width()) + " " + QString::number(pathRect.height()));

	xml.writeAttribute("svg:d", pathToString(item->path()));

	xml.writeEndElement();
}

void OdgWriter::writeItemGroup(QXmlStreamWriter& xml, DrawingItemGroup* item)
{
	// Limited support for group items because <draw:g> element does not support transforms

	QList<DrawingItem*> items = item->items();

	xml.writeStartElement("draw:g");

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->setPosition((*itemIter)->position() + item->position());

	writeItems(xml, items);

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->setPosition((*itemIter)->position() - item->position());

	xml.writeEndElement();
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
	default: break;
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
	case DrawingItemStyle::ArrowNormal:
	case DrawingItemStyle::ArrowTriangle:
	case DrawingItemStyle::ArrowTriangleFilled:
	case DrawingItemStyle::ArrowConcave:
	case DrawingItemStyle::ArrowConcaveFilled:
	case DrawingItemStyle::ArrowHarpoon:
	case DrawingItemStyle::ArrowHarpoonMirrored:
		name = "TriangleFilled"; break;
	case DrawingItemStyle::ArrowReverse:
		name = "Reverse"; break;
	case DrawingItemStyle::ArrowCircle:
	case DrawingItemStyle::ArrowCircleFilled:
		name = "CircleFilled"; break;
	case DrawingItemStyle::ArrowDiamond:
	case DrawingItemStyle::ArrowDiamondFilled:
		name = "DiamondFilled"; break;
	default: break;
	}
	
	if (!name.isEmpty())
		name = name + "_" + QString::number(arrowSize) + "_" + QString::number(penWidth);
	
	return name;
}

QString OdgWriter::arrowStylePath(DrawingItemStyle::ArrowStyle arrowStyle, qreal arrowSize,
	qreal penWidth, QRectF& viewBox) const
{
	//const qreal sqrt2 = qSqrt(2);

	QString pathString;
	QPainterPath path;
	QRectF innerBox;

	switch (arrowStyle)
	{
	case DrawingItemStyle::ArrowNormal:
	case DrawingItemStyle::ArrowTriangle:
	case DrawingItemStyle::ArrowTriangleFilled:
	case DrawingItemStyle::ArrowConcave:
	case DrawingItemStyle::ArrowConcaveFilled:
	case DrawingItemStyle::ArrowHarpoon:
	case DrawingItemStyle::ArrowHarpoonMirrored:
		viewBox = QRectF(-arrowSize / 2 - penWidth / 2, -penWidth / 2, arrowSize + penWidth, arrowSize + penWidth);
		/*path.moveTo(-penWidth * sqrt2 / 2, -penWidth * sqrt2 / 2);
		path.lineTo(-arrowSize / 2 - penWidth * sqrt2 / 2, arrowSize - penWidth * sqrt2 / 2);
		path.lineTo(-arrowSize / 2, arrowSize + penWidth / 2);
		path.lineTo(arrowSize / 2, arrowSize + penWidth / 2);
		path.lineTo(arrowSize / 2 + penWidth * sqrt2 / 2, arrowSize - penWidth * sqrt2 / 2);
		path.lineTo(penWidth * sqrt2 / 2, -penWidth * sqrt2 / 2);*/
		path.moveTo(0, -penWidth / 2);
		path.lineTo(-arrowSize / 2 - penWidth / 2, arrowSize + penWidth / 2);
		path.lineTo(arrowSize / 2 + penWidth / 2, arrowSize + penWidth / 2);
		path.closeSubpath();
		pathString = pathToString(path);
		break;
	case DrawingItemStyle::ArrowReverse:
		viewBox = QRectF(-arrowSize / 2 - penWidth / 2, -penWidth / 2, arrowSize + penWidth, arrowSize + penWidth);
		path.moveTo(0, arrowSize + penWidth / 2);
		path.lineTo(-arrowSize / 2 - penWidth / 2, -penWidth / 2);
		path.lineTo(arrowSize / 2 + penWidth / 2, -penWidth / 2);
		path.closeSubpath();
		pathString = pathToString(path);
		break;
	case DrawingItemStyle::ArrowCircle:
	case DrawingItemStyle::ArrowCircleFilled:
		viewBox = QRectF(0, 0, arrowSize + penWidth, arrowSize + penWidth);
		path.moveTo(viewBox.right(), viewBox.center().y());
		path.arcTo(viewBox, 0, 360);
		pathString = pathToString(path);
		break;
	case DrawingItemStyle::ArrowDiamond:
	case DrawingItemStyle::ArrowDiamondFilled:
		viewBox = QRectF(0, 0, arrowSize + penWidth, arrowSize + penWidth);
		path.moveTo(viewBox.center().x(), viewBox.top());
		path.lineTo(viewBox.right(), viewBox.center().y());
		path.lineTo(viewBox.center().x(), viewBox.bottom());
		path.lineTo(viewBox.left(), viewBox.center().y());
		path.closeSubpath();
		pathString = pathToString(path);
		break;
	default:
		break;
	}

	return pathString;
}

bool OdgWriter::arrowStyleCentered(DrawingItemStyle::ArrowStyle arrowStyle) const
{
	return (arrowStyle == DrawingItemStyle::ArrowCircle ||
		arrowStyle == DrawingItemStyle::ArrowCircleFilled ||
		arrowStyle == DrawingItemStyle::ArrowDiamond ||
		arrowStyle == DrawingItemStyle::ArrowDiamondFilled ||
		arrowStyle == DrawingItemStyle::ArrowX);
}

//==================================================================================================

QString OdgWriter::alignmentToString(Qt::Alignment align) const
{
	QString str;

	if (align & Qt::AlignLeft) str = "left";
	else if (align & Qt::AlignRight) str = "right";
	else if (align & Qt::AlignHCenter) str = "center";
	else if (align & Qt::AlignTop) str = "top";
	else if (align & Qt::AlignBottom) str = "bottom";
	else if (align & Qt::AlignVCenter) str = "middle";

	return str;
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

QString OdgWriter::pathToString(const QPainterPath& path) const
{
	QString pathStr;

	for(int i = 0; i < path.elementCount(); i++)
	{
		QPainterPath::Element element = path.elementAt(i);

		switch (element.type)
		{
		case QPainterPath::MoveToElement:
			pathStr += "M " + QString::number(element.x) + " " + QString::number(element.y) + " ";
			break;
		case QPainterPath::LineToElement:
			pathStr += "L " + QString::number(element.x) + " " + QString::number(element.y) + " ";
			break;
		case QPainterPath::CurveToElement:
			pathStr += "C " + QString::number(element.x) + " " + QString::number(element.y) + " ";
			break;
		case QPainterPath::CurveToDataElement:
			pathStr += QString::number(element.x) + " " + QString::number(element.y) + " ";
			break;
		}
	}

	return pathStr.trimmed();
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

QString OdgWriter::transformToString(DrawingItem* item) const
{
	QPointF mappedPos = mDiagramTransform.map(item->position());
	QTransform transform = item->transform();

	qreal rotation = qAsin(transform.m12());
	transform.rotate(-rotation * 180 / 3.141592654);

	qreal hScale = transform.m11();
	qreal vScale = transform.m22();

	QString str;
	str += (hScale != 1.0 || vScale != 1.0) ? "scale(" + QString::number(hScale) + "," + QString::number(vScale) + ") " : "";
	str += (rotation != 0) ? "rotate(" + QString::number(rotation) + ") " : "";
	str += "translate(" + QString::number(mappedPos.x()) + mDiagramUnits + "," +
		QString::number(mappedPos.y()) + mDiagramUnits + ")";
	return str;
}
