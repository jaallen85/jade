// DrawingReferenceItem.h
// Copyright (C) 2021  Jason Allen
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

#ifndef DRAWINGREFERENCEITEM_H
#define DRAWINGREFERENCEITEM_H

#include <DrawingItem.h>

class DrawingReferenceItem : public DrawingItem
{
private:
	QString mReferenceName;

	QRectF mBoundingRect;
	QPainterPath mShape;

public:
	DrawingReferenceItem();
	DrawingReferenceItem(const DrawingReferenceItem& item);
	virtual ~DrawingReferenceItem();

	virtual QString uniqueKey() const override;
	virtual DrawingItem* copy() const override;

	void setReferenceName(const QString& caption);
	QString referenceName() const;

	virtual void setProperties(const QHash<QString,QVariant>& properties) override;
	virtual QHash<QString,QVariant> properties() const override;

	virtual QRectF boundingRect() const override;
	virtual QPainterPath shape() const override;
	virtual QPointF centerPosition() const override;
	virtual bool isValid() const override;

	virtual void render(QPainter* painter) override;

	virtual void writeToXml(QXmlStreamWriter* xml) override;
	virtual void readFromXml(QXmlStreamReader* xml) override;
};

#endif
