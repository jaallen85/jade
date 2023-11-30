// File: OdgReader.cpp
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

#include "OdgReader.h"
#include <QRegularExpression>
#include <QXmlStreamReader>
#include <quazip.h>
#include <quazipfile.h>
#include "OdgPage.h"
#include "OdgEllipseItem.h"
#include "OdgLineItem.h"
#include "OdgRoundedRectItem.h"
#include "OdgTextItem.h"
#include "OdgTextEllipseItem.h"
#include "OdgTextRoundedRectItem.h"

OdgReader::OdgReader(const QString& fileName) :
    mUnits(Odg::UnitsInches), mPageSize(8.2, 6.2), mPageMargins(0.1, 0.1, 0.1, 0.1), mBackgroundColor(255, 255, 255),
    mGrid(0.05), mGridStyle(Odg::GridLines), mGridColor(77, 153, 153), mGridSpacingMajor(8), mGridSpacingMinor(2),
    mPages(), mFile(fileName)
{
    // Nothing more to do here.
}

OdgReader::~OdgReader()
{
    close();
    qDeleteAll(mStyles);
    qDeleteAll(mPages);
}

//======================================================================================================================

Odg::Units OdgReader::units() const
{
    return mUnits;
}

QSizeF OdgReader::pageSize() const
{
    return mPageSize;
}

QMarginsF OdgReader::pageMargins() const
{
    return mPageMargins;
}

QColor OdgReader::backgroundColor() const
{
    return mBackgroundColor;
}

//======================================================================================================================

double OdgReader::grid() const
{
    return mGrid;
}

Odg::GridStyle OdgReader::gridStyle() const
{
    return mGridStyle;
}

QColor OdgReader::gridColor() const
{
    return mGridColor;
}

int OdgReader::gridSpacingMajor() const
{
    return mGridSpacingMajor;
}

int OdgReader::gridSpacingMinor() const
{
    return mGridSpacingMinor;
}

//======================================================================================================================

QList<OdgPage*> OdgReader::takePages()
{
    QList<OdgPage*> pages = mPages;
    mPages.clear();
    return pages;
}

//======================================================================================================================

bool OdgReader::open()
{
    return mFile.open(QFile::ReadOnly);
}

void OdgReader::close()
{
    mFile.close();
}

//======================================================================================================================

bool OdgReader::read()
{
    QuaZip odgArchive(&mFile);
    if (!odgArchive.open(QuaZip::mdUnzip)) return false;

    QuaZipFile xmlFile(&odgArchive);
    QXmlStreamReader xml;

    // Read document settings from settings.xml
    odgArchive.setCurrentFile("settings.xml");
    if (!xmlFile.open(QFile::ReadOnly)) return false;

    xml.setDevice(&xmlFile);
    if (!xml.readNextStartElement() || xml.qualifiedName() != QStringLiteral("office:document-settings")) return false;
    readDocumentSettings(xml);
    xmlFile.close();

    // Read document styles from styles.xml
    odgArchive.setCurrentFile("styles.xml");
    if (!xmlFile.open(QFile::ReadOnly)) return false;

    xml.setDevice(&xmlFile);
    if (!xml.readNextStartElement() || xml.qualifiedName() != QStringLiteral("office:document-styles")) return false;
    readDocumentStyles(xml);
    xmlFile.close();

    // Read document content from content.xml
    odgArchive.setCurrentFile("content.xml");
    if (!xmlFile.open(QFile::ReadOnly)) return false;

    xml.setDevice(&xmlFile);
    if (!xml.readNextStartElement() || xml.qualifiedName() != QStringLiteral("office:document-content")) return false;
    readDocumentContent(xml);
    xmlFile.close();

    return true;
}

//======================================================================================================================

void OdgReader::readDocumentSettings(QXmlStreamReader& xml)
{
    // No attributes for <office:document-settings> element

    // Read <office:document-settings> sub-elements
    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("office:settings"))
            readSettings(xml);
        else
            xml.skipCurrentElement();
    }

    xml.skipCurrentElement();
}

void OdgReader::readSettings(QXmlStreamReader& xml)
{
    // No attributes for <office:settings> element

    // Read <office:settings> sub-elements
    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("config:config-item-set"))
            readConfigItemSet(xml);
        else
            xml.skipCurrentElement();
    }
}

