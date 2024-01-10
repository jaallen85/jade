// File: OdgWriter.cpp
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

#include "OdgWriter.h"
#include "OdgPage.h"
#include "OdgStyle.h"
#include "OdgCurveItem.h"
#include "OdgEllipseItem.h"
#include "OdgGroupItem.h"
#include "OdgLineItem.h"
#include "OdgPathItem.h"
#include "OdgPolygonItem.h"
#include "OdgPolylineItem.h"
#include "OdgRoundedRectItem.h"
#include "OdgTextItem.h"
#include "OdgTextEllipseItem.h"
#include "OdgTextRoundedRectItem.h"
#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QXmlStreamWriter>
#include <quazip.h>
#include <quazipfile.h>

OdgWriter::OdgWriter(const QString& fileName) :
    mUnits(Odg::UnitsInches), mPageSize(8.2, 6.2), mPageMargins(0.1, 0.1, 0.1, 0.1), mBackgroundColor(255, 255, 255),
    mGrid(0.05), mGridStyle(Odg::GridLines), mGridColor(77, 153, 153), mGridSpacingMajor(8), mGridSpacingMinor(2),
    mDefaultStyle(nullptr), mPages(), mFile(fileName), mStyles(), mTextStyleNeeded(), mItemStyles()
{
    // Nothing more to do here.
}

OdgWriter::~OdgWriter()
{
    close();
    qDeleteAll(mStyles);
}

//======================================================================================================================

void OdgWriter::setFileName(const QString& fileName)
{
    mFile.setFileName(fileName);

}

QString OdgWriter::fileName() const
{
    return mFile.fileName();
}

//======================================================================================================================

void OdgWriter::setUnits(Odg::Units units)
{
    mUnits = units;
}

void OdgWriter::setPageSize(const QSizeF& size)
{
    if (size.width() > 0 && size.height() > 0) mPageSize = size;
}

void OdgWriter::setPageMargins(const QMarginsF& margins)
{
    if (margins.left() >= 0 && margins.top() >= 0 && margins.right() >= 0 && margins.bottom() >= 0)
        mPageMargins = margins;
}

void OdgWriter::setBackgroundColor(const QColor& color)
{
    mBackgroundColor = color;
}

//======================================================================================================================

void OdgWriter::setGrid(double grid)
{
    if (grid >= 0) mGrid = grid;
}

void OdgWriter::setGridStyle(Odg::GridStyle style)
{
    mGridStyle = style;
}

void OdgWriter::setGridColor(const QColor& color)
{
    mGridColor = color;
}

void OdgWriter::setGridSpacingMajor(int spacing)
{
    if (spacing >= 0) mGridSpacingMajor = spacing;
}

void OdgWriter::setGridSpacingMinor(int spacing)
{
    if (spacing >= 0) mGridSpacingMinor = spacing;
}

//======================================================================================================================

void OdgWriter::setDefaultStyle(OdgStyle* style)
{
    mDefaultStyle = style;
    if (mDefaultStyle) mDefaultStyle->setName("standard");
}

void OdgWriter::setPages(const QList<OdgPage*>& pages)
{
    mPages = pages;
}

//======================================================================================================================

bool OdgWriter::open()
{
    return mFile.open(QFile::WriteOnly);
}

void OdgWriter::close()
{
    mFile.close();
}

//======================================================================================================================

bool OdgWriter::write()
{
    analyzeDrawingForStyles();

    QuaZip odgArchive(&mFile);
    if (!odgArchive.open(QuaZip::mdCreate)) return false;

    QuaZipFile odgFile(&odgArchive);
    QXmlStreamWriter xml;
    //xml.setAutoFormatting(true);
    //xml.setAutoFormattingIndent(2);

    // Mimetype file
    if (!odgFile.open(QFile::WriteOnly, QuaZipNewInfo("mimetype"))) return false;
    odgFile.write(QByteArray("application/vnd.oasis.opendocument.graphics"));
    odgFile.close();

    // Manifest file
    if (!odgFile.open(QFile::WriteOnly, QuaZipNewInfo("META-INF/manifest.xml"))) return false;
    xml.setDevice(&odgFile);
    writeManifest(xml);
    odgFile.close();

    // Write meta information to meta.xml
    if (!odgFile.open(QFile::WriteOnly, QuaZipNewInfo("meta.xml"))) return false;
    xml.setDevice(&odgFile);
    writeDocumentMeta(xml);
    odgFile.close();

    // Write settings to settings.xml
    if (!odgFile.open(QFile::WriteOnly, QuaZipNewInfo("settings.xml"))) return false;
    xml.setDevice(&odgFile);
    writeDocumentSettings(xml);
    odgFile.close();

    // Write document styles to styles.xml
    if (!odgFile.open(QFile::WriteOnly, QuaZipNewInfo("styles.xml"))) return false;
    xml.setDevice(&odgFile);
    writeDocumentStyles(xml);
    odgFile.close();

    // Write document content to content.xml
    if (!odgFile.open(QFile::WriteOnly, QuaZipNewInfo("content.xml"))) return false;
    xml.setDevice(&odgFile);
    writeDocumentContent(xml);
    odgFile.close();

    // Write thumbnail
    if (!mPages.isEmpty())
    {
        if (!odgFile.open(QFile::WriteOnly, QuaZipNewInfo("Thumbnails/thumbnail.png"))) return false;
        QImage thumbnail = createThumbnail(mPages.first());
        thumbnail.save(&odgFile, "PNG");
        odgFile.close();
    }

    return true;
}

void OdgWriter::writeToClipboard()
{
    QString clipboardText;

    analyzeDrawingForStyles();

    QXmlStreamWriter xml(&clipboardText);
    //xml.setAutoFormatting(true);
    //xml.setAutoFormattingIndent(2);

    xml.writeStartDocument();
    xml.writeStartElement("office:document");

    // Write <office:document> attributes
    xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    xml.writeAttribute("xmlns:config", "urn:oasis:names:tc:opendocument:xmlns:config:1.0");
    xml.writeAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
    xml.writeAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
    xml.writeAttribute("xmlns:loext", "urn:org:documentfoundation:names:experimental:office:xmlns:loext:1.0");
    xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    xml.writeAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
    xml.writeAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
    xml.writeAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
    xml.writeAttribute("xmlns:jade", "https://github.com/jaallen85/jade");
    xml.writeAttribute("office:version", "1.3");

    // Write <office:document> sub-elements
    writeSettings(xml);
    writeContentFontFaces(xml);
    writeStyles(xml);
    writeContentAutomaticStyles(xml);
    writeBody(xml);

    xml.writeEndElement();
    xml.writeEndDocument();

    QApplication::clipboard()->setText(clipboardText);
}

//======================================================================================================================

void OdgWriter::analyzeDrawingForStyles()
{
    qDeleteAll(mStyles);
    mStyles.clear();
    mTextStyleNeeded.clear();
    mItemStyles.clear();

    for(auto& page : qAsConst(mPages))
    {
        const QList<OdgItem*> items = page->items();
        for(auto& item : items) analyzeItemForStyles(item);
    }
}

void OdgWriter::analyzeItemForStyles(OdgItem* item)
{
    mItemStyles.insert(item, findOrCreateStyle(item));

    OdgGroupItem* groupItem = dynamic_cast<OdgGroupItem*>(item);
    if (groupItem)
    {
        const QList<OdgItem*> items = groupItem->items();
        for(auto& item : items) analyzeItemForStyles(item);
    }
}

