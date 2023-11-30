// File: OdgTextEllipseItem.h
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

#ifndef ODGTEXTELLIPSEITEM_H
#define ODGTEXTELLIPSEITEM_H

#include "OdgEllipseItem.h"
#include <QFont>

class OdgTextEllipseItem : public OdgEllipseItem
{
private:
    QString mCaption;

    QFont mFont;
    Qt::Alignment mTextAlignment;
    QSizeF mTextPadding;
    QBrush mTextBrush;

    QRectF mTextRect;

public:
    OdgTextEllipseItem();

    void setCaption(const QString& caption);
    QString caption() const;

    void setFont(const QFont& font);
    void setTextAlignment(Qt::Alignment alignment);
    void setTextPadding(const QSizeF& padding);
    void setTextBrush(const QBrush& brush);
    QFont font() const;
    Qt::Alignment textAlignment() const;
    QSizeF textPadding() const;
    QBrush textBrush() const;

    bool isValid() const override;

    void paint(QPainter& painter) override;

    void scaleBy(double scale) override;

private:
    QPointF calculateAnchorPoint(Qt::Alignment alignment) const;
};

#endif