void OdgReader::readConfigItemSet(QXmlStreamReader& xml)
{
    // Read attributes of <config:config-item-set> element
    const QXmlStreamAttributes attributes = xml.attributes();
    if (attributes.hasAttribute("config:name") && attributes.value("config:name") == QStringLiteral("jade:settings"))
    {
        // Read <config:config-item-set> sub-elements
        while (xml.readNextStartElement())
        {
            if (xml.qualifiedName() == QStringLiteral("config:config-item"))
                readConfigItem(xml);
            else
                xml.skipCurrentElement();
        }
    }
    else
    {
        // Ignore the rest of <config:config-item-set> element
        xml.skipCurrentElement();
    }
}

void OdgReader::readConfigItem(QXmlStreamReader& xml)
{
    // Read attributes and text for <config:config-item> element
    const QXmlStreamAttributes attributes = xml.attributes();
    const QString text = xml.readElementText();
    if (attributes.hasAttribute("config:name") && attributes.hasAttribute("config:type"))
    {
        const QStringView name = attributes.value("config:name");
        const QStringView type = attributes.value("config:type");
        if (name == QStringLiteral("units") && type == QStringLiteral("string"))
        {
            mUnits = Odg::unitsFromString(text);
        }
        else if (name == QStringLiteral("grid") && type == QStringLiteral("double"))
        {
            double grid = lengthFromString(text);
            if (grid >= 0) mGrid = grid;
        }
        else if (name == QStringLiteral("gridStyle") && type == QStringLiteral("string"))
        {
            QString styleStr = text.trimmed().toLower();
            if (styleStr == "dots")
                mGridStyle = Odg::GridDots;
            else if (styleStr == "lines")
                mGridStyle = Odg::GridLines;
            else
                mGridStyle = Odg::GridHidden;
        }
        else if (name == QStringLiteral("gridColor") && type == QStringLiteral("string"))
        {
            mGridColor = colorFromString(text);
        }
        else if (name == QStringLiteral("gridSpacingMajor") && type == QStringLiteral("int"))
        {
            int spacing = text.toInt();
            mGridSpacingMajor = spacing;
        }
        else if (name == QStringLiteral("gridSpacingMinor") && type == QStringLiteral("int"))
        {
            int spacing = text.toInt();
            mGridSpacingMinor = spacing;
        }
    }

    // No sub-elements for <config:config-item>
}

//======================================================================================================================

void OdgReader::readDocumentStyles(QXmlStreamReader& xml)
{
    // No attributes for <office:document-styles> element

    // Read <office:document-styles> sub-elements
    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("office:styles"))
            readStyles(xml);
        else if (xml.qualifiedName() == QStringLiteral("office:automatic-styles"))
            readAutomaticPageStyles(xml);
        else if (xml.qualifiedName() == QStringLiteral("office:master-styles"))
            readMasterPageStyles(xml);
        else
            xml.skipCurrentElement();
    }

    xml.skipCurrentElement();
}

void OdgReader::readStyles(QXmlStreamReader& xml)
{
    // No attributes for <office:styles> element

    // Read <office:styles> sub-elements
    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("style:default-style"))
        {
            OdgStyle* defaultStyle = new OdgStyle(mUnits, true);
            readStyle(xml, defaultStyle);
            mStyles.prepend(defaultStyle);
        }
        else if (xml.qualifiedName() == QStringLiteral("style:style"))
        {
            OdgStyle* defaultStyle = new OdgStyle(mUnits, false);
            readStyle(xml, defaultStyle);
            mStyles.prepend(defaultStyle);
        }
        else xml.skipCurrentElement();
    }
}

void OdgReader::readAutomaticPageStyles(QXmlStreamReader& xml)
{
    // No attributes for <office:automatic-styles> element

    // Read <office:automatic-styles> sub-elements
    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("style:page-layout"))
            readPageLayout(xml);
        else if (xml.qualifiedName() == QStringLiteral("style:style"))
            readPageStyle(xml);
        else
            xml.skipCurrentElement();
    }
}

void OdgReader::readMasterPageStyles(QXmlStreamReader& xml)
{
    // No attributes for <office:master-styles> element

    // Todo: Read <office:master-styles> sub-elements
    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("style:master-page"))
            readMasterPage(xml);
        else
            xml.skipCurrentElement();
    }
}

