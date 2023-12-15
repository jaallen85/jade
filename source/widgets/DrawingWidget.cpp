// File: DrawingWidget.cpp
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

#include "DrawingWidget.h"
#include "DrawingUndo.h"
#include "ElectricItems.h"
#include "LogicItems.h"
#include "OdgControlPoint.h"
#include "OdgGluePoint.h"
#include "OdgItem.h"
#include "OdgPage.h"
#include "OdgReader.h"
#include "OdgStyle.h"
#include "OdgWriter.h"
#include "OdgCurveItem.h"
#include "OdgEllipseItem.h"
#include "OdgGroupItem.h"
#include "OdgLineItem.h"
#include "OdgPathItem.h"
#include "OdgPolygonItem.h"
#include "OdgPolylineItem.h"
#include "OdgRoundedRectItem.h"
#include "OdgTextItem.h"
#include "OdgTextEllipseItem.h"
#include "OdgTextRoundedRectItem.h"
#include <QActionGroup>
#include <QApplication>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QScrollBar>
#include <QStyle>
#include <QStyleHintReturnMask>
#include <QStyleOptionRubberBand>
#include <QWheelEvent>

DrawingWidget::DrawingWidget() : QAbstractScrollArea(), OdgDrawing(),
    mDrawingTemplate(nullptr), mStyleTemplate(nullptr), mDefaultStyle(nullptr),
    mCurrentPage(nullptr), mNewPageCount(0),
    mTransform(), mTransformInverse(), mMode(Odg::SelectMode), mUndoStack(),
    mMouseState(MouseIdle), mMouseButtonDownPosition(), mMouseButtonDownScenePosition(),
    mMouseDragged(false), mSelectMouseState(SelectMouseIdle), mSelectedItems(), mSelectedItemsCenter(),
	mSelectMouseDownItem(nullptr), mSelectMouseDownPoint(nullptr), mSelectFocusItem(nullptr),
	mSelectMoveItemsInitialPositions(), mSelectMoveItemsPreviousDeltaPosition(),
	mSelectResizeItemInitialPosition(), mSelectResizeItemPreviousPosition(), mSelectRubberBandRect(),
    mScrollInitialHorizontalValue(0), mScrollInitialVerticalValue(0), mZoomRubberBandRect(),
    mPlaceItems(), mPlaceByMousePressAndRelease(false),
    mPanOriginalCursor(Qt::ArrowCursor), mPanStartPosition(), mPanCurrentPosition(), mPanTimer(),
    mModeActionGroup(nullptr), mNoItemContextMenu(nullptr), mSingleItemContextMenu(nullptr),
    mSinglePolyItemContextMenu(nullptr), mSingleGroupItemContextMenu(nullptr), mMultipleItemContextMenu(nullptr)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setMouseTracking(true);

    mDrawingTemplate = new OdgDrawing();
    mStyleTemplate = new OdgStyle(mUnits, true);

    mDefaultStyle = new OdgStyle(mUnits, true);

    mUndoStack.setUndoLimit(256);
    connect(&mUndoStack, SIGNAL(cleanChanged(bool)), this, SLOT(emitCleanChanged(bool)));

    mPanTimer.setInterval(5);
    connect(&mPanTimer, SIGNAL(timeout()), this, SLOT(mousePanEvent()));

    createActions();
    createContextMenu();
}

DrawingWidget::~DrawingWidget()
{
    selectNone();
    setSelectMode();
    setCurrentPage(nullptr);
    setDefaultStyle(nullptr);
    setStyleTemplate(nullptr);
    setDrawingTemplate(nullptr);

    qDeleteAll(mPathItems);
}

//======================================================================================================================

void DrawingWidget::createActions()
{
    mModeActionGroup = new QActionGroup(this);
    connect(mModeActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(setModeFromAction(QAction*)));

    addNormalAction("Undo", this, SLOT(undo()), ":/icons/edit-undo.png", "Ctrl+Z");
    addNormalAction("Redo", this, SLOT(redo()), ":/icons/edit-redo.png", "Ctrl+Shift+Z");
    addNormalAction("Cut", this, SLOT(cut()), ":/icons/edit-cut.png", "Ctrl+X");
    addNormalAction("Copy", this, SLOT(copy()), ":/icons/edit-copy.png", "Ctrl+C");
    addNormalAction("Paste", this, SLOT(paste()), ":/icons/edit-paste.png", "Ctrl+V");
    addNormalAction("Delete", this, SLOT(removeItems()), ":/icons/edit-delete.png", "Delete");
    addNormalAction("Select All", this, SLOT(selectAll()), ":/icons/edit-select-all.png", "Ctrl+A");
    addNormalAction("Select None", this, SLOT(selectNone()));

    addModeAction("Select Mode", ":/icons/edit-select.png", "Escape");
    addModeAction("Scroll Mode", ":/icons/transform-move.png");
    addModeAction("Zoom Mode", ":/icons/page-zoom.png");
    addModeAction("Place Line", ":/icons/draw-line.png");
    addModeAction("Place Curve", ":/icons/draw-curve.png");
    addModeAction("Place Polyline", ":/icons/draw-polyline.png");
    addModeAction("Place Rectangle", ":/icons/draw-rectangle.png");
    addModeAction("Place Ellipse", ":/icons/draw-ellipse.png");
    addModeAction("Place Polygon", ":/icons/draw-polygon.png");
    addModeAction("Place Text", ":/icons/draw-text.png");
    addModeAction("Place Text Rectangle", ":/icons/text-rectangle.png");
    addModeAction("Place Text Ellipse", ":/icons/text-ellipse.png");
    addPathItems("Electric Items", ElectricItems::items(), ElectricItems::icons());
    addPathItems("Logic Items", LogicItems::items(), LogicItems::icons());

    addNormalAction("Rotate", this, SLOT(rotate()), ":/icons/object-rotate-right.png", "R");
    addNormalAction("Rotate Back", this, SLOT(rotateBack()), ":/icons/object-rotate-left.png", "Shift+R");
    addNormalAction("Flip Horizontal", this, SLOT(flipHorizontal()), ":/icons/object-flip-horizontal.png", "F");
    addNormalAction("Flip Vertical", this, SLOT(flipVertical()), ":/icons/object-flip-vertical.png", "Shift+F");
    addNormalAction("Bring Forward", this, SLOT(bringForward()), ":/icons/object-bring-forward.png");
    addNormalAction("Send Backward", this, SLOT(sendBackward()), ":/icons/object-send-backward.png");
    addNormalAction("Bring to Front", this, SLOT(bringToFront()), ":/icons/object-bring-to-front.png");
    addNormalAction("Send to Back", this, SLOT(sendToBack()), ":/icons/object-send-to-back.png");
    addNormalAction("Group", this, SLOT(group()), ":/icons/merge.png", "Ctrl+G");
    addNormalAction("Ungroup", this, SLOT(ungroup()), ":/icons/split.png");
    addNormalAction("Insert Point", this, SLOT(insertPoint()), ":/icons/format-add-node.png");
    addNormalAction("Remove Point", this, SLOT(removePoint()), ":/icons/format-remove-node.png");

    addNormalAction("Insert Page", this, SLOT(insertPage()), ":/icons/archive-insert.png");
    addNormalAction("Duplicate Page", this, SLOT(duplicatePage()), ":/icons/edit-copy.png");
    addNormalAction("Remove Page", this, SLOT(removePage()), ":/icons/archive-remove.png");

    addNormalAction("Zoom In", this, SLOT(zoomIn()), ":/icons/zoom-in.png", ".");
    addNormalAction("Zoom Out", this, SLOT(zoomOut()), ":/icons/zoom-out.png", ",");
    addNormalAction("Zoom Fit", this, SLOT(zoomFit()), ":/icons/zoom-fit-best.png", "/");
}

void DrawingWidget::createContextMenu()
{
    QList<QAction*> actions = this->actions();

    mNoItemContextMenu = new QMenu(this);
    mNoItemContextMenu->addAction(actions.at(UndoAction));
    mNoItemContextMenu->addAction(actions.at(RedoAction));
    mNoItemContextMenu->addSeparator();
    mNoItemContextMenu->addAction(actions.at(CutAction));
    mNoItemContextMenu->addAction(actions.at(CopyAction));
    mNoItemContextMenu->addAction(actions.at(PasteAction));
    mNoItemContextMenu->addSeparator();
    mNoItemContextMenu->addAction(actions.at(ZoomInAction));
    mNoItemContextMenu->addAction(actions.at(ZoomOutAction));
    mNoItemContextMenu->addAction(actions.at(ZoomFitAction));

    mSingleItemContextMenu = new QMenu(this);
    mSingleItemContextMenu->addAction(actions.at(CutAction));
    mSingleItemContextMenu->addAction(actions.at(CopyAction));
    mSingleItemContextMenu->addAction(actions.at(PasteAction));
    mSingleItemContextMenu->addAction(actions.at(DeleteAction));
    mSingleItemContextMenu->addSeparator();
    mSingleItemContextMenu->addAction(actions.at(RotateAction));
    mSingleItemContextMenu->addAction(actions.at(RotateBackAction));
    mSingleItemContextMenu->addAction(actions.at(FlipHorizontalAction));
    mSingleItemContextMenu->addAction(actions.at(FlipVerticalAction));
    mSingleItemContextMenu->addSeparator();
    mSingleItemContextMenu->addAction(actions.at(BringForwardAction));
    mSingleItemContextMenu->addAction(actions.at(SendBackwardAction));
    mSingleItemContextMenu->addAction(actions.at(BringToFrontAction));
    mSingleItemContextMenu->addAction(actions.at(SendToBackAction));

    mSinglePolyItemContextMenu = new QMenu(this);
    mSinglePolyItemContextMenu->addAction(actions.at(CutAction));
    mSinglePolyItemContextMenu->addAction(actions.at(CopyAction));
    mSinglePolyItemContextMenu->addAction(actions.at(PasteAction));
    mSinglePolyItemContextMenu->addAction(actions.at(DeleteAction));
    mSinglePolyItemContextMenu->addSeparator();
    mSinglePolyItemContextMenu->addAction(actions.at(InsertPointAction));
    mSinglePolyItemContextMenu->addAction(actions.at(RemovePointAction));
    mSinglePolyItemContextMenu->addSeparator();
    mSinglePolyItemContextMenu->addAction(actions.at(RotateAction));
    mSinglePolyItemContextMenu->addAction(actions.at(RotateBackAction));
    mSinglePolyItemContextMenu->addAction(actions.at(FlipHorizontalAction));
    mSinglePolyItemContextMenu->addAction(actions.at(FlipVerticalAction));
    mSinglePolyItemContextMenu->addSeparator();
    mSinglePolyItemContextMenu->addAction(actions.at(BringForwardAction));
    mSinglePolyItemContextMenu->addAction(actions.at(SendBackwardAction));
    mSinglePolyItemContextMenu->addAction(actions.at(BringToFrontAction));
    mSinglePolyItemContextMenu->addAction(actions.at(SendToBackAction));

    mSingleGroupItemContextMenu = new QMenu(this);
    mSingleGroupItemContextMenu->addAction(actions.at(CutAction));
    mSingleGroupItemContextMenu->addAction(actions.at(CopyAction));
    mSingleGroupItemContextMenu->addAction(actions.at(PasteAction));
    mSingleGroupItemContextMenu->addAction(actions.at(DeleteAction));
    mSingleGroupItemContextMenu->addSeparator();
    mSingleGroupItemContextMenu->addAction(actions.at(RotateAction));
    mSingleGroupItemContextMenu->addAction(actions.at(RotateBackAction));
    mSingleGroupItemContextMenu->addAction(actions.at(FlipHorizontalAction));
    mSingleGroupItemContextMenu->addAction(actions.at(FlipVerticalAction));
    mSingleGroupItemContextMenu->addSeparator();
    mSingleGroupItemContextMenu->addAction(actions.at(BringForwardAction));
    mSingleGroupItemContextMenu->addAction(actions.at(SendBackwardAction));
    mSingleGroupItemContextMenu->addAction(actions.at(BringToFrontAction));
    mSingleGroupItemContextMenu->addAction(actions.at(SendToBackAction));
    mSingleGroupItemContextMenu->addSeparator();
    mSingleGroupItemContextMenu->addAction(actions.at(GroupAction));
    mSingleGroupItemContextMenu->addAction(actions.at(UngroupAction));

    mMultipleItemContextMenu = new QMenu(this);
    mMultipleItemContextMenu->addAction(actions.at(CutAction));
    mMultipleItemContextMenu->addAction(actions.at(CopyAction));
    mMultipleItemContextMenu->addAction(actions.at(PasteAction));
    mMultipleItemContextMenu->addAction(actions.at(DeleteAction));
    mMultipleItemContextMenu->addSeparator();
    mMultipleItemContextMenu->addAction(actions.at(RotateAction));
    mMultipleItemContextMenu->addAction(actions.at(RotateBackAction));
    mMultipleItemContextMenu->addAction(actions.at(FlipHorizontalAction));
    mMultipleItemContextMenu->addAction(actions.at(FlipVerticalAction));
    mMultipleItemContextMenu->addSeparator();
    mMultipleItemContextMenu->addAction(actions.at(BringForwardAction));
    mMultipleItemContextMenu->addAction(actions.at(SendBackwardAction));
    mMultipleItemContextMenu->addAction(actions.at(BringToFrontAction));
    mMultipleItemContextMenu->addAction(actions.at(SendToBackAction));
    mMultipleItemContextMenu->addSeparator();
    mMultipleItemContextMenu->addAction(actions.at(GroupAction));
    mMultipleItemContextMenu->addAction(actions.at(UngroupAction));
    mMultipleItemContextMenu->addSeparator();
    mMultipleItemContextMenu->addAction(actions.at(ZoomInAction));
    mMultipleItemContextMenu->addAction(actions.at(ZoomOutAction));
    mMultipleItemContextMenu->addAction(actions.at(ZoomFitAction));
}

