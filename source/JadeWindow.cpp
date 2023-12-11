// File: JadeWindow.cpp
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

#include "JadeWindow.h"
#include "DrawingWidget.h"
#include "OdgItem.h"
#include "OdgPage.h"
#include "PagesWidget.h"
#include "PropertiesWidget.h"
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QShowEvent>
#include <QStatusBar>
#include <QToolBar>

JadeWindow::JadeWindow() : QMainWindow(), mDrawingWidget(nullptr),
    mPagesWidget(nullptr), mPagesDock(nullptr), mPropertiesWidget(nullptr), mPropertiesDock(nullptr),
    mModeLabel(nullptr), mModifiedLabel(nullptr), mMouseInfoLabel(nullptr), mZoomCombo(nullptr),
    mFilePath(), mNewDrawingCount(0), mWorkingDir(), mPromptOverwrite(true), mPromptCloseUnsaved(true),
    mPagesDockVisibleOnClose(true), mPropertiesDockVisibleOnClose(true)
{
    mDrawingWidget = new DrawingWidget();
    setCentralWidget(mDrawingWidget);

    mPagesWidget = new PagesWidget(mDrawingWidget);
    mPagesDock = addDockWidget("Pages", mPagesWidget, Qt::LeftDockWidgetArea);

    mPropertiesWidget = new PropertiesWidget(mDrawingWidget);
    mPropertiesDock = addDockWidget("Properties", mPropertiesWidget, Qt::RightDockWidgetArea);

    mModeLabel = addStatusBarLabel("Select Mode", "Place Text Ellipse", mDrawingWidget, SIGNAL(modeTextChanged(QString)));
    mModifiedLabel = addStatusBarLabel("", "Modified", mDrawingWidget, SIGNAL(cleanTextChanged(QString)));
    mMouseInfoLabel = addStatusBarLabel("", "(XXXX.XX,XXXX.XX)", mDrawingWidget, SIGNAL(mouseInfoChanged(QString)));

    createActions();
    createMenus();
    createToolBars();

    setWindowTitle("Jade");
    setWindowIcon(QIcon(":/icons/jade.png"));
    resize(1762, 1000);

    loadSettings();
}

//======================================================================================================================

QDockWidget* JadeWindow::addDockWidget(const QString& title, QWidget* widget, Qt::DockWidgetArea area)
{
    QDockWidget* dockWidget = new QDockWidget(title);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setFeatures(QDockWidget::DockWidgetClosable);
    dockWidget->setWidget(widget);
    dockWidget->setContextMenuPolicy(Qt::PreventContextMenu);
    QMainWindow::addDockWidget(area, dockWidget);
    return dockWidget;
}

QLabel* JadeWindow::addStatusBarLabel(const QString& text, const QString& minimumWidthText,
                                      const QObject* signalObject, const char* signalFunction)
{
    QLabel* label = new QLabel(text);
    label->setMinimumWidth(QFontMetrics(label->font()).boundingRect(minimumWidthText).width() + 64);
    statusBar()->addWidget(label);
    connect(signalObject, signalFunction, label, SLOT(setText(QString)));
    return label;
}

//======================================================================================================================

void JadeWindow::createActions()
{
    addAction("New", this, SLOT(newDrawing()), ":/icons/document-new.png", "Ctrl+N");
    addAction("Open...", this, SLOT(openDrawing()), ":/icons/document-open.png", "Ctrl+O");
    addAction("Save", this, SLOT(saveDrawing()), ":/icons/document-save.png", "Ctrl+S");
    addAction("Save As...", this, SLOT(saveDrawingAs()), ":/icons/document-save-as.png", "Ctrl+Shift+S");
    addAction("Close", this, SLOT(closeDrawing()), ":/icons/document-close.png", "Ctrl+W");
    addAction("Export PNG...", this, SLOT(exportPng()), ":/icons/image-x-generic.png");
    addAction("Export SVG...", this, SLOT(exportSvg()), ":/icons/image-svg+xml.png");
    addAction("Preferences...", this, SLOT(preferences()), ":/icons/configure.png");
    addAction("Exit", this, SLOT(close()), ":/icons/application-exit.png");

    QAction* propertiesAction = addAction("Properties...", mPropertiesDock, SLOT(setVisible(bool)),
                                          ":/icons/view-form.png");
    propertiesAction->setCheckable(true);
    propertiesAction->setChecked(true);
    connect(mPropertiesDock, SIGNAL(visibilityChanged(bool)), propertiesAction, SLOT(setChecked(bool)));

    QAction* pagesAction = addAction("Pages...", mPagesDock, SLOT(setVisible(bool)), ":/icons/view-choose.png");
    pagesAction->setCheckable(true);
    pagesAction->setChecked(true);
    connect(mPagesDock, SIGNAL(visibilityChanged(bool)), pagesAction, SLOT(setChecked(bool)));

    addAction("About...", this, SLOT(about()), ":/icons/help-about.png");
    addAction("About Qt...", qApp, SLOT(aboutQt()));
}

