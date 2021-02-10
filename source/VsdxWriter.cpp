// VsdxWriter.cpp
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

#include "VsdxWriter.h"
#include <DrawingCurveItem.h>
#include <DrawingEllipseItem.h>
#include <DrawingItemGroup.h>
#include <DrawingLineItem.h>
#include <DrawingPathItem.h>
#include <DrawingPolygonItem.h>
#include <DrawingPolylineItem.h>
#include <DrawingReferenceItem.h>
#include <DrawingRectItem.h>
#include <DrawingTextItem.h>
#include <DrawingTextEllipseItem.h>
#include <DrawingTextRectItem.h>
#include <DrawingWidget.h>
#include <QTextStream>
#include <quazip.h>
#include <quazipfile.h>

VsdxWriter::VsdxWriter(DrawingWidget* drawing)
{
	mDrawing = drawing;

	mDrawingUnits = "in";
	mDrawingScale = 0.001;
	mPageMargin = 0.25;
	//mDrawingUnits = "mm";
	//mDrawingScale = 0.025;
	//mPageMargin = 5.0;

	mShapeIndex = 0;
}

VsdxWriter::~VsdxWriter()
{
	mDrawing = nullptr;
}

//==================================================================================================

bool VsdxWriter::save(const QString& filePath)
{
	bool errorOccurred = false;

	mShapeIndex = 0;

	QuaZip vsdxFile(filePath);

	errorOccurred = !vsdxFile.open(QuaZip::mdCreate);
	if (!errorOccurred)
	{
		if (!errorOccurred) errorOccurred = !createFileInZip(&vsdxFile, "[Content_Types].xml", writeContentTypes());

		if (!errorOccurred) errorOccurred = !createFileInZip(&vsdxFile, "_rels/.rels", writeRels());

		if (!errorOccurred) errorOccurred = !createFileInZip(&vsdxFile, "docProps/app.xml", writeApp());
		if (!errorOccurred) errorOccurred = !createFileInZip(&vsdxFile, "docProps/core.xml", writeCore());
		if (!errorOccurred) errorOccurred = !createFileInZip(&vsdxFile, "docProps/custom.xml", writeCustom());

		if (!errorOccurred) errorOccurred = !createFileInZip(&vsdxFile, "visio/pages/_rels/pages.xml.rels", writePagesRels());
		if (!errorOccurred) errorOccurred = !createFileInZip(&vsdxFile, "visio/pages/pages.xml", writePages());
		if (!errorOccurred) errorOccurred = !createFileInZip(&vsdxFile, "visio/pages/page1.xml", writePage1());

		if (!errorOccurred) errorOccurred = !createFileInZip(&vsdxFile, "visio/_rels/document.xml.rels", writeDocumentRels());
		if (!errorOccurred) errorOccurred = !createFileInZip(&vsdxFile, "visio/document.xml", writeDocument());
		if (!errorOccurred) errorOccurred = !createFileInZip(&vsdxFile, "visio/windows.xml", writeWindows());

		vsdxFile.close();
	}

	return !errorOccurred;
}

//==================================================================================================

bool VsdxWriter::createFileInZip(QuaZip* zip, const QString& path, const QString& content)
{
	QuaZipFile outputFile(zip);

	bool errorOccurred = !outputFile.open(QIODevice::WriteOnly, QuaZipNewInfo(path));
	if (!errorOccurred)
	{
		QTextStream outputStream(&outputFile);
		outputStream << content;
		outputStream.flush();
		outputFile.close();
	}

	return !errorOccurred;
}

QString VsdxWriter::writeContentTypes()
{
	QString contentTypes;

	contentTypes += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
	contentTypes += "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">\n";
	contentTypes += "  <Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>\n";
	contentTypes += "  <Default Extension=\"xml\" ContentType=\"application/xml\"/>\n";
	contentTypes += "  <Override PartName=\"/visio/document.xml\" ContentType=\"application/vnd.ms-visio.drawing.main+xml\"/>\n";
	contentTypes += "  <Override PartName=\"/visio/pages/pages.xml\" ContentType=\"application/vnd.ms-visio.pages+xml\"/>\n";
	contentTypes += "  <Override PartName=\"/visio/pages/page1.xml\" ContentType=\"application/vnd.ms-visio.page+xml\"/>\n";
	contentTypes += "  <Override PartName=\"/visio/windows.xml\" ContentType=\"application/vnd.ms-visio.windows+xml\"/>\n";
	contentTypes += "  <Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>\n";
	contentTypes += "  <Override PartName=\"/docProps/app.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.extended-properties+xml\"/>\n";
	contentTypes += "  <Override PartName=\"/docProps/custom.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.custom-properties+xml\"/>\n";
	contentTypes += "</Types>\n";

	return contentTypes;
}

