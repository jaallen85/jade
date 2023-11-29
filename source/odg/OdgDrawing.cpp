// File: OdgDrawing.cpp
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

#include "OdgDrawing.h"
#include "OdgPage.h"
#include "OdgReader.h"
#include "OdgStyle.h"
#include <QFile>
#include <quazip.h>
#include <quazipfile.h>

OdgDrawing::OdgDrawing() :
    mUnits(Odg::UnitsInches), mPageSize(8.2, 6.2), mPageMargins(0.1, 0.1, 0.1, 0.1), mBackgroundColor(255, 255, 255),
    mGrid(0.05), mGridVisible(true), mGridColor(77, 153, 153), mGridSpacingMajor(8), mGridSpacingMinor(2),
    mStyles(), mPages()
{
    // Nothing more to do here.
}

OdgDrawing::~OdgDrawing()
{
    clearPages();
    clearStyles();
}

//======================================================================================================================

void OdgDrawing::setUnits(Odg::Units units)
{
    if (mUnits != units)
    {
        double scaleFactor = Odg::convertUnits(1, mUnits, units);

        mUnits = units;

        mPageSize.setWidth(mPageSize.width() * scaleFactor);
        mPageSize.setHeight(mPageSize.height() * scaleFactor);
        mPageMargins.setLeft(mPageMargins.left() * scaleFactor);
        mPageMargins.setTop(mPageMargins.top() * scaleFactor);
        mPageMargins.setRight(mPageMargins.right() * scaleFactor);
        mPageMargins.setBottom(mPageMargins.bottom() * scaleFactor);

        mGrid *= scaleFactor;

        for(auto& style : qAsConst(mStyles))
            style->scaleBy(scaleFactor);

        for(auto& page : qAsConst(mPages))
            page->scaleBy(scaleFactor);
    }
}

void OdgDrawing::setPageSize(const QSizeF& size)
{
    if (size.width() > 0 && size.height() > 0) mPageSize = size;
}

void OdgDrawing::setPageMargins(const QMarginsF& margins)
{
    if (margins.left() >= 0 && margins.top() >= 0 && margins.right() >= 0 && margins.bottom() >= 0)
        mPageMargins = margins;
}

void OdgDrawing::setBackgroundColor(const QColor& color)
{
    mBackgroundColor = color;
}


Odg::Units OdgDrawing::units() const
{
    return mUnits;
}

QSizeF OdgDrawing::pageSize() const
{
    return mPageSize;
}

QMarginsF OdgDrawing::pageMargins() const
{
    return mPageMargins;
}

QColor OdgDrawing::backgroundColor() const
{
    return mBackgroundColor;
}

QRectF OdgDrawing::pageRect() const
{
    return QRectF(-mPageMargins.left(), -mPageMargins.top(), mPageSize.width(), mPageSize.height());
}

QRectF OdgDrawing::contentRect() const
{
    return QRectF(0, 0, mPageSize.width() - mPageMargins.left() - mPageMargins.right(),
                  mPageSize.height() - mPageMargins.top() - mPageMargins.bottom());
}

//======================================================================================================================

void OdgDrawing::setGrid(double grid)
{
    if (grid >= 0) mGrid = grid;
}

void OdgDrawing::setGridVisible(bool visible)
{
    mGridVisible = visible;
}

void OdgDrawing::setGridColor(const QColor& color)
{
    mGridColor = color;
}

void OdgDrawing::setGridSpacingMajor(int spacing)
{
    if (spacing >= 0) mGridSpacingMajor = spacing;
}

void OdgDrawing::setGridSpacingMinor(int spacing)
{
    if (spacing >= 0) mGridSpacingMinor = spacing;
}

double OdgDrawing::grid() const
{
    return mGrid;
}

bool OdgDrawing::isGridVisible() const
{
    return mGridVisible;
}

QColor OdgDrawing::gridColor() const
{
    return mGridColor;
}

int OdgDrawing::gridSpacingMajor() const
{
    return mGridSpacingMajor;
}

int OdgDrawing::gridSpacingMinor() const
{
    return mGridSpacingMinor;
}

double OdgDrawing::roundToGrid(double value) const
{
    qreal result = value;

    if (mGrid > 0)
    {
        qreal mod = fmod(value, mGrid);
        result = value - mod;
        if (mod >= mGrid/2) result += mGrid;
        else if (mod <= -mGrid/2) result -= mGrid;
    }

    return result;
}

QPointF OdgDrawing::roundToGrid(const QPointF& position) const
{
    return QPointF(roundToGrid(position.x()), roundToGrid(position.y()));
}

