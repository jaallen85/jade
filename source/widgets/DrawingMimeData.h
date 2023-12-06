// File: DrawingMimeData.h
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

#ifndef DRAWINGMIMEDATA_H
#define DRAWINGMIMEDATA_H

#include <QMimeData>

class OdgItem;

class DrawingMimeData : public QMimeData
{
    Q_OBJECT

private:
    QList<OdgItem*> mItems;

public:
    DrawingMimeData();
    ~DrawingMimeData();

    void setItems(const QList<OdgItem*>& items);
    QList<OdgItem*> items() const;

    bool hasFormat(const QString& mimeType) const override;
    QStringList formats() const override;

private:
    QVariant retrieveData(const QString& mimeType, QMetaType type) const override;

public:
    static QString format();
};

#endif
