/* DiagramUndo.h
 *
 * Copyright (C) 2013-2017 Jason Allen
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

#ifndef DIAGRAMUNDO_H
#define DIAGRAMUNDO_H

#include <DiagramWidget.h>

enum DiagramUndoCommandIndex { DiagramSetItemsStyleCommandId = 1000,
	DiagramSetItemCornerRadiusCommandId, DiagramSetItemCaptionCommandId,
	DiagramSetPropertiesCommandId };

class DiagramSetItemsStyleCommand : public QUndoCommand
{
private:
	DiagramWidget* mDiagram;
	QHash< DrawingItem*, QHash<DrawingItemStyle::Property,QVariant> > mProperties;
	QHash< DrawingItem*, QHash<DrawingItemStyle::Property,QVariant> > mOriginalProperties;

public:
	DiagramSetItemsStyleCommand(DiagramWidget* diagram, const QList<DrawingItem*>& items,
		const QHash<DrawingItemStyle::Property,QVariant>& properties);
	~DiagramSetItemsStyleCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DiagramSetItemCornerRadiusCommand : public QUndoCommand
{
private:
	DiagramWidget* mDiagram;
	DrawingItem* mItem;
	qreal mCornerRadiusX, mCornerRadiusY;
	qreal mOriginalCornerRadiusX, mOriginalCornerRadiusY;

public:
	DiagramSetItemCornerRadiusCommand(DiagramWidget* diagram, DrawingItem* item,
		qreal radiusX, qreal radiusY);
	~DiagramSetItemCornerRadiusCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DiagramSetItemCaptionCommand : public QUndoCommand
{
private:
	DiagramWidget* mDiagram;
	DrawingItem* mItem;
	QString mCaption;
	QString mOriginalCaption;

public:
	DiagramSetItemCaptionCommand(DiagramWidget* diagram, DrawingItem* item,
		const QString& caption);
	~DiagramSetItemCaptionCommand();

	int id() const;

	void redo();
	void undo();

	bool mergeWith(const QUndoCommand* command);
};

//==================================================================================================

class DiagramSetPropertiesCommand : public QUndoCommand
{
private:
	DiagramWidget* mDiagram;
	QHash<DiagramWidget::Property,QVariant> mProperties;
	QHash<DiagramWidget::Property,QVariant> mOriginalProperties;

public:
	DiagramSetPropertiesCommand(DiagramWidget* diagram, const QHash<DiagramWidget::Property,QVariant>& properties);
	~DiagramSetPropertiesCommand();

	int id() const;

	void redo();
	void undo();
};

#endif