void JadeWindow::createMenus()
{
    const QList<QAction*> windowActions = actions();
    const QList<QAction*> drawingActions = mDrawingWidget->actions();

    QMenu* fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(windowActions.at(JadeWindow::NewAction));
    fileMenu->addAction(windowActions.at(JadeWindow::OpenAction));
    fileMenu->addSeparator();
    fileMenu->addAction(windowActions.at(JadeWindow::SaveAction));
    fileMenu->addAction(windowActions.at(JadeWindow::SaveAsAction));
    fileMenu->addSeparator();
    fileMenu->addAction(windowActions.at(JadeWindow::CloseAction));
    fileMenu->addSeparator();
    fileMenu->addAction(windowActions.at(JadeWindow::ExportPngAction));
    fileMenu->addAction(windowActions.at(JadeWindow::ExportSvgAction));
    fileMenu->addSeparator();
    fileMenu->addAction(windowActions.at(JadeWindow::PreferencesAction));
    fileMenu->addSeparator();
    fileMenu->addAction(windowActions.at(JadeWindow::ExitAction));

    QMenu* editMenu = menuBar()->addMenu("Edit");
    editMenu->addAction(drawingActions.at(DrawingWidget::UndoAction));
    editMenu->addAction(drawingActions.at(DrawingWidget::RedoAction));
    editMenu->addSeparator();
    editMenu->addAction(drawingActions.at(DrawingWidget::CutAction));
    editMenu->addAction(drawingActions.at(DrawingWidget::CopyAction));
    editMenu->addAction(drawingActions.at(DrawingWidget::PasteAction));
    editMenu->addAction(drawingActions.at(DrawingWidget::DeleteAction));
    editMenu->addSeparator();
    editMenu->addAction(drawingActions.at(DrawingWidget::SelectAllAction));
    editMenu->addAction(drawingActions.at(DrawingWidget::SelectNoneAction));

    QMenu* placeMenu = menuBar()->addMenu("Place");
    placeMenu->addAction(drawingActions.at(DrawingWidget::SelectModeAction));
    placeMenu->addAction(drawingActions.at(DrawingWidget::ScrollModeAction));
    placeMenu->addAction(drawingActions.at(DrawingWidget::ZoomModeAction));
    placeMenu->addSeparator();
    placeMenu->addAction(drawingActions.at(DrawingWidget::PlaceLineAction));
    placeMenu->addAction(drawingActions.at(DrawingWidget::PlaceCurveAction));
    placeMenu->addAction(drawingActions.at(DrawingWidget::PlacePolylineAction));
    placeMenu->addAction(drawingActions.at(DrawingWidget::PlaceRectangleAction));
    placeMenu->addAction(drawingActions.at(DrawingWidget::PlaceEllipseAction));
    placeMenu->addAction(drawingActions.at(DrawingWidget::PlacePolygonAction));
    placeMenu->addAction(drawingActions.at(DrawingWidget::PlaceTextAction));
    placeMenu->addAction(drawingActions.at(DrawingWidget::PlaceTextRectangleAction));
    placeMenu->addAction(drawingActions.at(DrawingWidget::PlaceTextEllipseAction));

    QMenu* objectMenu = menuBar()->addMenu("Object");
    objectMenu->addAction(drawingActions.at(DrawingWidget::RotateAction));
    objectMenu->addAction(drawingActions.at(DrawingWidget::RotateBackAction));
    objectMenu->addAction(drawingActions.at(DrawingWidget::FlipHorizontalAction));
    objectMenu->addAction(drawingActions.at(DrawingWidget::FlipVerticalAction));
    objectMenu->addSeparator();
    objectMenu->addAction(drawingActions.at(DrawingWidget::BringForwardAction));
    objectMenu->addAction(drawingActions.at(DrawingWidget::SendBackwardAction));
    objectMenu->addAction(drawingActions.at(DrawingWidget::BringToFrontAction));
    objectMenu->addAction(drawingActions.at(DrawingWidget::SendToBackAction));
    objectMenu->addSeparator();
    objectMenu->addAction(drawingActions.at(DrawingWidget::GroupAction));
    objectMenu->addAction(drawingActions.at(DrawingWidget::UngroupAction));
    objectMenu->addSeparator();
    objectMenu->addAction(drawingActions.at(DrawingWidget::InsertPointAction));
    objectMenu->addAction(drawingActions.at(DrawingWidget::RemovePointAction));

    QMenu* drawingMenu = menuBar()->addMenu("Drawing");
    drawingMenu->addAction(windowActions.at(JadeWindow::ViewPropertiesAction));
    drawingMenu->addSeparator();
    drawingMenu->addAction(drawingActions.at(DrawingWidget::InsertPageAction));
    drawingMenu->addAction(drawingActions.at(DrawingWidget::DuplicatePageAction));
    drawingMenu->addAction(drawingActions.at(DrawingWidget::RemovePageAction));
    drawingMenu->addAction(windowActions.at(JadeWindow::ViewPagesAction));
    drawingMenu->addSeparator();
    drawingMenu->addAction(drawingActions.at(DrawingWidget::ZoomInAction));
    drawingMenu->addAction(drawingActions.at(DrawingWidget::ZoomOutAction));
    drawingMenu->addAction(drawingActions.at(DrawingWidget::ZoomFitAction));

    QMenu* aboutMenu = menuBar()->addMenu("About");
    aboutMenu->addAction(windowActions.at(JadeWindow::AboutAction));
    aboutMenu->addAction(windowActions.at(JadeWindow::AboutQtAction));

    menuBar()->setContextMenuPolicy(Qt::PreventContextMenu);
}

