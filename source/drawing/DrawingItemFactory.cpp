// DrawingItemFactory.cpp
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

#include "DrawingItemFactory.h"
#include "DrawingArrow.h"
#include "DrawingItemPoint.h"
#include "DrawingItemGroup.h"
#include <QtMath>

DrawingItemFactory::DrawingItemFactory() : QObject()
{
	qRegisterMetaType<DrawingArrow>();
	qRegisterMetaTypeStreamOperators<DrawingArrow>("DrawingArrow");

	mItems.append(new DrawingItemGroup());

	// initialize default properties
	QVariant startArrowVariant, endArrowVariant;
	startArrowVariant.setValue(DrawingArrow(Drawing::ArrowNone, 100.0));
	endArrowVariant.setValue(DrawingArrow(Drawing::ArrowNone, 100.0));

	mDefaultItemProperties.insert("pen", QPen(Qt::black, 12.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	mDefaultItemProperties.insert("brush", QBrush(Qt::white));
	mDefaultItemProperties.insert("startArrow", startArrowVariant);
	mDefaultItemProperties.insert("endArrow", endArrowVariant);
	mDefaultItemProperties.insert("textBrush", QBrush(Qt::black));
	mDefaultItemProperties.insert("font", QFont("Arial", 100.0, -1, false));
	mDefaultItemProperties.insert("alignment", (uint)Qt::AlignCenter);
}

DrawingItemFactory::~DrawingItemFactory()
{
	clearItems();
}

//==================================================================================================

bool DrawingItemFactory::registerItem(DrawingItem* item)
{
	bool itemRegistered = false;

	itemRegistered = (item && !containsItem(item->uniqueKey()));
	if (itemRegistered) mItems.append(item);

	return itemRegistered;
}

void DrawingItemFactory::clearItems()
{
	while (!mItems.isEmpty()) delete mItems.takeFirst();
}

bool DrawingItemFactory::containsItem(const QString& uniqueKey) const
{
	bool containsItem = false;

	for(auto itemIter = mItems.begin(); !containsItem && itemIter != mItems.end(); itemIter++)
		containsItem = ((*itemIter)->uniqueKey() == uniqueKey);

	return containsItem;
}

DrawingItem* DrawingItemFactory::createItem(const QString& uniqueKey) const
{
	DrawingItem* item = nullptr;

	for(auto itemIter = mItems.begin(); item == nullptr && itemIter != mItems.end(); itemIter++)
	{
		if ((*itemIter)->uniqueKey() == uniqueKey) item = (*itemIter)->copy();
	}

	if (item) item->setProperties(mDefaultItemProperties);

	return item;
}

//==================================================================================================

QString DrawingItemFactory::writeItemsToString(const QList<DrawingItem*>& items) const
{
	QString xmlString;

	QXmlStreamWriter xml(&xmlString);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(2);

	xml.writeStartDocument();

	xml.writeStartElement("items");
	writeItemsToXml(&xml, items);
	xml.writeEndElement();

	xml.writeEndDocument();

	return xmlString;
}

QList<DrawingItem*> DrawingItemFactory::readItemsFromString(const QString xmlString) const
{
	QList<DrawingItem*> items;

	QXmlStreamReader xml(xmlString);

	while (xml.readNextStartElement())
	{
		if (xml.name() == "items")
			items.append(readItemsFromXml(&xml));
		else
			xml.skipCurrentElement();
	}

	return items;
}

//==================================================================================================

void DrawingItemFactory::writeItemsToXml(QXmlStreamWriter* xml, const QList<DrawingItem*>& items) const
{
	if (xml)
	{
		for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		{
			xml->writeStartElement((*itemIter)->uniqueKey());
			(*itemIter)->writeToXml(xml);
			xml->writeEndElement();
		}
	}
}

QList<DrawingItem*> DrawingItemFactory::readItemsFromXml(QXmlStreamReader* xml) const
{
	QList<DrawingItem*> items;

	// Read items from XML
	if (xml)
	{
		DrawingItem* item;

		while (xml->readNextStartElement())
		{
			item = createItem(xml->name().toString());

			if (item)
			{
				item->readFromXml(xml);
				items.append(item);
			}
		}
	}

	// Connect items together
	QList<DrawingItemPoint*> itemPoints, otherItemPoints;
	qreal distance, threshold = 0.01;
	QPointF vec;
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		for(auto otherItemIter = itemIter + 1; otherItemIter != items.end(); otherItemIter++)
		{
			itemPoints = (*itemIter)->points();
			otherItemPoints = (*otherItemIter)->points();

			for(auto itemPointIter = itemPoints.begin();
				itemPointIter != itemPoints.end(); itemPointIter++)
			{
				for(auto otherItemPointIter = otherItemPoints.begin();
					otherItemPointIter != otherItemPoints.end(); otherItemPointIter++)
				{
					if ((*itemPointIter)->isConnectionPoint() &&
						(*otherItemPointIter)->isConnectionPoint() &&
						((*itemPointIter)->isFree() || (*otherItemPointIter)->isFree()))
					{
						vec = (*itemIter)->mapToScene((*itemPointIter)->position()) -
							(*otherItemIter)->mapToScene((*otherItemPointIter)->position());
						distance = qSqrt(vec.x() * vec.x() + vec.y() * vec.y());

						if (distance <= threshold)
						{
							(*itemPointIter)->addConnection(*otherItemPointIter);
							(*otherItemPointIter)->addConnection(*itemPointIter);
						}
					}
				}
			}
		}
	}

	return items;
}

//==================================================================================================

void DrawingItemFactory::setDefaultItemProperties(const QHash<QString,QVariant>& properties)
{
	mDefaultItemProperties = properties;
}

QHash<QString,QVariant> DrawingItemFactory::defaultItemProperties() const
{
	return mDefaultItemProperties;
}
