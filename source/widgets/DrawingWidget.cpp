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
#include "OdgPage.h"
#include "OdgItem.h"
#include "OdgReader.h"
#include <QActionGroup>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QScrollBar>
#include <QStyle>
#include <QWheelEvent>

DrawingWidget::DrawingWidget() : QAbstractScrollArea(), OdgDrawing(),
    mCurrentPage(nullptr), mNewPageCount(0), mTransform(), mTransformInverse(), mUndoStack(),
    mModeActionGroup(nullptr), mContextMenu(nullptr)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setMouseTracking(true);

    mUndoStack.setUndoLimit(256);
    connect(&mUndoStack, SIGNAL(cleanChanged(bool)), this, SLOT(emitCleanChanged(bool)));

    createActions();
    createContextMenu();
}

DrawingWidget::~DrawingWidget()
{
    setCurrentPage(nullptr);
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

    mContextMenu = new QMenu(this);
    mContextMenu->addAction(actions.at(UndoAction));
    mContextMenu->addAction(actions.at(RedoAction));
    mContextMenu->addSeparator();
    mContextMenu->addAction(actions.at(CutAction));
    mContextMenu->addAction(actions.at(CopyAction));
    mContextMenu->addAction(actions.at(PasteAction));
    mContextMenu->addAction(actions.at(DeleteAction));
    mContextMenu->addSeparator();
    mContextMenu->addAction(actions.at(InsertPointAction));
    mContextMenu->addAction(actions.at(RemovePointAction));
    mContextMenu->addSeparator();
    mContextMenu->addAction(actions.at(RotateAction));
    mContextMenu->addAction(actions.at(RotateBackAction));
    mContextMenu->addAction(actions.at(FlipHorizontalAction));
    mContextMenu->addAction(actions.at(FlipVerticalAction));
    mContextMenu->addSeparator();
    mContextMenu->addAction(actions.at(BringForwardAction));
    mContextMenu->addAction(actions.at(SendBackwardAction));
    mContextMenu->addAction(actions.at(BringToFrontAction));
    mContextMenu->addAction(actions.at(SendToBackAction));
    mContextMenu->addSeparator();
    mContextMenu->addAction(actions.at(GroupAction));
    mContextMenu->addAction(actions.at(UngroupAction));
    mContextMenu->addSeparator();
    mContextMenu->addAction(actions.at(ZoomInAction));
    mContextMenu->addAction(actions.at(ZoomOutAction));
    mContextMenu->addAction(actions.at(ZoomFitAction));
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

//======================================================================================================================

void DrawingWidget::setUnits(Odg::Units units)
{
    if (mUnits != units)
    {
        OdgDrawing::setUnits(units);
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
    return isClean();
}

void DrawingWidget::clear()
{
    mUndoStack.clear();

    mNewPageCount = 0;
    setCurrentPage(nullptr);
    clearPages();
}

bool DrawingWidget::isClean() const
{
    return true;
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

void DrawingWidget::undo()
{
    mUndoStack.undo();
}

void DrawingWidget::redo()
{
    mUndoStack.redo();
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
    mUndoStack.push(new DrawingInsertPageCommand(this, new OdgPage(name), currentPageIndex() + 1));
    zoomFit();
}

void DrawingWidget::duplicatePage()
{

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

}

//======================================================================================================================

void DrawingWidget::cut()
{

}

void DrawingWidget::copy()
{

}

void DrawingWidget::paste()
{

}

//======================================================================================================================

void DrawingWidget::selectAll()
{

}

void DrawingWidget::selectNone()
{

}

//======================================================================================================================

void DrawingWidget::rotate()
{

}

void DrawingWidget::rotateBack()
{

}

void DrawingWidget::flipHorizontal()
{

}

void DrawingWidget::flipVertical()
{

}

//======================================================================================================================

void DrawingWidget::bringForward()
{

}

void DrawingWidget::sendBackward()
{

}

void DrawingWidget::bringToFront()
{

}

void DrawingWidget::sendToBack()
{

}

//======================================================================================================================

void DrawingWidget::group()
{

}

void DrawingWidget::ungroup()
{

}

//======================================================================================================================

void DrawingWidget::insertPoint()
{

}

void DrawingWidget::removePoint()
{

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
        const QColor minorGridColor = QColor::fromHslF(gridColor.hslHueF(), gridColor.hslSaturationF(),
                                                       gridColor.lightnessF() + (1 - gridColor.lightnessF()) * 0.7);

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

void DrawingWidget::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        // Zoom in or out on a mouse wheel vertical event if the control key is held down.
        if (event->angleDelta().y() > 0)
            zoomIn();
        else if (event->angleDelta().y() < 0)
            zoomOut();
    }
    else QAbstractScrollArea::wheelEvent(event);
}

void DrawingWidget::contextMenuEvent(QContextMenuEvent* event)
{
    mContextMenu->popup(event->globalPos());
}

//======================================================================================================================

void DrawingWidget::setModeFromAction(QAction* action)
{

}

void DrawingWidget::emitCleanChanged(bool clean)
{
    emit cleanChanged(clean);
    emit cleanTextChanged(clean ? "" : "Modified");
}