void JadeWindow::createToolBars()
{
    // Create zoom combo box
    mZoomCombo = new QComboBox();
    mZoomCombo->addItems({"Fit to Page", "25%", "50%", "100%", "150%", "200%", "300%", "400%"});
    mZoomCombo->setMinimumWidth(QFontMetrics(mZoomCombo->font()).boundingRect("XXXXXX.XX%").width() + 48);
    mZoomCombo->setEditable(true);
    mZoomCombo->setCurrentIndex(3);
    connect(mZoomCombo, SIGNAL(textActivated(QString)), this, SLOT(setZoomLevel(QString)));
    connect(mDrawingWidget, SIGNAL(scaleChanged(double)), this, SLOT(setZoomComboText(double)));

    QWidget* zoomWidget = new QWidget();
    QHBoxLayout* zoomLayout = new QHBoxLayout();
    zoomLayout->addWidget(mZoomCombo);
    zoomLayout->setContentsMargins(0, 0, 0, 0);
    zoomWidget->setLayout(zoomLayout);

    // Create toolbars
    const QList<QAction*> windowActions = actions();
    const QList<QAction*> drawingActions = mDrawingWidget->actions();

    QToolBar* fileToolBar = addToolBar("File");
    fileToolBar->setMovable(false);
    fileToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    fileToolBar->addAction(windowActions.at(JadeWindow::NewAction));
    fileToolBar->addAction(windowActions.at(JadeWindow::OpenAction));
    fileToolBar->addAction(windowActions.at(JadeWindow::SaveAction));
    fileToolBar->addAction(windowActions.at(JadeWindow::CloseAction));

    QToolBar* placeToolBar = addToolBar("Place");
    placeToolBar->setMovable(false);
    placeToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    placeToolBar->addAction(drawingActions.at(DrawingWidget::SelectModeAction));
    placeToolBar->addAction(drawingActions.at(DrawingWidget::ScrollModeAction));
    placeToolBar->addAction(drawingActions.at(DrawingWidget::ZoomModeAction));
    placeToolBar->addSeparator();
    placeToolBar->addAction(drawingActions.at(DrawingWidget::PlaceLineAction));
    placeToolBar->addAction(drawingActions.at(DrawingWidget::PlaceCurveAction));
    placeToolBar->addAction(drawingActions.at(DrawingWidget::PlacePolylineAction));
    placeToolBar->addAction(drawingActions.at(DrawingWidget::PlaceRectangleAction));
    placeToolBar->addAction(drawingActions.at(DrawingWidget::PlaceEllipseAction));
    placeToolBar->addAction(drawingActions.at(DrawingWidget::PlacePolygonAction));
    placeToolBar->addAction(drawingActions.at(DrawingWidget::PlaceTextAction));
    placeToolBar->addAction(drawingActions.at(DrawingWidget::PlaceTextRectangleAction));
    placeToolBar->addAction(drawingActions.at(DrawingWidget::PlaceTextEllipseAction));

    QToolBar* objectToolBar = addToolBar("Object");
    objectToolBar->setMovable(false);
    objectToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    objectToolBar->addAction(drawingActions.at(DrawingWidget::RotateAction));
    objectToolBar->addAction(drawingActions.at(DrawingWidget::RotateBackAction));
    objectToolBar->addAction(drawingActions.at(DrawingWidget::FlipHorizontalAction));
    objectToolBar->addAction(drawingActions.at(DrawingWidget::FlipVerticalAction));
    objectToolBar->addSeparator();
    objectToolBar->addAction(drawingActions.at(DrawingWidget::BringForwardAction));
    objectToolBar->addAction(drawingActions.at(DrawingWidget::SendBackwardAction));
    objectToolBar->addAction(drawingActions.at(DrawingWidget::BringToFrontAction));
    objectToolBar->addAction(drawingActions.at(DrawingWidget::SendToBackAction));
    objectToolBar->addSeparator();
    objectToolBar->addAction(drawingActions.at(DrawingWidget::GroupAction));
    objectToolBar->addAction(drawingActions.at(DrawingWidget::UngroupAction));

    QToolBar* viewToolBar = addToolBar("View");
    viewToolBar->setMovable(false);
    viewToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    viewToolBar->addAction(drawingActions.at(DrawingWidget::ZoomInAction));
    viewToolBar->addWidget(zoomWidget);
    viewToolBar->addAction(drawingActions.at(DrawingWidget::ZoomOutAction));
}

