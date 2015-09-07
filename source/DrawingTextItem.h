/* DrawingTextItem.cpp
 *
 * Copyright (C) 2013-2015 Jason Allen
 *
 * This file is part of the libdrawing library.
 *
 * libdrawing is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdrawing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdrawing.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DRAWINGTEXTITEM_H
#define DRAWINGTEXTITEM_H

#include <DrawingItem.h>

class DrawingTextItem : public DrawingItem
{
public:
	DrawingTextItem();
	DrawingTextItem(const DrawingTextItem& item);
	virtual ~DrawingTextItem();

	virtual DrawingItem* copy() const;

	// Selectors
	void setCaption(const QString& caption);
	QString caption() const;

	void setTextColor(const QColor& color);
	QColor textColor() const;

	void setFont(const QFont& font);
	void setFontFamily(const QString& family);
	void setFontSize(qreal size);
	void setFontBold(bool bold);
	void setFontItalic(bool italic);
	void setFontUnderline(bool underline);
	void setFontOverline(bool overline);
	void setFontStrikeOut(bool strikeOut);
	QFont font() const;
	QString fontFamily() const;
	qreal fontSize() const;
	bool isFontBold() const;
	bool isFontItalic() const;
	bool isFontUnderline() const;
	bool isFontOverline() const;
	bool isFontStrikeOut() const;

	void setTextAlignmentHorizontal(Qt::Alignment alignment);
	void setTextAlignmentVertical(Qt::Alignment alignment);
	void setTextAlignment(Qt::Alignment alignment);
	Qt::Alignment textAlignmentHorizontal() const;
	Qt::Alignment textAlignmentVertical() const;
	Qt::Alignment textAlignment() const;

	// Description
	virtual QRectF boundingRect() const;
	virtual QPointF centerPos() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

private:
	QRectF buildTextLayout(QTextLayout& textLayout) const;
};

#endif
