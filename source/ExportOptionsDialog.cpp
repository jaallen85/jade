/* ExportOptionsDialog.cpp
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

#include "ExportOptionsDialog.h"

ExportOptionsDialog::ExportOptionsDialog(const QRectF& sceneRect, const QSize& exportSize,
	bool maintainAspectRatio, qreal defaultScale, QWidget* parent) : QDialog(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(createSizeGroup());
	mainLayout->addWidget(new QWidget());
	mainLayout->addWidget(createButtonBox());
	setLayout(mainLayout);

	setWindowTitle("Export Options");
	resize(240, 10);

	mSceneRect = sceneRect;

	mMaintainAspectRatioCheck->setChecked(maintainAspectRatio);

	if (exportSize.isValid())
	{
		if (maintainAspectRatio)
		{
			if (exportSize.height() > 0)
			{
				mHeightEdit->setText(QString::number(exportSize.height()));
				updateWidth();
			}
			else if (exportSize.height() > 0)
			{
				mWidthEdit->setText(QString::number(exportSize.width()));
				updateHeight();
			}
			else
			{
				mWidthEdit->setText(QString::number(exportSize.width()));
				mHeightEdit->setText(QString::number(exportSize.height()));
			}
		}
		else
		{
			mWidthEdit->setText(QString::number(exportSize.width()));
			mHeightEdit->setText(QString::number(exportSize.height()));
		}
	}
	else
	{
		mWidthEdit->setText(QString::number((int)(mSceneRect.width() * defaultScale)));
		mHeightEdit->setText(QString::number((int)(mSceneRect.height() * defaultScale)));
	}
}

ExportOptionsDialog::~ExportOptionsDialog() { }

//==================================================================================================

QSize ExportOptionsDialog::exportSize() const
{
	return QSize(mWidthEdit->text().toInt(), mHeightEdit->text().toInt());
}

bool ExportOptionsDialog::maintainAspectRatio() const
{
	return mMaintainAspectRatioCheck->isChecked();
}

//==================================================================================================

void ExportOptionsDialog::updateWidth()
{
	if (mMaintainAspectRatioCheck->isChecked())
	{
		mWidthEdit->setText(QString::number(qRound(mHeightEdit->text().toInt() *
			mSceneRect.width() / mSceneRect.height())));
	}
}

void ExportOptionsDialog::updateHeight()
{
	if (mMaintainAspectRatioCheck->isChecked())
	{
		mHeightEdit->setText(QString::number(qRound(mWidthEdit->text().toInt() *
			mSceneRect.height() / mSceneRect.width())));
	}
}

//==================================================================================================

QGroupBox* ExportOptionsDialog::createSizeGroup()
{
	QGroupBox* sizeGroup = new QGroupBox("Size");

	mWidthEdit = new QLineEdit();
	mHeightEdit = new QLineEdit();
	mWidthEdit->setValidator(new QIntValidator(0, 1E9));
	mHeightEdit->setValidator(new QIntValidator(0, 1E9));
	connect(mWidthEdit, SIGNAL(textEdited(const QString&)), this, SLOT(updateHeight()));
	connect(mHeightEdit, SIGNAL(textEdited(const QString&)), this, SLOT(updateWidth()));

	mMaintainAspectRatioCheck = new QCheckBox("Maintain Aspect Ratio");
	mMaintainAspectRatioCheck->setChecked(true);

	QWidget* widthHeightWidget = new QWidget();
	QFormLayout* widthHeightLayout = new QFormLayout();
	widthHeightLayout->addRow("Width: ", mWidthEdit);
	widthHeightLayout->addRow("Height: ", mHeightEdit);
	widthHeightLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	widthHeightLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	widthHeightLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	widthHeightLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(100);
	widthHeightLayout->setContentsMargins(0, 0, 0, 0);
	widthHeightWidget->setLayout(widthHeightLayout);

	QWidget* maintainWidget = new QWidget();
	QVBoxLayout* maintainLayout = new QVBoxLayout();
	maintainLayout->addWidget(mMaintainAspectRatioCheck);
	maintainLayout->setContentsMargins(0, 0, 0, 0);
	maintainWidget->setLayout(maintainLayout);

	QVBoxLayout* sizeLayout = new QVBoxLayout();
	sizeLayout->addWidget(widthHeightWidget);
	sizeLayout->addWidget(maintainWidget);
	sizeLayout->setSpacing(8);
	sizeGroup->setLayout(sizeLayout);

	return sizeGroup;
}

QDialogButtonBox* ExportOptionsDialog::createButtonBox()
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