//======================================================================================================================

void OdgDrawing::addStyle(OdgStyle* style)
{
    if (style) mStyles.append(style);
}

void OdgDrawing::insertStyle(int index, OdgStyle* style)
{
    if (style) mStyles.insert(index, style);
}

void OdgDrawing::removeStyle(OdgStyle* style)
{
    if (style) mStyles.removeAll(style);
}

void OdgDrawing::clearStyles()
{
    qDeleteAll(mStyles);
    mStyles.clear();
}

QList<OdgStyle*> OdgDrawing::styles() const
{
    return mStyles;
}

//======================================================================================================================

void OdgDrawing::addPage(OdgPage* page)
{
    if (page) mPages.append(page);
}

void OdgDrawing::insertPage(int index, OdgPage* page)
{
    if (page) mPages.insert(index, page);
}

void OdgDrawing::removePage(OdgPage* page)
{
    if (page) mPages.removeAll(page);
}

void OdgDrawing::clearPages()
{
    qDeleteAll(mPages);
    mStyles.clear();
}

QList<OdgPage*> OdgDrawing::pages() const
{
    return mPages;
}

//======================================================================================================================

void OdgDrawing::paint(QPainter* painter, bool isExport)
{

}

//======================================================================================================================

bool OdgDrawing::save(const QString& fileName)
{
    return true;
}

bool OdgDrawing::load(const QString& fileName)
{
    QFile odgFile(fileName);
    if (!odgFile.open(QFile::ReadOnly)) return false;

    QuaZip odgArchive(&odgFile);
    if (!odgArchive.open(QuaZip::mdUnzip)) return false;

    QuaZipFile xmlFile(&odgArchive);
    OdgReader xml;

    // Read document settings from settings.xml
    odgArchive.setCurrentFile("settings.xml");
    if (!xmlFile.open(QFile::ReadOnly)) return false;

    xml.setDevice(&xmlFile);
    if (!xml.readNextStartElement() || xml.qualifiedName() != QStringLiteral("office:document-settings")) return false;
    readDocumentSettings(&xml);
    xmlFile.close();

    // Read document styles from styles.xml
    odgArchive.setCurrentFile("styles.xml");
    if (!xmlFile.open(QFile::ReadOnly)) return false;

    xml.setDevice(&xmlFile);
    if (!xml.readNextStartElement() || xml.qualifiedName() != QStringLiteral("office:document-styles")) return false;
    readDocumentStyles(&xml);
    xmlFile.close();

    // Read document content from content.xml
    odgArchive.setCurrentFile("content.xml");
    if (!xmlFile.open(QFile::ReadOnly)) return false;

    xml.setDevice(&xmlFile);
    if (!xml.readNextStartElement() || xml.qualifiedName() != QStringLiteral("office:document-content")) return false;
    readDocumentContent(&xml);

    xmlFile.close();

    odgFile.close();

    return true;
}

//======================================================================================================================

void OdgDrawing::readDocumentSettings(OdgReader* xml)
{
    // No attributes for <office:document-settings> element

    // Read <office:document-settings> sub-elements
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("office:settings"))
            readSettings(xml);
        else
            xml->skipCurrentElement();
    }

    xml->skipCurrentElement();
}

void OdgDrawing::readSettings(OdgReader* xml)
{
    // No attributes for <office:settings> element

    // Read <office:settings> sub-elements
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("config:config-item-set"))
            readConfigItemSet(xml);
        else
            xml->skipCurrentElement();
    }
}

void OdgDrawing::readConfigItemSet(OdgReader* xml)
{
    // Read attributes of <config:config-item-set> element
    const QXmlStreamAttributes attributes = xml->attributes();
    if (attributes.hasAttribute("config:name") && attributes.value("config:name") == QStringLiteral("jade:settings"))
    {
        // Read <config:config-item-set> sub-elements
        while (xml->readNextStartElement())
        {
            if (xml->qualifiedName() == QStringLiteral("config:config-item"))
                readConfigItem(xml);
            else
                xml->skipCurrentElement();
        }
    }
    else
    {
        // Ignore the rest of <config:config-item-set> element
        xml->skipCurrentElement();
    }
}