QString VsdxWriter::writeRels()
{
	QString rels;

	rels += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
	rels += "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\n";
	rels += "  <Relationship Id=\"rId3\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>\n";
	rels += "  <Relationship Id=\"rId1\" Type=\"http://schemas.microsoft.com/visio/2010/relationships/document\" Target=\"visio/document.xml\"/>\n";
	rels += "  <Relationship Id=\"rId5\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/custom-properties\" Target=\"docProps/custom.xml\"/>\n";
	rels += "  <Relationship Id=\"rId4\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties\" Target=\"docProps/app.xml\"/>\n";
	rels += "</Relationships>\n";

	return rels;
}

QString VsdxWriter::writeApp()
{
	QString app;

	app += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
	app += "<Properties xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/extended-properties\" xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\">\n";
	app += "  <Template/>\n";
	app += "  <Application>Microsoft Visio</Application>\n";
	app += "  <ScaleCrop>false</ScaleCrop>\n";
	app += "  <HeadingPairs>\n";
	app += "    <vt:vector size=\"2\" baseType=\"variant\">\n";
	app += "      <vt:variant>\n";
	app += "        <vt:lpstr>Pages</vt:lpstr>\n";
	app += "      </vt:variant>\n";
	app += "      <vt:variant>\n";
	app += "        <vt:i4>1</vt:i4>\n";
	app += "      </vt:variant>\n";
	app += "    </vt:vector>\n";
	app += "  </HeadingPairs>\n";
	app += "  <TitlesOfParts>\n";
	app += "    <vt:vector size=\"1\" baseType=\"lpstr\">\n";
	app += "      <vt:lpstr>Page-1</vt:lpstr>\n";
	app += "    </vt:vector>\n";
	app += "  </TitlesOfParts>\n";
	app += "  <Manager/>\n";
	app += "  <Company/>\n";
	app += "  <LinksUpToDate>false</LinksUpToDate>\n";
	app += "  <SharedDoc>false</SharedDoc>\n";
	app += "  <HyperlinkBase/>\n";
	app += "  <HyperlinksChanged>false</HyperlinksChanged>\n";
	app += "  <AppVersion>16.0000</AppVersion>\n";
	app += "</Properties>\n";

	return app;
}

QString VsdxWriter::writeCore()
{
	QString core;

	core += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
	core += "<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dcterms=\"http://purl.org/dc/terms/\" xmlns:dcmitype=\"http://purl.org/dc/dcmitype/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n";
	core += "  <dc:title/>\n";
	core += "  <dc:subject/>\n";
	core += "  <dc:creator/>\n";
	core += "  <cp:keywords/>\n";
	core += "  <dc:description/>\n";
	core += "  <cp:category/>\n";
	core += "  <dc:language>en-US</dc:language>\n";
	core += "</cp:coreProperties>\n";

	return core;
}

QString VsdxWriter::writeCustom()
{
	QString custom;

	custom += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
	custom += "<Properties xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/custom-properties\" xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\">\n";
	custom += "  <property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"2\" name=\"_VPID_ALTERNATENAMES\">\n";
	custom += "    <vt:lpwstr/>\n";
	custom += "  </property>\n";
	custom += "  <property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"3\" name=\"BuildNumberCreated\">\n";
	custom += "    <vt:i4>1074146641</vt:i4>\n";
	custom += "  </property>\n";
	custom += "  <property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"4\" name=\"BuildNumberEdited\">\n";
	custom += "    <vt:i4>1074146641</vt:i4>\n";
	custom += "  </property>\n";
	custom += "  <property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"5\" name=\"IsMetric\">\n";
	custom += QString("    <vt:bool>") + ((mDrawingUnits == "mm") ? "true" : "false") + "</vt:bool>\n";
	custom += "  </property>\n";
	custom += "</Properties>\n";

	return custom;
}

QString VsdxWriter::writePagesRels()
{
	QString rels;

	rels += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
	rels += "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\n";
	rels += "  <Relationship Id=\"rId1\" Type=\"http://schemas.microsoft.com/visio/2010/relationships/page\" Target=\"page1.xml\"/>\n";
	rels += "</Relationships>\n";

	return rels;
}

