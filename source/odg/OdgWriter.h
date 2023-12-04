// File: OdgWriter.h
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

#ifndef ODGWRITER_H
#define ODGWRITER_H

#include <QColor>
#include <QFile>
#include <QList>
#include <QMarginsF>
#include <QPainterPath>
#include <QSizeF>
#include "OdgGlobal.h"

class QXmlStreamWriter;
class OdgItem;
class OdgPage;
class OdgStyle;

class OdgWriter
{
private:
    Odg::Units mUnits;
    QSizeF mPageSize;
    QMarginsF mPageMargins;
    QColor mBackgroundColor;

    double mGrid;
    Odg::GridStyle mGridStyle;
    QColor mGridColor;
    int mGridSpacingMajor;
    int mGridSpacingMinor;

    OdgStyle* mDefaultStyle;
    QList<OdgPage*> mPages;

    QFile mFile;
    QStringList mContentFontFaces;

public:
    OdgWriter(const QString& fileName);
    ~OdgWriter();

    void setUnits(Odg::Units units);
    void setPageSize(const QSizeF& size);
    void setPageMargins(const QMarginsF& margins);
    void setBackgroundColor(const QColor& color);

    void setGrid(double grid);
    void setGridStyle(Odg::GridStyle style);
    void setGridColor(const QColor& color);
    void setGridSpacingMajor(int spacing);
    void setGridSpacingMinor(int spacing);

    void setDefaultStyle(OdgStyle* style);
    void setPages(const QList<OdgPage*>& pages);

    bool open();
    void close();

    bool write();

private:
    void writeManifest(QXmlStreamWriter& xml);
    void writeManifestFileEntry(QXmlStreamWriter& xml, const QString& fullPath, const QString& mediaType);

    void writeDocumentMeta(QXmlStreamWriter& xml);

    void writeDocumentSettings(QXmlStreamWriter& xml);
    void writeSettings(QXmlStreamWriter& xml);
    void writeConfigItemSet(QXmlStreamWriter& xml);
    void writeConfigItem(QXmlStreamWriter& xml, const QString& name, const QString& type, const QString& value);

    void writeDocumentStyles(QXmlStreamWriter& xml);
    void writeStyleFontFaces(QXmlStreamWriter& xml);
    void writeStyles(QXmlStreamWriter& xml);
    void writeAutomaticPageStyles(QXmlStreamWriter& xml);
    void writeMasterPageStyles(QXmlStreamWriter& xml);
    void writeFontFace(QXmlStreamWriter& xml, const QString fontFamily, bool style = false);
    void writePageLayout(QXmlStreamWriter& xml);
    void writePageLayoutProperties(QXmlStreamWriter& xml);
    void writePageStyle(QXmlStreamWriter& xml, const QString& name);
    void writeDrawingPageProperties(QXmlStreamWriter& xml);
    void writeMasterPage(QXmlStreamWriter& xml);

    void writeDocumentContent(QXmlStreamWriter& xml);
    void writeContentFontFaces(QXmlStreamWriter& xml);
    void writeContentAutomaticStyles(QXmlStreamWriter& xml);
    void writeBody(QXmlStreamWriter& xml);
    void writeDrawing(QXmlStreamWriter& xml);
    void writePage(QXmlStreamWriter& xml, OdgPage* page);

    void writeItemFontFaces(QXmlStreamWriter& xml, const QList<OdgItem*>& items);
    void writeItemStyles(QXmlStreamWriter& xml, const QList<OdgItem*>& items);
    void writeItems(QXmlStreamWriter& xml, const QList<OdgItem*>& items);

    QString lengthToString(double length) const;
    QString xCoordinateToString(double x) const;
    QString yCoordinateToString(double y) const;
    QString percentToString(double value) const;
    QString colorToString(const QColor& color) const;

    QString transformToString(const QPointF& position, bool flipped, int rotation) const;
    QString viewBoxToString(const QRectF& viewBox) const;
    QString pointsToString(const QPolygonF& points) const;
    QString pathToString(const QPainterPath& path) const;
};

#endif
