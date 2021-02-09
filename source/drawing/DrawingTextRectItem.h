// DrawingCurveItem.h
// Copyright (C) 2020  Jason Allen
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

#ifndef DRAWINGTEXTRECTITEM_H
#define DRAWINGTEXTRECTITEM_H

#include <DrawingRectItem.h>

class DrawingTextRectItem : public DrawingRectItem
{
private:
	QString mCaption;
	QPen mTextPen;
	QFont mFont;

	QRectF mBoundingRect;
	QPainterPath mShape;
	QRectF mRectBoundingRect;
	QPainterPath mRectShape;
	QRectF mTextBoundingRect;

public:
	DrawingTextRectItem();
	DrawingTextRectItem(const DrawingTextRectItem& item);
	virtual ~DrawingTextRectItem();

	virtual QString uniqueKey() const override;
	virtual DrawingItem* copy() const override;

	void setCaption(const QString& caption);
	QString caption() const;

	void setTextBrush(const QBrush& brush);
	QBrush textBrush() const;

	void setFont(const QFont& font);
	QFont font() const;

	virtual void setProperties(const QHash<QString,QVariant>& properties) override;
	virtual QHash<QString,QVariant> properties() const override;

	virtual QRectF boundingRect() const override;
	virtual QPainterPath shape() const override;
	virtual bool isValid() const override;

	virtual void render(QPainter* painter) override;

	virtual void writeToXml(QXmlStreamWriter* xml) override;
	virtual void readFromXml(QXmlStreamReader* xml) override;

private:
	virtual void updateItemGeometry() override;
	virtual void updateTextRect(const QFont& font);
};

#endif
