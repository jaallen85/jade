// HelperWidgets.h
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

#ifndef HELPERWIDGETS_H
#define HELPERWIDGETS_H

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

class HideableCheckBox : public QCheckBox
{
	Q_OBJECT

private:
	bool mHideCheck;

public:
	HideableCheckBox(bool checkHidden, const QString& text, QWidget* parent = nullptr);
	HideableCheckBox(const QString& text, QWidget* parent = nullptr);
	HideableCheckBox(QWidget* parent = nullptr);
	~HideableCheckBox();

	void setCheckHidden(bool hidden);
	bool isCheckHidden() const;

private:
	void paintEvent(QPaintEvent* event);

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
};

//==================================================================================================

class PositionEdit;
class SizeEdit;

class PositionWidget : public QWidget
{
	Q_OBJECT

private:
	PositionEdit* mXEdit;
	PositionEdit* mYEdit;

public:
	PositionWidget(const QPointF& position = QPointF());
	~PositionWidget();

public slots:
	void setPosition(const QPointF& position);

public:
	QPointF position() const;

signals:
	void positionChanged(const QPointF& position);

private slots:
	void sendPositionChanged();
};

//==================================================================================================

class PositionEdit : public QLineEdit
{
	Q_OBJECT

private:
	qreal mPositionCached;

public:
	PositionEdit(qreal position = 0);
	~PositionEdit();

	QSize sizeHint() const;

public slots:
	void setPosition(qreal position);

public:
	qreal position() const;

signals:
	void positionChanged(qreal position);

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
	void sizeChanged(const QSizeF& size);

private slots:
	void sendSizeChanged();
};

//==================================================================================================

class SizeEdit : public QLineEdit
{
	Q_OBJECT

private:
	qreal mSizeCached;

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

	QColor colorAt(const QPoint& position, QRect& rect) const;
	int rectSize() const;
	int margin() const;

public:
	static QList<QColor> standardColors;
	static QList<QColor> customColors;

	static QPixmap createTransparentPixmap(int width, int height);
};

#endif