QString VsdxWriter::writePages()
{
	QString pages;

	QString pageWidth = QString::number(mDrawing->sceneRect().width() * mDrawingScale + 2 * mPageMargin);
	QString pageHeight = QString::number(mDrawing->sceneRect().height() * mDrawingScale + 2 * mPageMargin);
	QString xRulerOrigin = QString::number(mPageMargin - mDrawing->sceneRect().left() * mDrawingScale);
	QString yRulerOrigin = QString::number(mPageMargin + mDrawing->sceneRect().bottom() * mDrawingScale);
	QString xGridOrigin = xRulerOrigin;
	QString yGridOrigin = yRulerOrigin;

	pages += "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	pages += "<Pages xmlns=\"http://schemas.microsoft.com/office/visio/2012/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xml:space=\"preserve\">\n";
	pages += " <Page ID=\"0\" NameU=\"Page-1\" Name=\"Page-1\">\n";
	pages += "    <PageSheet LineStyle=\"0\" FillStyle=\"0\" TextStyle=\"0\">\n";
	pages += "      <Cell N=\"PageWidth\" V=\"" + pageWidth + "\"/>\n";
	pages += "      <Cell N=\"PageHeight\" V=\"" + pageHeight + "\"/>\n";
	pages += "      <Cell N=\"ShdwOffsetX\" V=\"0.125\"/>\n";
	pages += "      <Cell N=\"ShdwOffsetY\" V=\"-0.125\"/>\n";
	pages += "      <Cell N=\"PageScale\" V=\"1\" U=\"IN_F\"/>\n";
	pages += "      <Cell N=\"DrawingScale\" V=\"1\" U=\"IN_F\"/>\n";
	pages += "      <Cell N=\"DrawingSizeType\" V=\"3\"/>\n";
	pages += "      <Cell N=\"DrawingScaleType\" V=\"0\"/>\n";
	pages += "      <Cell N=\"InhibitSnap\" V=\"0\"/>\n";
	pages += "      <Cell N=\"PageLockReplace\" V=\"0\" U=\"BOOL\"/>\n";
	pages += "      <Cell N=\"PageLockDuplicate\" V=\"0\" U=\"BOOL\"/>\n";
	pages += "      <Cell N=\"UIVisibility\" V=\"0\"/>\n";
	pages += "      <Cell N=\"ShdwType\" V=\"0\"/>\n";
	pages += "      <Cell N=\"ShdwObliqueAngle\" V=\"0\"/>\n";
	pages += "      <Cell N=\"ShdwScaleFactor\" V=\"1\"/>\n";
	pages += "      <Cell N=\"DrawingResizeType\" V=\"2\"/>\n";
	pages += "      <Cell N=\"PageShapeSplit\" V=\"1\"/>\n";
	pages += "      <Cell N=\"XRulerOrigin\" V=\"" + xRulerOrigin + "\" U=\"" + mDrawingUnits + "\"/>\n";
	pages += "      <Cell N=\"YRulerOrigin\" V=\"" + yRulerOrigin + "\" U=\"" + mDrawingUnits + "\"/>\n";
	pages += "      <Cell N=\"XGridOrigin\" V=\"" + xGridOrigin + "\" U=\"" + mDrawingUnits + "\"/>\n";
	pages += "      <Cell N=\"YGridOrigin\" V=\"" + yGridOrigin + "\" U=\"" + mDrawingUnits + "\"/>\n";
	pages += "    </PageSheet>\n";
	pages += "    <Rel r:id=\"rId1\"/>\n";
	pages += "  </Page>\n";
	pages += "</Pages>\n";

	return pages;
}

QString VsdxWriter::writePage1()
{
	QString page;

	page += "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	page += "<PageContents xmlns=\"http://schemas.microsoft.com/office/visio/2012/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xml:space=\"preserve\">\n";
	page += "  <Shapes>\n";

	page += writeItems(mDrawing->items());

	page += "  </Shapes>\n";
	page += "</PageContents>\n";

	return page;
}

QString VsdxWriter::writeDocumentRels()
{
	QString rels;

	rels += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
	rels += "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\n";
	rels += "  <Relationship Id=\"rId2\" Type=\"http://schemas.microsoft.com/visio/2010/relationships/windows\" Target=\"windows.xml\"/>\n";
	rels += "  <Relationship Id=\"rId1\" Type=\"http://schemas.microsoft.com/visio/2010/relationships/pages\" Target=\"pages/pages.xml\"/>\n";
	rels += "</Relationships>\n";

	return rels;
}

