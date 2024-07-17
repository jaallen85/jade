// File: AboutDialog.cpp
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

#include "AboutDialog.h"
#include "version.h"
#include <QApplication>
#include <QDialogButtonBox>
#include <QFile>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent)
{
    QLabel* imageLabel = new QLabel();
    imageLabel->setPixmap(QPixmap(":/icons/jade.png"));

    QLabel* nameLabel = new QLabel("Jade " + QString(PROJECT_VERSION));
    QFont nameLabelFont = nameLabel->font();
    nameLabelFont.setBold(true);
    nameLabel->setFont(nameLabelFont);

    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(createAboutTab(), "About");
    tabWidget->addTab(createLicenseTab(), "License");

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->setCenterButtons(true);
    QPushButton* okButton = buttonBox->addButton(QDialogButtonBox::Ok);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    QGridLayout* gLayout = new QGridLayout();
    gLayout->addWidget(imageLabel, 0, 0);
    gLayout->addWidget(nameLabel, 0, 1);
    gLayout->addWidget(tabWidget, 1, 0, 1, 2);
    gLayout->addWidget(buttonBox, 2, 0, 1, 2);
    gLayout->setColumnStretch(1, 100);
    gLayout->setRowStretch(1, 100);
    gLayout->setSpacing(8);
    setLayout(gLayout);

    setWindowTitle("About");
    resize(500, 300);
}

//==================================================================================================

QFrame* AboutDialog::createAboutTab()
{
    QLabel* jadeLabel = new QLabel("Jade");
    QFont jadeLabelFont = jadeLabel->font();
    jadeLabelFont.setBold(true);
    jadeLabel->setFont(jadeLabelFont);
    jadeLabel->setAlignment(Qt::AlignCenter);

    QLabel* authorLabel = new QLabel("written by Jason Allen");
    authorLabel->setAlignment(Qt::AlignCenter);

    QLabel* copyrightLabel = new QLabel("Copyright (c) 2014-present");
    copyrightLabel->setAlignment(Qt::AlignCenter);

    QFrame* aboutFrame = new QFrame();
    QVBoxLayout* aboutLayout = new QVBoxLayout();
    aboutLayout->addWidget(new QWidget(), 100);
    aboutLayout->addWidget(jadeLabel);
    aboutLayout->addWidget(authorLabel);
    aboutLayout->addWidget(copyrightLabel);
    aboutLayout->addWidget(new QWidget(), 100);
    aboutFrame->setLayout(aboutLayout);

    return aboutFrame;
}

QFrame* AboutDialog::createLicenseTab()
{
    QTextEdit* textEdit = new QTextEdit();

    QFile gplFile(":/LICENSE");
    if (gplFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream gplStream(&gplFile);

        QApplication::setOverrideCursor(Qt::WaitCursor);
        textEdit->setPlainText(gplStream.readAll());
        QApplication::restoreOverrideCursor();

        gplFile.close();
    }

    QFrame* licenseFrame = new QFrame();
    QVBoxLayout* licenseLayout = new QVBoxLayout();
    licenseLayout->addWidget(textEdit);
    licenseFrame->setLayout(licenseLayout);

    return licenseFrame;
}
