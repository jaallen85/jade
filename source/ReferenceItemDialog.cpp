// ReferenceItemDialog.h
// Copyright (C) 2021  Jason Allen
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

#include "ReferenceItemDialog.h"
#include <DrawingItem.h>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

ReferenceItemDialog::ReferenceItemDialog(const QList<DrawingItem*>& referenceItems,
	QWidget* parent) : QDialog(parent)
{
	mReferenceItems = referenceItems;

	QVBoxLayout* dialogLayout = new QVBoxLayout();
	dialogLayout->addWidget(createContentWidget(), 100);
	dialogLayout->addWidget(createButtonBox());
	setLayout(dialogLayout);

	setWindowTitle("Reference Items");
	resize(600, 500);
}

ReferenceItemDialog::~ReferenceItemDialog()
{
	mReferenceItems.clear();
}

//==================================================================================================

void ReferenceItemDialog::setReferenceName(const QString& name)
{
	QList<QListWidgetItem*> matchingItems = mListWidget->findItems(name, Qt::MatchExactly);
	if (!matchingItems.isEmpty()) mListWidget->setCurrentItem(matchingItems.first());
}

QString ReferenceItemDialog::referenceName() const
{
	QString name;
	if (mListWidget->currentItem()) name = mListWidget->currentItem()->text();
	return name;
}

//==================================================================================================

void ReferenceItemDialog::updatePreview(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (current)
	{
		DrawingItem* item = nullptr;

		for(auto itemIter = mReferenceItems.begin();
			item == nullptr && itemIter != mReferenceItems.end(); itemIter++)
		{
			if ((*itemIter)->name() == current->text())
			{
				item = *itemIter;
				mPreviewWidget->setReferenceItem(item);
			}
		}
	}

	Q_UNUSED(previous);
}

//==================================================================================================

QWidget* ReferenceItemDialog::createContentWidget()
{
	mListWidget = new QListWidget();
	mListWidget->setMaximumWidth(200);

	mPreviewWidget = new ReferenceItemPreviewWidget();

	if (!mReferenceItems.isEmpty())
	{
		QListWidgetItem* newItem;

		for(auto itemIter = mReferenceItems.begin(); itemIter != mReferenceItems.end(); itemIter++)
		{
			newItem = new QListWidgetItem();
			newItem->setText((*itemIter)->name());
			newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			mListWidget->addItem(newItem);
		}
	}
	else
	{
		QListWidgetItem* newItem = new QListWidgetItem();
		newItem->setText("<No items found>");
		newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		mListWidget->addItem(newItem);
	}

	connect(mListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
		this, SLOT(updatePreview(QListWidgetItem*, QListWidgetItem*)));

	QWidget* contentWidget = new QWidget();
	QHBoxLayout* contentLayout = new QHBoxLayout();
	contentLayout->addWidget(mListWidget);
	contentLayout->addWidget(mPreviewWidget, 100);
	contentLayout->setContentsMargins(0, 0, 0, 0);
	contentWidget->setLayout(contentLayout);

	return contentWidget;
}

QDialogButtonBox* ReferenceItemDialog::createButtonBox()
{
	QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
	buttonBox->setCenterButtons(true);

	QPushButton* okButton = buttonBox->addButton("OK", QDialogButtonBox::AcceptRole);
	QPushButton* cancelButton = buttonBox->addButton("Cancel", QDialogButtonBox::RejectRole);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	okButton->setMinimumSize(72, 28);
	cancelButton->setMinimumSize(72, 28);

	return buttonBox;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

ReferenceItemPreviewWidget::ReferenceItemPreviewWidget(QWidget* parent) : QWidget(parent)
{
	mItem = nullptr;
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

ReferenceItemPreviewWidget::~ReferenceItemPreviewWidget()
{
	mItem = nullptr;
}

//==================================================================================================

QSize ReferenceItemPreviewWidget::sizeHint() const
{
	return QSize(100, 100);
}

//==================================================================================================

void ReferenceItemPreviewWidget::setReferenceItem(DrawingItem* item)
{
	mItem = item;
	update();
}

//==================================================================================================

void ReferenceItemPreviewWidget::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);

	if (mItem)
	{
		QImage image(width(), height(), QImage::Format_RGB32);
		image.fill(palette().brush(QPalette::Window).color());

		// Render scene
		QPainter painter(&image);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

		QRectF viewRect = mItem->shape().boundingRect();
		viewRect.adjust(-4, -4, 4, 4);

		qreal scale = qMin(width() / viewRect.width(), height() / viewRect.height());
		painter.translate(-viewRect.left() * scale, -viewRect.top() * scale);
		if (height() > viewRect.height() * scale) painter.translate(0, (height() - viewRect.height() * scale) / 2);
		if (width() > viewRect.width() * scale) painter.translate((width() - viewRect.width() * scale) / 2, 0);
		painter.scale(scale, scale);
		mItem->render(&painter);

		painter.end();

		// Render scene image on to widget
		QPainter widgetPainter(this);
		widgetPainter.drawImage(0, 0, image);

		Q_UNUSED(event);
	}
}
