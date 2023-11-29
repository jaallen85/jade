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
#include "OdgReader.h"
#include "OdgStyle.h"
#include <QFile>
#include <quazip.h>
#include <quazipfile.h>

OdgDrawing::OdgDrawing() : mUnits(Odg::UnitsInches),
    mGrid(0.05), mGridVisible(true), mGridColor(77, 153, 153), mGridSpacingMajor(8), mGridSpacingMinor(2), mStyles()
{

}

OdgDrawing::~OdgDrawing()
{
    qDeleteAll(mStyles);
}

//======================================================================================================================

void OdgDrawing::setUnits(Odg::Units units)
{
    if (mUnits != units)
    {
        double scaleFactor = Odg::convertUnits(1, mUnits, units);

        mUnits = units;

        mGrid *= scaleFactor;

        //for(auto& page : qAsConst(mPages))
        //    page->scale(scaleFactor);
    }
}

Odg::Units OdgDrawing::units() const
{
    return mUnits;
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

//======================================================================================================================

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
            setGrid(text.toDouble());
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

    // Todo: Read <office:automatic-styles> sub-elements
    xml->skipCurrentElement();
}

void OdgDrawing::readMasterPageStyles(OdgReader* xml)
{
    // No attributes for <office:master-styles> element

    // Todo: Read <office:master-styles> sub-elements
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
    // No attributes for <office:page> element

    // Read <office:page> sub-elements
    while (xml->readNextStartElement())
    {
        qDebug() << xml->qualifiedName();
        xml->skipCurrentElement();
    }
}
