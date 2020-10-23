// DrawingUndo.h
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

#ifndef DRAWINGUNDO_H
#define DRAWINGUNDO_H

#include <QHash>
#include <QUndoCommand>
#include <QVariant>

class DrawingWidget;

class DrawingUndoCommand : public QUndoCommand
{
public:
	enum Type { SetDrawingPropertiesType, NumberOfCommands };

public:
	DrawingUndoCommand(const QString& title = QString(), QUndoCommand* parent = nullptr);
	DrawingUndoCommand(const DrawingUndoCommand& command, QUndoCommand* parent = nullptr);
	virtual ~DrawingUndoCommand();

protected:
	virtual void mergeChildren(const QUndoCommand* command);
};

//==================================================================================================

class DrawingSetPropertiesCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mDrawing;
	QHash<QString,QVariant> mProperties;
	QHash<QString,QVariant> mOriginalProperties;

public:
	DrawingSetPropertiesCommand(DrawingWidget* drawing, const QHash<QString,QVariant>& properties,
		QUndoCommand* parent = nullptr);
	~DrawingSetPropertiesCommand();

	int id() const;

	void redo();
	void undo();
};

#endif