void DrawingWidget::addNormalAction(const QString& text, const QObject* slotObject, const char* slotFunction,
                                 const QString& iconPath, const QString& keySequence)
{
    QAction* action = new QAction(text, this);
    if (!iconPath.isEmpty())
        action->setIcon(QIcon(iconPath));
    if (!keySequence.isEmpty())
        action->setShortcut(QKeySequence(keySequence));
    QWidget::addAction(action);

    connect(action, SIGNAL(triggered(bool)), slotObject, slotFunction);
}


void DrawingWidget::addModeAction(const QString& text, const QString& iconPath, const QString& keySequence)
{
    QAction* action = new QAction(text, mModeActionGroup);
    if (!iconPath.isEmpty())
        action->setIcon(QIcon(iconPath));
    if (!keySequence.isEmpty())
        action->setShortcut(QKeySequence(keySequence));
    QWidget::addAction(action);

    action->setCheckable(true);
    if (mModeActionGroup->actions().size() == 1)
        action->setChecked(true);
}

void DrawingWidget::addPathItems(const QString& name, const QList<OdgPathItem*>& items, const QStringList& icons)
{
    QMenu* menu = new QMenu(name);
    QAction* menuAction;

    mPathItems.append(items);
    for(int i = 0; i < items.size() && i < icons.size(); i++)
    {
        QAction* menuAction = new QAction("Place " + items[i]->pathName(), mModeActionGroup);
        menuAction->setIcon(QIcon(icons[i]));
        menuAction->setCheckable(true);
        menu->addAction(menuAction);
    }

    QAction* action = new QAction(QIcon(icons.first()), name, this);
    action->setMenu(menu);
    QWidget::addAction(action);
}

//======================================================================================================================

void DrawingWidget::setDrawingTemplate(OdgDrawing* temp)
{
    delete mDrawingTemplate;
    mDrawingTemplate = temp;
}

void DrawingWidget::setStyleTemplate(OdgStyle* style)
{
    delete mStyleTemplate;
    mStyleTemplate = style;
}

OdgDrawing* DrawingWidget::drawingTemplate() const
{
    return mDrawingTemplate;
}

OdgStyle* DrawingWidget::styleTemplate() const
{
    return mStyleTemplate;
}

//======================================================================================================================

void DrawingWidget::setDefaultStyle(OdgStyle* style)
{
    delete mDefaultStyle;
    mDefaultStyle = style;
}

OdgStyle* DrawingWidget::defaultStyle() const
{
    return mDefaultStyle;
}

//======================================================================================================================

void DrawingWidget::setUnits(Odg::Units units)
{
    if (mUnits != units)
    {
        double scaleFactor = Odg::convertUnits(1, mUnits, units);

        OdgDrawing::setUnits(units);

        if (scaleFactor != 0) setScale(scale() / scaleFactor);

        emit propertyChanged("units", static_cast<int>(mUnits));
    }
}

void DrawingWidget::setPageSize(const QSizeF& size)
{
    if (size.width() > 0 && size.height() > 0 && mPageSize != size)
    {
        OdgDrawing::setPageSize(size);
        emit propertyChanged("pageSize", mPageSize);
    }
}

void DrawingWidget::setPageMargins(const QMarginsF& margins)
{
    if (margins.left() >= 0 && margins.top() >= 0 && margins.right() >= 0 && margins.bottom() >= 0 && mPageMargins != margins)
    {
        OdgDrawing::setPageMargins(margins);
        emit propertyChanged("pageMargins", QVariant::fromValue<QMarginsF>(mPageMargins));
    }
}

void DrawingWidget::setBackgroundColor(const QColor& color)
{
    if (mBackgroundColor != color)
    {
        OdgDrawing::setBackgroundColor(color);
        emit propertyChanged("backgroundColor", mBackgroundColor);
    }
}

void DrawingWidget::setGrid(double grid)
{
    if (grid >= 0 && mGrid != grid)
    {
        OdgDrawing::setGrid(grid);
        emit propertyChanged("grid", mGrid);
    }
}

void DrawingWidget::setGridStyle(Odg::GridStyle style)
{
    if (mGridStyle != style)
    {
        OdgDrawing::setGridStyle(style);
        emit propertyChanged("gridStyle", mGridStyle);
    }
}

void DrawingWidget::setGridColor(const QColor& color)
{
    if (mGridColor != color)
    {
        OdgDrawing::setGridColor(color);
        emit propertyChanged("gridColor", mGridColor);
    }
}

void DrawingWidget::setGridSpacingMajor(int spacing)
{
    if (spacing >= 0 && mGridSpacingMajor != spacing)
    {
        OdgDrawing::setGridSpacingMajor(spacing);
        emit propertyChanged("gridSpacingMajor", mGridSpacingMajor);
    }
}

void DrawingWidget::setGridSpacingMinor(int spacing)
{
    if (spacing >= 0 && mGridSpacingMinor != spacing)
    {
        OdgDrawing::setGridSpacingMinor(spacing);
        emit propertyChanged("gridSpacingMinor", mGridSpacingMinor);
    }
}

//======================================================================================================================

void DrawingWidget::setProperty(const QString& name, const QVariant& value)
{
    OdgDrawing::setProperty(name, value);
    if (name == "pageSize" || name == "pageMargins") zoomFit();
    else viewport()->update();
}

QVariant DrawingWidget::property(const QString& name) const
{
    return OdgDrawing::property(name);
}

//======================================================================================================================

OdgPage* DrawingWidget::currentPage() const
{
    return mCurrentPage;
}

int DrawingWidget::currentPageIndex() const
{
    return (mCurrentPage) ? mPages.indexOf(mCurrentPage) : -1;
}

//======================================================================================================================

double DrawingWidget::scale() const
{
    return mTransform.m11();
}

QRectF DrawingWidget::visibleRect() const
{
    return QRectF(mapToScene(QPoint(0, 0)), mapToScene(QPoint(viewport()->width() - 1, viewport()->height() - 1)));
}

QPointF DrawingWidget::mapToScene(const QPoint& position) const
{
    const QPoint scrollPosition(horizontalScrollBar()->value(), verticalScrollBar()->value());
    return mTransformInverse.map(QPointF(position + scrollPosition));
}

QRectF DrawingWidget::mapToScene(const QRect& rect) const
{
    return QRectF(mapToScene(rect.topLeft()), mapToScene(rect.bottomRight()));
}

QPoint DrawingWidget::mapFromScene(const QPointF& position) const
{
    const QPoint scrollPosition(horizontalScrollBar()->value(), verticalScrollBar()->value());
    return mTransform.map(position).toPoint() - scrollPosition;
}

QRect DrawingWidget::mapFromScene(const QRectF& rect) const
{
    return QRect(mapFromScene(rect.topLeft()), mapFromScene(rect.bottomRight()));
}

//======================================================================================================================

Odg::DrawingMode DrawingWidget::mode() const
{
    return mMode;
}

QList<OdgItem*> DrawingWidget::currentItems() const
{
    if (mMode == Odg::SelectMode) return mSelectedItems;
    else if (mMode == Odg::PlaceMode) return mPlaceItems;
    return QList<OdgItem*>();
}

QList<OdgItem*> DrawingWidget::selectedItems() const
{
    return mSelectedItems;
}

OdgItem* DrawingWidget::mouseDownItem() const
{
    return mSelectMouseDownItem;
}

OdgControlPoint* DrawingWidget::mouseDownPoint() const
{
    return mSelectMouseDownPoint;
}

OdgItem* DrawingWidget::focusItem() const
{
    return mSelectFocusItem;
}

QList<OdgItem*> DrawingWidget::placeItems() const
{
    return mPlaceItems;
}

//======================================================================================================================

void DrawingWidget::paint(QPainter& painter, bool isExport)
{
    if (mCurrentPage)
    {
        drawBackground(painter, !isExport, !isExport);
        drawItems(painter, mCurrentPage->items());
    }
}

//======================================================================================================================

void DrawingWidget::createNew()
{
    if (mDrawingTemplate)
    {
        blockSignals(true);

        setUnits(mDrawingTemplate->units());
        setPageSize(mDrawingTemplate->pageSize());
        setPageMargins(mDrawingTemplate->pageMargins());
        setBackgroundColor(mDrawingTemplate->backgroundColor());
        setGrid(mDrawingTemplate->grid());
        setGridStyle(mDrawingTemplate->gridStyle());
        setGridColor(mDrawingTemplate->gridColor());
        setGridSpacingMajor(mDrawingTemplate->gridSpacingMajor());
        setGridSpacingMinor(mDrawingTemplate->gridSpacingMinor());

        if (mStyleTemplate && mDefaultStyle)
        {
            mDefaultStyle->setPenStyle(mStyleTemplate->penStyle());
            mDefaultStyle->setPenWidth(mStyleTemplate->penWidth());
            mDefaultStyle->setPenColor(mStyleTemplate->penColor());
            mDefaultStyle->setBrushColor(mStyleTemplate->brushColor());

            mDefaultStyle->setStartMarkerStyle(mStyleTemplate->startMarkerStyle());
            mDefaultStyle->setStartMarkerSize(mStyleTemplate->startMarkerSize());
            mDefaultStyle->setEndMarkerStyle(mStyleTemplate->endMarkerStyle());
            mDefaultStyle->setEndMarkerSize(mStyleTemplate->endMarkerSize());

            mDefaultStyle->setFontFamily(mStyleTemplate->fontFamily());
            mDefaultStyle->setFontSize(mStyleTemplate->fontSize());
            mDefaultStyle->setFontStyle(mStyleTemplate->fontStyle());
            mDefaultStyle->setTextAlignment(mStyleTemplate->textAlignment());
            mDefaultStyle->setTextPadding(mStyleTemplate->textPadding());
            mDefaultStyle->setTextColor(mStyleTemplate->textColor());
        }

        blockSignals(false);
        emit propertiesChanged();
    }

    insertPage();

    mUndoStack.clear();
}

bool DrawingWidget::load(const QString& fileName)
{
    OdgReader reader(fileName);
    if (!reader.open())
    {
        QMessageBox::critical(this, "File Error", "Error opening " + fileName + " for reading.");
        return false;
    }

    if (!reader.read())
    {
        QMessageBox::critical(this, "File Error", "Error reading " + fileName + ".  File is invalid.");
        return false;
    }

    clear();

    blockSignals(true);
    setUnits(reader.units());
    setPageSize(reader.pageSize());
    setPageMargins(reader.pageMargins());
    setBackgroundColor(reader.backgroundColor());
    setGrid(reader.grid());
    setGridStyle(reader.gridStyle());
    setGridColor(reader.gridColor());
    setGridSpacingMajor(reader.gridSpacingMajor());
    setGridSpacingMinor(reader.gridSpacingMinor());
    blockSignals(false);
    emit propertiesChanged();

    setDefaultStyle(reader.takeDefaultStyle());

    const QList<OdgPage*> pages = reader.takePages();
    for(auto& page : pages)
        addPage(page);

    setCurrentPageIndex(0);
    zoomFit();

    mUndoStack.setClean();
    return true;
}

bool DrawingWidget::save(const QString& fileName)
{
    OdgWriter writer(fileName);
    if (!writer.open())
    {
        QMessageBox::critical(this, "File Error", "Error opening " + fileName + " for writing.");
        return false;
    }

    writer.setUnits(mUnits);
    writer.setPageSize(mPageSize);
    writer.setPageMargins(mPageMargins);
    writer.setBackgroundColor(mBackgroundColor);
    writer.setGrid(mGrid);
    writer.setGridStyle(mGridStyle);
    writer.setGridColor(mGridColor);
    writer.setGridSpacingMajor(mGridSpacingMajor);
    writer.setGridSpacingMinor(mGridSpacingMinor);

    writer.setDefaultStyle(mDefaultStyle);
    writer.setPages(mPages);

    if (!writer.write())
    {
        QMessageBox::critical(this, "File Error", "Error writing " + fileName + ".  File is invalid.");
        return false;
    }

    mUndoStack.setClean();
    return true;
}

void DrawingWidget::clear()
{
    selectNone();
    setSelectMode();

    mUndoStack.clear();

    mNewPageCount = 0;
    setCurrentPage(nullptr);
    clearPages();
}

bool DrawingWidget::isClean() const
{
    return mUndoStack.isClean();
}

//======================================================================================================================

void DrawingWidget::insertPage(int index, OdgPage* page)
{
    if (page)
    {
        OdgDrawing::insertPage(index, page);
        emit pageInserted(page, index);
        setCurrentPage(page);
    }
}

void DrawingWidget::removePage(OdgPage* page)
{
    if (page)
    {
        const int index = mPages.indexOf(page);
        if (0 <= index && index < mPages.size())
        {
            // Determine the index of the current page after this page is removed
            int newCurrentPageIndex = currentPageIndex();
            if (newCurrentPageIndex == index)
            {
                if (index > 0)
                    newCurrentPageIndex = index - 1;
                else if (index < mPages.size() - 1)
                    newCurrentPageIndex = index + 1;
            }
            else newCurrentPageIndex--;

            // Remove the page from the drawing
            OdgDrawing::removePage(page);
            emit pageRemoved(page, index);
            setCurrentPageIndex(newCurrentPageIndex);
        }
    }
}