QAction* JadeWindow::addAction(const QString& text, const QObject* slotObject, const char* slotFunction,
                               const QString& iconPath, const QString& keySequence)
{
    QAction* action = new QAction(text, this);
    connect(action, SIGNAL(triggered(bool)), slotObject, slotFunction);

    if (!iconPath.isEmpty())
        action->setIcon(QIcon(iconPath));
    if (!keySequence.isEmpty())
        action->setShortcut(QKeySequence(keySequence));

    QMainWindow::addAction(action);
    return action;
}

//======================================================================================================================

void JadeWindow::newDrawing()
{
    // Close any open drawing first
    closeDrawing();

    // Create a new drawing only if there is no open drawing (i.e. close was successful or unneeded)
    if (!mDrawingWidget->isVisible())
    {
        mDrawingWidget->createNew();
        mNewDrawingCount++;
        setFilePath("Untitled " + QString::number(mNewDrawingCount));
        setDrawingVisible(true);
    }
}

void JadeWindow::openDrawing(const QString& path)
{
    // Prompt the user for the file path if none is passed as an argument
    QString finalPath;

    if (path.isEmpty())
    {
        finalPath = QFileDialog::getOpenFileName(
            this, "Open File", mWorkingDir, "ODF Drawing (*.odg);;All Files (*)", nullptr,
            ((mPromptOverwrite) ? (QFileDialog::Options)0 : QFileDialog::DontConfirmOverwrite));
    }
    else finalPath = path;

    // If a valid path was selected or provided, proceed with the open operation
    if (!finalPath.isEmpty())
    {
        // Close any open drawing before proceeding
        closeDrawing();

        // Open an existing drawing only if there is no open drawing (i.e. close was successful or unnecessary)
        if (!mDrawingWidget->isVisible())
        {
            // Use the selected path to load the drawing from file
            if (mDrawingWidget->load(finalPath))
            {
                setFilePath(finalPath);
                setDrawingVisible(true);
            }

            // Update the cached working directory
            mWorkingDir = QFileInfo(finalPath).dir().path();
        }
    }
}