OdgStyle* OdgWriter::findOrCreateStyle(OdgItem* item)
{
    // Create new style based on this item
    OdgStyle* newStyle = new OdgStyle(mUnits);
    newStyle->setName("style" + QString::number(mStyles.size() + 1));
    newStyle->setParent(mDefaultStyle);

    bool hasPenStyle = false, hasPenWidth = false, hasPenColor = false, hasBrushColor = false;
    const Qt::PenStyle penStyle = static_cast<Qt::PenStyle>(checkIntProperty(item, "penStyle", hasPenStyle));
    const double penWidth = checkDoubleProperty(item, "penWidth", hasPenWidth);
    const QColor penColor = checkProperty<QColor>(item, "penColor", hasPenColor);
    const QColor brushColor = checkProperty<QColor>(item, "brushColor", hasBrushColor);

    if (hasPenStyle) newStyle->setPenStyleIfNeeded(penStyle);
    if (hasPenWidth) newStyle->setPenWidthIfNeeded(penWidth);
    if (hasPenColor) newStyle->setPenColorIfNeeded(penColor);
    if (hasBrushColor) newStyle->setBrushColorIfNeeded(brushColor);

    bool hasStartMarkerStyle = false, hasStartMarkerSize = false, hasEndMarkerStyle = false,
        hasEndMarkerSize = false;
    const Odg::MarkerStyle startMarkerStyle = static_cast<Odg::MarkerStyle>(checkIntProperty(item, "startMarkerStyle",
                                                                                             hasStartMarkerStyle));
    const double startMarkerSize = checkDoubleProperty(item, "startMarkerSize", hasStartMarkerSize);
    const Odg::MarkerStyle endMarkerStyle = static_cast<Odg::MarkerStyle>(checkIntProperty(item, "endMarkerStyle",
                                                                                           hasEndMarkerStyle));
    const double endMarkerSize = checkDoubleProperty(item, "endMarkerSize", hasEndMarkerSize);

    if (hasStartMarkerStyle) newStyle->setStartMarkerStyleIfNeeded(startMarkerStyle);
    if (hasStartMarkerSize) newStyle->setStartMarkerSizeIfNeeded(startMarkerSize);
    if (hasEndMarkerStyle) newStyle->setEndMarkerStyleIfNeeded(endMarkerStyle);
    if (hasEndMarkerSize) newStyle->setEndMarkerSizeIfNeeded(endMarkerSize);

    bool hasFontFamily = false, hasFontSize = false, hasFontStyle = false, hasTextAlignment = false,
        hasTextPadding = false, hasTextColor = false;
    const QString fontFamily = checkStringProperty(item, "fontFamily", hasFontFamily);
    const double fontSize = checkDoubleProperty(item, "fontSize", hasFontSize);
    const OdgFontStyle fontStyle = checkProperty<OdgFontStyle>(item, "fontStyle", hasFontStyle);
    const Qt::Alignment textAlignment = static_cast<Qt::Alignment>(checkIntProperty(item, "textAlignment",
                                                                                    hasTextAlignment));
    const QSizeF textPadding = checkProperty<QSizeF>(item, "textPadding", hasTextPadding);
    const QColor textColor = checkProperty<QColor>(item, "textColor", hasTextColor);

    if (hasFontFamily) newStyle->setFontFamilyIfNeeded(fontFamily);
    if (hasFontSize) newStyle->setFontSizeIfNeeded(fontSize);
    if (hasFontStyle) newStyle->setFontStyleIfNeeded(fontStyle);
    if (hasTextAlignment) newStyle->setTextAlignmentIfNeeded(textAlignment);
    if (hasTextPadding) newStyle->setTextPaddingIfNeeded(textPadding);
    if (hasTextColor) newStyle->setTextColorIfNeeded(textColor);

    OdgTextItem* textItem = dynamic_cast<OdgTextItem*>(item);
    if (textItem)
    {
        newStyle->setPenStyleIfNeeded(Qt::NoPen);
        newStyle->setBrushColorIfNeeded(QColor(mBackgroundColor.red(), mBackgroundColor.green(),
                                               mBackgroundColor.blue(), 0));
    }

    // If new style already matches an existing style, then use that one instead
    OdgStyle* matchingStyle = nullptr;
    for(auto& style : qAsConst(mStyles))
    {
        if (newStyle->isEquivalentTo(style))
        {
            matchingStyle = style;
            break;
        }
    }

    if (matchingStyle)
    {
        delete newStyle;
        return matchingStyle;
    }

    mStyles.append(newStyle);
    mTextStyleNeeded.insert(newStyle, !item->property("caption").isNull());
    return newStyle;
}

int OdgWriter::checkIntProperty(OdgItem* item, const QString& name, bool& hasProperty) const
{
    const QVariant propertyValue = item->property(name);
    hasProperty = (!propertyValue.isNull() && propertyValue.canConvert<int>());
    return (hasProperty) ? propertyValue.toInt() : 0;
}

double OdgWriter::checkDoubleProperty(OdgItem* item, const QString& name, bool& hasProperty) const
{
    const QVariant propertyValue = item->property(name);
    hasProperty = (!propertyValue.isNull() && propertyValue.canConvert<double>());
    return (hasProperty) ? propertyValue.toDouble() : 0;
}

QString OdgWriter::checkStringProperty(OdgItem* item, const QString& name, bool& hasProperty) const
{
    const QVariant propertyValue = item->property(name);
    hasProperty = (!propertyValue.isNull() && propertyValue.canConvert<QString>());
    return (hasProperty) ? propertyValue.toString() : 0;
}

template<class T> T OdgWriter::checkProperty(OdgItem* item, const QString& name, bool& hasProperty) const
{
    const QVariant propertyValue = item->property(name);
    hasProperty = (!propertyValue.isNull() && propertyValue.canConvert<T>());
    return (hasProperty) ? propertyValue.value<T>() : T();
}

//======================================================================================================================

void OdgWriter::writeManifest(QXmlStreamWriter& xml)
{
    xml.writeStartDocument();

    xml.writeStartElement("manifest:manifest");

    // Write <manifest:manifest> attributes
    xml.writeAttribute("xmlns:manifest", "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0");
    xml.writeAttribute("manifest:version", "1.3");
    xml.writeAttribute("xmlns:loext", "urn:org:documentfoundation:names:experimental:office:xmlns:loext:1.0");

    // Write <manifest:manifest> sub-elements
    writeManifestFileEntry(xml, "/", "application/vnd.oasis.opendocument.graphics");
    writeManifestFileEntry(xml, "content.xml", "text/xml");
    writeManifestFileEntry(xml, "meta.xml", "text/xml");
    writeManifestFileEntry(xml, "settings.xml", "text/xml");
    writeManifestFileEntry(xml, "styles.xml", "text/xml");
    if (!mPages.isEmpty()) writeManifestFileEntry(xml, "Thumbnails/thumbnail.png", "image/png");

    xml.writeEndElement();

    xml.writeEndDocument();
}

void OdgWriter::writeManifestFileEntry(QXmlStreamWriter& xml, const QString& fullPath, const QString& mediaType)
{
    xml.writeStartElement("manifest:file-entry");

    // Write <manifest:file-entry> attributes
    xml.writeAttribute("manifest:full-path", fullPath);
    if (fullPath == "/") xml.writeAttribute("manifest:version", "1.3");
    xml.writeAttribute("manifest:media-type", mediaType);

    // No <manifest:file-entry> sub-elements

    xml.writeEndElement();
}

//======================================================================================================================