//======================================================================================================================

void DrawingWidget::setPageProperty(OdgPage* page, const QString& name, const QVariant& value)
{
    if (page)
    {
        page->setProperty(name, value);
        if (page == mCurrentPage)
            emit currentPagePropertyChanged(name, value);
    }
}

//======================================================================================================================

void DrawingWidget::addItems(OdgPage* page, const QList<OdgItem*>& items, bool place)
{
    if (page)
    {
        // Add the items to the page
        for(auto& item : items) page->addItem(item);

        // Connect control/glue points if necessary
        if (place) placeItems(items);

        // Signal any listeners that items were inserted
        emit itemsInserted(items);
        viewport()->update();
    }
}

void DrawingWidget::insertItems(OdgPage* page, const QList<OdgItem*>& items, const QHash<OdgItem*,int>& indices,
                                bool place)
{
    if (page)
    {
        // Insert the items into the page
        for(auto& item : items) page->insertItem(indices.value(item), item);

        // Connect control/glue points if necessary
        if (place) placeItems(items);

        // Signal any listeners that items were inserted
        emit itemsInserted(items);

        viewport()->update();
    }
}

void DrawingWidget::removeItems(OdgPage* page, const QList<OdgItem*>& items)
{
    if (page)
    {
        // Unselect items if necessary
        setSelectedItems(QList<OdgItem*>());

        // Disonnect control/glue points
        unplaceItems(items);

        // Remove the items from the page
        for(auto& item : items) page->removeItem(item);

        // Signal any listeners that items were removed
        emit itemsRemoved(items);

        viewport()->update();
    }
}

void DrawingWidget::reorderItems(OdgPage* page, const QList<OdgItem*>& items)
{
    if (page)
    {
        // Reorder the items within the page
        for(auto& item : items) page->removeItem(item);
        for(auto& item : items) page->addItem(item);

        viewport()->update();
    }
}

//======================================================================================================================

void DrawingWidget::moveItems(const QList<OdgItem*>& items, const QHash<OdgItem*,QPointF>& positions, bool place)
{
    // Move the items
    for(auto& item : items) item->setPosition(positions.value(item));

    // Maintain any connections after the move
    maintainItemConnections(items);

    // Connect control/glue points if necessary
    if (place) placeItems(items);

    // Signal any listeners that current items' geometry may have changed
    if (items == currentItems())
    {
        if (mMode == Odg::SelectMode) updateSelectionCenter();
        emit currentItemsGeometryChanged(items);
    }

    viewport()->update();
}

void DrawingWidget::resizeItem(OdgControlPoint* point, const QPointF& position, bool snapTo45Degrees, bool disconnect,
                               bool place)
{
    OdgItem* item = (point) ? point->item() : nullptr;
    if (item)
    {
        QList<OdgItem*> items;
        items.append(item);

        // Resize the item
        item->resize(point, position, snapTo45Degrees);

        // Disconnect this point from its glue point
        if (disconnect) point->disconnect();

        // Maintain any connections after the resize (applies to other item control points only since we just
        // disconnected this point)
        maintainItemConnections(items);

        // Connect control/glue points if necessary
        if (place) placeItems(items);

        // Signal any listeners that current items' geometry may have changed
        if (items == currentItems())
        {
            if (mMode == Odg::SelectMode) updateSelectionCenter();
            emit currentItemsGeometryChanged(items);
        }

        viewport()->update();
    }
}

void DrawingWidget::resizeItem2(OdgControlPoint* point1, const QPointF& p1, OdgControlPoint* point2, const QPointF& p2,
                                bool place)
{
    OdgItem* item1 = (point1) ? point1->item() : nullptr;
    OdgItem* item2 = (point2) ? point2->item() : nullptr;
    if (item1 && item2 && item1 == item2)
    {
        QList<OdgItem*> items;
        items.append(item1);

        // Resize the item
        item1->resize(point1, p1, false);
        item2->resize(point2, p2, false);

        // Disconnect this point from its glue point
        point1->disconnect();
        point2->disconnect();

        // Maintain any connections after the resize (applies to other item control points only since we just
        // disconnected this point)
        maintainItemConnections(items);

        // Connect control/glue points if necessary
        if (place) placeItems(items);

        // Signal any listeners that current items' geometry may have changed
        if (items == currentItems())
        {
            if (mMode == Odg::SelectMode) updateSelectionCenter();
            emit currentItemsGeometryChanged(items);
        }

        viewport()->update();
    }
}

//======================================================================================================================

void DrawingWidget::rotateItems(const QList<OdgItem*>& items, const QPointF& position)
{
    // Rotate the items
    for(auto& item : items) item->rotate(position);

    // Maintain any connections after the rotation
    maintainItemConnections(items);

    // Signal any listeners that current items' geometry may have changed
    if (items == currentItems()) emit currentItemsGeometryChanged(items);

    viewport()->update();
}

void DrawingWidget::rotateBackItems(const QList<OdgItem*>& items, const QPointF& position)
{
    // Rotate the items back
    for(auto& item : items) item->rotateBack(position);

    // Maintain any connections after the rotation
    maintainItemConnections(items);

    // Signal any listeners that current items' geometry may have changed
    if (items == currentItems()) emit currentItemsGeometryChanged(items);

    viewport()->update();
}

void DrawingWidget::flipItemsHorizontal(const QList<OdgItem*>& items, const QPointF& position)
{
    // Flip the items horizontally
    for(auto& item : items) item->flipHorizontal(position);

    // Maintain any connections after the flip operation
    maintainItemConnections(items);

    // Signal any listeners that current items' geometry may have changed
    if (items == currentItems()) emit currentItemsGeometryChanged(items);

    viewport()->update();
}

void DrawingWidget::flipItemsVertical(const QList<OdgItem*>& items, const QPointF& position)
{
    // Flip the items vertically
    for(auto& item : items) item->flipVertical(position);

    // Maintain any connections after the flip operation
    maintainItemConnections(items);

    // Signal any listeners that current items' geometry may have changed
    if (items == currentItems()) emit currentItemsGeometryChanged(items);

    viewport()->update();
}

//======================================================================================================================

void DrawingWidget::insertItemPoint(OdgItem* item, int index, OdgControlPoint* point)
{
    if (item)
    {
        QList<OdgItem*> items;
        items.append(item);

        // Insert the new point
        item->insertControlPoint(index, point);

        // Signal any listeners that current items' geometry may have changed
        if (items == currentItems())
        {
            if (mMode == Odg::SelectMode) updateSelectionCenter();
            emit currentItemsGeometryChanged(items);
        }

        updateActions();
        viewport()->update();
    }
}

void DrawingWidget::removeItemPoint(OdgItem* item, OdgControlPoint* point)
{
    if (item)
    {
        QList<OdgItem*> items;
        items.append(item);

        // Remove the current point
        item->removeControlPoint(point);

        // Signal any listeners that current items' geometry may have changed
        if (items == currentItems())
        {
            if (mMode == Odg::SelectMode) updateSelectionCenter();
            emit currentItemsGeometryChanged(items);
        }

        updateActions();
        viewport()->update();
    }
}

//======================================================================================================================

void DrawingWidget::placeItems(const QList<OdgItem*>& items)
{
    if (mCurrentPage)
    {
        QList<OdgGluePoint*> currentPageItemGluePoints;
        QList<OdgControlPoint*> controlPoints;

        const QList<OdgItem*> currentPageItems = mCurrentPage->items();
        for(auto& currentPageItem : currentPageItems)
        {
            // Only consider connections to items in the current page that aren't part of items or mPlaceItems
            if (!items.contains(currentPageItem) && !mPlaceItems.contains(currentPageItem))
            {
                currentPageItemGluePoints = currentPageItem->gluePoints();
                for(auto& currentPageItemGluePoint : qAsConst(currentPageItemGluePoints))
                {
                    for(auto& item : items)
                    {
                        controlPoints = item->controlPoints();
                        for(auto& controlPoint : qAsConst(controlPoints))
                        {
                            if (shouldConnect(controlPoint, currentPageItemGluePoint))
                                controlPoint->connect(currentPageItemGluePoint);
                        }
                    }
                }
            }
        }
    }
}

void DrawingWidget::unplaceItems(const QList<OdgItem*>& items)
{
    QList<OdgControlPoint*> controlPoints;
    QList<OdgGluePoint*> gluePoints;
    OdgGluePoint* gluePoint;

    for(auto& item : items)
    {
        // Keep connections to other items within items

        // Disconnect each control point connected to a glue point of an item not in items
        controlPoints = item->controlPoints();
        for(auto& controlPoint : qAsConst(controlPoints))
        {
            gluePoint = controlPoint->gluePoint();
            if (gluePoint && !items.contains(gluePoint->item()))
                controlPoint->disconnect();
        }

        // Disconnect each glue point connected to a control point of an item not in items
        gluePoints = item->gluePoints();
        for(auto& gluePoint : qAsConst(gluePoints))
        {
            controlPoints = gluePoint->connections();
            for(auto& controlPoint : qAsConst(controlPoints))
            {
                if (!items.contains(controlPoint->item()))
                    controlPoint->disconnect();
            }
        }
    }
}

void DrawingWidget::maintainItemConnections(const QList<OdgItem*>& items)
{
    QList<OdgGluePoint*> gluePoints;
    QList<OdgControlPoint*> controlPoints;
    OdgItem* targetItem = nullptr;
    QPointF gluePointScenePosition;

    for(auto& item : items)
    {
        gluePoints = item->gluePoints();
        for(auto& gluePoint : qAsConst(gluePoints))
        {
            gluePointScenePosition = item->mapToScene(gluePoint->position());

            controlPoints = gluePoint->connections();
            for(auto& controlPoint : qAsConst(controlPoints))
            {
                targetItem = controlPoint->item();
                if (targetItem && targetItem->mapToScene(controlPoint->position()) != gluePointScenePosition)
                    resizeItem(controlPoint, gluePointScenePosition, false, false, false);
            }
        }
    }
}

//======================================================================================================================

void DrawingWidget::setItemsProperty(const QList<OdgItem*>& items, const QString& name, const QVariant& value)
{
    // Update the items' property
    for(auto& item : items) item->setProperty(name, value);

    // Signal any listeners that current items' property may have changed
    if (items == currentItems())
    {
        if (mMode == Odg::SelectMode) updateSelectionCenter();
        emit currentItemsPropertyChanged(items);
    }

    viewport()->update();
}

void DrawingWidget::setItemsProperty(const QList<OdgItem*>& items, const QString& name,
                                     const QHash<OdgItem*,QVariant>& values)
{
    // Update the items' property
    for(auto& item : items) item->setProperty(name, values.value(item));

    // Signal any listeners that current items' property may have changed
    if (items == currentItems())
    {
        if (mMode == Odg::SelectMode) updateSelectionCenter();
        emit currentItemsPropertyChanged(items);
    }

    viewport()->update();
}

//======================================================================================================================

void DrawingWidget::setSelectedItems(const QList<OdgItem*>& items)
{
    if (mSelectedItems != items)
    {
        // Set previous items as no longer selected
        for(auto& item : qAsConst(mSelectedItems)) item->setSelected(false);

        // Select the new set of items
        mSelectedItems = items;
        for(auto& item : qAsConst(mSelectedItems)) item->setSelected(true);
        updateSelectionCenter();
		updateActions();

        // Signal any listeners that the current items changed
        if (mMode == Odg::SelectMode) emit currentItemsChanged(mSelectedItems);

        viewport()->update();
    }
}

//======================================================================================================================

void DrawingWidget::setScale(double scale)
{
    if (scale > 0)
    {
        const QPoint previousMousePosition = mapFromGlobal(QCursor::pos());
        const QPointF previousMouseScenePosition = mapToScene(previousMousePosition);

        // Update view to the new scale
        updateTransformAndScrollBars(scale);
        emit scaleChanged(scale);

        // Keep mouse cursor on the same position, if possible.  Otherwise keep the center of the scene rect in the
        // center of the view.
        if (viewport()->rect().contains(previousMousePosition))
            mouseCursorOn(previousMouseScenePosition);
        else
            centerOn(pageRect().center());
    }
}

void DrawingWidget::zoomIn()
{
    setScale(scale() * qSqrt(qSqrt(2)));
}

void DrawingWidget::zoomOut()
{
    setScale(scale() / qSqrt(qSqrt(2)));
}

void DrawingWidget::zoomFit()
{
    zoomToRect(QRectF());
}

void DrawingWidget::zoomToRect(const QRectF& rect)
{
    const QRectF finalRect = (rect.width() > 0 && rect.height() > 0) ? rect : pageRect();

    // Update view to the new scale
    const double scaleX = (maximumViewportSize().width() - 3) / finalRect.width();
    const double scaleY = (maximumViewportSize().height() - 3) / finalRect.height();
    const double scale = qMin(scaleX, scaleY);

    updateTransformAndScrollBars(scale);
    emit scaleChanged(scale);

    // Put the center of the rect in the center of the view
    centerOn(finalRect.center());
}

void DrawingWidget::ensureVisible(const QRectF& rect)
{
    if (!visibleRect().contains(rect)) zoomToRect(rect);
}

