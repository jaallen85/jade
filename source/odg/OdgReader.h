// File: OdgReader.h
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

#ifndef ODGREADER_H
#define ODGREADER_H

#include <QColor>
#include <QFile>
#include <QList>
#include <QMarginsF>
#include <QRectF>
#include "OdgStyle.h"

class QXmlStreamReader;
class OdgItem;
class OdgPage;

class OdgReader
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

    QList<OdgPage*> mPages;

    QFile mFile;
    QList<OdgStyle*> mStyles;

public:
    OdgReader(const QString& fileName);
    ~OdgReader();

    Odg::Units units() const;
    QSizeF pageSize() const;
    QMarginsF pageMargins() const;
    QColor backgroundColor() const;

    double grid() const;
    Odg::GridStyle gridStyle() const;
    QColor gridColor() const;
    int gridSpacingMajor() const;
    int gridSpacingMinor() const;

    QList<OdgPage*> takePages();

    bool open();
    void close();

    bool read();

private:
    void readDocumentSettings(QXmlStreamReader& xml);
    void readSettings(QXmlStreamReader& xml);
    void readConfigItemSet(QXmlStreamReader& xml);
    void readConfigItem(QXmlStreamReader& xml);

    void readDocumentStyles(QXmlStreamReader& xml);
    void readStyles(QXmlStreamReader& xml);
    void readAutomaticPageStyles(QXmlStreamReader& xml);
    void readMasterPageStyles(QXmlStreamReader& xml);
    void readPageLayout(QXmlStreamReader& xml);
    void readPageStyle(QXmlStreamReader& xml);
    void readMasterPage(QXmlStreamReader& xml);

    void readDocumentContent(QXmlStreamReader& xml);
    void readBody(QXmlStreamReader& xml);
    void readDrawing(QXmlStreamReader& xml);

    void readStyle(QXmlStreamReader& xml, OdgStyle* style);
    void readStyleGraphicProperties(QXmlStreamReader& xml, OdgStyle* style);
    void readStyleParagraphProperties(QXmlStreamReader& xml, OdgStyle* style);
    void readStyleTextProperties(QXmlStreamReader& xml, OdgStyle* style);
    OdgStyle* findStyle(const QStringView& name) const;

    void readPage(QXmlStreamReader& xml, OdgPage* page);
    QList<OdgItem*> readItems(QXmlStreamReader& xml);
    OdgItem* readLine(QXmlStreamReader& xml);
    OdgItem* readRect(QXmlStreamReader& xml);
    OdgItem* readEllipse(QXmlStreamReader& xml);
    OdgItem* readPolyline(QXmlStreamReader& xml);
    OdgItem* readPolygon(QXmlStreamReader& xml);
    OdgItem* readPath(QXmlStreamReader& xml);
    OdgItem* readCustomShape(QXmlStreamReader& xml);
    OdgItem* readGroup(QXmlStreamReader& xml);

    QString checkForCaption(QXmlStreamReader& xml);
    QString checkForCaptionAndEnhancedGeometry(QXmlStreamReader& xml, QString& enhancedGeometryType,
                                               QPainterPath& enhancedGeometryPath, QRectF& enhancedGeometryPathRect);

    double lengthFromString(const QStringView& str) const;
    double lengthFromString(const QString& str) const;
    double xCoordinateFromString(const QStringView& str) const;
    double xCoordinateFromString(const QString& str) const;
    double yCoordinateFromString(const QStringView& str) const;
    double yCoordinateFromString(const QString& str) const;
    double percentFromString(const QStringView& str) const;
    double percentFromString(const QString& str) const;
    QColor colorFromString(const QStringView& str) const;
    QColor colorFromString(const QString& str) const;

    void transformFromString(const QStringView& str, QPointF& position, bool& flipped, int& rotation) const;
    void transformFromString(const QString& str, QPointF& position, bool& flipped, int& rotation) const;
    QRectF viewBoxFromString(const QStringView& str) const;
    QRectF viewBoxFromString(const QString& str) const;
    QPolygonF pointsFromString(const QStringView& str) const;
    QPolygonF pointsFromString(const QString& str) const;
    QPainterPath pathFromString(const QStringView& str) const;
    QPainterPath pathFromString(const QString& str) const;
    QPainterPath pathFromEnhancedString(const QStringView& str) const;
    QPainterPath pathFromEnhancedString(const QString& str) const;
};

#endif
