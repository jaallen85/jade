/* AboutDialog.cpp
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

#include "AboutDialog.h"
#include "MainWindow.h"

const static QString sVersion = "1.1.0d0";
const static QString sDates = "2014-2016";

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent)
{
	QLabel* imageLabel = new QLabel();
	QLabel* nameLabel = new QLabel("Jade " + sVersion);
	QTabWidget* tabWidget = new QTabWidget();
	QPushButton* okButton = new QPushButton(tr("OK"));
	QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);

	imageLabel->setPixmap(QPixmap(":/icons/jade/diagram.png"));
	tabWidget->addTab(createAboutTab(), "About");
	//tabWidget->addTab(createLicenseTab(), "License Agreement");

	QFont font = nameLabel->font();
	font.setBold(true);
	nameLabel->setFont(font);

	okButton->setDefault(true);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	buttonBox->setCenterButtons(true);
	buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);

	QGridLayout* gLayout = new QGridLayout();
	gLayout->addWidget(imageLabel, 0, 0);
	gLayout->addWidget(nameLabel, 0, 1);
	gLayout->addWidget(tabWidget, 1, 0, 1, 2);
	gLayout->addWidget(buttonBox, 2, 0, 1, 2);
	gLayout->setColumnStretch(1, 100);
	gLayout->setRowStretch(1, 100);
	gLayout->setSpacing(8);
	setLayout(gLayout);

	setWindowTitle("About Jade");
	resize(500, 300);
}

AboutDialog::~AboutDialog() { }

//==================================================================================================

QFrame* AboutDialog::createAboutTab()
{
	QFont font;
	QFrame* aboutFrame = new QFrame();
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(new QWidget(), 100);

	QLabel* label = new QLabel("Jade");
	font = label->font();
	font.setBold(true);
	label->setFont(font);
	label->setAlignment(Qt::AlignCenter);
	vLayout->addWidget(label);

	label = new QLabel("written by Jason Allen");
	label->setAlignment(Qt::AlignCenter);
	vLayout->addWidget(label);

	label = new QLabel("Copyright (c) " + sDates);
	label->setAlignment(Qt::AlignCenter);
	vLayout->addWidget(label);

	vLayout->addWidget(new QWidget(), 100);
	aboutFrame->setLayout(vLayout);

	return aboutFrame;
}

QFrame* AboutDialog::createLicenseTab()
{
	QFrame* licenseFrame = new QFrame();

	QTextEdit* textEdit = new QTextEdit();
	QFile gplFile(":/gpl-3.0.txt");
	if (gplFile.open(QFile::ReadOnly | QFile::Text))
	{
		QTextStream gplStream(&gplFile);

		QApplication::setOverrideCursor(Qt::WaitCursor);
		textEdit->setPlainText(gplStream.readAll());
		QApplication::restoreOverrideCursor();

		gplFile.close();
	}

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(textEdit);
	licenseFrame->setLayout(vLayout);

	return licenseFrame;
}