void DrawingWidget::centerOn(const QPointF& position)
{
    // Scroll the scroll bars so the specified position is in the center of the view
    const QPointF oldPosition = mapToScene(viewport()->rect().center());
    const double scale = this->scale();

    const int horizontalDelta = qRound((position.x() - oldPosition.x()) * scale);
    const int verticalDelta = qRound((position.y() - oldPosition.y()) * scale);

    horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
    verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);

    viewport()->update();
}

void DrawingWidget::mouseCursorOn(const QPointF& position)
{
    // Scroll the scroll bars so the specified position is beneath the mouse cursor
    const QPointF oldPosition = mapToScene(mapFromGlobal(QCursor::pos()));
    const double scale = this->scale();

    const int horizontalDelta = qRound((position.x() - oldPosition.x()) * scale);
    const int verticalDelta = qRound((position.y() - oldPosition.y()) * scale);

    horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
    verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);

    viewport()->update();
}

//======================================================================================================================

void DrawingWidget::setSelectMode()
{
    if (mMode != Odg::SelectMode)
    {
        // Assume select mode state already clear

        // Clear place mode state
        if (!mPlaceItems.isEmpty()) emit currentItemsChanged(QList<OdgItem*>());
        qDeleteAll(mPlaceItems);
        mPlaceItems.clear();

        // Set up select mode
        mMode = Odg::SelectMode;
        setCursor(QCursor(Qt::ArrowCursor));

        // Notify listeners of mode change
        QAction* selectAction = actions().at(SelectModeAction);
        if (!selectAction->isChecked()) selectAction->setChecked(true);

        emit modeChanged(static_cast<int>(mMode));
        emit modeTextChanged("Select Mode");

        viewport()->update();
    }
}

void DrawingWidget::setScrollMode()
{
    if (mMode != Odg::ScrollMode)
    {
        // Clear select mode state
        selectNone();
        mSelectMouseDownItem = nullptr;
        mSelectMouseDownPoint = nullptr;
        mSelectFocusItem = nullptr;
		mSelectMoveItemsInitialPositions.clear();

        // Clear place mode state
        if (!mPlaceItems.isEmpty()) emit currentItemsChanged(QList<OdgItem*>());
        qDeleteAll(mPlaceItems);
        mPlaceItems.clear();

        // Set up scroll mode
        mMode = Odg::ScrollMode;
        setCursor(QCursor(Qt::OpenHandCursor));

        // Notify listeners of mode change
        emit modeChanged(static_cast<int>(mMode));
        emit modeTextChanged("Scroll Mode");

        viewport()->update();
    }
}

void DrawingWidget::setZoomMode()
{
    if (mMode != Odg::ZoomMode)
    {
        selectNone();
        mSelectMouseDownItem = nullptr;
        mSelectMouseDownPoint = nullptr;
        mSelectFocusItem = nullptr;
		mSelectMoveItemsInitialPositions.clear();

        // Clear place mode state
        if (!mPlaceItems.isEmpty()) emit currentItemsChanged(QList<OdgItem*>());
        qDeleteAll(mPlaceItems);
        mPlaceItems.clear();

        // Set up zoom mode
        mMode = Odg::ZoomMode;
        setCursor(QCursor(Qt::CrossCursor));

        // Notify listeners of mode change
        emit modeChanged(static_cast<int>(mMode));
        emit modeTextChanged("Zoom Mode");

        viewport()->update();
    }
}

void DrawingWidget::setPlaceMode(const QList<OdgItem*>& items, bool placeByMousePressAndRelease)
{
    if (!items.isEmpty() && (mMode != Odg::PlaceMode || mPlaceItems != items))
    {
        // Clear select mode state
        selectNone();
        mSelectMouseDownItem = nullptr;
        mSelectMouseDownPoint = nullptr;
        mSelectFocusItem = nullptr;
		mSelectMoveItemsInitialPositions.clear();

        // Clear place mode state
        if (!mPlaceItems.isEmpty()) emit currentItemsChanged(QList<OdgItem*>());
        qDeleteAll(mPlaceItems);
        mPlaceItems.clear();

        // Set up place mode
        mMode = Odg::PlaceMode;
        setCursor(QCursor(Qt::CrossCursor));

        mPlaceItems = items;
        mPlaceByMousePressAndRelease = placeByMousePressAndRelease;

        // Center the place items under the mouse cursor
        const QPointF centerPosition = roundToGrid(itemsCenter(mPlaceItems));
        const QPointF deltaPosition = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())) - centerPosition);
        for(auto& item : items)
            item->setPosition(item->position() + deltaPosition);

        // Notify listeners of mode change
        emit modeChanged(static_cast<int>(mMode));
        emit modeTextChanged("Place Mode");
        emit currentItemsChanged(mPlaceItems);

        viewport()->update();
    }
}

//======================================================================================================================

void DrawingWidget::undo()
{
    if (mMode == Odg::SelectMode) mUndoStack.undo();
    else setSelectMode();
}

void DrawingWidget::redo()
{
    if (mMode == Odg::SelectMode) mUndoStack.redo();
    else setSelectMode();
}

//======================================================================================================================

void DrawingWidget::setDrawingProperty(const QString& name, const QVariant& value)
{
    if (mCurrentPage) mUndoStack.push(new DrawingSetPropertyCommand(this, name, value));
}

//======================================================================================================================

void DrawingWidget::insertPage()
{
    // Determine a unique name for the new page
    QString name;
    bool nameIsUnique = false;

    while (!nameIsUnique)
    {
        mNewPageCount++;
        name = "Page " + QString::number(mNewPageCount);

        nameIsUnique = true;
        for(const auto& page : qAsConst(mPages))
        {
            if (name == page->name())
            {
                nameIsUnique = false;
                break;
            }
        }
    }

    // Create the new page and add it to the view
    OdgPage* newPage = new OdgPage(name);
    mUndoStack.push(new DrawingInsertPageCommand(this, newPage, currentPageIndex() + 1));
    zoomFit();
}

void DrawingWidget::duplicatePage()
{
    if (mCurrentPage)
    {
        // Determine a unique name for the new page
        QString name;
        bool nameIsUnique = false;
        int newPageCount = 1;

        while (!nameIsUnique)
        {
            newPageCount++;
            name = mCurrentPage->name() + " (" + QString::number(newPageCount) + ")";

            nameIsUnique = true;
            for(const auto& page : qAsConst(mPages))
            {
                if (name == page->name())
                {
                    nameIsUnique = false;
                    break;
                }
            }
        }

        // Create the new page and add it to the view
        OdgPage* newPage = new OdgPage(name);

        QList<OdgItem*> copiedItems = OdgItem::copyItems(mCurrentPage->items());
        for(auto& item : copiedItems) newPage->addItem(item);

        mUndoStack.push(new DrawingInsertPageCommand(this, newPage, currentPageIndex() + 1));
        zoomFit();
    }
}

void DrawingWidget::removePage()
{
    if (mCurrentPage) mUndoStack.push(new DrawingRemovePageCommand(this, mCurrentPage));
}

void DrawingWidget::movePage(int index)
{
    if (mCurrentPage) mUndoStack.push(new DrawingMovePageCommand(this, mCurrentPage, index));
}

//======================================================================================================================

void DrawingWidget::setCurrentPage(OdgPage* page)
{
    if (page != mCurrentPage)
    {
        mCurrentPage = page;
        emit currentPageChanged(mCurrentPage);
        emit currentPageIndexChanged(currentPageIndex());
        viewport()->update();
    }
}

void DrawingWidget::setCurrentPageIndex(int index)
{
    setCurrentPage((0 <= index && index < mPages.size()) ? mPages.at(index) : nullptr);
}

//======================================================================================================================

void DrawingWidget::setPageProperty(const QString& name, const QVariant& value)
{
    if (mCurrentPage) mUndoStack.push(new DrawingSetPagePropertyCommand(this, mCurrentPage, name, value));
}

void DrawingWidget::renamePage(const QString& name)
{
    setPageProperty("name", name);
}

//======================================================================================================================

void DrawingWidget::removeItems()
{
    if (mCurrentPage && mMode == Odg::SelectMode)
    {
        if (!mSelectedItems.isEmpty())
            mUndoStack.push(new DrawingRemoveItemsCommand(this, mCurrentPage, mSelectedItems));
    }
    else setSelectMode();
}

//======================================================================================================================

void DrawingWidget::cut()
{
    copy();
    removeItems();
}

void DrawingWidget::copy()
{
    if (mCurrentPage && mMode == Odg::SelectMode && !mSelectedItems.isEmpty())
    {
        OdgWriter writer;
        writer.setUnits(mUnits);
        writer.setPageSize(mPageSize);
        writer.setPageMargins(mPageMargins);
        writer.setBackgroundColor(mBackgroundColor);
        writer.setGrid(mGrid);
        writer.setGridStyle(mGridStyle);
        writer.setGridColor(mGridColor);
        writer.setGridSpacingMajor(mGridSpacingMajor);
        writer.setGridSpacingMinor(mGridSpacingMinor);

        writer.setDefaultStyle(mDefaultStyle);

        OdgPage* page = new OdgPage(mCurrentPage->name());
        for(auto& item : qAsConst(mSelectedItems))
            page->addItem(item);
        writer.setPages(QList<OdgPage*>(1, page));

        writer.writeToClipboard();
    }
}

void DrawingWidget::paste()
{
    if (mCurrentPage && mMode == Odg::SelectMode)
    {
        OdgReader reader;
        reader.readFromClipboard();

        const QList<OdgPage*> pages = reader.takePages();
        if (!pages.isEmpty())
        {
            OdgPage* page = pages.first();

            const QList<OdgItem*> items = page->items();
            for(auto& item : qAsConst(items))
                page->removeItem(item);

            if (mUnits != reader.units())
            {
                double scaleFactor = Odg::convertUnits(1.0, reader.units(), mUnits);
                for(auto& item : qAsConst(items))
                    item->scaleBy(scaleFactor);
            }

            if (!items.isEmpty())
                setPlaceMode(items, false);
        }

        qDeleteAll(pages);
    }
}

//======================================================================================================================

void DrawingWidget::selectAll()
{
    if (mCurrentPage && mMode == Odg::SelectMode) setSelectedItems(mCurrentPage->items());
}

void DrawingWidget::selectNone()
{
    if (mMode == Odg::SelectMode) setSelectedItems(QList<OdgItem*>());
}

//======================================================================================================================

void DrawingWidget::move(const QPointF& position)
{
    if (mMode == Odg::SelectMode && mSelectedItems.size() == 1)
    {
        QHash<OdgItem*,QPointF> newPositions;
        newPositions.insert(mSelectedItems.first(), position);
        mUndoStack.push(new DrawingMoveItemsCommand(this, mSelectedItems, newPositions, true));
    }
}

void DrawingWidget::moveDelta(const QPointF& delta)
{
    if (mMode == Odg::SelectMode && !mSelectedItems.isEmpty())
    {
        QHash<OdgItem*,QPointF> newPositions;
        for(auto& item : mSelectedItems)
            newPositions.insert(item, item->position() + delta);
        mUndoStack.push(new DrawingMoveItemsCommand(this, mSelectedItems, newPositions, true));
    }
}

void DrawingWidget::resize(OdgControlPoint* point, const QPointF& position)
{
    if (point && mMode == Odg::SelectMode && mSelectedItems.size() == 1)
        mUndoStack.push(new DrawingResizeItemCommand(this, point, position, false, true));
}

void DrawingWidget::resize2(OdgControlPoint* point1, const QPointF& p1, OdgControlPoint* point2, const QPointF& p2)
{
    if (point1 && point2 && mMode == Odg::SelectMode && mSelectedItems.size() == 1)
        mUndoStack.push(new DrawingResizeItem2Command(this, point1, p1, point2, p2, true));
}

//======================================================================================================================

void DrawingWidget::rotate()
{
    if (mMode == Odg::SelectMode)
    {
        if (!mSelectedItems.isEmpty())
            mUndoStack.push(new DrawingRotateItemsCommand(this, mSelectedItems, mSelectedItemsCenter));
    }
    else if (mMode == Odg::PlaceMode)
    {
        if (!mPlaceByMousePressAndRelease)
        {
            QPointF placeItemsCenter = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())));
            rotateItems(mPlaceItems, placeItemsCenter);
        }
    }
}

void DrawingWidget::rotateBack()
{
    if (mMode == Odg::SelectMode)
    {
        if (!mSelectedItems.isEmpty())
            mUndoStack.push(new DrawingRotateBackItemsCommand(this, mSelectedItems, mSelectedItemsCenter));
    }
    else if (mMode == Odg::PlaceMode)
    {
        if (!mPlaceByMousePressAndRelease)
        {
            QPointF placeItemsCenter = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())));
            rotateBackItems(mPlaceItems, placeItemsCenter);
        }
    }
}

void DrawingWidget::flipHorizontal()
{
    if (mMode == Odg::SelectMode)
    {
        if (!mSelectedItems.isEmpty())
            mUndoStack.push(new DrawingFlipItemsHorizontalCommand(this, mSelectedItems, mSelectedItemsCenter));
    }
    else if (mMode == Odg::PlaceMode)
    {
        if (!mPlaceByMousePressAndRelease)
        {
            QPointF placeItemsCenter = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())));
            flipItemsHorizontal(mPlaceItems, placeItemsCenter);
        }
    }
}

