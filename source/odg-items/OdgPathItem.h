// File: OdgPathItem.h
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

#ifndef ODGPATHITEM_H
#define ODGPATHITEM_H

#include "OdgRectItem.h"
#include <QPen>

class OdgPathItem : public OdgRectItem
{
private:
    QPainterPath mPath;
    QRectF mPathRect;

	QPainterPath mTransformedPath;

public:
    OdgPathItem();

	OdgItem* copy() const override;

	void setRect(const QRectF& rect) override;

	void setPath(const QPainterPath& path, const QRectF& pathRect);
    QPainterPath path() const;
	QRectF pathRect() const;

    QPainterPath shape() const override;
    bool isValid() const override;

    void paint(QPainter& painter) override;

	void placeCreateEvent(const QRectF& contentRect, double grid) override;

private:
	void updateTransformedPath();
};

#endif
