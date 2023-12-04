// File: OdgPage.h
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

#ifndef ODGPAGE_H
#define ODGPAGE_H

#include <QList>
#include <QString>
#include <QVariant>

class OdgItem;

class OdgPage
{
private:
    QString mName;

    QList<OdgItem*> mItems;

public:
    OdgPage(const QString& name = QString());
    ~OdgPage();

    void setName(const QString& name);
    QString name() const;

    void addItem(OdgItem* item);
    void insertItem(int index, OdgItem* item);
    void removeItem(OdgItem* item);
    void clearItems();
    QList<OdgItem*> items() const;

    void setProperty(const QString& name, const QVariant& value);
    QVariant property(const QString& name) const;
};

#endif