void DrawingWidget::flipVertical()
{
    if (mMode == Odg::SelectMode)
    {
        if (!mSelectedItems.isEmpty())
            mUndoStack.push(new DrawingFlipItemsVerticalCommand(this, mSelectedItems, mSelectedItemsCenter));
    }
    else if (mMode == Odg::PlaceMode)
    {
        if (!mPlaceByMousePressAndRelease)
        {
            QPointF placeItemsCenter = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())));
            flipItemsVertical(mPlaceItems, placeItemsCenter);
        }
    }
}

//======================================================================================================================

void DrawingWidget::bringForward()
{
    if (mCurrentPage && mMode == Odg::SelectMode && !mSelectedItems.isEmpty())
    {
        QList<OdgItem*> itemsOrdered = mCurrentPage->items();
        int itemIndex = -1;

        for(auto rIter = mSelectedItems.rbegin(); rIter != mSelectedItems.rend(); rIter++)
        {
            itemIndex = itemsOrdered.indexOf(*rIter);
            if (0 <= itemIndex && itemIndex < itemsOrdered.size() - 1)
            {
                itemsOrdered.remove(itemIndex);
                itemsOrdered.insert(itemIndex + 1, *rIter);
            }
        }

        mUndoStack.push(new DrawingReorderItemsCommand(this, mCurrentPage, itemsOrdered));
    }
}

void DrawingWidget::sendBackward()
{
    if (mCurrentPage && mMode == Odg::SelectMode && !mSelectedItems.isEmpty())
    {
        QList<OdgItem*> itemsOrdered = mCurrentPage->items();
        int itemIndex = -1;

        for(auto iter = mSelectedItems.begin(); iter != mSelectedItems.end(); iter++)
        {
            itemIndex = itemsOrdered.indexOf(*iter);
            if (0 < itemIndex && itemIndex < itemsOrdered.size())
            {
                itemsOrdered.remove(itemIndex);
                itemsOrdered.insert(itemIndex - 1, *iter);
            }
        }

        mUndoStack.push(new DrawingReorderItemsCommand(this, mCurrentPage, itemsOrdered));
    }
}

void DrawingWidget::bringToFront()
{
    if (mCurrentPage && mMode == Odg::SelectMode && !mSelectedItems.isEmpty())
    {
        QList<OdgItem*> itemsOrdered = mCurrentPage->items();
        int itemIndex = -1;

        for(auto iter = mSelectedItems.begin(); iter != mSelectedItems.end(); iter++)
        {
            itemIndex = itemsOrdered.indexOf(*iter);
            if (0 <= itemIndex && itemIndex < itemsOrdered.size() - 1)
            {
                itemsOrdered.remove(itemIndex);
                itemsOrdered.append(*iter);
            }
        }

        mUndoStack.push(new DrawingReorderItemsCommand(this, mCurrentPage, itemsOrdered));
    }
}

void DrawingWidget::sendToBack()
{
    if (mCurrentPage && mMode == Odg::SelectMode && !mSelectedItems.isEmpty())
    {
        QList<OdgItem*> itemsOrdered = mCurrentPage->items();
        int itemIndex = -1;

        for(auto rIter = mSelectedItems.rbegin(); rIter != mSelectedItems.rend(); rIter++)
        {
            itemIndex = itemsOrdered.indexOf(*rIter);
            if (0 < itemIndex && itemIndex < itemsOrdered.size())
            {
                itemsOrdered.remove(itemIndex);
                itemsOrdered.prepend(*rIter);
            }
        }

        mUndoStack.push(new DrawingReorderItemsCommand(this, mCurrentPage, itemsOrdered));
    }
}

//======================================================================================================================

void DrawingWidget::group()
{
    if (mCurrentPage && mMode == Odg::SelectMode && mSelectedItems.size() > 1)
    {
        QList<OdgItem*> itemsToGroup;
        for(auto& item : qAsConst(mSelectedItems))
        {
            if (mCurrentPage->items().contains(item))
                itemsToGroup.append(item);
        }

        if (!itemsToGroup.isEmpty())
            mUndoStack.push(new DrawingGroupItemsCommand(this, mCurrentPage, itemsToGroup));
    }
}

void DrawingWidget::ungroup()
{
    if (mCurrentPage && mMode == Odg::SelectMode && mSelectedItems.size() == 1)
    {
        OdgGroupItem* groupItem = dynamic_cast<OdgGroupItem*>(mSelectedItems.first());
        if (groupItem && mCurrentPage->items().contains(groupItem))
            mUndoStack.push(new DrawingUngroupItemsCommand(this, mCurrentPage, groupItem));
    }
}

//======================================================================================================================

void DrawingWidget::insertPoint()
{
    if (mCurrentPage && mMode == Odg::SelectMode && mSelectedItems.size() == 1)
    {
        OdgItem* item = mSelectedItems.first();
        if (item->canInsertPoints())
        {
            int insertIndex = item->insertPointIndex(mMouseButtonDownScenePosition);
            if (insertIndex >= 0)
                mUndoStack.push(new DrawingInsertPointCommand(this, item, insertIndex, new OdgControlPoint()));
        }
    }
}

void DrawingWidget::removePoint()
{
    if (mCurrentPage && mMode == Odg::SelectMode && mSelectedItems.size() == 1)
    {
        OdgItem* item = mSelectedItems.first();
        if (item->canRemovePoints())
        {
            int removeIndex = item->removePointIndex(mMouseButtonDownScenePosition);
            if (0 <= removeIndex && removeIndex < item->controlPoints().size())
                mUndoStack.push(new DrawingRemovePointCommand(this, item, item->controlPoints().at(removeIndex)));
        }
    }
}

//======================================================================================================================

void DrawingWidget::setItemsProperty(const QString& name, const QVariant& value)
{
    if (mMode == Odg::SelectMode)
    {
        if (!mSelectedItems.isEmpty())
            mUndoStack.push(new DrawingSetItemsPropertyCommand(this, mSelectedItems, name, value));
    }
    else if (mMode == Odg::PlaceMode)
    {
        if (!mPlaceItems.isEmpty())
            setItemsProperty(mPlaceItems, name, value);
    }
}

void DrawingWidget::setDefaultStyleProperty(const QString& name, const QVariant& value)
{
    if (mDefaultStyle)
    {
        if (name == "penStyle" && value.canConvert<int>())
            mDefaultStyle->setPenStyle(static_cast<Qt::PenStyle>(value.toInt()));
        else if (name == "penWidth" && value.canConvert<double>())
            mDefaultStyle->setPenWidth(value.toDouble());
        else if (name == "penColor" && value.canConvert<QColor>())
            mDefaultStyle->setPenColor(value.value<QColor>());
        else if (name == "brushColor" && value.canConvert<QColor>())
            mDefaultStyle->setBrushColor(value.value<QColor>());
        else if (name == "startMarkerStyle" && value.canConvert<int>())
            mDefaultStyle->setStartMarkerStyle(static_cast<Odg::MarkerStyle>(value.toInt()));
        else if (name == "startMarkerSize" && value.canConvert<double>())
            mDefaultStyle->setStartMarkerSize(value.toDouble());
        else if (name == "endMarkerStyle" && value.canConvert<int>())
            mDefaultStyle->setEndMarkerStyle(static_cast<Odg::MarkerStyle>(value.toInt()));
        else if (name == "endMarkerSize" && value.canConvert<double>())
            mDefaultStyle->setEndMarkerSize(value.toDouble());
        else if (name == "fontFamily" && value.canConvert<QString>())
            mDefaultStyle->setFontFamily(value.toString());
        else if (name == "fontSize" && value.canConvert<double>())
            mDefaultStyle->setFontSize(value.toDouble());
        else if (name == "fontStyle" && value.canConvert<OdgFontStyle>())
            mDefaultStyle->setFontStyle(value.value<OdgFontStyle>());
        else if (name == "textAlignment" && value.canConvert<int>())
            mDefaultStyle->setTextAlignment(static_cast<Qt::Alignment>(value.toInt()));
        else if (name == "textPadding" && value.canConvert<QSizeF>())
            mDefaultStyle->setTextPadding(value.value<QSizeF>());
        else if (name == "textColor" && value.canConvert<QColor>())
            mDefaultStyle->setTextColor(value.value<QColor>());
    }
}

//======================================================================================================================

void DrawingWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(viewport());
    painter.setBrush(palette().brush(QPalette::Dark));
    painter.setPen(QPen(Qt::NoPen));
    painter.drawRect(rect());

    if (mCurrentPage)
    {
        painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
        painter.setTransform(mTransform, true);

        paint(painter);

        switch (mMode)
        {
        case Odg::SelectMode:
            drawItemPoints(painter, mSelectedItems);
            drawHotpoints(painter, mSelectedItems);
            drawRubberBand(painter, mSelectRubberBandRect);
            break;
        case Odg::ScrollMode:
            break;
        case Odg::ZoomMode:
            drawRubberBand(painter, mZoomRubberBandRect);
            break;
        case Odg::PlaceMode:
            drawItems(painter, mPlaceItems);
            drawHotpoints(painter, mPlaceItems);
            break;
        }
    }
}

void DrawingWidget::drawBackground(QPainter& painter, bool drawBorder, bool drawGrid)
{
    const QColor backgroundColor = this->backgroundColor();
    const QColor pageBorderColor(255 - backgroundColor.red(), 255 - backgroundColor.green(), 255 - backgroundColor.blue());
    const QColor contentBorderColor(128, 128, 128);

    // Fill background
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, false);
    painter.setBackground(QBrush(backgroundColor));
    painter.setBrush(QBrush(backgroundColor));
    if (drawBorder)
        painter.setPen(QPen(Qt::NoPen));
    else
        painter.setPen(QPen(QBrush(pageBorderColor), 0));
    painter.drawRect(pageRect());

    // Draw content border
    if (drawBorder)
    {
        painter.setBrush(QBrush(Qt::transparent));
        painter.setPen(QPen(QBrush(contentBorderColor), 0));
        painter.drawRect(contentRect());
    }

    // Draw grid
    if (drawGrid && grid() > 0)
    {
        const QColor gridColor = this->gridColor();
        const QColor minorGridColor(gridColor.red(), gridColor.green(), gridColor.blue(), gridColor.alpha() / 3);

        switch (gridStyle())
        {
        case Odg::GridLines:
            // Minor and Major grid lines
            if (gridSpacingMinor() > 0)
                drawGridLines(painter, minorGridColor, gridSpacingMinor());
            if (gridSpacingMajor() > 0)
                drawGridLines(painter, gridColor, gridSpacingMajor());

            // Draw content border again
            painter.setBrush(QBrush(Qt::transparent));
            painter.setPen(QPen(QBrush(gridColor), 0));
            painter.drawRect(contentRect());
            break;
        default:    // Odg::GridHidden
            break;
        }
    }
}

void DrawingWidget::drawGridLines(QPainter& painter, const QColor& color, int spacing)
{
    const QRectF gridRect = contentRect();

    painter.setPen(QPen(QBrush(color), 0, Qt::SolidLine));

    const double gridInterval = grid() * spacing;
    const int gridLeftIndex = qCeil(gridRect.left() / gridInterval);
    const int gridRightIndex = qFloor(gridRect.right() / gridInterval);
    const int gridTopIndex = qCeil(gridRect.top() / gridInterval);
    const int gridBottomIndex = qFloor(gridRect.bottom() / gridInterval);

    double x = 0, y = 0;
    for(int xIndex = gridLeftIndex; xIndex <= gridRightIndex; xIndex++)
    {
        x = xIndex * gridInterval;
        painter.drawLine(QLineF(x, gridRect.top(), x, gridRect.bottom()));
    }

    for(int yIndex = gridTopIndex; yIndex <= gridBottomIndex; yIndex++)
    {
        y = yIndex * gridInterval;
        painter.drawLine(QLineF(gridRect.left(), y, gridRect.right(), y));
    }
}

void DrawingWidget::drawItems(QPainter& painter, const QList<OdgItem*>& items)
{
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);

    for(auto& item : items)
    {
        painter.setTransform(item->transform(), true);
        item->paint(painter);
        painter.setTransform(item->transformInverse(), true);
    }
}

void DrawingWidget::drawItemPoints(QPainter& painter, const QList<OdgItem*>& items)
{
    QColor borderColor(255 - mBackgroundColor.red(), 255 - mBackgroundColor.green(), 255 - mBackgroundColor.blue());

    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, false);
    painter.setPen(QPen(borderColor, 0));

    // Draw glue points
    painter.setBrush(QColor(255, 255, 0));

    QList<OdgGluePoint*> gluePoints;
    for(auto& item : items)
    {
        gluePoints = item->gluePoints();
        for(auto& gluePoint : qAsConst(gluePoints))
            painter.drawEllipse(pointRect(gluePoint));
    }

    // Draw control points
    painter.setBrush(QColor(0, 224, 0));

    QList<OdgControlPoint*> controlPoints;
    for(auto& item : items)
    {
        controlPoints = item->controlPoints();
        for(auto& controlPoint : qAsConst(controlPoints))
            painter.drawRect(pointRect(controlPoint));
    }
}