void OdgReader::readPageLayout(QXmlStreamReader& xml)
{
    // ASSUMPTION: Each ODG document contains one and only one style:page-layout element

    // Ignore attributes for <style:page-layout> element

    // Read sub-elements for <style:page-layout>
    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("style:page-layout-properties"))
        {
            // Read attributes for <style:page-layout-properties> element
            const QXmlStreamAttributes attributes = xml.attributes();
            for(auto& attribute : attributes)
            {
                if (attribute.qualifiedName() == QStringLiteral("fo:page-width"))
                {
                    double width = lengthFromString(attribute.value());
                    if (width > 0) mPageSize.setWidth(width);
                }
                else if (attribute.qualifiedName() == QStringLiteral("fo:page-height"))
                {
                    double height = lengthFromString(attribute.value());
                    if (height > 0) mPageSize.setHeight(height);
                }
                else if (attribute.qualifiedName() == QStringLiteral("fo:margin-left"))
                {
                    double margin = lengthFromString(attribute.value());
                    if (margin > 0) mPageMargins.setLeft(margin);
                }
                else if (attribute.qualifiedName() == QStringLiteral("fo:margin-top"))
                {
                    double margin = lengthFromString(attribute.value());
                    if (margin > 0) mPageMargins.setTop(margin);
                }
                else if (attribute.qualifiedName() == QStringLiteral("fo:margin-right"))
                {
                    double margin = lengthFromString(attribute.value());
                    if (margin > 0) mPageMargins.setRight(margin);
                }
                else if (attribute.qualifiedName() == QStringLiteral("fo:margin-bottom"))
                {
                    double margin = lengthFromString(attribute.value());
                    if (margin > 0) mPageMargins.setBottom(margin);
                }
            }
        }

        xml.skipCurrentElement();
    }
}

void OdgReader::readPageStyle(QXmlStreamReader& xml)
{
    // ASSUMPTION: Each ODG document contains one and only one style:style element with style:family attribute set
    // to "drawing-page".

    // Ignore attributes for <style:style> element

    // Read sub-elements for <style:style>
    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("style:drawing-page-properties"))
        {
            // Read attributes for <style:drawing-page-properties> element
            const QXmlStreamAttributes attributes = xml.attributes();
            for(auto& attribute : attributes)
            {
                if (attribute.qualifiedName() == QStringLiteral("draw:fill"))
                {
                    if (attribute.value() == QStringLiteral("solid"))
                        mBackgroundColor = QColor(0, 0, 0);
                    else
                        mBackgroundColor = QColor(0, 0, 0, 0);
                }
                else if (attribute.qualifiedName() == QStringLiteral("draw:fill-color"))
                {
                    mBackgroundColor = colorFromString(attribute.value());
                }
                else if (attribute.qualifiedName() == QStringLiteral("draw:opacity"))
                {
                    mBackgroundColor.setAlphaF(percentFromString(attribute.value()));
                }
            }
        }

        xml.skipCurrentElement();
    }
}

void OdgReader::readMasterPage(QXmlStreamReader& xml)
{
    // ASSUMPTION: Each ODG document contains one and only one style:master-page element

    // Ignore attributes for <style:master-page> element

    // No sub-elements for <style:master-page>
    xml.skipCurrentElement();
}

//======================================================================================================================

void OdgReader::readDocumentContent(QXmlStreamReader& xml)
{
    // No attributes for <office:document-content> element

    // Read <office:document-content> sub-elements
    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("office:automatic-styles"))
            readStyles(xml);
        else if (xml.qualifiedName() == QStringLiteral("office:body"))
            readBody(xml);
        else
            xml.skipCurrentElement();
    }

    xml.skipCurrentElement();
}

void OdgReader::readBody(QXmlStreamReader& xml)
{
    // No attributes for <office:body> element

    // Read <office:body> sub-elements
    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("office:drawing"))
            readDrawing(xml);
        else
            xml.skipCurrentElement();
    }
}

void OdgReader::readDrawing(QXmlStreamReader& xml)
{
    // No attributes for <office:drawing> element

    // Read <office:drawing> sub-elements
    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("draw:page"))
        {
            OdgPage* page = new OdgPage("Page " + QString::number(mPages.size()));
            readPage(xml, page);
            mPages.append(page);
        }
        else xml.skipCurrentElement();
    }
}

//======================================================================================================================

void OdgReader::readStyle(QXmlStreamReader& xml, OdgStyle* style)
{
    // Read attributes of <style:style> element
    const QXmlStreamAttributes attributes = xml.attributes();
    for(auto& attribute : attributes)
    {
        if (attribute.qualifiedName() == QStringLiteral("style:name"))
            style->setName(attribute.value().toString());
        else if (attribute.qualifiedName() == QStringLiteral("style:parent-style-name"))
        {
            QString parentName = attribute.value().toString();
            for(auto& parentStyle : mStyles)
            {
                if (parentStyle->name() == parentName)
                {
                    style->setParent(parentStyle);
                    break;
                }
            }
        }
    }

    // Read <style:style> sub-elements
    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("style:graphic-properties"))
            readStyleGraphicProperties(xml, style);
        else if (xml.qualifiedName() == QStringLiteral("style:paragraph-properties"))
            readStyleParagraphProperties(xml, style);
        else if (xml.qualifiedName() == QStringLiteral("style:text-properties"))
            readStyleTextProperties(xml, style);
        else
            xml.skipCurrentElement();
    }
}