QString VsdxWriter::writeDocument()
{
	QString document;

	document += "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	document += "<VisioDocument xmlns=\"http://schemas.microsoft.com/office/visio/2012/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xml:space=\"preserve\">\n";
	document += "</VisioDocument>\n";

	return document;
}

QString VsdxWriter::writeWindows()
{
	QString windows;

	windows += "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	windows += "<Windows xmlns=\"http://schemas.microsoft.com/office/visio/2012/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xml:space=\"preserve\">\n";
	windows += "</Windows>\n";

	return windows;
}

//==================================================================================================

QString VsdxWriter::writeItems(const QList<DrawingItem*>& items)
{
	QString shapes;
	QString key;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		key = (*itemIter)->uniqueKey();

		if (key == "rect") shapes += writeRectItem(dynamic_cast<DrawingRectItem*>(*itemIter));
		else if (key == "ellipse") shapes += writeEllipseItem(dynamic_cast<DrawingEllipseItem*>(*itemIter));
		else if (key == "line") shapes += writeLineItem(dynamic_cast<DrawingLineItem*>(*itemIter));
		else if (key == "curve") shapes += writeCurveItem(dynamic_cast<DrawingCurveItem*>(*itemIter));
		else if (key == "polyline") shapes += writePolylineItem(dynamic_cast<DrawingPolylineItem*>(*itemIter));
		else if (key == "polygon") shapes += writePolygonItem(dynamic_cast<DrawingPolygonItem*>(*itemIter));
		else if (key == "text") shapes += writeTextItem(dynamic_cast<DrawingTextItem*>(*itemIter));
		else if (key == "textRect") shapes += writeTextRectItem(dynamic_cast<DrawingTextRectItem*>(*itemIter));
		else if (key == "textEllipse") shapes += writeTextEllipseItem(dynamic_cast<DrawingTextEllipseItem*>(*itemIter));
		else if (key == "path") shapes += writePathItem(dynamic_cast<DrawingPathItem*>(*itemIter));
		else if (key == "group") shapes += writeGroupItem(dynamic_cast<DrawingItemGroup*>(*itemIter));
		else if (key == "reference") shapes += writeReferenceItem(dynamic_cast<DrawingReferenceItem*>(*itemIter));
	}

	return shapes;
}

QString VsdxWriter::writeRectItem(DrawingRectItem* item)
{
	QString shape;

	if (item)
	{
		// todo: configure Jade to be better for exporting to Visio
		// enable dynamic grid with base of 1000
		// curve item, polygon item, text rect item, text ellipse item, and path items should be a factor of 1000


	}

	return shape;
}

QString VsdxWriter::writeEllipseItem(DrawingEllipseItem* item)
{
	QString shape;

	if (item)
	{

	}

	return shape;
}