void DrawingWidget::drawHotpoints(QPainter& painter, const QList<OdgItem*>& items)
{
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, false);
    painter.setBrush(QBrush(QColor(255, 128, 0, 192)));
    painter.setPen(QPen(Qt::NoPen));

    if (mCurrentPage)
    {
        QList<OdgGluePoint*> currentPageItemGluePoints;
        QList<OdgControlPoint*> controlPoints;
        QRectF rect;

        const QList<OdgItem*> currentPageItems = mCurrentPage->items();
        for(auto& currentPageItem : currentPageItems)
        {
            // Only consider connections to items in the current page that aren't part of items or mPlaceItems
            if (!items.contains(currentPageItem) && !mPlaceItems.contains(currentPageItem))
            {
                currentPageItemGluePoints = currentPageItem->gluePoints();
                for(auto& currentPageItemGluePoint : qAsConst(currentPageItemGluePoints))
                {
                    for(auto& item : items)
                    {
                        controlPoints = item->controlPoints();
                        for(auto& controlPoint : qAsConst(controlPoints))
                        {
                            if (shouldConnect(controlPoint, currentPageItemGluePoint))
                            {
                                rect = pointRect(controlPoint);
                                rect.adjust(-rect.width(), -rect.height(), rect.width(), rect.height());
                                painter.drawEllipse(rect);
                            }
                        }
                    }
                }
            }
        }
    }
}

void DrawingWidget::drawRubberBand(QPainter& painter, const QRect& rect)
{
    if (rect.width() > 0 && rect.height() > 0)
    {
        painter.save();
        painter.resetTransform();
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);

        QStyleOptionRubberBand option;
        option.initFrom(viewport());
        option.rect = rect;
        option.shape = QRubberBand::Rectangle;

        QStyleHintReturnMask mask;
        if (viewport()->style()->styleHint(QStyle::SH_RubberBand_Mask, &option, viewport(), &mask))
            painter.setClipRegion(mask.region, Qt::IntersectClip);

        viewport()->style()->drawControl(QStyle::CE_RubberBand, &option, &painter, viewport());

        painter.restore();
    }
}

//======================================================================================================================

void DrawingWidget::resizeEvent(QResizeEvent* event)
{
    QAbstractScrollArea::resizeEvent(event);
    updateTransformAndScrollBars();
}

void DrawingWidget::updateTransformAndScrollBars(double scale)
{
    if (scale <= 0) scale = this->scale();

    const QRectF pageRect = this->pageRect();
    const int contentWidth = qRound(pageRect.width() * scale) + 1;
    const int contentHeight = qRound(pageRect.height() * scale) + 1;

    int viewportWidth = maximumViewportSize().width();
    int viewportHeight = maximumViewportSize().height();

    const int scrollBarExtent = style()->pixelMetric(QStyle::PM_ScrollBarExtent, nullptr, this);
    if (contentWidth > viewportWidth && verticalScrollBarPolicy() == Qt::ScrollBarAsNeeded)
        viewportWidth -= scrollBarExtent;
    if (contentHeight > viewportHeight && horizontalScrollBarPolicy() == Qt::ScrollBarAsNeeded)
        viewportHeight -= scrollBarExtent;

    // Update transform
    mTransform.reset();
    mTransform.translate(-pageRect.left() * scale, -pageRect.top() * scale);
    if (contentWidth < viewportWidth)
        mTransform.translate(-(pageRect.width() * scale - viewportWidth + 1) / 2, 0);
    if (contentHeight < viewportHeight)
        mTransform.translate(0, -(pageRect.height() * scale - viewportHeight + 1) / 2);
    mTransform.scale(scale, scale);

    mTransformInverse = mTransform.inverted();

    // Update scroll bars
    if (contentWidth > viewportWidth)
    {
        horizontalScrollBar()->setRange(-1, contentWidth - viewportWidth + 1);
        horizontalScrollBar()->setSingleStep(qRound(viewportWidth / 80.0));
        horizontalScrollBar()->setPageStep(viewportWidth);
    }
    else horizontalScrollBar()->setRange(0, 0);

    if (contentHeight > viewportHeight)
    {
        verticalScrollBar()->setRange(-1, contentHeight - viewportHeight + 1);
        verticalScrollBar()->setSingleStep(qRound(viewportHeight / 80.0));
        verticalScrollBar()->setPageStep(viewportHeight);
    }
    else verticalScrollBar()->setRange(0, 0);
}

//======================================================================================================================

void DrawingWidget::mousePressEvent(QMouseEvent* event)
{
    if (event && mMouseState == MouseIdle)
    {
        mMouseButtonDownPosition = event->pos();
        mMouseButtonDownScenePosition = mapToScene(mMouseButtonDownPosition);

        if (event->button() == Qt::LeftButton)
        {
            mMouseState = HandlingLeftButtonEvent;

            // Handle the left mouse press event depending on the current mode
            switch (mMode)
            {
            case Odg::SelectMode:
                selectModeLeftMousePressEvent(event);
                break;
            case Odg::ScrollMode:
                scrollModeLeftMousePressEvent(event);
                break;
            case Odg::ZoomMode:
                zoomModeLeftMousePressEvent(event);
                break;
            case Odg::PlaceMode:
                placeModeLeftMousePressEvent(event);
                break;
            };
        }
        else if (event->button() == Qt::RightButton)
        {
            mMouseState = HandlingRightButtonEvent;

            // Handle the right mouse press event depending on the current mode. Modes other than SelectMode ignore
            // this event.
            if (mMode == Odg::SelectMode) selectModeRightMousePressEvent(event);
        }
        else if (event->button() == Qt::MiddleButton)
        {
            mMouseState = HandlingMiddleButtonEvent;

            // Prepare for mouse pan events
            mPanOriginalCursor = cursor().shape();
            setCursor(QCursor(Qt::SizeAllCursor));

            mPanStartPosition = mMouseButtonDownPosition;
            mPanCurrentPosition = mMouseButtonDownPosition;
            mPanTimer.start();
        }
    }
}

void DrawingWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    mousePressEvent(event);
}

void DrawingWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event)
    {
        if ((event->buttons() & Qt::LeftButton) and mMouseState == HandlingLeftButtonEvent)
        {
            const double dragDistance = (mMouseButtonDownPosition - event->pos()).manhattanLength();
            mMouseDragged = (mMouseDragged || dragDistance >= QApplication::startDragDistance());
            if (mMouseDragged)
            {
                // Handle the left mouse drag event depending on the current mode
                switch (mMode)
                {
                case Odg::SelectMode:
                    selectModeLeftMouseDragEvent(event);
                    break;
                case Odg::ScrollMode:
                    scrollModeLeftMouseDragEvent(event);
                    break;
                case Odg::ZoomMode:
                    zoomModeLeftMouseDragEvent(event);
                    break;
                case Odg::PlaceMode:
                    placeModeLeftMouseDragEvent(event);
                    break;
                };
            }
        }
        else if ((event->buttons() & Qt::RightButton) and mMouseState == HandlingRightButtonEvent)
        {
            // In all modes, right mouse drag events are ignored
        }
        else if ((event->buttons() & Qt::MiddleButton) and mMouseState == HandlingMiddleButtonEvent)
        {
            // Update current position for mouse pan events
            mPanCurrentPosition = event->pos();
        }
        else
        {
            // Handle the left mouse move event (with no buttons held down) depending on the current mode
            switch (mMode)
            {
            case Odg::SelectMode:
                selectModeNoButtonMouseMoveEvent(event);
                break;
            case Odg::ScrollMode:
                scrollModeNoButtonMouseMoveEvent(event);
                break;
            case Odg::ZoomMode:
                zoomModeNoButtonMouseMoveEvent(event);
                break;
            case Odg::PlaceMode:
                placeModeNoButtonMouseMoveEvent(event);
                break;
            };
        }
    }
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event)
    {
        if ((event->button() == Qt::LeftButton) and mMouseState == HandlingLeftButtonEvent)
        {
            // Handle the left mouse release event depending on the current mode
            switch (mMode)
            {
            case Odg::SelectMode:
                selectModeLeftMouseReleaseEvent(event);
                break;
            case Odg::ScrollMode:
                scrollModeLeftMouseReleaseEvent(event);
                break;
            case Odg::ZoomMode:
                zoomModeLeftMouseReleaseEvent(event);
                break;
            case Odg::PlaceMode:
                placeModeLeftMouseReleaseEvent(event);
                break;
            };

            mMouseState = MouseIdle;
            mMouseDragged = false;
        }
        else if ((event->button() == Qt::RightButton) and mMouseState == HandlingRightButtonEvent)
        {
            // Handle the right mouse press event depending on the current mode. Modes other than SelectMode will
            // trigger the view to go to SelectMode.
            if (mMode == Odg::SelectMode)
                selectModeRightMouseReleaseEvent(event);
            else
                setSelectMode();

            mMouseState = MouseIdle;
        }
        else if ((event->button() == Qt::MiddleButton) and mMouseState == HandlingMiddleButtonEvent)
        {
            // Stop mouse pan events
            setCursor(QCursor(mPanOriginalCursor));
            mPanTimer.stop();

            mMouseState = MouseIdle;
        }
    }
}

void DrawingWidget::wheelEvent(QWheelEvent* event)
{
    if (event && (event->modifiers() & Qt::ControlModifier))
    {
        // Zoom in or out on a mouse wheel vertical event if the control key is held down.
        if (event->angleDelta().y() > 0)
            zoomIn();
        else if (event->angleDelta().y() < 0)
            zoomOut();
    }
    else QAbstractScrollArea::wheelEvent(event);
}

//======================================================================================================================

void DrawingWidget::selectModeNoButtonMouseMoveEvent(QMouseEvent* event)
{
	emit mouseInfoChanged(createMouseInfo(mapToScene(event->pos())));
}

void DrawingWidget::selectModeLeftMousePressEvent(QMouseEvent* event)
{
	mSelectMouseState = SelectMouseSelectItem;

	// Update mouse down item
	mSelectMouseDownItem = itemAt(mMouseButtonDownScenePosition);

	// Determine if the user clicked on one of the mouse down item's control points
	mSelectMouseDownPoint = nullptr;
	if (mSelectMouseDownItem && mSelectMouseDownItem->isSelected() && mSelectedItems.size() == 1)
	{
		const QList<OdgControlPoint*> controlPoints = mSelectMouseDownItem->controlPoints();
		for(auto& controlPoint : controlPoints)
		{
			if (pointRect(controlPoint).contains(mMouseButtonDownScenePosition))
			{
				mSelectMouseDownPoint = controlPoint;
				break;
			}
		}
	}

	// Update focus item
    mSelectFocusItem = mSelectMouseDownItem;

	viewport()->update();
}

void DrawingWidget::selectModeLeftMouseDragEvent(QMouseEvent* event)
{
	switch (mSelectMouseState)
	{
	case SelectMouseSelectItem:
		// If we clicked on a control point within a selected item and the item can be resized, then resize the item.
		// Otherwise, if we clicked on a selected item, move the item.  Otherwise we didn't click on a
		// selected item, so start drawing a rubber band for item selection.
		if (mSelectMouseDownItem && mSelectMouseDownItem->isSelected())
		{
			bool canResize = (mSelectMouseDownPoint && mSelectedItems.size() == 1 &&
							  mSelectMouseDownItem->controlPoints().size() > 1);
			if (canResize)
			{
				mSelectMouseState = SelectMouseResizeItem;
				selectModeResizeItemStartEvent(event);
			}
			else
			{
				mSelectMouseState = SelectMouseMoveItems;
				selectModeMoveItemsStartEvent(event);
			}
		}
		else
		{
			mSelectMouseState = SelectMouseRubberBand;
			selectModeRubberBandStartEvent(event);
		}
		break;
	case SelectMouseMoveItems:
		selectModeMoveItemsUpdateEvent(event);		// Move the selected items within the scene
		break;
	case SelectMouseResizeItem:
		selectModeResizeItemUpdateEvent(event);		// Resize the selected item within the scene
		break;
	case SelectMouseRubberBand:
		selectModeRubberBandUpdateEvent(event);		// Update the rubber band rect to be used for item selection
		break;
	default:	// SelectMouseIdle
		// Do nothing.
		break;
	}
}

void DrawingWidget::selectModeLeftMouseReleaseEvent(QMouseEvent* event)
{
	switch (mSelectMouseState)
	{
	case SelectMouseSelectItem:
		selectModeSingleSelectEvent(event);			// Select or deselect single item as needed
		break;
	case SelectMouseMoveItems:
		selectModeMoveItemsEndEvent(event);			// Finish moving the selected items within the scene
		break;
	case SelectMouseResizeItem:
		selectModeResizeItemEndEvent(event);		// Finish resizing the selected item within the scene
		break;
	case SelectMouseRubberBand:
		selectModeRubberBandEndEvent(event);		// Select all the items under the rubber band rect
		break;
	default:	// SelectMouseIdle
		// Do nothing.
		break;
	}

	mSelectMouseState = SelectMouseIdle;
	mSelectMouseDownItem = nullptr;
	mSelectMouseDownPoint = nullptr;
}

void DrawingWidget::selectModeRightMousePressEvent(QMouseEvent* event)
{
	// Update mouse down item
	mSelectMouseDownItem = itemAt(mMouseButtonDownScenePosition);
}

void DrawingWidget::selectModeRightMouseReleaseEvent(QMouseEvent* event)
{
    // Show context menu depending on whether or not the right-click occurred on a selected item
    // and if so, what kind of item it was.
    if (mCurrentPage && mMode == Odg::SelectMode)
    {
        if (mSelectMouseDownItem && mSelectMouseDownItem->isSelected())
        {
            if (mSelectedItems.size() == 1)
            {
                QList<QAction*> actions = this->actions();

                if (actions.at(InsertPointAction)->isEnabled())
                    mSinglePolyItemContextMenu->popup(mapToGlobal(event->pos()));
                else if (actions.at(GroupAction)->isEnabled() || actions.at(UngroupAction)->isEnabled())
                    mSingleGroupItemContextMenu->popup(mapToGlobal(event->pos()));
                else
                    mSingleItemContextMenu->popup(mapToGlobal(event->pos()));
            }
            else mMultipleItemContextMenu->popup(mapToGlobal(event->pos()));
        }
        else
        {
            setSelectedItems(QList<OdgItem*>());
            mNoItemContextMenu->popup(mapToGlobal(event->pos()));
        }
    }

    // Clear the mouse down item
	mSelectMouseDownItem = nullptr;
}