void OdgReader::readStyleGraphicProperties(QXmlStreamReader& xml, OdgStyle* style)
{
    // Read attributes of <style:graphic-properties> element
    const QXmlStreamAttributes attributes = xml.attributes();
    for(auto& attribute : attributes)
    {
        if (attribute.qualifiedName() == QStringLiteral("draw:stroke"))
        {
            if (attribute.value() == QStringLiteral("solid"))
                style->setPenStyle(Qt::SolidLine);
            else if (attribute.value() == QStringLiteral("dash"))
                style->setPenStyle(Qt::DashLine);
            else
                style->setPenStyle(Qt::NoPen);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:stroke-dash"))
        {
            if (attribute.value() == QStringLiteral("Dash_20__28_Rounded_29_"))
                style->setPenStyle(Qt::DashLine);
            else if (attribute.value() == QStringLiteral("Dot_20__28_Rounded_29_"))
                style->setPenStyle(Qt::DotLine);
            else if (attribute.value() == QStringLiteral("Dash_20_Dot_20__28_Rounded_29_"))
                style->setPenStyle(Qt::DashDotLine);
            else if (attribute.value() == QStringLiteral("Dash_20_Dot_20_Dot_20__28_Rounded_29_"))
                style->setPenStyle(Qt::DashDotDotLine);
        }
        else if (attribute.qualifiedName() == QStringLiteral("svg:stroke-width"))
        {
            style->setPenWidth(lengthFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("svg:stroke-color"))
        {
            style->setPenColor(colorFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("svg:stroke-opacity"))
        {
            QColor penColor = style->penColor();
            penColor.setAlphaF(percentFromString(attribute.value()));
            style->setPenColor(penColor);
        }
        else if (attribute.qualifiedName() == QStringLiteral("svg:stroke-linecap"))
        {
            if (attribute.value() == QStringLiteral("butt"))
                style->setPenCapStyle(Qt::FlatCap);
            else if (attribute.value() == QStringLiteral("square"))
                style->setPenCapStyle(Qt::SquareCap);
            else
                style->setPenCapStyle(Qt::RoundCap);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:stroke-linejoin"))
        {
            if (attribute.value() == QStringLiteral("miter"))
                style->setPenJoinStyle(Qt::MiterJoin);
            else if (attribute.value() == QStringLiteral("bevel"))
                style->setPenJoinStyle(Qt::BevelJoin);
            else
                style->setPenJoinStyle(Qt::RoundJoin);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:fill"))
        {
            if (attribute.value() == QStringLiteral("solid"))
                style->setBrushColor(QColor(0, 0, 0));
            else
                style->setBrushColor(QColor(0, 0, 0, 0));
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:fill-color"))
        {
            style->setBrushColor(colorFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:opacity"))
        {
            QColor brushColor = style->brushColor();
            brushColor.setAlphaF(percentFromString(attribute.value()));
            style->setBrushColor(brushColor);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:marker-start"))
        {
            if (attribute.value() == QStringLiteral("Triangle"))
                style->setStartMarkerStyle(Odg::TriangleMarker);
            else if (attribute.value() == QStringLiteral("Circle"))
                style->setStartMarkerStyle(Odg::CircleMarker);
            else
                style->setStartMarkerStyle(Odg::NoMarker);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:marker-start-width"))
        {
            style->setStartMarkerSize(lengthFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:marker-end"))
        {
            if (attribute.value() == QStringLiteral("Triangle"))
                style->setEndMarkerStyle(Odg::TriangleMarker);
            else if (attribute.value() == QStringLiteral("Circle"))
                style->setEndMarkerStyle(Odg::CircleMarker);
            else
                style->setEndMarkerStyle(Odg::NoMarker);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:marker-end-width"))
        {
            style->setEndMarkerSize(lengthFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:textarea-horizontal-align"))
        {
            Qt::Alignment textAlignment = (style->textAlignment() & (~Qt::AlignHorizontal_Mask));
            if (attribute.value() == QStringLiteral("left"))
                style->setTextAlignment(textAlignment | Qt::AlignLeft);
            else if (attribute.value() == QStringLiteral("right"))
                style->setTextAlignment(textAlignment | Qt::AlignRight);
            else
                style->setTextAlignment(textAlignment | Qt::AlignHCenter);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:textarea-vertical-align"))
        {
            Qt::Alignment textAlignment = (style->textAlignment() & (~Qt::AlignVertical_Mask));
            if (attribute.value() == QStringLiteral("top"))
                style->setTextAlignment(textAlignment | Qt::AlignTop);
            else if (attribute.value() == QStringLiteral("bottom"))
                style->setTextAlignment(textAlignment | Qt::AlignBottom);
            else
                style->setTextAlignment(textAlignment | Qt::AlignVCenter);
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:padding-left"))
        {
            style->setTextPadding(QSizeF(lengthFromString(attribute.value()), style->textPadding().height()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:padding-top"))
        {
            style->setTextPadding(QSizeF(style->textPadding().width(), lengthFromString(attribute.value())));
        }
    }

    // No sub-elements for <style:graphic-properties>
    xml.skipCurrentElement();
}

void OdgReader::readStyleParagraphProperties(QXmlStreamReader& xml, OdgStyle* style)
{
    // Currently nothing to do for <style:paragraph-properties> element
    xml.skipCurrentElement();
}

void OdgReader::readStyleTextProperties(QXmlStreamReader& xml, OdgStyle* style)
{
    // Read attributes of <style:text-properties> element
    const QXmlStreamAttributes attributes = xml.attributes();
    for(auto& attribute : attributes)
    {
        if (attribute.qualifiedName() == QStringLiteral("style:font-name"))
        {
            style->setFontFamily(attribute.value().toString());
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:font-size"))
        {
            style->setFontSize(lengthFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:font-weight"))
        {
            OdgFontStyle fontStyle = style->fontStyle();
            fontStyle.setBold(attribute.value() == QStringLiteral("bold"));
            style->setFontStyle(fontStyle);
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:font-style"))
        {
            OdgFontStyle fontStyle = style->fontStyle();
            fontStyle.setItalic(attribute.value() == QStringLiteral("italic"));
            style->setFontStyle(fontStyle);
        }
        else if (attribute.qualifiedName() == QStringLiteral("style:text-underline-style"))
        {
            OdgFontStyle fontStyle = style->fontStyle();
            fontStyle.setUnderline(attribute.value() == QStringLiteral("solid"));
            style->setFontStyle(fontStyle);
        }
        else if (attribute.qualifiedName() == QStringLiteral("style:text-line-through-style"))
        {
            OdgFontStyle fontStyle = style->fontStyle();
            fontStyle.setStrikeOut(attribute.value() == QStringLiteral("solid"));
            style->setFontStyle(fontStyle);
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:color"))
        {
            style->setTextColor(colorFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("loext:opacity"))
        {
            QColor textColor = style->textColor();
            textColor.setAlphaF(percentFromString(attribute.value()));
            style->setTextColor(textColor);
        }
    }

    // No sub-elements for <style:text-properties>
    xml.skipCurrentElement();
}

OdgStyle* OdgReader::findStyle(const QStringView& name) const
{
    for(auto& style : qAsConst(mStyles))
    {
        if (style->name() == name)
            return style;
    }

    return mStyles.first();
}

//======================================================================================================================

void OdgReader::readPage(QXmlStreamReader& xml, OdgPage* page)
{
    // Read attributes for <office:page> element
    const QXmlStreamAttributes attributes = xml.attributes();
    if (attributes.hasAttribute("draw:name"))
        page->setName(attributes.value("draw:name").toString());

    // Read <office:page> sub-elements
    const QList<OdgItem*> items = readItems(xml);
    for(auto& item : items)
        page->addItem(item);
}

QList<OdgItem*> OdgReader::readItems(QXmlStreamReader& xml)
{
    QList<OdgItem*> items;
    OdgItem* item = nullptr;

    // Read <office:page> or <draw:g> sub-elements
    while (xml.readNextStartElement())
    {
        item = nullptr;

        if (xml.qualifiedName() == QStringLiteral("draw:line"))
            item = readLine(xml);
        else if (xml.qualifiedName() == QStringLiteral("draw:rect"))
            item = readRect(xml);
        else if (xml.qualifiedName() == QStringLiteral("draw:ellipse"))
            item = readEllipse(xml);
        else if (xml.qualifiedName() == QStringLiteral("draw:polyline"))
            item = readPolyline(xml);
        else if (xml.qualifiedName() == QStringLiteral("draw:polygon"))
            item = readPolygon(xml);
        else if (xml.qualifiedName() == QStringLiteral("draw:path"))
            item = readPath(xml);
        else if (xml.qualifiedName() == QStringLiteral("draw:custom-shape"))
            item = readCustomShape(xml);
        else if (xml.qualifiedName() == QStringLiteral("draw:g"))
            item = readGroup(xml);
        else
            xml.skipCurrentElement();

        if (item) items.append(item);
    }

    return items;
}

OdgItem* OdgReader::readLine(QXmlStreamReader& xml)
{
    // Read attributes of <draw:line> element
    OdgStyle* style = mStyles.first();
    QPointF position;
    bool flipped = false;
    int rotation = 0;
    double x1 = 0, y1 = 0, x2 = 0, y2 = 0;

    const QXmlStreamAttributes attributes = xml.attributes();
    for(auto& attribute : attributes)
    {
        if (attribute.qualifiedName() == QStringLiteral("draw:style-name"))
            style = findStyle(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("draw:transform"))
            transformFromString(attribute.value(), position, flipped, rotation);
        else if (attribute.qualifiedName() == QStringLiteral("svg:x1"))
            x1 = lengthFromString(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("svg:y2"))
            y1 = lengthFromString(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("svg:x2"))
            x2 = lengthFromString(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("svg:y2"))
            y2 = lengthFromString(attribute.value());
    }

    // No sub-elements for <draw:line>
    xml.skipCurrentElement();

    // Create OdgLineItem
    OdgLineItem* lineItem = new OdgLineItem();
    lineItem->setPosition(position);
    lineItem->setFlipped(flipped);
    lineItem->setRotation(rotation);
    lineItem->setLine(QLineF(x1, y1, x2, y2));
    lineItem->setPen(style->lookupPen());
    lineItem->setStartMarker(style->lookupStartMarker());
    lineItem->setEndMarker(style->lookupEndMarker());
    if (lineItem->isValid()) return lineItem;

    delete lineItem;
    return nullptr;
}

OdgItem* OdgReader::readRect(QXmlStreamReader& xml)
{
    // Read attributes of <draw:rect> element
    OdgStyle* graphicStyle = mStyles.first();
    OdgStyle* paragraphStyle = mStyles.first();
    QPointF position;
    bool flipped = false;
    int rotation = 0;
    double left = 0, top = 0, width = 0, height = 0;
    double cornerRadius = 0;
    QStringView textItemHintStr;

    const QXmlStreamAttributes attributes = xml.attributes();
    for(auto& attribute : attributes)
    {
        if (attribute.qualifiedName() == QStringLiteral("draw:style-name"))
        {
            graphicStyle = findStyle(attribute.value());
            if (paragraphStyle == mStyles.first()) paragraphStyle = graphicStyle;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:text-style-name"))
            paragraphStyle = findStyle(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("draw:transform"))
            transformFromString(attribute.value(), position, flipped, rotation);
        else if (attribute.qualifiedName() == QStringLiteral("svg:x"))
            left = lengthFromString(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("svg:y"))
            top = lengthFromString(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("svg:width"))
            width = lengthFromString(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("svg:height"))
            height = lengthFromString(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("draw:corner-radius"))
            cornerRadius = lengthFromString(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("jade:text-item-hint"))
            textItemHintStr = attribute.value();
    }

    // Read sub-elements for <draw:rect>
    QString caption = checkForCaption(xml);

    // Create OdgTextItem, OdgTextRoundedRectItem, or OdgRoundedRectItem as needed
    if (textItemHintStr == QStringLiteral("text"))
    {
        // Create OdgTextItem
        OdgTextItem* textItem = new OdgTextItem();
        textItem->setPosition(position);
        textItem->setFlipped(flipped);
        textItem->setRotation(rotation);
        textItem->setFont(paragraphStyle->lookupFont());
        textItem->setTextAlignment(paragraphStyle->lookupTextAlignment());
        textItem->setTextPadding(paragraphStyle->lookupTextPadding());
        textItem->setTextBrush(paragraphStyle->lookupTextBrush());
        textItem->setCaption(caption);
        if (textItem->isValid()) return textItem;

        delete textItem;
        return nullptr;
    }

    if (textItemHintStr == QStringLiteral("text-rect") || !caption.isEmpty())
    {
        // Create OdgTextRoundedRectItem
        OdgTextRoundedRectItem* textRectItem = new OdgTextRoundedRectItem();
        textRectItem->setPosition(position);
        textRectItem->setFlipped(flipped);
        textRectItem->setRotation(rotation);
        textRectItem->setRect(QRectF(left, top, width, height));
        textRectItem->setCornerRadius(cornerRadius);
        textRectItem->setBrush(graphicStyle->lookupBrush());
        textRectItem->setPen(graphicStyle->lookupPen());
        textRectItem->setFont(paragraphStyle->lookupFont());
        textRectItem->setTextAlignment(paragraphStyle->lookupTextAlignment());
        textRectItem->setTextPadding(paragraphStyle->lookupTextPadding());
        textRectItem->setTextBrush(paragraphStyle->lookupTextBrush());
        textRectItem->setCaption(caption);
        if (textRectItem->isValid()) return textRectItem;

        delete textRectItem;
        return nullptr;
    }

    // Create OdgRoundedRectItem
    OdgRoundedRectItem* rectItem = new OdgRoundedRectItem();
    rectItem->setPosition(position);
    rectItem->setFlipped(flipped);
    rectItem->setRotation(rotation);
    rectItem->setRect(QRectF(left, top, width, height));
    rectItem->setCornerRadius(cornerRadius);
    rectItem->setBrush(graphicStyle->lookupBrush());
    rectItem->setPen(graphicStyle->lookupPen());
    if (rectItem->isValid()) return rectItem;

    delete rectItem;
    return nullptr;
}

OdgItem* OdgReader::readEllipse(QXmlStreamReader& xml)
{
    // Read attributes of <draw:ellipse> element
    OdgStyle* graphicStyle = mStyles.first();
    OdgStyle* paragraphStyle = mStyles.first();
    QPointF position;
    bool flipped = false;
    int rotation = 0;
    double left = 0, top = 0, width = 0, height = 0;
    QStringView textItemHintStr;

    const QXmlStreamAttributes attributes = xml.attributes();
    for(auto& attribute : attributes)
    {
        if (attribute.qualifiedName() == QStringLiteral("draw:style-name"))
        {
            graphicStyle = findStyle(attribute.value());
            if (paragraphStyle == mStyles.first()) paragraphStyle = graphicStyle;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:text-style-name"))
            paragraphStyle = findStyle(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("draw:transform"))
            transformFromString(attribute.value(), position, flipped, rotation);
        else if (attribute.qualifiedName() == QStringLiteral("svg:x"))
            left = lengthFromString(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("svg:y"))
            top = lengthFromString(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("svg:width"))
            width = lengthFromString(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("svg:height"))
            height = lengthFromString(attribute.value());
        else if (attribute.qualifiedName() == QStringLiteral("jade:text-item-hint"))
            textItemHintStr = attribute.value();
    }

    // Read sub-elements for <draw:ellipse>
    QString caption = checkForCaption(xml);

    // Create OdgTextEllipseItem or OdgEllipseItem as needed
    if (textItemHintStr == QStringLiteral("text-ellipse") || !caption.isEmpty())
    {
        // Create OdgTextRoundedRectItem
        OdgTextEllipseItem* textEllipseItem = new OdgTextEllipseItem();
        textEllipseItem->setPosition(position);
        textEllipseItem->setFlipped(flipped);
        textEllipseItem->setRotation(rotation);
        textEllipseItem->setRect(QRectF(left, top, width, height));
        textEllipseItem->setBrush(graphicStyle->lookupBrush());
        textEllipseItem->setPen(graphicStyle->lookupPen());
        textEllipseItem->setFont(paragraphStyle->lookupFont());
        textEllipseItem->setTextAlignment(paragraphStyle->lookupTextAlignment());
        textEllipseItem->setTextPadding(paragraphStyle->lookupTextPadding());
        textEllipseItem->setTextBrush(paragraphStyle->lookupTextBrush());
        textEllipseItem->setCaption(caption);
        if (textEllipseItem->isValid()) return textEllipseItem;

        delete textEllipseItem;
        return nullptr;
    }

    // Create OdgRoundedRectItem
    OdgEllipseItem* ellipseItem = new OdgEllipseItem();
    ellipseItem->setPosition(position);
    ellipseItem->setFlipped(flipped);
    ellipseItem->setRotation(rotation);
    ellipseItem->setRect(QRectF(left, top, width, height));
    ellipseItem->setBrush(graphicStyle->lookupBrush());
    ellipseItem->setPen(graphicStyle->lookupPen());
    if (ellipseItem->isValid()) return ellipseItem;

    delete ellipseItem;
    return nullptr;
}

OdgItem* OdgReader::readPolyline(QXmlStreamReader& xml)
{
    xml.skipCurrentElement();
    return nullptr;
}

OdgItem* OdgReader::readPolygon(QXmlStreamReader& xml)
{
    xml.skipCurrentElement();
    return nullptr;
}

OdgItem* OdgReader::readPath(QXmlStreamReader& xml)
{
    xml.skipCurrentElement();
    return nullptr;
}

OdgItem* OdgReader::readCustomShape(QXmlStreamReader& xml)
{
    xml.skipCurrentElement();
    return nullptr;
}

OdgItem* OdgReader::readGroup(QXmlStreamReader& xml)
{
    xml.skipCurrentElement();
    return nullptr;
}

QString OdgReader::checkForCaption(QXmlStreamReader& xml)
{
    QString caption;

    while (xml.readNextStartElement())
    {
        if (xml.qualifiedName() == QStringLiteral("text:p"))
        {
            // Ignore attributes for <text:p>

            // Read sub-elements for <text:p>
            while (xml.readNextStartElement())
            {
                if (xml.qualifiedName() == QStringLiteral("text:span"))
                {
                    if (caption.isEmpty())
                        caption = xml.readElementText();
                    else
                        caption += "\n" + xml.readElementText();
                }
            }
        }
    }

    return caption;
}

//======================================================================================================================

double OdgReader::lengthFromString(const QStringView& str) const
{
    static const QRegularExpression re(R"([-+]?(?:(?:\d*\.\d+)|(?:\d+\.?))(?:[Ee][+-]?\d+)?)");

    QRegularExpressionMatch match = re.match(str);
    if (match.hasMatch())
    {
        bool lengthOk = false;
        double length = match.captured(0).toDouble(&lengthOk);
        if (!lengthOk) return 0;

        bool unitsOk = false;
        QStringView unitsStr = str.last(str.size() - match.captured(0).size()).trimmed();
        Odg::Units units = Odg::unitsFromString(unitsStr, &unitsOk);

        if (!unitsOk)
        {
            // Assume the value provided is in the same units as mUnits
            return length;
        }

        return Odg::convertUnits(length, units, mUnits);
    }

    return 0;
}

double OdgReader::lengthFromString(const QString& str) const
{
    return lengthFromString(QStringView(str));
}

double OdgReader::xCoordinateFromString(const QStringView& str) const
{
    return lengthFromString(str) - mPageMargins.left();
}

double OdgReader::xCoordinateFromString(const QString& str) const
{
    return xCoordinateFromString(QStringView(str));
}

double OdgReader::yCoordinateFromString(const QStringView& str) const
{
    return lengthFromString(str) - mPageMargins.top();
}

double OdgReader::yCoordinateFromString(const QString& str) const
{
    return yCoordinateFromString(QStringView(str));
}

void OdgReader::transformFromString(const QStringView& str, QPointF& position, bool& flipped, int& rotation) const
{
    const QList<QStringView> tokens = str.split(QStringLiteral(")"));
    QStringView trimmedToken;
    for(auto& token : tokens)
    {
        trimmedToken = token.trimmed();
        if (trimmedToken == QStringLiteral("translate("))
        {
            const QList<QStringView> coordinateTokens = trimmedToken.mid(10, -1).split(QStringLiteral(","));
            if (coordinateTokens.size() == 2)
            {
                position.setX(position.x() + xCoordinateFromString(coordinateTokens[0].trimmed()));
                position.setY(position.y() + yCoordinateFromString(coordinateTokens[1].trimmed()));
            }
        }
        else if (trimmedToken == QStringLiteral("scale("))
        {
            flipped = !flipped;
        }
        else if (trimmedToken == QStringLiteral("rotate("))
        {
            bool angleOk = false;
            double angleRadians = trimmedToken.mid(7, -1).toDouble(&angleOk);
            if (angleOk) rotation = rotation + qRound(angleRadians / (-M_PI / 2));
        }
    }
}

void OdgReader::transformFromString(const QString& str, QPointF& position, bool& flipped, int& rotation) const
{
    transformFromString(QStringView(str), position, flipped, rotation);
}

double OdgReader::percentFromString(const QStringView& str) const
{
    if (str.endsWith(QStringLiteral("%")))
    {
        bool valueOk = false;
        double value = str.first(str.size() - 1).toDouble(&valueOk);
        return (valueOk) ? value / 100 : 0;
    }

    bool valueOk = false;
    double value = str.toDouble(&valueOk);
    return (valueOk) ? value : 0;
}

double OdgReader::percentFromString(const QString& str) const
{
    return percentFromString(QStringView(str));
}

QColor OdgReader::colorFromString(const QStringView& str) const
{
    return QColor(str);
}

QColor OdgReader::colorFromString(const QString& str) const
{
    return QColor(str);
}
