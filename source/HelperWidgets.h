/* HelperWidgets.h
 *
 * Copyright (C) 2013-2016 Jason Allen
 *
 * This file is part of the jade application.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HELPERWIDGETS_H
#define HELPERWIDGETS_H

#include <DiagramWidget.h>

class PositionEdit;
class SizeEdit;

class PositionWidget : public QWidget
{
	Q_OBJECT

private:
	PositionEdit* mXEdit;
	PositionEdit* mYEdit;

public:
	PositionWidget(const QPointF& pos = QPointF());
	~PositionWidget();

public slots:
	void setPos(const QPointF& pos);

public:
	QPointF pos() const;

signals:
	void positionChanged(const QPointF& pos);

private slots:
	void sendPositionChanged();
};

//==================================================================================================

class PositionEdit : public QLineEdit
{
	Q_OBJECT

public:
	PositionEdit(qreal pos = 0);
	~PositionEdit();

	QSize sizeHint() const;

public slots:
	void setPos(qreal pos);

public:
	qreal pos() const;

signals:
	void positionChanged(qreal pos);

private slots:
	void sendPositionChanged();
};

//==================================================================================================

class SizeWidget : public QWidget
{
	Q_OBJECT

private:
	SizeEdit* mXEdit;
	SizeEdit* mYEdit;

public:
	SizeWidget(const QSizeF& size = QSizeF());
	~SizeWidget();

public slots:
	void setSize(const QSizeF& size);

public:
	QSizeF size() const;

signals:
	void sizeChanged(const QSizeF& pos);

private slots:
	void sendSizeChanged();
};

//==================================================================================================

class SizeEdit : public QLineEdit
{
	Q_OBJECT

public:
	SizeEdit(qreal size = 0);
	~SizeEdit();

	QSize sizeHint() const;

public slots:
	void setSize(qreal size);

public:
	qreal size() const;

signals:
	void sizeChanged(qreal size);

private slots:
	void sendSizeChanged();
};

//==================================================================================================

class ColorWidget : public QPushButton
{
	Q_OBJECT

private:
	QColor mColor;

public:
	ColorWidget(const QColor& color = QColor(0, 0, 0));
	~ColorWidget();

	QColor color() const;

public slots:
	void setColor(const QColor& color);

signals:
	void colorChanged(const QColor& color);

private slots:
	void runColorDialog();

private:
	QString textFromColor(const QColor& color) const;
	QIcon iconFromColor(const QColor& color) const;
};

//==================================================================================================

class ColorSelectWidget : public QWidget
{
	Q_OBJECT

private:
	QColor mHoverColor;
	QRect mHoverRect;

public:
	ColorSelectWidget();
	~ColorSelectWidget();

	QSize sizeHint() const;

signals:
	void colorSelected(const QColor& color);

private:
	void paintEvent(QPaintEvent* event);

	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

	QColor colorAt(const QPoint& pos, QRect& colorRect) const;
	int rectSize() const;
	int margin() const;

public:
	static QList<QColor> standardColors;
	static QList<QColor> customColors;

	static QPixmap createTransparentPixmap(int width, int height);
};

//==================================================================================================

class ArrowStyleCombo : public QComboBox
{
	Q_OBJECT

public:
	ArrowStyleCombo(DrawingItemStyle::ArrowStyle style = DrawingItemStyle::ArrowNone);
	~ArrowStyleCombo();

public slots:
	void setStyle(DrawingItemStyle::ArrowStyle style);

public:
	DrawingItemStyle::ArrowStyle style() const;
};

//==================================================================================================

class PenStyleCombo : public QComboBox
{
	Q_OBJECT

public:
	PenStyleCombo(Qt::PenStyle style = Qt::SolidLine);
	~PenStyleCombo();

public slots:
	void setStyle(Qt::PenStyle style);

public:
	Qt::PenStyle style() const;
};

//==================================================================================================

class GridStyleCombo : public QComboBox
{
	Q_OBJECT

public:
	GridStyleCombo(DiagramWidget::GridRenderStyle style = DiagramWidget::GridDots);
	~GridStyleCombo();

public slots:
	void setStyle(DiagramWidget::GridRenderStyle style);

public:
	DiagramWidget::GridRenderStyle style() const;
};

//==================================================================================================

class FontStyleWidget : public QWidget
{
	Q_OBJECT

private:
	QToolButton* mBoldButton;
	QToolButton* mItalicButton;
	QToolButton* mUnderlineButton;
	QToolButton* mStrikeThroughButton;

public:
	FontStyleWidget();
	~FontStyleWidget();

public slots:
	void setBold(bool bold);
	void setItalic(bool italic);
	void setUnderline(bool underline);
	void setStrikeThrough(bool strikeThrough);

public:
	bool isBold() const;
	bool isItalic() const;
	bool isUnderline() const;
	bool isStrikeThrough() const;

signals:
	void boldChanged(bool bold) const;
	void italicChanged(bool bold) const;
	void underlineChanged(bool bold) const;
	void strikeThroughChanged(bool bold) const;
};

//==================================================================================================

class TextAlignmentWidget : public QWidget
{
	Q_OBJECT

private:
	QToolButton* mLeftAlignButton;
	QToolButton* mHCenterAlignButton;
	QToolButton* mRightAlignButton;

	QToolButton* mTopAlignButton;
	QToolButton* mVCenterAlignButton;
	QToolButton* mBottomAlignButton;

public:
	TextAlignmentWidget();
	~TextAlignmentWidget();

public slots:
	void setAlignment(Qt::Alignment horizontal, Qt::Alignment vertical);

public:
	Qt::Alignment horizontalAlignment() const;
	Qt::Alignment verticalAlignment() const;

signals:
	void horizontalAlignmentChanged(Qt::Alignment alignment) const;
	void verticalAlignmentChanged(Qt::Alignment alignment) const;

private slots:
	void handleHorizontalAlignmentChange();
	void handleVerticalAlignmentChange();
};

#endif