void JadeWindow::saveDrawing(const QString& path)
{
    if (mDrawingWidget->isVisible())
    {
        if (path.isEmpty() && (mFilePath.isEmpty() || mFilePath.startsWith("Untitled")))
        {
            // If no path is provided and self._filePath is invalid, then do a 'save-as' instead
            saveDrawingAs();
        }
        else
        {
            // Use either the provided path or the cached self._filePath to save the drawing to file
            const QString finalPath = (path.isEmpty()) ? mFilePath : path;
            if (mDrawingWidget->save(finalPath))
                setFilePath(finalPath);
        }
    }
}

void JadeWindow::saveDrawingAs()
{
    if (mDrawingWidget->isVisible())
    {
        // Prompt the user for a new file path
        QString path = (mFilePath.isEmpty() || mFilePath.startsWith("Untitled")) ? mWorkingDir : mFilePath;
        QString finalPath = QFileDialog::getSaveFileName(
            this, "Save File", path, "ODF Drawing (*.odg);;All Files (*)", nullptr,
            ((mPromptOverwrite) ? (QFileDialog::Options)0 : QFileDialog::DontConfirmOverwrite));

        // If a valid path was selected, proceed with the save operation
        if (!finalPath.isEmpty())
        {
            // Ensure that the selected path ends with the proper file suffix
            if (!finalPath.endsWith(".odg", Qt::CaseInsensitive))
                finalPath = finalPath + ".odg";

            // Use the selected path to save the drawing to file
            if (mDrawingWidget->save(finalPath))
                setFilePath(finalPath);

            // Update the cached working directory
            mWorkingDir = QFileInfo(finalPath).dir().path();
        }
    }
}

void JadeWindow::closeDrawing()
{
    if (mDrawingWidget->isVisible())
    {
        bool proceedToClose = true;

        if (mPromptCloseUnsaved && !mDrawingWidget->isClean())
        {
            // If drawing has unsaved changes, prompt the user whether to save before closing
            QMessageBox::StandardButton response = QMessageBox::question(
                this, "Save Changes",
                "Save changes to " + QFileInfo(mFilePath).fileName() + " before closing?",
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

            // If the Yes button was selected, do a 'save' or 'save-as' operation as needed
            if (response == QMessageBox::Yes)
            {
                if (mFilePath.isEmpty() || mFilePath.startsWith("Untitled"))
                    saveDrawingAs();
                else
                    saveDrawing();
            }

            // Allow the close to proceed if the user clicked Yes and the save was successful or if the user clicked No
            proceedToClose = (response == QMessageBox::No || (response == QMessageBox::Yes && mDrawingWidget->isClean()));
        }

        if (proceedToClose)
        {
            // Hide the drawing and clear it to its default state
            setDrawingVisible(false);
            setFilePath("");
        }
    }
}

//======================================================================================================================

void JadeWindow::exportPng()
{
    OdgPage* currentPage = mDrawingWidget->currentPage();
    if (mDrawingWidget->isVisible() && currentPage)
    {
        // Determine export path
        QString exportPath;
        if (mFilePath.startsWith("Untitled"))
            exportPath = QDir(mWorkingDir).absoluteFilePath(currentPage->name() + ".png");
        else
            exportPath = mFilePath.left(mFilePath.size() - 4) + ".png";

        // Export the PNG image
        //QRectF exportRect = mDrawingWidget->pageRect();
        QRectF exportRect;
        const QList<OdgItem*> items = currentPage->items();
        for(auto& item : items)
            exportRect = exportRect.united(item->mapToScene(item->boundingRect()));

        const double pixelsPerInch = 600;
        const double exportScale = Odg::convertUnits(pixelsPerInch, mDrawingWidget->units(), Odg::UnitsInches);
        const int exportWidth = qRound(exportRect.width() * exportScale);
        const int exportHeight = qRound(exportRect.height() * exportScale);

        QImage pngImage(exportWidth, exportHeight, QImage::Format_ARGB32);

        QPainter painter(&pngImage);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
        painter.scale(exportScale, exportScale);
        painter.translate(-exportRect.left(), -exportRect.top());
        mDrawingWidget->paint(painter, true);
        painter.end();

        pngImage.save(exportPath);
    }
}

void JadeWindow::exportSvg()
{

}

//======================================================================================================================

void JadeWindow::preferences()
{

}

void JadeWindow::about()
{

}

//======================================================================================================================

void JadeWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    if (event && !event->spontaneous())
    {
        mPropertiesDock->raise();
        mDrawingWidget->zoomFit();
    }
}