void OdgDrawing::readConfigItem(OdgReader* xml)
{
    // Read attributes and text for <config:config-item> element
    const QXmlStreamAttributes attributes = xml->attributes();
    const QString text = xml->readElementText();
    if (attributes.hasAttribute("config:name") && attributes.hasAttribute("config:type"))
    {
        const QStringView name = attributes.value("config:name");
        const QStringView type = attributes.value("config:type");
        if (name == QStringLiteral("units") && type == QStringLiteral("string"))
        {
            setUnits(Odg::unitsFromString(text));
            xml->setUnits(mUnits);
        }
        else if (name == QStringLiteral("grid") && type == QStringLiteral("double"))
            setGrid(xml->lengthFromString(text));
        else if (name == QStringLiteral("gridVisible") && type == QStringLiteral("boolean"))
            setGridVisible(text.trimmed().toLower() == "true");
        else if (name == QStringLiteral("gridColor") && type == QStringLiteral("string"))
            setGridColor(xml->colorFromString(text));
        else if (name == QStringLiteral("gridSpacingMajor") && type == QStringLiteral("int"))
            setGridSpacingMajor(text.toInt());
        else if (name == QStringLiteral("gridSpacingMinor") && type == QStringLiteral("int"))
            setGridSpacingMinor(text.toInt());
    }

    // No sub-elements for <config:config-item>
}

//======================================================================================================================

void OdgDrawing::readDocumentStyles(OdgReader* xml)
{
    // No attributes for <office:document-styles> element

    // Read <office:document-styles> sub-elements
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("office:styles"))
            readStyles(xml);
        else if (xml->qualifiedName() == QStringLiteral("office:automatic-styles"))
            readAutomaticPageStyles(xml);
        else if (xml->qualifiedName() == QStringLiteral("office:master-styles"))
            readMasterPageStyles(xml);
        else
            xml->skipCurrentElement();
    }

    xml->skipCurrentElement();
}

void OdgDrawing::readStyles(OdgReader* xml)
{
    // No attributes for <office:styles> element

    // Read <office:styles> sub-elements
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("style:default-style"))
        {
            OdgStyle* defaultStyle = new OdgStyle(mUnits, true);
            defaultStyle->readFromXml(xml, mStyles);
            mStyles.prepend(defaultStyle);
        }
        else if (xml->qualifiedName() == QStringLiteral("style:style"))
        {
            OdgStyle* style = new OdgStyle(mUnits);
            style->readFromXml(xml, mStyles);
            mStyles.append(style);
        }
        else xml->skipCurrentElement();
    }
}

void OdgDrawing::readAutomaticPageStyles(OdgReader* xml)
{
    // No attributes for <office:automatic-styles> element

    // Read <office:automatic-styles> sub-elements
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("style:page-layout"))
            readPageLayout(xml);
        else if (xml->qualifiedName() == QStringLiteral("style:style"))
            readPageStyle(xml);
        else
            xml->skipCurrentElement();
    }
}

void OdgDrawing::readMasterPageStyles(OdgReader* xml)
{
    // No attributes for <office:master-styles> element

    // Todo: Read <office:master-styles> sub-elements
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("style:master-page"))
            readMasterPage(xml);
        else
            xml->skipCurrentElement();
    }
}

void OdgDrawing::readPageLayout(OdgReader* xml)
{
    // ASSUMPTION: Each ODG document contains one and only one style:page-layout element

    // Ignore attributes for <style:page-layout> element

    // Read sub-elements for <style:page-layout>
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("style:page-layout-properties"))
        {
            // Read attributes for <style:page-layout-properties> element
            const QXmlStreamAttributes attributes = xml->attributes();
            for(auto& attribute : attributes)
            {
                if (attribute.qualifiedName() == QStringLiteral("fo:page-width"))
                {
                    setPageSize(QSizeF(xml->lengthFromString(attribute.value()), mPageSize.height()));
                    xml->setPageSize(mPageSize);
                }
                else if (attribute.qualifiedName() == QStringLiteral("fo:page-height"))
                {
                    setPageSize(QSizeF(mPageSize.width(), xml->lengthFromString(attribute.value())));
                    xml->setPageSize(mPageSize);
                }
                else if (attribute.qualifiedName() == QStringLiteral("fo:margin-left"))
                {
                    setPageMargins(QMarginsF(xml->lengthFromString(attribute.value()), mPageMargins.top(),
                                             mPageMargins.right(), mPageMargins.bottom()));
                    xml->setPageMargins(mPageMargins);
                }
                else if (attribute.qualifiedName() == QStringLiteral("fo:margin-top"))
                {
                    setPageMargins(QMarginsF(mPageMargins.left(), xml->lengthFromString(attribute.value()),
                                             mPageMargins.right(), mPageMargins.bottom()));
                    xml->setPageMargins(mPageMargins);
                }
                else if (attribute.qualifiedName() == QStringLiteral("fo:margin-right"))
                {
                    setPageMargins(QMarginsF(mPageMargins.left(), mPageMargins.top(),
                                             xml->lengthFromString(attribute.value()), mPageMargins.bottom()));
                    xml->setPageMargins(mPageMargins);
                }
                else if (attribute.qualifiedName() == QStringLiteral("fo:margin-bottom"))
                {
                    setPageMargins(QMarginsF(mPageMargins.left(), mPageMargins.top(),
                                             mPageMargins.right(), xml->lengthFromString(attribute.value())));
                    xml->setPageMargins(mPageMargins);
                }
            }
        }

        xml->skipCurrentElement();
    }
}