//======================================================================================================================

void DrawingWidget::selectModeSingleSelectEvent(QMouseEvent* event)
{
	// Add or remove the selectMouseDownItem from the current selection as appropriate
	const bool controlDown = ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier);

	QList<OdgItem*> newSelection;
	if (controlDown) newSelection = mSelectedItems;

	if (mSelectMouseDownItem)
	{
		if (controlDown && newSelection.contains(mSelectMouseDownItem))
			newSelection.removeAll(mSelectMouseDownItem);
		else
			newSelection.append(mSelectMouseDownItem);
	}

    setSelectedItems(newSelection);

	emit mouseInfoChanged("");
	viewport()->update();
}

//======================================================================================================================

void DrawingWidget::selectModeRubberBandStartEvent(QMouseEvent* event)
{
	mSelectRubberBandRect = QRect();
}

void DrawingWidget::selectModeRubberBandUpdateEvent(QMouseEvent* event)
{
	// Update the mSelectRubberBandRect based on the mouse event
	mSelectRubberBandRect = QRect(event->pos(), mMouseButtonDownPosition).normalized();

	emit mouseInfoChanged(createMouseInfo(mMouseButtonDownScenePosition, mapToScene(event->pos())));
	viewport()->update();
}

void DrawingWidget::selectModeRubberBandEndEvent(QMouseEvent* event)
{
	// Select the items in the mSelectRubberBandRect if the rect's width/height are sufficiently large
	if (qAbs(mSelectRubberBandRect.width()) > QApplication::startDragDistance() &&
		qAbs(mSelectRubberBandRect.height()) > QApplication::startDragDistance())
	{
		const QList<OdgItem*> itemsInRubberBand = items(mapToScene(mSelectRubberBandRect).normalized());

		const bool controlDown = ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier);
		if (controlDown)
		{
			QList<OdgItem*> itemsToSelect = mSelectedItems;
			for(auto& item : itemsInRubberBand)
			{
				if (!itemsToSelect.contains(item))
					itemsToSelect.append(item);
			}
			setSelectedItems(itemsToSelect);
		}
		else setSelectedItems(itemsInRubberBand);
	}

	// Reset the select rubber band event variables
	mSelectRubberBandRect = QRect();

	emit mouseInfoChanged(createMouseInfo(mMouseButtonDownScenePosition, mapToScene(event->pos())));
	viewport()->update();
}

//======================================================================================================================

void DrawingWidget::selectModeMoveItemsStartEvent(QMouseEvent* event)
{
	for(auto& item : qAsConst(mSelectedItems))
		mSelectMoveItemsInitialPositions.insert(item, item->position());
	mSelectMoveItemsPreviousDeltaPosition = QPointF();
}

void DrawingWidget::selectModeMoveItemsUpdateEvent(QMouseEvent* event)
{
	selectModeMoveItems(mapToScene(event->pos()), false, false);
}

void DrawingWidget::selectModeMoveItemsEndEvent(QMouseEvent* event)
{
	selectModeMoveItems(mapToScene(event->pos()), true, mSelectedItems.size() == 1);

	mSelectMoveItemsInitialPositions.clear();
	mSelectMoveItemsPreviousDeltaPosition = QPointF();
}

void DrawingWidget::selectModeMoveItems(const QPointF& mousePosition, bool finalMove, bool placeItems)
{
	if (!mSelectedItems.isEmpty())
	{
		const QPointF deltaPosition = roundToGrid(mousePosition - mMouseButtonDownScenePosition);
		if (finalMove || deltaPosition != mSelectMoveItemsPreviousDeltaPosition)
		{
			QHash<OdgItem*,QPointF> newPositions;
			for(auto& item : qAsConst(mSelectedItems))
				newPositions.insert(item, mSelectMoveItemsInitialPositions.value(item) + deltaPosition);

			mUndoStack.push(new DrawingMoveItemsCommand(this, mSelectedItems, newPositions, placeItems));

			if (!finalMove)
			{
				const QPointF position2 = newPositions.value(mSelectedItems.first());
				const QPointF position1 = position2 - deltaPosition;
				emit mouseInfoChanged(createMouseInfo(position1, position2));
			}
			else emit mouseInfoChanged("");

			mSelectMoveItemsPreviousDeltaPosition = deltaPosition;
		}
	}
}

//======================================================================================================================

void DrawingWidget::selectModeResizeItemStartEvent(QMouseEvent* event)
{
	if (mSelectMouseDownItem && mSelectMouseDownPoint)
		mSelectResizeItemInitialPosition = mSelectMouseDownItem->mapToScene(mSelectMouseDownPoint->position());
	mSelectResizeItemPreviousPosition = QPointF();
}

void DrawingWidget::selectModeResizeItemUpdateEvent(QMouseEvent* event)
{
	const bool shiftDown = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);
	selectModeResizeItem(mapToScene(event->pos()), shiftDown, false);
}

void DrawingWidget::selectModeResizeItemEndEvent(QMouseEvent* event)
{
	const bool shiftDown = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);
	selectModeResizeItem(mapToScene(event->pos()), shiftDown, true);

	mSelectResizeItemInitialPosition = QPointF();
	mSelectResizeItemPreviousPosition = QPointF();
}

void DrawingWidget::selectModeResizeItem(const QPointF& mousePosition, bool snapTo45Degrees, bool finalResize)
{
	if (mSelectMouseDownItem && mSelectMouseDownPoint)
	{
		const QPointF newPosition = roundToGrid(mousePosition);
		if (finalResize || newPosition != mSelectResizeItemPreviousPosition)
		{
			mUndoStack.push(new DrawingResizeItemCommand(this, mSelectMouseDownPoint, newPosition, snapTo45Degrees,
														 finalResize));

			if (!finalResize)
				emit mouseInfoChanged(createMouseInfo(mSelectResizeItemInitialPosition, newPosition));
			else
				emit mouseInfoChanged("");

			mSelectResizeItemPreviousPosition = newPosition;
		}
	}
}

//======================================================================================================================

void DrawingWidget::scrollModeNoButtonMouseMoveEvent(QMouseEvent* event)
{
    emit mouseInfoChanged(createMouseInfo(mMouseButtonDownScenePosition));
}

void DrawingWidget::scrollModeLeftMousePressEvent(QMouseEvent* event)
{
    setCursor(QCursor(Qt::ClosedHandCursor));
    mScrollInitialHorizontalValue = horizontalScrollBar()->value();
    mScrollInitialVerticalValue = verticalScrollBar()->value();
}

void DrawingWidget::scrollModeLeftMouseDragEvent(QMouseEvent* event)
{
    // Scroll the scroll bars to keep the mouse cursor over the same view position (when possible)
    if (event)
    {
        const int deltaX = mMouseButtonDownPosition.x() - event->pos().x();
        const int deltaY = mMouseButtonDownPosition.y() - event->pos().y();
        horizontalScrollBar()->setValue(mScrollInitialHorizontalValue + deltaX);
        verticalScrollBar()->setValue(mScrollInitialVerticalValue + deltaY);

        viewport()->update();
    }
}

void DrawingWidget::scrollModeLeftMouseReleaseEvent(QMouseEvent* event)
{
    setCursor(QCursor(Qt::OpenHandCursor));
    emit mouseInfoChanged("");
}

//======================================================================================================================

void DrawingWidget::zoomModeNoButtonMouseMoveEvent(QMouseEvent* event)
{
    emit mouseInfoChanged(createMouseInfo(mMouseButtonDownScenePosition));
}

void DrawingWidget::zoomModeLeftMousePressEvent(QMouseEvent* event)
{
    mZoomRubberBandRect = QRect();
}

void DrawingWidget::zoomModeLeftMouseDragEvent(QMouseEvent* event)
{
    if (event)
    {
        mZoomRubberBandRect = QRect(event->pos(), mMouseButtonDownPosition).normalized();

        emit mouseInfoChanged(createMouseInfo(mMouseButtonDownScenePosition, mapToScene(event->pos())));
        viewport()->update();
    }
}

void DrawingWidget::zoomModeLeftMouseReleaseEvent(QMouseEvent* event)
{
    // Zoom the view to the zoomRubberBandRect if the rect's width/height are sufficiently large
    if (qAbs(mZoomRubberBandRect.width()) > QApplication::startDragDistance() &&
        qAbs(mZoomRubberBandRect.height()) > QApplication::startDragDistance())
    {
        zoomToRect(mapToScene(mZoomRubberBandRect).normalized());
        setSelectMode();
    }

    mZoomRubberBandRect = QRect();

    emit mouseInfoChanged("");
    viewport()->update();
}

//======================================================================================================================

void DrawingWidget::placeModeNoButtonMouseMoveEvent(QMouseEvent* event)
{
	// Move the place items within the scene relative to the center of those items.
	const QPointF center = roundToGrid(itemsCenter(mPlaceItems));
	const QPointF delta = roundToGrid(mapToScene(event->pos())) - center;
	if (delta.x() != 0 || delta.y() != 0)
	{
		QHash<OdgItem*,QPointF> newPositions;
		for(auto& item : mPlaceItems)
			newPositions.insert(item, item->position() + delta);

		moveItems(mPlaceItems, newPositions, false);

		emit mouseInfoChanged(createMouseInfo(roundToGrid(mapToScene(event->pos()))));
	}
}

void DrawingWidget::placeModeLeftMousePressEvent(QMouseEvent* event)
{
	// Nothing to do here.
}

void DrawingWidget::placeModeLeftMouseDragEvent(QMouseEvent* event)
{
	if (mPlaceByMousePressAndRelease && mPlaceItems.size() == 1)
	{
		// Resize item's end point to the current mouse position
		OdgItem* placeItem = mPlaceItems.first();
		OdgControlPoint* placeItemResizeStartPoint = placeItem->placeResizeStartPoint();
		OdgControlPoint* placeItemResizeEndPoint = placeItem->placeResizeEndPoint();

		if (placeItemResizeStartPoint && placeItemResizeEndPoint)
		{
			const QPointF startPosition = placeItem->mapToScene(placeItemResizeStartPoint->position());
			const QPointF endPosition = roundToGrid(mapToScene(event->pos()));
			const bool shiftDown = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);

            resizeItem(placeItemResizeEndPoint, endPosition, shiftDown, false, false);

			emit mouseInfoChanged(createMouseInfo(startPosition, endPosition));
		}
		else placeModeNoButtonMouseMoveEvent(event);
	}
	else placeModeNoButtonMouseMoveEvent(event);
}

void DrawingWidget::placeModeLeftMouseReleaseEvent(QMouseEvent* event)
{
	if (mCurrentPage && (mPlaceItems.size() > 2 || (mPlaceItems.size() == 1 && mPlaceItems.first()->isValid())))
	{
		// Add the items to the scene
		mUndoStack.push(new DrawingAddItemsCommand(this, mCurrentPage, mPlaceItems, true));

		// Create a new set of place items
		QList<OdgItem*> newPlaceItems = OdgItem::copyItems(mPlaceItems);
		if (mPlaceByMousePressAndRelease)
		{
			for(auto& item : qAsConst(newPlaceItems))
				item->placeCreateEvent(contentRect(), mGrid);
		}

		mPlaceItems.clear();
		setPlaceMode(newPlaceItems, mPlaceByMousePressAndRelease);
	}
}

//======================================================================================================================

QString DrawingWidget::createMouseInfo(const QPointF& position) const
{
    return "(" + QString::number(position.x(), 'f', 2) + "," + QString::number(position.y(), 'f', 2) + ")";
}

QString DrawingWidget::createMouseInfo(const QPointF& p1, const QPointF& p2) const
{
    return "(" + QString::number(p1.x(), 'f', 2) + "," + QString::number(p1.y(), 'f', 2) + ") " +
           "(" + QString::number(p2.x(), 'f', 2) + "," + QString::number(p2.y(), 'f', 2) + ")  " +
           QString(QChar(0x394)) + " = " +
           "(" + QString::number(p2.x() - p1.x(), 'f', 2) + "," + QString::number(p2.y() - p1.y(), 'f', 2) + ")";
}

//======================================================================================================================

OdgItem* DrawingWidget::itemAt(const QPointF& position) const
{
    if (mCurrentPage)
    {
        // Favor selected items; if not found in the selected items, search all items in the scene
        for(auto rIter = mSelectedItems.rbegin(); rIter != mSelectedItems.rend(); rIter++)
        {
            if (isPointInItem(*rIter, position))
                return *rIter;
        }

        const QList<OdgItem*> currentPageItems = mCurrentPage->items();
        for(auto rIter = currentPageItems.rbegin(); rIter != currentPageItems.rend(); rIter++)
        {
            if (isPointInItem(*rIter, position))
                return *rIter;
        }
    }

    return nullptr;
}

QList<OdgItem*> DrawingWidget::items(const QRectF& rect) const
{
    QList<OdgItem*> foundItems;

    if (mCurrentPage)
    {
        const QList<OdgItem*> currentPageItems = mCurrentPage->items();
        for(auto& currentPageItem : currentPageItems)
        {
            if (isItemInRect(currentPageItem, rect))
                foundItems.append(currentPageItem);
        }
    }

    return foundItems;
}

