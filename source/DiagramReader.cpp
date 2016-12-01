/* DiagramReader.cpp
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

#include "DiagramReader.h"

DiagramReader::DiagramReader(QIODevice* device) : QXmlStreamReader(device) { }

DiagramReader::DiagramReader(const QString& data) : QXmlStreamReader(data) { }

DiagramReader::~DiagramReader() { }

//==================================================================================================

void DiagramReader::read(DiagramWidget* drawing)
{

}

void DiagramReader::readItems(QList<DrawingItem*>& items)
{

}
