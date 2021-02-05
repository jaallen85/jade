// DrawingTextItem.h
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

#ifndef DRAWINGTEXTITEM_H
#define DRAWINGTEXTITEM_H

#include <DrawingItem.h>

class DrawingTextItem : public DrawingItem
{
private:
	QString mCaption;
	QPen mPen;
	QFont mFont;
	Qt::Alignment mAlignment;

	QRectF mTextRect;
	QPainterPath mTextShape;

public:
	DrawingTextItem();
	DrawingTextItem(const DrawingTextItem& item);
	virtual ~DrawingTextItem();

	virtual QString uniqueKey() const override;
	virtual DrawingItem* copy() const override;

	void setCaption(const QString& caption);
	QString caption() const;

	void setTextBrush(const QBrush& brush);
	QBrush textBrush() const;

	void setFont(const QFont& font);
	QFont font() const;

	void setAlignment(Qt::Alignment alignment);
	Qt::Alignment alignment() const;

	virtual void setProperties(const QHash<QString,QVariant>& properties) override;
	virtual QHash<QString,QVariant> properties() const override;

	virtual QRectF boundingRect() const override;
	virtual QPainterPath shape() const override;
	virtual QPointF centerPosition() const override;
	virtual bool isValid() const override;

	virtual void render(QPainter* painter) override;

	virtual void writeToXml(QXmlStreamWriter* xml) override;
	virtual void readFromXml(QXmlStreamReader* xml) override;

protected:
	void updateTextRect(const QFont& font);
};

#endif