QString VsdxWriter::writeLineItem(DrawingLineItem* item)
{
	QString shape;

	if (item)
	{
		QPointF startPoint = mapFromScene(item->mapToScene(item->line().p1()));
		QPointF endPoint = mapFromScene(item->mapToScene(item->line().p2()));
		QPointF centerPoint = (startPoint + endPoint) / 2;
		qreal xSpan = qAbs(endPoint.x() - startPoint.x());
		qreal ySpan = qAbs(endPoint.y() - startPoint.y());
		qreal width = qSqrt(xSpan * xSpan + ySpan * ySpan);
		qreal angle = qAtan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

		mShapeIndex++;

		QString indexStr = QString::number(mShapeIndex);
		QString pinXStr = QString::number(centerPoint.x());
		QString pinYStr = QString::number(centerPoint.y());
		QString widthStr = QString::number(width);
		QString locPinXStr = QString::number(width / 2);
		QString angleStr = QString::number(angle);
		QString beginXStr = QString::number(startPoint.x());
		QString beginYStr = QString::number(startPoint.y());
		QString endXStr = QString::number(endPoint.x());
		QString endYStr = QString::number(endPoint.y());

		shape += "    <Shape ID='" + indexStr + "' Type='Shape' LineStyle='3' FillStyle='3' TextStyle='3'>\n";
		shape += "      <Cell N='PinX' V='" + pinXStr + "' F='(BeginX+EndX)/2'/>\n";
		shape += "      <Cell N='PinY' V='" + pinYStr + "' F='(BeginY+EndY)/2'/>\n";
		shape += "      <Cell N='Width' V='" + widthStr + "' F='SQRT((EndX-BeginX)^2+(EndY-BeginY)^2)'/>\n";
		shape += "      <Cell N='Height' V='0'/>\n";
		shape += "      <Cell N='LocPinX' V='" + locPinXStr + "' F='Width*0.5'/>\n";
		shape += "      <Cell N='LocPinY' V='0' F='Height*0.5'/>\n";
		shape += "      <Cell N='Angle' V='" + angleStr + "' F='ATAN2(EndY-BeginY,EndX-BeginX)'/>\n";
		shape += "      <Cell N='FlipX' V='0'/>\n";
		shape += "      <Cell N='FlipY' V='0'/>\n";
		shape += "      <Cell N='ResizeMode' V='0'/>\n";
		shape += "      <Cell N='BeginX' V='" + beginXStr + "'/>\n";
		shape += "      <Cell N='BeginY' V='" + beginYStr + "'/>\n";
		shape += "      <Cell N='EndX' V='" + endXStr + "'/>\n";
		shape += "      <Cell N='EndY' V='" + endYStr + "'/>\n";

		shape += writeStyle(Qt::transparent, item->pen());
		shape += writeArrow(item->startArrow(), item->pen(), true);
		shape += writeArrow(item->endArrow(), item->pen(), false);

		shape += "      <Section N='Geometry' IX='0'>\n";
		shape += "    	  <Cell N='NoFill' V='1'/>\n";
		shape += "    	  <Cell N='NoLine' V='0'/>\n";
		shape += "    	  <Cell N='NoShow' V='0'/>\n";
		shape += "    	  <Cell N='NoSnap' V='0'/>\n";
		shape += "    	  <Cell N='NoQuickDrag' V='0'/>\n";
		shape += "    	  <Row T='MoveTo' IX='1'>\n";
		shape += "    	    <Cell N='X' V='0' F='Width*0'/>\n";
		shape += "    	    <Cell N='Y' V='0'/>\n";
		shape += "    	  </Row>\n";
		shape += "        <Row T='LineTo' IX='2'>\n";
		shape += "          <Cell N='X' V='" + widthStr + "' F='Width*1'/>\n";
		shape += "      	<Cell N='Y' V='0'/>\n";
		shape += "        </Row>\n";
		shape += "      </Section>\n";
		shape += "    </Shape>\n";
	}

	return shape;
}

QString VsdxWriter::writeCurveItem(DrawingCurveItem* item)
{
	QString shape;

	if (item)
	{

	}

	return shape;
}

QString VsdxWriter::writePolylineItem(DrawingPolylineItem* item)
{
	QString shape;

	if (item)
	{

	}

	return shape;
}

QString VsdxWriter::writePolygonItem(DrawingPolygonItem* item)
{
	QString shape;

	if (item)
	{

	}

	return shape;
}

QString VsdxWriter::writeTextItem(DrawingTextItem* item)
{
	QString shape;

	if (item)
	{

	}

	return shape;
}

QString VsdxWriter::writeTextRectItem(DrawingTextRectItem* item)
{
	QString shape;

	if (item)
	{

	}

	return shape;
}

QString VsdxWriter::writeTextEllipseItem(DrawingTextEllipseItem* item)
{
	QString shape;

	if (item)
	{

	}

	return shape;
}

QString VsdxWriter::writePathItem(DrawingPathItem* item)
{
	QString shape;

	if (item)
	{

	}

	return shape;
}

QString VsdxWriter::writeGroupItem(DrawingItemGroup* item)
{
	QString shape;

	if (item)
	{

	}

	return shape;
}

QString VsdxWriter::writeReferenceItem(DrawingReferenceItem* item)
{
	QString shape;

	if (item)
	{

	}

	return shape;
}

//==================================================================================================