void JadeWindow::closeEvent(QCloseEvent* event)
{
    if (event)
    {
        closeDrawing();
        if (!mDrawingWidget->isVisible())
        {
            saveSettings();
            event->accept();
        }
        else event->ignore();
    }
}

//======================================================================================================================

void JadeWindow::setDrawingVisible(bool visible)
{
    mDrawingWidget->setVisible(visible);

    // Update dock widget visibility
    if (visible)
    {
        mPagesDock->setVisible(mPagesDockVisibleOnClose);
        mPropertiesDock->setVisible(mPropertiesDockVisibleOnClose);
    }
    else
    {
        mPagesDockVisibleOnClose = mPagesDock->isVisible();
        mPropertiesDockVisibleOnClose = mPropertiesDock->isVisible();
        mPagesDock->setVisible(false);
        mPropertiesDock->setVisible(false);
    }

    // Update actions
    const QList<QAction*> windowActions = actions();
    windowActions.at(JadeWindow::SaveAction)->setEnabled(visible);
    windowActions.at(JadeWindow::SaveAsAction)->setEnabled(visible);
    windowActions.at(JadeWindow::CloseAction)->setEnabled(visible);
    windowActions.at(JadeWindow::ExportPngAction)->setEnabled(visible);
    windowActions.at(JadeWindow::ExportSvgAction)->setEnabled(visible);
    windowActions.at(JadeWindow::ViewPagesAction)->setEnabled(visible);
    windowActions.at(JadeWindow::ViewPropertiesAction)->setEnabled(visible);

    const QList<QAction*> drawingActions = mDrawingWidget->actions();
    for(auto& action : drawingActions)
        action->setEnabled(visible);

    // Update drawing
    if (visible)
        mDrawingWidget->zoomFit();
    else
        mDrawingWidget->clear();
}

void JadeWindow::setFilePath(const QString& path)
{
    mFilePath = path;

    // Update window title
    const QString fileName = QFileInfo(mFilePath).fileName();
    if (fileName.isEmpty())
        setWindowTitle("Jade");
    else
        setWindowTitle(fileName + " - Jade");
}

//======================================================================================================================

void JadeWindow::setZoomLevel(const QString& zoomLevelText)
{
    if (zoomLevelText == "Fit to Page")
        mDrawingWidget->zoomFit();
    else
    {
        double zoomLevel = 1.0;
        bool zoomLevelOk = false;

        if (zoomLevelText.endsWith("%"))
            zoomLevel = zoomLevelText.first(zoomLevelText.size() - 1).toDouble(&zoomLevelOk);
        else
            zoomLevel = zoomLevelText.toDouble(&zoomLevelOk);

        if (zoomLevelOk)
        {
            double scale = zoomLevel * Odg::convertUnits(2, mDrawingWidget->units(), Odg::UnitsInches);
            mDrawingWidget->setScale(scale);
            mZoomCombo->clearFocus();
        }
    }
}

void JadeWindow::setZoomComboText(double scale)
{
    double zoomLevel = scale / Odg::convertUnits(2, mDrawingWidget->units(), Odg::UnitsInches);
    mZoomCombo->setCurrentText(QString::number(zoomLevel, 'f', 2) + "%");
}

//======================================================================================================================

void JadeWindow::saveSettings()
{

}

void JadeWindow::loadSettings()
{

}