void OdgDrawing::readPageStyle(OdgReader* xml)
{
    // ASSUMPTION: Each ODG document contains one and only one style:style element with style:family attribute set
    // to "drawing-page".

    // Ignore attributes for <style:style> element

    // Read sub-elements for <style:style>
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("style:drawing-page-properties"))
        {
            // Read attributes for <style:drawing-page-properties> element
            const QXmlStreamAttributes attributes = xml->attributes();
            for(auto& attribute : attributes)
            {
                if (attribute.qualifiedName() == QStringLiteral("draw:fill"))
                {
                    if (attribute.value() == QStringLiteral("solid"))
                        setBackgroundColor(QColor(0, 0, 0));
                    else
                        setBackgroundColor(QColor(0, 0, 0, 0));
                }
                else if (attribute.qualifiedName() == QStringLiteral("draw:fill-color"))
                {
                    setBackgroundColor(xml->colorFromString(attribute.value()));
                }
                else if (attribute.qualifiedName() == QStringLiteral("draw:opacity"))
                {
                    mBackgroundColor.setAlphaF(xml->percentFromString(attribute.value()));
                }
            }
        }

        xml->skipCurrentElement();
    }
}

void OdgDrawing::readMasterPage(OdgReader* xml)
{
    // ASSUMPTION: Each ODG document contains one and only one style:master-page element

    // Ignore attributes for <style:master-page> element

    // No sub-elements for <style:master-page>
    xml->skipCurrentElement();
}

//======================================================================================================================

#include <QDebug>

void OdgDrawing::readDocumentContent(OdgReader* xml)
{
    QList<OdgStyle*> contentStyles;

    // No attributes for <office:document-content> element

    // Read <office:document-content> sub-elements
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("office:automatic-styles"))
            readContentStyles(xml, contentStyles);
        else if (xml->qualifiedName() == QStringLiteral("office:body"))
            readBody(xml);
        else
            xml->skipCurrentElement();
    }

    xml->skipCurrentElement();

    qDeleteAll(contentStyles);
}

void OdgDrawing::readContentStyles(OdgReader* xml, QList<OdgStyle*>& contentStyles)
{
    // No attributes for <office:automatic-styles> element

    // Todo: Read <office:automatic-styles> sub-elements
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("style:style"))
        {
            OdgStyle* style = new OdgStyle(mUnits);
            style->readFromXml(xml, mStyles);
            contentStyles.append(style);
        }
        else xml->skipCurrentElement();
    }
}

void OdgDrawing::readBody(OdgReader* xml)
{
    // No attributes for <office:body> element

    // Read <office:body> sub-elements
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("office:drawing"))
            readDrawing(xml);
        else
            xml->skipCurrentElement();
    }
}

void OdgDrawing::readDrawing(OdgReader* xml)
{
    // No attributes for <office:drawing> element

    // Read <office:drawing> sub-elements
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("draw:page"))
            readPage(xml);
        else
            xml->skipCurrentElement();
    }
}

void OdgDrawing::readPage(OdgReader* xml)
{
    /*OdgPage* page = new OdgPage("Page " + QString::number(mPages.size()));

    // Read attributes for <office:page> element
    const QXmlStreamAttributes attributes = xml->attributes();
    if (attributes.hasAttribute("draw:name"))
        page->setName(attributes.value("draw:name").toString());

    // Read <office:page> sub-elements
    const QList<OdgItem*> items = readItems(xml);
    for(auto& item : items)
        page->addItem(item);

    addPage(page);*/

    xml->skipCurrentElement();
}

void OdgDrawing::readItems(OdgReader* xml)
{
    // Read <office:page> or <draw:g> sub-elements
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("draw:line"))
        {

        }
        else if (xml->qualifiedName() == QStringLiteral("draw:polyline"))
        {

        }
        else if (xml->qualifiedName() == QStringLiteral("draw:custom-shape"))
        {

        }
        else if (xml->qualifiedName() == QStringLiteral("draw:g"))
        {

        }
        else xml->skipCurrentElement();
    }
}