QString VsdxWriter::writeStyle(const QBrush& brush, const QPen& pen)
{
	QString shape;

	QColor brushColor = brush.color();
	QColor penColor = pen.brush().color();

	// Visio boilerplate stuff
	shape += "      <Cell N='TextBkgnd' V='#ffffff' F='THEMEGUARD(THEMEVAL(\"BackgroundColor\")+1)'/>\n";
	shape += "      <Cell N='QuickStyleLineMatrix' V='1'/>\n";
	shape += "      <Cell N='QuickStyleFillMatrix' V='1'/>\n";
	shape += "      <Cell N='QuickStyleEffectsMatrix' V='1'/>\n";
	shape += "      <Cell N='QuickStyleFontMatrix' V='1'/>\n";

	// Brush color
	shape += "	  <Cell N='FillGradientEnabled' V='0'/>\n";
	shape += "	  <Cell N='FillForegnd' V='" + colorToString(brushColor) + "'/>\n";

	if (brushColor.alpha() == 0)
		shape = "    <Cell N='FillPattern' V='0'/>";
	else
	{
		// Brush color alpha
		if (brushColor.alpha() != 255)
		{
			shape += "	  <Cell N='FillForegndTrans' V='" + QString::number(1.0 - brushColor.alphaF()) + "'/>\n";
			shape += "	  <Cell N='FillBkgndTrans' V='" + QString::number(1.0 - brushColor.alphaF()) + "'/>\n";
		}
	}

	// Pen width of 16.0 = 1 pt.  1 pt = 1/72 in.
	shape += "	  <Cell N='LineWeight' V='" + QString::number(pen.widthF() / 16 / 72) + "'/>\n";

	// Pen color
	shape += "	  <Cell N='LineGradientEnabled' V='0'/>\n";
	shape += "	  <Cell N='LineColor' V='" + colorToString(penColor) + "'/>\n";

	if (penColor.alpha() == 0)
		shape += "	  <Cell N='LinePattern' V='0'/>\n";
	else
	{
		// Pen color alpha
		if (penColor.alpha() != 255)
			shape += "	  <Cell N='LineColorTrans' V='" + QString::number(1.0 - penColor.alphaF()) + "'/>\n";

		// Pen style
		switch (pen.style())
		{
		case Qt::NoPen:
			shape += "	  <Cell N='LinePattern' V='0'/>\n";
			break;
		case Qt::DotLine:
			shape += "	  <Cell N='LinePattern' V='10'/>\n";
			break;
		case Qt::DashLine:
		case Qt::DashDotLine:
		case Qt::DashDotDotLine:
			shape += "	  <Cell N='LinePattern' V='9'/>\n";
			break;
		default:
			break;
		}
	}

	return shape;
}

QString VsdxWriter::writeArrow(const DrawingArrow& arrow, const QPen& pen, bool startArrow)
{
	QString shape;

	if (arrow.style() != Drawing::ArrowNone)
	{
		QString arrowStr = "0";
		QString arrowSizeStr = "2";

		if (arrow.style() == Drawing::ArrowNormal)
			arrowStr = "3";
		else if (arrow.style() == Drawing::ArrowTriangleFilled || arrow.style() == Drawing::ArrowConcaveFilled)
			arrowStr = "4";
		else if (arrow.style() == Drawing::ArrowTriangle || arrow.style() == Drawing::ArrowConcave)
			arrowStr = "16";
		else if (arrow.style() == Drawing::ArrowCircleFilled)
			arrowStr = "10";
		else if (arrow.style() == Drawing::ArrowCircle)
			arrowStr = "20";

		// This is just a basic mapping of the arrow.size() field to the various options allowed in Visio
		if (arrow.size() < pen.widthF() * 2)
			arrowSizeStr = "0";
		else if (arrow.size() < pen.widthF() * 5)
			arrowSizeStr = "1";

		if (startArrow)
		{
			shape += "	  <Cell N='BeginArrow' V='" + arrowStr + "'/>\n";
			shape += "	  <Cell N='BeginArrowSize' V='" + arrowSizeStr + "'/>\n";
		}
		else
		{
			shape += "	  <Cell N='EndArrow' V='" + arrowStr + "'/>\n";
			shape += "	  <Cell N='EndArrowSize' V='" + arrowSizeStr + "'/>\n";
		}
	}

	return shape;
}

//==================================================================================================

QString VsdxWriter::colorToString(const QColor& color) const
{
	return QString("#%1%2%3").arg(color.red(), 2, 16, QChar('0')).arg(
		color.green(), 2, 16, QChar('0')).arg(color.blue(), 2, 16, QChar('0')).toLower();
}

//==================================================================================================

QPointF VsdxWriter::mapFromScene(const QPointF& position) const
{
	QPointF newPosition;
	newPosition.setX(mPageMargin - mDrawing->sceneRect().left() * mDrawingScale + position.x() * mDrawingScale);
	newPosition.setY(mPageMargin + mDrawing->sceneRect().bottom() * mDrawingScale - position.y() * mDrawingScale);
	return newPosition;
}