//======================================================================================================================

QRectF DrawingWidget::itemsRect(const QList<OdgItem*>& items) const
{
    QRectF rect;
    for(auto& item : items)
        rect = rect.united(item->mapToScene(item->boundingRect()).normalized());
    return rect;
}

QPointF DrawingWidget::itemsCenter(const QList<OdgItem*>& items) const
{
    if (items.size() > 1)
        return itemsRect(items).center();
    if (items.size() == 1)
        return items.first()->position();
    return QPointF();
}

bool DrawingWidget::isItemInRect(OdgItem* item, const QRectF& rect) const
{
    return rect.contains(item->mapToScene(item->boundingRect()).normalized());
}

bool DrawingWidget::isPointInItem(OdgItem* item, const QPointF& position) const
{
    // Check item shape
    if (itemAdjustedShape(item).contains(item->mapFromScene(position))) return true;

    // Check item points if selected
    if (item->isSelected())
    {
        const QList<OdgControlPoint*> controlPoints = item->controlPoints();
        for(auto& point : controlPoints)
        {
            if (pointRect(point).contains(position))
                return true;
        }
    }

    return false;
}

QPainterPath DrawingWidget::itemAdjustedShape(OdgItem* item) const
{
    // Determine minimum pen width
    const QPointF mappedPenSize = mapToScene(QPoint(8, 8)) - mapToScene(QPoint(0, 0));
    const double minimumPenWidth = qMax(qAbs(mappedPenSize.x()), qAbs(mappedPenSize.y()));

    // Override item's default pen width if needed
    const QVariant originalPenWidthProperty = item->property("penWidth");
    if (originalPenWidthProperty.isNull() || !originalPenWidthProperty.canConvert<double>()) return item->shape();
    if (originalPenWidthProperty.toDouble() > minimumPenWidth) return item->shape();

    QPainterPath adjustedShape;
    item->setProperty("penWidth", minimumPenWidth);
    adjustedShape = item->shape();
    item->setProperty("penWidth", originalPenWidthProperty);
    return adjustedShape;
}

//======================================================================================================================

bool DrawingWidget::shouldConnect(OdgControlPoint* controlPoint, OdgGluePoint* gluePoint) const
{
    // Both points must be valid and members of valid items (but not the same item)
    // The controlPoint must be connectable to glue points
    // The controlPoint must not already be connected to the glue point
    if (controlPoint && controlPoint->item() && gluePoint && gluePoint->item() &&
        controlPoint->item() != gluePoint->item() && controlPoint->isConnectable() &&
        controlPoint->gluePoint() != gluePoint)
    {
        QPointF vec = controlPoint->item()->mapToScene(controlPoint->position()) -
                      gluePoint->item()->mapToScene(gluePoint->position());
        return (qSqrt(vec.x() * vec.x() + vec.y() * vec.y()) <= 1E-6);
    }

    return false;
}

QRectF DrawingWidget::pointRect(OdgControlPoint* point) const
{
    OdgItem* item = point->item();
    if (item)
    {
        const QPointF position = item->mapToScene(point->position());
        const double size = 8 / scale();
        return QRectF(position.x() - size / 2, position.y() - size / 2, size, size);
    }
    return QRectF();
}

QRectF DrawingWidget::pointRect(OdgGluePoint* point) const
{
    OdgItem* item = point->item();
    if (item)
    {
        const QPointF position = item->mapToScene(point->position());
        const double size = 6 / scale();
        return QRectF(position.x() - size / 2, position.y() - size / 2, size, size);
    }
    return QRectF();
}

//======================================================================================================================

void DrawingWidget::updateSelectionCenter()
{
    mSelectedItemsCenter = roundToGrid(itemsCenter(mSelectedItems));
}

void DrawingWidget::updateActions()
{
    const bool canGroup = (mSelectedItems.size() > 1);
    const bool canUngroup = (mSelectedItems.size() == 1 &&
                             dynamic_cast<OdgGroupItem*>(mSelectedItems.first()) != nullptr);
    const bool canInsertPoints = (mSelectedItems.size() == 1 && mSelectedItems.first()->canInsertPoints());
    const bool canRemovePoints = (mSelectedItems.size() == 1 && mSelectedItems.first()->canRemovePoints());

    QList<QAction*> actions = this->actions();
    actions.at(GroupAction)->setEnabled(canGroup);
    actions.at(UngroupAction)->setEnabled(canUngroup);
    actions.at(InsertPointAction)->setEnabled(canInsertPoints);
    actions.at(RemovePointAction)->setEnabled(canRemovePoints);
}

//======================================================================================================================

void DrawingWidget::mousePanEvent()
{
    if (mPanCurrentPosition.x() - mPanStartPosition.x() < 0)
    {
        // Scroll to the left, adjusting the horizontal scroll bar minimum if necessary
        const int delta = qRound((mPanCurrentPosition.x() - mPanStartPosition.x()) / 64.0);

        if (horizontalScrollBar()->value() + delta < horizontalScrollBar()->minimum())
        {
            horizontalScrollBar()->setMinimum(horizontalScrollBar()->value() + delta);
            horizontalScrollBar()->setValue(horizontalScrollBar()->minimum());
        }
        else horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta);
    }
    else if (mPanCurrentPosition.x() - mPanStartPosition.x() > 0)
    {
        // Scroll to the right, adjusting the horizontal scroll bar maximum if necessary
        const int delta = qRound((mPanCurrentPosition.x() - mPanStartPosition.x()) / 64.0);

        if (horizontalScrollBar()->value() + delta > horizontalScrollBar()->maximum())
        {
            horizontalScrollBar()->setMaximum(horizontalScrollBar()->value() + delta);
            horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
        }
        else horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta);
    }

    if (mPanCurrentPosition.y() - mPanStartPosition.y() < 0)
    {
        // Scroll up, adjusting the vertical scroll bar minimum if necessary
        const int delta = qRound((mPanCurrentPosition.y() - mPanStartPosition.y()) / 64.0);

        if (verticalScrollBar()->value() + delta < verticalScrollBar()->minimum())
        {
            verticalScrollBar()->setMinimum(verticalScrollBar()->value() + delta);
            verticalScrollBar()->setValue(verticalScrollBar()->minimum());
        }
        else verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
    }
    else if (mPanCurrentPosition.y() - mPanStartPosition.y() > 0)
    {
        // Scroll down, adjusting the vertical scroll bar maximum if necessary
        const int delta = qRound((mPanCurrentPosition.y() - mPanStartPosition.y()) / 64.0);

        if (verticalScrollBar()->value() + delta > verticalScrollBar()->maximum())
        {
            verticalScrollBar()->setMaximum(verticalScrollBar()->value() + delta);
            verticalScrollBar()->setValue(verticalScrollBar()->maximum());
        }
        else verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
    }

    viewport()->update();
}

//======================================================================================================================

void DrawingWidget::setModeFromAction(QAction* action)
{
    QList<QAction*> actions = this->actions();

    if (action == actions.at(SelectModeAction))
    {
        setSelectMode();
    }
    else if (action == actions.at(ScrollModeAction))
    {
        setScrollMode();
    }
    else if (action == actions.at(ZoomModeAction))
    {
        setZoomMode();
    }
    else if (action == actions.at(PlaceLineAction))
    {
        OdgLineItem* lineItem = new OdgLineItem();
        if (mDefaultStyle)
        {
            lineItem->setPen(mDefaultStyle->lookupPen());
            lineItem->setStartMarker(mDefaultStyle->lookupStartMarker());
            lineItem->setEndMarker(mDefaultStyle->lookupEndMarker());
        }
        setPlaceMode(QList<OdgItem*>(1, lineItem), true);
    }
    else if (action == actions.at(PlaceCurveAction))
    {
        OdgCurveItem* curveItem = new OdgCurveItem();
        if (mDefaultStyle)
        {
            curveItem->setPen(mDefaultStyle->lookupPen());
            curveItem->setStartMarker(mDefaultStyle->lookupStartMarker());
            curveItem->setEndMarker(mDefaultStyle->lookupEndMarker());
            curveItem->placeCreateEvent(contentRect(), mGrid);
        }
        setPlaceMode(QList<OdgItem*>(1, curveItem), false);
    }
    else if (action == actions.at(PlacePolylineAction))
    {
        OdgPolylineItem* polylineItem = new OdgPolylineItem();
        if (mDefaultStyle)
        {
            polylineItem->setPen(mDefaultStyle->lookupPen());
            polylineItem->setStartMarker(mDefaultStyle->lookupStartMarker());
            polylineItem->setEndMarker(mDefaultStyle->lookupEndMarker());
        }
        setPlaceMode(QList<OdgItem*>(1, polylineItem), true);
    }
    else if (action == actions.at(PlaceRectangleAction))
    {
        OdgRoundedRectItem* rectItem = new OdgRoundedRectItem();
        if (mDefaultStyle)
        {
            rectItem->setCornerRadius(0);
            rectItem->setBrush(mDefaultStyle->lookupBrush());
            rectItem->setPen(mDefaultStyle->lookupPen());
            rectItem->placeCreateEvent(contentRect(), mGrid);
        }
        setPlaceMode(QList<OdgItem*>(1, rectItem), false);
    }
    else if (action == actions.at(PlaceEllipseAction))
    {
        OdgEllipseItem* ellipseItem = new OdgEllipseItem();
        if (mDefaultStyle)
        {
            ellipseItem->setBrush(mDefaultStyle->lookupBrush());
            ellipseItem->setPen(mDefaultStyle->lookupPen());
            ellipseItem->placeCreateEvent(contentRect(), mGrid);
        }
        setPlaceMode(QList<OdgItem*>(1, ellipseItem), false);
    }
    else if (action == actions.at(PlacePolygonAction))
    {
        OdgPolygonItem* polygonItem = new OdgPolygonItem();
        if (mDefaultStyle)
        {
            polygonItem->setBrush(mDefaultStyle->lookupBrush());
            polygonItem->setPen(mDefaultStyle->lookupPen());
            polygonItem->placeCreateEvent(contentRect(), mGrid);
        }
        setPlaceMode(QList<OdgItem*>(1, polygonItem), false);
    }
    else if (action == actions.at(PlaceTextAction))
    {
        OdgTextItem* textItem = new OdgTextItem();
        if (mDefaultStyle)
        {
            textItem->setFont(mDefaultStyle->lookupFont());
            textItem->setTextAlignment(mDefaultStyle->lookupTextAlignment());
            textItem->setTextPadding(mDefaultStyle->lookupTextPadding());
            textItem->setTextBrush(mDefaultStyle->lookupTextBrush());
            textItem->placeCreateEvent(contentRect(), mGrid);
        }
        setPlaceMode(QList<OdgItem*>(1, textItem), false);
    }
    else if (action == actions.at(PlaceTextRectangleAction))
    {
        OdgTextRoundedRectItem* textRectItem = new OdgTextRoundedRectItem();
        if (mDefaultStyle)
        {
            textRectItem->setCornerRadius(0);
            textRectItem->setBrush(mDefaultStyle->lookupBrush());
            textRectItem->setPen(mDefaultStyle->lookupPen());
            textRectItem->setFont(mDefaultStyle->lookupFont());
            textRectItem->setTextAlignment(mDefaultStyle->lookupTextAlignment());
            textRectItem->setTextPadding(mDefaultStyle->lookupTextPadding());
            textRectItem->setTextBrush(mDefaultStyle->lookupTextBrush());
            textRectItem->placeCreateEvent(contentRect(), mGrid);
        }
        setPlaceMode(QList<OdgItem*>(1, textRectItem), false);
    }
    else if (action == actions.at(PlaceTextEllipseAction))
    {
        OdgTextEllipseItem* textEllipseItem = new OdgTextEllipseItem();
        if (mDefaultStyle)
        {
            textEllipseItem->setBrush(mDefaultStyle->lookupBrush());
            textEllipseItem->setPen(mDefaultStyle->lookupPen());
            textEllipseItem->setFont(mDefaultStyle->lookupFont());
            textEllipseItem->setTextAlignment(mDefaultStyle->lookupTextAlignment());
            textEllipseItem->setTextPadding(mDefaultStyle->lookupTextPadding());
            textEllipseItem->setTextBrush(mDefaultStyle->lookupTextBrush());
            textEllipseItem->placeCreateEvent(contentRect(), mGrid);
        }
        setPlaceMode(QList<OdgItem*>(1, textEllipseItem), false);
    }
    else
    {
        OdgPathItem* foundItem = nullptr;
        QString actionText;

        for(auto& pathItem : qAsConst(mPathItems))
        {
            actionText = action->text();
            if (actionText.mid(6, -1) == pathItem->pathName())
            {
                foundItem = pathItem;
                break;
            }
        }

        if (foundItem)
        {
            OdgPathItem* newItem = new OdgPathItem();
            newItem->setPathName(foundItem->pathName());
            newItem->setPath(foundItem->path(), foundItem->pathRect());
            newItem->setBrush(mDefaultStyle->lookupBrush());
            newItem->setPen(mDefaultStyle->lookupPen());
            newItem->placeCreateEvent(contentRect(), mGrid);
            setPlaceMode(QList<OdgItem*>(1, newItem), false);
        }
        else setSelectMode();
    }
}

void DrawingWidget::emitCleanChanged(bool clean)
{
    emit cleanChanged(clean);
    emit cleanTextChanged(clean ? "" : "Modified");
}
