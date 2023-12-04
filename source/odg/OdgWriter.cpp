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
#include <QXmlStreamWriter>
#include <quazip.h>
#include <quazipfile.h>

OdgWriter::OdgWriter(const QString& fileName) :
    mUnits(Odg::UnitsInches), mPageSize(8.2, 6.2), mPageMargins(0.1, 0.1, 0.1, 0.1), mBackgroundColor(255, 255, 255),
    mGrid(0.05), mGridStyle(Odg::GridLines), mGridColor(77, 153, 153), mGridSpacingMajor(8), mGridSpacingMinor(2),
    mDefaultStyle(nullptr), mPages(), mFile(fileName), mContentFontFaces()
{
    // Nothing more to do here.
}

OdgWriter::~OdgWriter()
{
    close();
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
    mContentFontFaces.clear();

    QuaZip odgArchive(&mFile);
    if (!odgArchive.open(QuaZip::mdCreate)) return false;

    QuaZipFile odgFile(&odgArchive);
    QXmlStreamWriter xml;
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);

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

    return true;
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
    if (mDefaultStyle) writeFontFace(xml, mDefaultStyle->lookupFontFamily(), true);

    xml.writeEndElement();
}

void OdgWriter::writeFontFace(QXmlStreamWriter& xml, const QString fontFamily, bool style)
{
    bool writeXmlElement = false;

    if (style)
        writeXmlElement = true;
    else if (!mContentFontFaces.contains(fontFamily))
    {
        writeXmlElement = true;
        mContentFontFaces.append(fontFamily);
    }

    if (writeXmlElement)
    {
        xml.writeStartElement("style:font-face");

        // Write <style:font-face> attributes
        xml.writeAttribute("style:name", fontFamily);
        xml.writeAttribute("svg:font-family", fontFamily);

        // No <style:font-face> sub-elements

        xml.writeEndElement();
    }
}

void OdgWriter::writeStyles(QXmlStreamWriter& xml)
{
    xml.writeStartElement("office:styles");

    // No <office:styles> attributes

    // No <office:styles> sub-elements

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
    for(auto& page : qAsConst(mPages))
        writeItemFontFaces(xml, page->items());

    xml.writeEndElement();
}

void OdgWriter::writeContentAutomaticStyles(QXmlStreamWriter& xml)
{
    xml.writeStartElement("office:automatic-styles");

    // No <office:automatic-styles> attributes

    // Write <office:automatic-styles> sub-elements
    writePageStyle(xml, "PageStyle");
    for(auto& page : qAsConst(mPages))
        writeItemStyles(xml, page->items());

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

void OdgWriter::writeItemFontFaces(QXmlStreamWriter& xml, const QList<OdgItem*>& items)
{

}
void OdgWriter::writeItemStyles(QXmlStreamWriter& xml, const QList<OdgItem*>& items)
{

}

void OdgWriter::writeItems(QXmlStreamWriter& xml, const QList<OdgItem*>& items)
{

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
            break;
        default:
            pathStr += QString::number(element.x, 'g', 8) + " " + QString::number(element.y, 'g', 8) + " ";
            break;
        }
    }

    return pathStr.trimmed();
}