QImage OdgWriter::createThumbnail(OdgPage* page)
{
    Q_ASSERT(page != nullptr);

    QImage thumbnail(256, 256, QImage::Format_RGB888);
    thumbnail.fill(mBackgroundColor.toRgb());

    const QRectF pageRect(-mPageMargins.left(), -mPageMargins.top(), mPageSize.width(), mPageSize.height());
    const qreal scale = qMin(thumbnail.width() / pageRect.width(), thumbnail.height() / pageRect.height());
    const qreal xOffset = (thumbnail.width() - pageRect.width() * scale) / 2;
    const qreal yOffset = (thumbnail.height() - pageRect.height() * scale) / 2;

    QPainter painter(&thumbnail);
    painter.translate(xOffset, yOffset);
    painter.scale(scale, scale);
    painter.translate(-pageRect.left(), -pageRect.top());

    // Draw items
    const QList<OdgItem*> items = page->items();
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
    for(auto& item : items)
    {
        painter.setTransform(item->transform(), true);
        item->paint(painter);
        painter.setTransform(item->transformInverse(), true);
    }

    painter.end();

    return thumbnail;
}

//======================================================================================================================

void OdgWriter::writeDocumentMeta(QXmlStreamWriter& xml)
{
    xml.writeStartDocument();

    xml.writeStartElement("office:document-meta");

    // Write <office:document-meta> attributes
    xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    xml.writeAttribute("office:version", "1.3");

    // No <office:document-meta> sub-elements

    xml.writeEndElement();
    xml.writeEndDocument();
}

//======================================================================================================================

void OdgWriter::writeDocumentSettings(QXmlStreamWriter& xml)
{
    xml.writeStartDocument();

    xml.writeStartElement("office:document-settings");

    // Write <office:document-settings> attributes
    xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    xml.writeAttribute("xmlns:config", "urn:oasis:names:tc:opendocument:xmlns:config:1.0");
    xml.writeAttribute("office:version", "1.3");

    // Write <office:document-settings> sub-elements
    writeSettings(xml);

    xml.writeEndElement();

    xml.writeEndDocument();
}

void OdgWriter::writeSettings(QXmlStreamWriter& xml)
{
    xml.writeStartElement("office:settings");

    // No <office:settings> attributes

    // Write <office:settings> sub-elements
    writeConfigItemSet(xml);

    xml.writeEndElement();
}

void OdgWriter::writeConfigItemSet(QXmlStreamWriter& xml)
{
    xml.writeStartElement("config:config-item-set");

    // Write <config:config-item-set> attributes
    xml.writeAttribute("config:name", "jade:settings");

    // Write <config:config-item-set> sub-elements
    QString gridStyleStr = "hidden";
    switch (mGridStyle)
    {
    case Odg::GridLines:
        gridStyleStr = "lines";
        break;
    default:    // Odg::GridHidden
        break;
    }

    writeConfigItem(xml, "units", "string", Odg::unitsToString(mUnits));
    writeConfigItem(xml, "grid", "double", QString::number(mGrid, 'g', 8));
    writeConfigItem(xml, "gridStyle", "string", gridStyleStr);
    writeConfigItem(xml, "gridColor", "string", colorToString(mGridColor));
    writeConfigItem(xml, "gridSpacingMajor", "int", QString::number(mGridSpacingMajor));
    writeConfigItem(xml, "gridSpacingMinor", "int", QString::number(mGridSpacingMinor));

    xml.writeEndElement();
}

void OdgWriter::writeConfigItem(QXmlStreamWriter& xml, const QString& name, const QString& type, const QString& value)
{
    xml.writeStartElement("config:config-item");

    // Write <config:config-item> attributes
    xml.writeAttribute("config:name", name);
    xml.writeAttribute("config:type", type);
    xml.writeCharacters(value);

    // No <config:config-item> sub-elements

    xml.writeEndElement();
}

//======================================================================================================================

void OdgWriter::writeDocumentStyles(QXmlStreamWriter& xml)
{
    xml.writeStartDocument();

    xml.writeStartElement("office:document-styles");

    // Write <office:document-styles> attributes
    xml.writeAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
    xml.writeAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
    xml.writeAttribute("xmlns:loext", "urn:org:documentfoundation:names:experimental:office:xmlns:loext:1.0");
    xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    xml.writeAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
    xml.writeAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
    xml.writeAttribute("office:version", "1.3");

    // Write <office:document-styles> sub-elements
    writeStyleFontFaces(xml);
    writeStyles(xml);
    writeAutomaticPageStyles(xml);
    writeMasterPageStyles(xml);

    xml.writeEndElement();
    xml.writeEndDocument();
}

void OdgWriter::writeStyleFontFaces(QXmlStreamWriter& xml)
{
    xml.writeStartElement("office:font-face-decls");

    // No <office:font-face-decls> attributes

    // Write <office:font-face-decls> sub-elements
    if (mDefaultStyle) writeFontFace(xml, mDefaultStyle->lookupFontFamily());

    xml.writeEndElement();
}

