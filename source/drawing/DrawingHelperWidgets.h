// DrawingHelperWidgets.h
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

#ifndef DRAWINGHELPERWIDGETS_H
#define DRAWINGHELPERWIDGETS_H

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

class DrawingHideableCheckBox : public QCheckBox
{
	Q_OBJECT

private:
	bool mHideCheck;

public:
	DrawingHideableCheckBox(bool checkHidden, const QString& text, QWidget* parent = nullptr);
	DrawingHideableCheckBox(const QString& text, QWidget* parent = nullptr);
	DrawingHideableCheckBox(QWidget* parent = nullptr);
	~DrawingHideableCheckBox();

	void setCheckHidden(bool hidden);
	bool isCheckHidden() const;

private:
	void paintEvent(QPaintEvent* event);

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
};

//==================================================================================================

class DrawingPositionEdit;
class DrawingSizeEdit;

class DrawingPositionWidget : public QWidget
{
	Q_OBJECT

private:
	DrawingPositionEdit* mXEdit;
	DrawingPositionEdit* mYEdit;

public:
	DrawingPositionWidget(const QPointF& position = QPointF());
	~DrawingPositionWidget();

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

class DrawingPositionEdit : public QLineEdit
{
	Q_OBJECT

private:
	qreal mPositionCached;

public:
	DrawingPositionEdit(qreal position = 0);
	~DrawingPositionEdit();

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

class DrawingSizeWidget : public QWidget
{
	Q_OBJECT

private:
	DrawingSizeEdit* mXEdit;
	DrawingSizeEdit* mYEdit;

public:
	DrawingSizeWidget(const QSizeF& size = QSizeF());
	~DrawingSizeWidget();

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

class DrawingSizeEdit : public QLineEdit
{
	Q_OBJECT

private:
	qreal mSizeCached;

public:
	DrawingSizeEdit(qreal size = 0);
	~DrawingSizeEdit();

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

class DrawingColorWidget : public QPushButton
{
	Q_OBJECT

private:
	QColor mColor;

public:
	DrawingColorWidget(const QColor& color = QColor(0, 0, 0));
	~DrawingColorWidget();

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

class DrawingColorSelectWidget : public QWidget
{
	Q_OBJECT

private:
	QColor mHoverColor;
	QRect mHoverRect;

public:
	DrawingColorSelectWidget();
	~DrawingColorSelectWidget();

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