void OdgWriter::writeFontFace(QXmlStreamWriter& xml, const QString fontFamily)
{
    xml.writeStartElement("style:font-face");

    // Write <style:font-face> attributes
    xml.writeAttribute("style:name", fontFamily);
    xml.writeAttribute("svg:font-family", fontFamily);

    // No <style:font-face> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writeStyles(QXmlStreamWriter& xml)
{
    xml.writeStartElement("office:styles");

    // No <office:styles> attributes

    // Write <office:styles> sub-elements
    writeDashStyle(xml, "Dash_20__28_Rounded_29_", "Dash (Rounded)", "1", "201%", "", "", "199%");
    writeDashStyle(xml, "Dot_20__28_Rounded_29_", "Dot (Rounded)", "1", "1%", "", "", "199%");
    writeDashStyle(xml, "Dash_20_Dot_20__28_Rounded_29_", "Dash Dot (Rounded)", "1", "201%", "1", "1%", "199%");
    writeDashStyle(xml, "Dash_20_Dot_20_Dot_20__28_Rounded_29_", "Dash Dot Dot (Rounded)", "1", "201%", "2", "1%", "199%");

    writeMarkerStyle(xml, "Triangle", "0 0 1013 1130", "M1009 1050l-449-1008-22-30-29-12-34 12-21 26-449 1012-5 13v8l5 "
                                                       "21 12 21 17 13 21 4h903l21-4 21-13 9-21 4-21v-8z");
    writeMarkerStyle(xml, "Circle", "0 0 1131 1131", "M462 1118l-102-29-102-51-93-72-72-93-51-102-29-102-13-105 13-102 "
                                                     "29-106 51-102 72-89 93-72 102-50 102-34 106-9 101 9 106 34 98 50 "
                                                     "93 72 72 89 51 102 29 106 13 102-13 105-29 102-51 102-72 93-93 "
                                                     "72-98 51-106 29-101 13z");

    writeStyle(xml, mDefaultStyle, false);

    xml.writeEndElement();
}

void OdgWriter::writeDashStyle(QXmlStreamWriter& xml, const QString& name, const QString& displayName,
                               const QString& dots1, const QString& dots1Length, const QString& dots2,
                               const QString& dots2Length, const QString& distance)
{
    xml.writeStartElement("draw:stroke-dash");

    // Write <draw:stroke-dash> attributes
    xml.writeAttribute("draw:name", name);
    xml.writeAttribute("draw:display-name", displayName);
    xml.writeAttribute("draw:style", "round");
    xml.writeAttribute("draw:dots1", dots1);
    xml.writeAttribute("draw:dots1-length", dots1Length);
    if (!dots2.isEmpty()) xml.writeAttribute("draw:dots2", dots2);
    if (!dots2Length.isEmpty()) xml.writeAttribute("draw:dots2-length", dots2Length);
    xml.writeAttribute("draw:distance", distance);

    // No <draw:stroke-dash> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writeMarkerStyle(QXmlStreamWriter& xml, const QString& name, const QString& viewBox,
                                 const QString& path)
{
    xml.writeStartElement("draw:marker");

    // Write <draw:stroke-dash> attributes
    xml.writeAttribute("draw:name", name);
    xml.writeAttribute("svg:viewBox", viewBox);
    xml.writeAttribute("svg:d", path);

    // No <draw:stroke-dash> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writeAutomaticPageStyles(QXmlStreamWriter& xml)
{
    xml.writeStartElement("office:automatic-styles");

    // No <office:automatic-styles> attributes

    // Write <office:automatic-styles> sub-elements
    writePageLayout(xml);
    writePageStyle(xml, "DefaultPageStyle");

    xml.writeEndElement();
}

void OdgWriter::writeMasterPageStyles(QXmlStreamWriter& xml)
{
    xml.writeStartElement("office:master-styles");

    // No <office:master-styles> attributes

    // Write <office:master-styles> sub-elements
    writeMasterPage(xml);

    xml.writeEndElement();
}

void OdgWriter::writePageLayout(QXmlStreamWriter& xml)
{
    xml.writeStartElement("style:page-layout");

    // Write <style:page-layout> attributes
    xml.writeAttribute("style:name", "DefaultPageLayout");

    // Write <style:page-layout> sub-elements
    writePageLayoutProperties(xml);

    xml.writeEndElement();
}

void OdgWriter::writePageLayoutProperties(QXmlStreamWriter& xml)
{
    xml.writeStartElement("style:page-layout-properties");

    // Write <style:page-layout-properties> attributes
    xml.writeAttribute("fo:page-width", lengthToString(mPageSize.width()));
    xml.writeAttribute("fo:page-height", lengthToString(mPageSize.height()));
    xml.writeAttribute("fo:margin-left", lengthToString(mPageMargins.left()));
    xml.writeAttribute("fo:margin-top", lengthToString(mPageMargins.top()));
    xml.writeAttribute("fo:margin-right", lengthToString(mPageMargins.right()));
    xml.writeAttribute("fo:margin-bottom", lengthToString(mPageMargins.bottom()));

    // No <style:page-layout-properties> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writePageStyle(QXmlStreamWriter& xml, const QString& name)
{
    xml.writeStartElement("style:style");

    // Write <style:style> attributes
    xml.writeAttribute("style:name", name);
    xml.writeAttribute("style:family", "drawing-page");

    // No <style:style> sub-elements
    writeDrawingPageProperties(xml);

    xml.writeEndElement();
}

void OdgWriter::writeDrawingPageProperties(QXmlStreamWriter& xml)
{
    xml.writeStartElement("style:drawing-page-properties");

    // Write <style:drawing-page-properties> attributes
    if (mBackgroundColor == QColor(0, 0, 0, 0))
        xml.writeAttribute("draw:fill", "none");
    else
    {
        xml.writeAttribute("draw:fill", "solid");
        xml.writeAttribute("draw:fill-color", colorToString(mBackgroundColor));
        if (mBackgroundColor.alpha() != 255)
            xml.writeAttribute("draw:opacity", percentToString(mBackgroundColor.alphaF()));

        xml.writeAttribute("draw:background-size", "full");
    }

    // No <style:drawing-page-properties> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writeMasterPage(QXmlStreamWriter& xml)
{
    xml.writeStartElement("style:master-page");

    // Write <style:master-pages> attributes
    xml.writeAttribute("style:name", "Default");
    xml.writeAttribute("style:page-layout-name", "DefaultPageLayout");
    xml.writeAttribute("draw:style-name", "DefaultPageStyle");

    // No <style:master-page> sub-elements

    xml.writeEndElement();
}

//======================================================================================================================

void OdgWriter::writeDocumentContent(QXmlStreamWriter& xml)
{
    xml.writeStartDocument();

    xml.writeStartElement("office:document-content");

    // Write <office:document-content> attributes
    xml.writeAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
    xml.writeAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
    xml.writeAttribute("xmlns:loext", "urn:org:documentfoundation:names:experimental:office:xmlns:loext:1.0");
    xml.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    xml.writeAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
    xml.writeAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
    xml.writeAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
    xml.writeAttribute("xmlns:jade", "https://github.com/jaallen85/jade");
    xml.writeAttribute("office:version", "1.3");

    // Write <office:document-content> sub-elements
    writeContentFontFaces(xml);
    writeContentAutomaticStyles(xml);
    writeBody(xml);

    xml.writeEndElement();
    xml.writeEndDocument();
}

void OdgWriter::writeContentFontFaces(QXmlStreamWriter& xml)
{
    xml.writeStartElement("office:font-face-decls");

    // No <office:font-face-decls> attributes

    // No <office:font-face-decls> sub-elements
    QStringList fontFaces;
    QString fontFamily;

    if (mDefaultStyle)
    {
        fontFamily = mDefaultStyle->lookupFontFamily();
        fontFaces.append(fontFamily);
        writeFontFace(xml, fontFamily);
    }

    for(auto& style : qAsConst(mStyles))
    {
        if (style->isFontFamilyValid())
        {
            fontFamily = style->fontFamily();
            if (!fontFaces.contains(fontFamily))
            {
                fontFaces.append(fontFamily);
                writeFontFace(xml, fontFamily);
            }
        }
    }

    xml.writeEndElement();
}

void OdgWriter::writeContentAutomaticStyles(QXmlStreamWriter& xml)
{
    xml.writeStartElement("office:automatic-styles");

    // No <office:automatic-styles> attributes

    // Write <office:automatic-styles> sub-elements
    writePageStyle(xml, "PageStyle");
    for(auto& style : qAsConst(mStyles))
        writeStyle(xml, style, mTextStyleNeeded.value(style));

    xml.writeEndElement();
}

void OdgWriter::writeBody(QXmlStreamWriter& xml)
{
    xml.writeStartElement("office:body");

    // No <office:body> attributes

    // Write <office:body> sub-elements
    writeDrawing(xml);

    xml.writeEndElement();
}

void OdgWriter::writeDrawing(QXmlStreamWriter& xml)
{
    xml.writeStartElement("office:drawing");

    // No <office:drawing> attributes

    // Write <office:drawing> sub-elements
    for(auto& page : qAsConst(mPages))
        writePage(xml, page);

    xml.writeEndElement();
}

void OdgWriter::writePage(QXmlStreamWriter& xml, OdgPage* page)
{
    xml.writeStartElement("draw:page");

    // Write <draw:page> attributes
    xml.writeAttribute("draw:name", page->name());
    xml.writeAttribute("draw:style-name", "PageStyle");
    xml.writeAttribute("draw:master-page-name", "Default");

    // Write <draw:page> sub-elements
    writeItems(xml, page->items());

    xml.writeEndElement();
}

//======================================================================================================================

void OdgWriter::writeStyle(QXmlStreamWriter& xml, OdgStyle* style, bool writeParagraphAndTextStyle)
{
    // Main style
    xml.writeStartElement("style:style");

    // Write <style:style> attributes
    xml.writeAttribute("style:name", style->name());
    xml.writeAttribute("style:family", "graphic");
    if (style->parent()) xml.writeAttribute("style:parent-style-name", style->parent()->name());

    // Write <style:style> sub-elements
    const bool shouldWriteGraphicProperties = (style->isPenStyleValid() || style->isPenWidthValid() ||
                                               style->isPenColorValid() || style->isPenCapStyleValid() ||
                                               style->isPenJoinStyleValid() || style->isBrushColorValid() ||
                                               style->isStartMarkerStyleValid() || style->isStartMarkerSizeValid() ||
                                               style->isEndMarkerStyleValid() || style->isEndMarkerSizeValid() ||
                                               style->isTextAlignmentValid() || style->isTextPaddingValid());
    const bool shouldWriteParagraphProperties = style->isTextAlignmentValid();
    const bool shouldWriteTextProperties = (style->isFontFamilyValid() || style->isFontSizeValid() ||
                                            style->isFontStyleValid() || style->isTextColorValid());
    if (shouldWriteGraphicProperties) writeStyleGraphicProperties(xml, style);
    if (shouldWriteParagraphProperties) writeStyleParagraphProperties(xml, style);
    if (shouldWriteTextProperties) writeStyleTextProperties(xml, style);

    xml.writeEndElement();

    if (writeParagraphAndTextStyle)
    {
        // Write paragraph style
        xml.writeStartElement("style:style");
        xml.writeAttribute("style:name", style->name() + "_p");
        xml.writeAttribute("style:family", "paragraph");
        if (style->parent()) xml.writeAttribute("style:parent-style-name", style->parent()->name());
        if (shouldWriteParagraphProperties) writeStyleParagraphProperties(xml, style);
        if (shouldWriteTextProperties) writeStyleTextProperties(xml, style);
        xml.writeEndElement();

        // Related text style
        xml.writeStartElement("style:style");
        xml.writeAttribute("style:name", style->name() + "_t");
        xml.writeAttribute("style:family", "text");
        if (style->parent()) xml.writeAttribute("style:parent-style-name", style->parent()->name());
        if (shouldWriteTextProperties) writeStyleTextProperties(xml, style);
        xml.writeEndElement();
    }
}

void OdgWriter::writeStyleGraphicProperties(QXmlStreamWriter& xml, OdgStyle* style)
{
    xml.writeStartElement("style:graphic-properties");

    // Write <style:graphic-properties> attributes
    if (style->isPenStyleValid())
    {
        switch (style->penStyle())
        {
        case Qt::SolidLine:
            xml.writeAttribute("draw:stroke", "solid");
            break;
        case Qt::DashLine:
            xml.writeAttribute("draw:stroke", "dash");
            xml.writeAttribute("draw:stroke-dash", "Dash_20__28_Rounded_29_");
            break;
        case Qt::DotLine:
            xml.writeAttribute("draw:stroke", "dash");
            xml.writeAttribute("draw:stroke-dash", "Dot_20__28_Rounded_29_");
            break;
        case Qt::DashDotLine:
            xml.writeAttribute("draw:stroke", "dash");
            xml.writeAttribute("draw:stroke-dash", "Dash_20_Dot_20__28_Rounded_29_");
            break;
        case Qt::DashDotDotLine:
            xml.writeAttribute("draw:stroke", "dash");
            xml.writeAttribute("draw:stroke-dash", "Dash_20_Dot_20_Dot_20__28_Rounded_29_");
            break;
        default:
            xml.writeAttribute("draw:stroke", "none");
            break;
        }
    }

    if (style->isPenWidthValid())
        xml.writeAttribute("svg:stroke-width", lengthToString(style->penWidth()));

    if (style->isPenColorValid())
    {
        QColor penColor = style->penColor();
        xml.writeAttribute("svg:stroke-color", colorToString(penColor));
        if (penColor.alpha() != 255)
            xml.writeAttribute("svg:stroke-opacity", percentToString(penColor.alphaF()));
    }

    if (style->isPenCapStyleValid())
    {
        switch (style->penCapStyle())
        {
        case Qt::FlatCap:
            xml.writeAttribute("svg:stroke-linecap", "butt");
            break;
        case Qt::SquareCap:
            xml.writeAttribute("svg:stroke-linecap", "square");
            break;
        default:
            xml.writeAttribute("svg:stroke-linecap", "round");
            break;
        }
    }

    if (style->isPenJoinStyleValid())
    {
        switch (style->penJoinStyle())
        {
        case Qt::MiterJoin:
        case Qt::SvgMiterJoin:
            xml.writeAttribute("draw:stroke-linejoin", "miter");
            break;
        case Qt::BevelJoin:
            xml.writeAttribute("draw:stroke-linejoin", "bevel");
            break;
        default:
            xml.writeAttribute("draw:stroke-linejoin", "round");
            break;
        }
    }

    if (style->isBrushColorValid())
    {
        QColor brushColor = style->brushColor();
        if (brushColor.red() == 0 && brushColor.green() == 0 && brushColor.blue() == 0 && brushColor.alpha() == 0)
        {
            xml.writeAttribute("draw:fill", "none");
        }
        else
        {
            xml.writeAttribute("draw:fill", "solid");
            xml.writeAttribute("draw:fill-color", colorToString(brushColor));
            if (brushColor.alpha() != 255)
                xml.writeAttribute("draw:opacity", percentToString(brushColor.alphaF()));
        }
    }

    if (style->isStartMarkerStyleValid())
    {
        switch (style->startMarkerStyle())
        {
        case Odg::TriangleMarker:
            xml.writeAttribute("draw:marker-start", "Triangle");
            xml.writeAttribute("draw:marker-start-center", "false");
            break;
        case Odg::CircleMarker:
            xml.writeAttribute("draw:marker-start", "Circle");
            xml.writeAttribute("draw:marker-start-center", "true");
            break;
        default:
            xml.writeAttribute("draw:marker-start", "none");
            break;
        }
    }

    if (style->isStartMarkerSizeValid())
        xml.writeAttribute("draw:marker-start-width", lengthToString(style->startMarkerSize()));

    if (style->isEndMarkerStyleValid())
    {
        switch (style->endMarkerStyle())
        {
        case Odg::TriangleMarker:
            xml.writeAttribute("draw:marker-end", "Triangle");
            xml.writeAttribute("draw:marker-end-center", "false");
            break;
        case Odg::CircleMarker:
            xml.writeAttribute("draw:marker-end", "Circle");
            xml.writeAttribute("draw:marker-end-center", "true");
            break;
        default:
            xml.writeAttribute("draw:marker-end", "none");
            break;
        }
    }

    if (style->isEndMarkerSizeValid())
        xml.writeAttribute("draw:marker-end-width", lengthToString(style->endMarkerSize()));

    if (style->isTextAlignmentValid())
    {
        Qt::Alignment horizontal = (style->textAlignment() & Qt::AlignHorizontal_Mask);
        if (horizontal & Qt::AlignHCenter)
            xml.writeAttribute("draw:textarea-horizontal-align", "center");
        else if (horizontal & Qt::AlignRight)
            xml.writeAttribute("draw:textarea-horizontal-align", "right");
        else
            xml.writeAttribute("draw:textarea-horizontal-align", "left");

        Qt::Alignment vertical = (style->textAlignment() & Qt::AlignVertical_Mask);
        if (vertical & Qt::AlignVCenter)
            xml.writeAttribute("draw:textarea-vertical-align", "middle");
        else if (vertical & Qt::AlignBottom)
            xml.writeAttribute("draw:textarea-vertical-align", "bottom");
        else
            xml.writeAttribute("draw:textarea-vertical-align", "top");
    }

    if (style->isTextPaddingValid())
    {
        const QSizeF textPadding = style->textPadding();
        xml.writeAttribute("fo:padding-left", lengthToString(textPadding.width()));
        xml.writeAttribute("fo:padding-top", lengthToString(textPadding.height()));
        xml.writeAttribute("fo:padding-right", lengthToString(textPadding.width()));
        xml.writeAttribute("fo:padding-bottom", lengthToString(textPadding.height()));
    }

    // No <style:graphic-properties> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writeStyleParagraphProperties(QXmlStreamWriter& xml, OdgStyle* style)
{
    xml.writeStartElement("style:paragraph-properties");

    // Write <style:paragraph-properties> attributes
    if (style->isTextAlignmentValid())
    {
        Qt::Alignment horizontal = (style->textAlignment() & Qt::AlignHorizontal_Mask);
        if (horizontal & Qt::AlignHCenter) xml.writeAttribute("fo:text-align", "center");
        else if (horizontal & Qt::AlignRight) xml.writeAttribute("fo:text-align", "end");
        else xml.writeAttribute("fo:text-align", "start");
    }

    // No <style:paragraph-properties> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writeStyleTextProperties(QXmlStreamWriter& xml, OdgStyle* style)
{
    xml.writeStartElement("style:text-properties");

    // Write <style:text-properties> attributes
    if (style->isFontFamilyValid())
        xml.writeAttribute("style:font-name", style->fontFamily());

    if (style->isFontSizeValid())
        xml.writeAttribute("fo:font-size", lengthToString(style->fontSize() * 72 / 96));

    if (style->isFontStyleValid())
    {
        OdgFontStyle fontStyle = style->fontStyle();
        xml.writeAttribute("fo:font-weight", fontStyle.bold() ? "bold" : "normal");
        xml.writeAttribute("fo:font-style", fontStyle.italic() ? "italic" : "normal");

        if (fontStyle.underline())
        {
            xml.writeAttribute("style:text-underline-style", "solid");
            xml.writeAttribute("style:text-underline-width", "single");
            xml.writeAttribute("style:text-underline-color", "font-color");
        }
        else xml.writeAttribute("style:text-underline-style", "none");

        if (fontStyle.strikeOut())
        {
            xml.writeAttribute("style:text-line-through-style", "solid");
            xml.writeAttribute("style:text-line-through-type", "single");
        }
        else xml.writeAttribute("style:text-line-through-style", "none");
    }

    if (style->isTextColorValid())
    {
        QColor textColor = style->textColor();
        xml.writeAttribute("fo:color", colorToString(textColor));
        if (textColor.alpha() != 255)
            xml.writeAttribute("loext:opacity", percentToString(textColor.alphaF()));
    }

    // No <style:text-properties> sub-elements

    xml.writeEndElement();
}

//======================================================================================================================

void OdgWriter::writeItems(QXmlStreamWriter& xml, const QList<OdgItem*>& items)
{
    OdgStyle* style = nullptr;
    QString styleName;

    OdgCurveItem* curveItem = nullptr;
    OdgEllipseItem* ellipseItem = nullptr;
    OdgGroupItem* groupItem = nullptr;
    OdgLineItem* lineItem = nullptr;
    OdgPathItem* pathItem = nullptr;
    OdgPolygonItem* polygonItem = nullptr;
    OdgPolylineItem* polylineItem = nullptr;
    OdgRectItem* rectItem = nullptr;
    OdgRoundedRectItem* roundedRectItem = nullptr;
    OdgTextEllipseItem* textEllipseItem = nullptr;
    OdgTextItem* textItem = nullptr;
    OdgTextRoundedRectItem* textRoundedRectItem = nullptr;

    for(auto& item : items)
    {
        style = mItemStyles.value(item);
        styleName = (style) ? style->name() : "";

        lineItem = dynamic_cast<OdgLineItem*>(item);
        if (lineItem)
        {
            writeLineItem(xml, lineItem, styleName);
            continue;
        }

        curveItem = dynamic_cast<OdgCurveItem*>(item);
        if (curveItem)
        {
            writeCurveItem(xml, curveItem, styleName);
            continue;
        }

        polylineItem = dynamic_cast<OdgPolylineItem*>(item);
        if (polylineItem)
        {
            writePolylineItem(xml, polylineItem, styleName);
            continue;
        }

        textItem = dynamic_cast<OdgTextItem*>(item);
        if (textItem)
        {
            writeTextItem(xml, textItem, styleName);
            continue;
        }

        textRoundedRectItem = dynamic_cast<OdgTextRoundedRectItem*>(item);
        if (textRoundedRectItem)
        {
            writeTextRoundedRectItem(xml, textRoundedRectItem, styleName);
            continue;
        }

        roundedRectItem = dynamic_cast<OdgRoundedRectItem*>(item);
        if (roundedRectItem)
        {
            writeRoundedRectItem(xml, roundedRectItem, styleName);
            continue;
        }

        textEllipseItem = dynamic_cast<OdgTextEllipseItem*>(item);
        if (textEllipseItem)
        {
            writeTextEllipseItem(xml, textEllipseItem, styleName);
            continue;
        }

        ellipseItem = dynamic_cast<OdgEllipseItem*>(item);
        if (ellipseItem)
        {
            writeEllipseItem(xml, ellipseItem, styleName);
            continue;
        }

        pathItem = dynamic_cast<OdgPathItem*>(item);
        if (pathItem)
        {
            writePathItem(xml, pathItem, styleName);
            continue;
        }

        rectItem = dynamic_cast<OdgRectItem*>(item);
        if (rectItem)
        {
            writeRectItem(xml, rectItem, styleName);
            continue;
        }

        polygonItem = dynamic_cast<OdgPolygonItem*>(item);
        if (polygonItem)
        {
            writePolygonItem(xml, polygonItem, styleName);
            continue;
        }

        groupItem = dynamic_cast<OdgGroupItem*>(item);
        if (groupItem)
        {
            writeGroupItem(xml, groupItem, styleName);
            continue;
        }
    }
}

void OdgWriter::writeLineItem(QXmlStreamWriter& xml, OdgLineItem* item, const QString& styleName)
{
    xml.writeStartElement("draw:line");

    // Write <draw:line> attributes
    xml.writeAttribute("draw:style-name", styleName);

    const QString transform = transformToString(item->position(), item->isFlipped(), item->rotation());
    if (!transform.isEmpty()) xml.writeAttribute("draw:transform", transform);

    QLineF line = item->line();
    xml.writeAttribute("svg:x1", lengthToString(line.x1()));
    xml.writeAttribute("svg:y1", lengthToString(line.y1()));
    xml.writeAttribute("svg:x2", lengthToString(line.x2()));
    xml.writeAttribute("svg:y2", lengthToString(line.y2()));

    // No <draw:line> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writeCurveItem(QXmlStreamWriter& xml, OdgCurveItem* item, const QString& styleName)
{
    xml.writeStartElement("draw:path");

    // Write <draw:path> attributes
    xml.writeAttribute("draw:style-name", styleName);

    const QString transform = transformToString(item->position(), item->isFlipped(), item->rotation());
    if (!transform.isEmpty()) xml.writeAttribute("draw:transform", transform);

    const OdgCurve curve = item->curve();
    QPainterPath curvePath;
    curvePath.moveTo(curve.p1());
    curvePath.cubicTo(curve.cp1(), curve.cp2(), curve.p2());
    const QRectF curveBoundingRect = curvePath.boundingRect();

    xml.writeAttribute("svg:x", lengthToString(curveBoundingRect.left()));
    xml.writeAttribute("svg:y", lengthToString(curveBoundingRect.top()));
    xml.writeAttribute("svg:width", lengthToString(curveBoundingRect.width()));
    xml.writeAttribute("svg:height", lengthToString(curveBoundingRect.height()));
    xml.writeAttribute("svg:viewBox", viewBoxToString(curveBoundingRect));
    xml.writeAttribute("svg:d", pathToString(curvePath));

    // No <draw:path> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writeRectItem(QXmlStreamWriter& xml, OdgRectItem* item, const QString& styleName)
{
    xml.writeStartElement("draw:rect");

    // Write <draw:rect> attributes
    xml.writeAttribute("draw:style-name", styleName);

    const QString transform = transformToString(item->position(), item->isFlipped(), item->rotation());
    if (!transform.isEmpty()) xml.writeAttribute("draw:transform", transform);

    QRectF rect = item->rect();
    xml.writeAttribute("svg:x", lengthToString(rect.left()));
    xml.writeAttribute("svg:y", lengthToString(rect.top()));
    xml.writeAttribute("svg:width", lengthToString(rect.width()));
    xml.writeAttribute("svg:height", lengthToString(rect.height()));

    // No <draw:rect> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writeRoundedRectItem(QXmlStreamWriter& xml, OdgRoundedRectItem* item, const QString& styleName)
{
    xml.writeStartElement("draw:rect");

    // Write <draw:rect> attributes
    xml.writeAttribute("draw:style-name", styleName);

    const QString transform = transformToString(item->position(), item->isFlipped(), item->rotation());
    if (!transform.isEmpty()) xml.writeAttribute("draw:transform", transform);

    QRectF rect = item->rect();
    xml.writeAttribute("svg:x", lengthToString(rect.left()));
    xml.writeAttribute("svg:y", lengthToString(rect.top()));
    xml.writeAttribute("svg:width", lengthToString(rect.width()));
    xml.writeAttribute("svg:height", lengthToString(rect.height()));
    if (item->cornerRadius() != 0) xml.writeAttribute("draw:corner-radius", lengthToString(item->cornerRadius()));

    // No <draw:rect> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writeEllipseItem(QXmlStreamWriter& xml, OdgEllipseItem* item, const QString& styleName)
{
    xml.writeStartElement("draw:ellipse");

    // Write <draw:ellipse> attributes
    xml.writeAttribute("draw:style-name", styleName);

    const QString transform = transformToString(item->position(), item->isFlipped(), item->rotation());
    if (!transform.isEmpty()) xml.writeAttribute("draw:transform", transform);

    QRectF ellipse = item->ellipse();
    xml.writeAttribute("svg:x", lengthToString(ellipse.left()));
    xml.writeAttribute("svg:y", lengthToString(ellipse.top()));
    xml.writeAttribute("svg:width", lengthToString(ellipse.width()));
    xml.writeAttribute("svg:height", lengthToString(ellipse.height()));

    // No <draw:ellipse> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writeTextItem(QXmlStreamWriter& xml, OdgTextItem* item, const QString& styleName)
{
    xml.writeStartElement("draw:rect");

    // Write <draw:rect> attributes
    xml.writeAttribute("draw:style-name", styleName);
    xml.writeAttribute("draw:text-style-name", styleName + "_p");

    const QString transform = transformToString(item->position(), item->isFlipped(), item->rotation());
    if (!transform.isEmpty()) xml.writeAttribute("draw:transform", transform);

    QRectF rect = item->boundingRect();
    if (mGrid > 0)
    {
        // Snap the edges of the rect to the grid
        const double left = mGrid * qFloor(rect.left() / mGrid);
        const double top = mGrid * qFloor(rect.top() / mGrid);
        const double right = mGrid * qCeil(rect.right() / mGrid);
        const double bottom = mGrid * qCeil(rect.bottom() / mGrid);
        rect = QRectF(left, top, right - left, bottom - top);
    }

    xml.writeAttribute("svg:x", lengthToString(rect.left()));
    xml.writeAttribute("svg:y", lengthToString(rect.top()));
    xml.writeAttribute("svg:width", lengthToString(rect.width()));
    xml.writeAttribute("svg:height", lengthToString(rect.height()));
    xml.writeAttribute("jade:text-item-hint", "text");

    // Write <draw:rect> sub-elements
    writeCaption(xml, item->caption(), styleName);

    xml.writeEndElement();
}

void OdgWriter::writeTextRoundedRectItem(QXmlStreamWriter& xml, OdgTextRoundedRectItem* item, const QString& styleName)
{
    xml.writeStartElement("draw:rect");

    // Write <draw:rect> attributes
    xml.writeAttribute("draw:style-name", styleName);
    xml.writeAttribute("draw:text-style-name", styleName + "_p");

    const QString transform = transformToString(item->position(), item->isFlipped(), item->rotation());
    if (!transform.isEmpty()) xml.writeAttribute("draw:transform", transform);

    QRectF rect = item->rect();
    xml.writeAttribute("svg:x", lengthToString(rect.left()));
    xml.writeAttribute("svg:y", lengthToString(rect.top()));
    xml.writeAttribute("svg:width", lengthToString(rect.width()));
    xml.writeAttribute("svg:height", lengthToString(rect.height()));
    if (item->cornerRadius() != 0) xml.writeAttribute("draw:corner-radius", lengthToString(item->cornerRadius()));
    xml.writeAttribute("jade:text-item-hint", "text-rect");

    // Write <draw:rect> sub-elements
    writeCaption(xml, item->caption(), styleName);

    xml.writeEndElement();
}

void OdgWriter::writeTextEllipseItem(QXmlStreamWriter& xml, OdgTextEllipseItem* item, const QString& styleName)
{
    xml.writeStartElement("draw:ellipse");

    // Write <draw:ellipse> attributes
    xml.writeAttribute("draw:style-name", styleName);
    xml.writeAttribute("draw:text-style-name", styleName + "_p");

    const QString transform = transformToString(item->position(), item->isFlipped(), item->rotation());
    if (!transform.isEmpty()) xml.writeAttribute("draw:transform", transform);

    QRectF ellipse = item->ellipse();
    xml.writeAttribute("svg:x", lengthToString(ellipse.left()));
    xml.writeAttribute("svg:y", lengthToString(ellipse.top()));
    xml.writeAttribute("svg:width", lengthToString(ellipse.width()));
    xml.writeAttribute("svg:height", lengthToString(ellipse.height()));
    xml.writeAttribute("jade:text-item-hint", "text-ellipse");

    // Write <draw:ellipse> sub-elements
    writeCaption(xml, item->caption(), styleName);

    xml.writeEndElement();
}

void OdgWriter::writePolylineItem(QXmlStreamWriter& xml, OdgPolylineItem* item, const QString& styleName)
{
    xml.writeStartElement("draw:polyline");

    // Write <draw:polyline> attributes
    xml.writeAttribute("draw:style-name", styleName);

    const QString transform = transformToString(item->position(), item->isFlipped(), item->rotation());
    if (!transform.isEmpty()) xml.writeAttribute("draw:transform", transform);

    const QPolygonF polyline = item->polyline();
    const QRectF polylineBoundingRect = polyline.boundingRect();
    xml.writeAttribute("svg:x", lengthToString(polylineBoundingRect.left()));
    xml.writeAttribute("svg:y", lengthToString(polylineBoundingRect.top()));
    xml.writeAttribute("svg:width", lengthToString(polylineBoundingRect.width()));
    xml.writeAttribute("svg:height", lengthToString(polylineBoundingRect.height()));
    xml.writeAttribute("svg:viewBox", viewBoxToString(polylineBoundingRect));
    xml.writeAttribute("draw:points", pointsToString(polyline));

    // No <draw:polyline> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writePolygonItem(QXmlStreamWriter& xml, OdgPolygonItem* item, const QString& styleName)
{
    xml.writeStartElement("draw:polygon");

    // Write <draw:polygon> attributes
    xml.writeAttribute("draw:style-name", styleName);

    const QString transform = transformToString(item->position(), item->isFlipped(), item->rotation());
    if (!transform.isEmpty()) xml.writeAttribute("draw:transform", transform);

    const QPolygonF polygon = item->polygon();
    const QRectF polygonBoundingRect = polygon.boundingRect();
    xml.writeAttribute("svg:x", lengthToString(polygonBoundingRect.left()));
    xml.writeAttribute("svg:y", lengthToString(polygonBoundingRect.top()));
    xml.writeAttribute("svg:width", lengthToString(polygonBoundingRect.width()));
    xml.writeAttribute("svg:height", lengthToString(polygonBoundingRect.height()));
    xml.writeAttribute("svg:viewBox", viewBoxToString(polygonBoundingRect));
    xml.writeAttribute("draw:points", pointsToString(polygon));

    // No <draw:polygon> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writePathItem(QXmlStreamWriter& xml, OdgPathItem* item, const QString& styleName)
{
    xml.writeStartElement("draw:path");

    // Write <draw:path> attributes
    xml.writeAttribute("draw:style-name", styleName);

    const QString transform = transformToString(item->position(), item->isFlipped(), item->rotation());
    if (!transform.isEmpty()) xml.writeAttribute("draw:transform", transform);

    QRectF rect = item->rect();
    xml.writeAttribute("svg:x", lengthToString(rect.left()));
    xml.writeAttribute("svg:y", lengthToString(rect.top()));
    xml.writeAttribute("svg:width", lengthToString(rect.width()));
    xml.writeAttribute("svg:height", lengthToString(rect.height()));
    xml.writeAttribute("svg:viewBox", viewBoxToString(item->pathRect()));
    xml.writeAttribute("svg:d", pathToString(item->path()));

    // No <draw:path> sub-elements

    xml.writeEndElement();
}

void OdgWriter::writeGroupItem(QXmlStreamWriter& xml, OdgGroupItem* item, const QString& styleName)
{
    xml.writeStartElement("draw:g");

    // No <draw:g> attributes

    // NOTE: The elaborate code in this function is necessary because the draw:g element does not support the
    // draw:transform attribute.

    // Save group and child item's original position/transform to be restored later
    const QPointF originalPosition = item->position();
    const int originalRotation = item->rotation();
    const bool originalFlipped = item->isFlipped();

    const QList<OdgItem*> childItems = item->items();
    QHash<OdgItem*,QPointF> originalChildPosition;
    QHash<OdgItem*,int> originalChildRotation;
    QHash<OdgItem*,bool> originalChildFlipped;
    for(auto& child : childItems)
    {
        originalChildPosition.insert(child, child->position());
        originalChildRotation.insert(child, child->rotation());
        originalChildFlipped.insert(child, child->isFlipped());
    }

    // Apply the group's position/transform to each item
    for(auto& child : childItems)
    {
        child->setPosition(item->mapToScene(child->position()));
        child->setRotation(child->rotation() + item->rotation());
        if (item->isFlipped()) child->setFlipped(!child->isFlipped());
    }

    // Clear group's position/transform
    item->setPosition(QPointF(0, 0));
    item->setRotation(0);
    item->setFlipped(false);

    // Write <draw:path> sub-elements
    writeItems(xml, childItems);

    // Restore group and child items' original position/transform
    for(auto& child : childItems)
    {
        child->setPosition(originalChildPosition.value(child));
        child->setRotation(originalChildRotation.value(child));
        child->setFlipped(originalChildFlipped.value(child));
    }

    item->setPosition(originalPosition);
    item->setRotation(originalRotation);
    item->setFlipped(originalFlipped);

    xml.writeEndElement();
}

void OdgWriter::writeCaption(QXmlStreamWriter& xml, const QString& caption, const QString& styleName)
{
    const QStringList lines = caption.split("\n");
    for(auto& line : lines)
    {
        xml.writeStartElement("text:p");

        xml.writeAttribute("text:style-name", styleName + "_p");

        xml.writeStartElement("text:span");
        xml.writeAttribute("text:style-name", styleName + "_t");
        xml.writeCharacters(line);
        xml.writeEndElement();

        xml.writeEndElement();
    }
}

//======================================================================================================================

QString OdgWriter::lengthToString(double length) const
{
    return QString::number(length, 'g', 8) + Odg::unitsToString(mUnits);
}

QString OdgWriter::xCoordinateToString(double x) const
{
    return lengthToString(x + mPageMargins.left());
}

QString OdgWriter::yCoordinateToString(double y) const
{
    return lengthToString(y + mPageMargins.top());
}

QString OdgWriter::percentToString(double value) const
{
    return QString::number(value * 100, 'f', 2) + "%";
}

QString OdgWriter::colorToString(const QColor& color) const
{
    return color.name(QColor::HexRgb);
}

//======================================================================================================================

QString OdgWriter::transformToString(const QPointF& position, bool flipped, int rotation) const
{
    QString transformStr;
    if (rotation != 0)
        transformStr += "rotate(" + QString::number(qDegreesToRadians(rotation * -90), 'f', 6) + ") ";
    if (flipped)
        transformStr += "scale(-1, 1) ";
    if (position.x() != 0 || position.y() != 0)
        transformStr += "translate(" + xCoordinateToString(position.x()) + ", " + yCoordinateToString(position.y()) + ") ";
    return transformStr.trimmed();
}

QString OdgWriter::viewBoxToString(const QRectF& viewBox) const
{
    QString leftStr = QString::number(viewBox.left(), 'g', 8);
    QString topStr = QString::number(viewBox.top(), 'g', 8);
    QString widthStr = QString::number(viewBox.width(), 'g', 8);
    QString heightStr = QString::number(viewBox.height(), 'g', 8);
    return leftStr + " " + topStr + " " + widthStr + " " + heightStr;
}

QString OdgWriter::pointsToString(const QPolygonF& points) const
{
    QString pointsStr;
    for(auto& point : points)
        pointsStr += QString::number(point.x(), 'g', 8) + "," + QString::number(point.y(), 'g', 8) + " ";
    return pointsStr.trimmed();
}

QString OdgWriter::pathToString(const QPainterPath& path) const
{
    QString pathStr;

    const int elementCount = path.elementCount();
    for(int elementIndex = 0; elementIndex < elementCount; elementIndex++)
    {
        const QPainterPath::Element element = path.elementAt(elementIndex);
        switch (element.type)
        {
        case QPainterPath::MoveToElement:
            pathStr += "M " + QString::number(element.x, 'g', 8) + " " + QString::number(element.y, 'g', 8) + " ";
            break;
        case QPainterPath::LineToElement:
            pathStr += "L " + QString::number(element.x, 'g', 8) + " " + QString::number(element.y, 'g', 8) + " ";
            break;
        case QPainterPath::CurveToElement:
            pathStr += "C " + QString::number(element.x, 'g', 8) + " " + QString::number(element.y, 'g', 8) + " ";
            break;
        case QPainterPath::CurveToDataElement:
            pathStr += QString::number(element.x, 'g', 8) + " " + QString::number(element.y, 'g', 8) + " ";
            break;
        default:
            pathStr += QString::number(element.x, 'g', 8) + " " + QString::number(element.y, 'g', 8) + " ";
            break;
        }
    }

    return pathStr.trimmed();
}
