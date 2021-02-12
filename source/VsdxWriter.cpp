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
	mShapeDepth = 0;

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

	contentTypes += "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n";
	contentTypes += "<Types xmlns='http://schemas.openxmlformats.org/package/2006/content-types'>\n";
	contentTypes += "  <Default Extension='rels' ContentType='application/vnd.openxmlformats-package.relationships+xml'/>\n";
	contentTypes += "  <Default Extension='xml' ContentType='application/xml'/>\n";
	contentTypes += "  <Override PartName='/visio/document.xml' ContentType='application/vnd.ms-visio.drawing.main+xml'/>\n";
	contentTypes += "  <Override PartName='/visio/pages/pages.xml' ContentType='application/vnd.ms-visio.pages+xml'/>\n";
	contentTypes += "  <Override PartName='/visio/pages/page1.xml' ContentType='application/vnd.ms-visio.page+xml'/>\n";
	contentTypes += "  <Override PartName='/visio/windows.xml' ContentType='application/vnd.ms-visio.windows+xml'/>\n";
	contentTypes += "  <Override PartName='/docProps/core.xml' ContentType='application/vnd.openxmlformats-package.core-properties+xml'/>\n";
	contentTypes += "  <Override PartName='/docProps/app.xml' ContentType='application/vnd.openxmlformats-officedocument.extended-properties+xml'/>\n";
	contentTypes += "  <Override PartName='/docProps/custom.xml' ContentType='application/vnd.openxmlformats-officedocument.custom-properties+xml'/>\n";
	contentTypes += "</Types>\n";

	return contentTypes;
}

QString VsdxWriter::writeRels()
{
	QString rels;

	rels += "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n";
	rels += "<Relationships xmlns='http://schemas.openxmlformats.org/package/2006/relationships'>\n";
	rels += "  <Relationship Id='rId3' Type='http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties' Target='docProps/core.xml'/>\n";
	rels += "  <Relationship Id='rId1' Type='http://schemas.microsoft.com/visio/2010/relationships/document' Target='visio/document.xml'/>\n";
	rels += "  <Relationship Id='rId5' Type='http://schemas.openxmlformats.org/officeDocument/2006/relationships/custom-properties' Target='docProps/custom.xml'/>\n";
	rels += "  <Relationship Id='rId4' Type='http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties' Target='docProps/app.xml'/>\n";
	rels += "</Relationships>\n";

	return rels;
}

QString VsdxWriter::writeApp()
{
	QString app;

	app += "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n";
	app += "<Properties xmlns='http://schemas.openxmlformats.org/officeDocument/2006/extended-properties' xmlns:vt='http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes'>\n";
	app += "  <Template/>\n";
	app += "  <Application>Microsoft Visio</Application>\n";
	app += "  <ScaleCrop>false</ScaleCrop>\n";
	app += "  <HeadingPairs>\n";
	app += "    <vt:vector size='2' baseType='variant'>\n";
	app += "      <vt:variant>\n";
	app += "        <vt:lpstr>Pages</vt:lpstr>\n";
	app += "      </vt:variant>\n";
	app += "      <vt:variant>\n";
	app += "        <vt:i4>1</vt:i4>\n";
	app += "      </vt:variant>\n";
	app += "    </vt:vector>\n";
	app += "  </HeadingPairs>\n";
	app += "  <TitlesOfParts>\n";
	app += "    <vt:vector size='1' baseType='lpstr'>\n";
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

	core += "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n";
	core += "<cp:coreProperties xmlns:cp='http://schemas.openxmlformats.org/package/2006/metadata/core-properties' xmlns:dc='http://purl.org/dc/elements/1.1/' xmlns:dcterms='http://purl.org/dc/terms/' xmlns:dcmitype='http://purl.org/dc/dcmitype/' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>\n";
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

	custom += "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n";
	custom += "<Properties xmlns='http://schemas.openxmlformats.org/officeDocument/2006/custom-properties' xmlns:vt='http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes'>\n";
	custom += "  <property fmtid='{D5CDD505-2E9C-101B-9397-08002B2CF9AE}' pid='2' name='_VPID_ALTERNATENAMES'>\n";
	custom += "    <vt:lpwstr/>\n";
	custom += "  </property>\n";
	custom += "  <property fmtid='{D5CDD505-2E9C-101B-9397-08002B2CF9AE}' pid='3' name='BuildNumberCreated'>\n";
	custom += "    <vt:i4>1074146641</vt:i4>\n";
	custom += "  </property>\n";
	custom += "  <property fmtid='{D5CDD505-2E9C-101B-9397-08002B2CF9AE}' pid='4' name='BuildNumberEdited'>\n";
	custom += "    <vt:i4>1074146641</vt:i4>\n";
	custom += "  </property>\n";
	custom += "  <property fmtid='{D5CDD505-2E9C-101B-9397-08002B2CF9AE}' pid='5' name='IsMetric'>\n";
	custom += QString("    <vt:bool>") + ((mDrawingUnits == "mm") ? "true" : "false") + "</vt:bool>\n";
	custom += "  </property>\n";
	custom += "</Properties>\n";

	return custom;
}

QString VsdxWriter::writePagesRels()
{
	QString rels;

	rels += "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n";
	rels += "<Relationships xmlns='http://schemas.openxmlformats.org/package/2006/relationships'>\n";
	rels += "  <Relationship Id='rId1' Type='http://schemas.microsoft.com/visio/2010/relationships/page' Target='page1.xml'/>\n";
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

	pages += "<?xml version='1.0' encoding='utf-8'?>\n";
	pages += "<Pages xmlns='http://schemas.microsoft.com/office/visio/2012/main' xmlns:r='http://schemas.openxmlformats.org/officeDocument/2006/relationships' xml:space='preserve'>\n";
	pages += " <Page ID='0' NameU='Page-1' Name='Page-1'>\n";
	pages += "    <PageSheet LineStyle='0' FillStyle='0' TextStyle='0'>\n";
	pages += "      <Cell N='PageWidth' V='" + pageWidth + "'/>\n";
	pages += "      <Cell N='PageHeight' V='" + pageHeight + "'/>\n";
	pages += "      <Cell N='ShdwOffsetX' V='0.125'/>\n";
	pages += "      <Cell N='ShdwOffsetY' V='-0.125'/>\n";
	pages += "      <Cell N='PageScale' V='1' U='IN_F'/>\n";
	pages += "      <Cell N='DrawingScale' V='1' U='IN_F'/>\n";
	pages += "      <Cell N='DrawingSizeType' V='3'/>\n";
	pages += "      <Cell N='DrawingScaleType' V='0'/>\n";
	pages += "      <Cell N='InhibitSnap' V='0'/>\n";
	pages += "      <Cell N='PageLockReplace' V='0' U='BOOL'/>\n";
	pages += "      <Cell N='PageLockDuplicate' V='0' U='BOOL'/>\n";
	pages += "      <Cell N='UIVisibility' V='0'/>\n";
	pages += "      <Cell N='ShdwType' V='0'/>\n";
	pages += "      <Cell N='ShdwObliqueAngle' V='0'/>\n";
	pages += "      <Cell N='ShdwScaleFactor' V='1'/>\n";
	pages += "      <Cell N='DrawingResizeType' V='2'/>\n";
	pages += "      <Cell N='PageShapeSplit' V='1'/>\n";
	pages += "      <Cell N='XRulerOrigin' V='" + xRulerOrigin + "' U='" + mDrawingUnits + "'/>\n";
	pages += "      <Cell N='YRulerOrigin' V='" + yRulerOrigin + "' U='" + mDrawingUnits + "'/>\n";
	pages += "      <Cell N='XGridOrigin' V='" + xGridOrigin + "' U='" + mDrawingUnits + "'/>\n";
	pages += "      <Cell N='YGridOrigin' V='" + yGridOrigin + "' U='" + mDrawingUnits + "'/>\n";
	pages += "    </PageSheet>\n";
	pages += "    <Rel r:id='rId1'/>\n";
	pages += "  </Page>\n";
	pages += "</Pages>\n";

	return pages;
}

QString VsdxWriter::writePage1()
{
	QString page;

	page += "<?xml version='1.0' encoding='utf-8'?>\n";
	page += "<PageContents xmlns='http://schemas.microsoft.com/office/visio/2012/main' xmlns:r='http://schemas.openxmlformats.org/officeDocument/2006/relationships' xml:space='preserve'>\n";
	page += "  <Shapes>\n";

	page += writeItems(mDrawing->items());

	page += "  </Shapes>\n";
	page += "</PageContents>\n";

	return page;
}

QString VsdxWriter::writeDocumentRels()
{
	QString rels;

	rels += "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n";
	rels += "<Relationships xmlns='http://schemas.openxmlformats.org/package/2006/relationships'>\n";
	rels += "  <Relationship Id='rId2' Type='http://schemas.microsoft.com/visio/2010/relationships/windows' Target='windows.xml'/>\n";
	rels += "  <Relationship Id='rId1' Type='http://schemas.microsoft.com/visio/2010/relationships/pages' Target='pages/pages.xml'/>\n";
	rels += "</Relationships>\n";

	return rels;
}

QString VsdxWriter::writeDocument()
{
	QString document;

	document += "<?xml version='1.0' encoding='utf-8'?>\n";
	document += "<VisioDocument xmlns='http://schemas.microsoft.com/office/visio/2012/main' xmlns:r='http://schemas.openxmlformats.org/officeDocument/2006/relationships' xml:space='preserve'>\n";
	document += "</VisioDocument>\n";

	return document;
}

QString VsdxWriter::writeWindows()
{
	QString windows;

	windows += "<?xml version='1.0' encoding='utf-8'?>\n";
	windows += "<Windows xmlns='http://schemas.microsoft.com/office/visio/2012/main' xmlns:r='http://schemas.openxmlformats.org/officeDocument/2006/relationships' xml:space='preserve'>\n";
	windows += "</Windows>\n";

	return windows;
}

//==================================================================================================

QString VsdxWriter::writeItems(const QList<DrawingItem*>& items)
{
	QString shapes;
	QString key;

	mShapeDepth++;

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

	mShapeDepth--;

	return shapes;
}

QString VsdxWriter::writeRectItem(DrawingRectItem* item)
{
	QString shape;

	if (item)
	{
		QString indent(2 + 2 * mShapeDepth, ' ');
		QString indentP(2 + 2 * mShapeDepth + 2, ' ');
		QString indentPP(2 + 2 * mShapeDepth + 4, ' ');
		QString indentPPP(2 + 2 * mShapeDepth + 6, ' ');

		mShapeIndex++;
		shape += indent + QString("<Shape ID='%1' Type='Shape' LineStyle='3' FillStyle='3' TextStyle='3'>\n").arg(mShapeIndex);

		shape += writePositionAndSize(item->position(), item->boundingRect(), item->transform());

		if (item->cornerRadius() != 0)
			shape += indentPP + QString("<Cell N='Rounding' V='%1'/>\n").arg(item->cornerRadius() * mDrawingScale, 0, 'g', 10);

		shape += writeStyle(item->brush(), item->pen());

		shape += indentP + QString("<Section N='Geometry' IX='0'>\n");
		shape += indentPP + QString("<Cell N='NoFill' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoLine' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoShow' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoSnap' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoQuickDrag' V='0'/>\n");
		shape += indentPP + QString("<Row T='RelMoveTo' IX='1'>\n");
		shape += indentPPP + QString("<Cell N='X' V='0'/>\n");
		shape += indentPPP + QString("<Cell N='Y' V='0'/>\n");
		shape += indentPP + QString("</Row>\n");
		shape += indentPP + QString("<Row T='RelLineTo' IX='2'>\n");
		shape += indentPPP + QString("<Cell N='X' V='1'/>\n");
		shape += indentPPP + QString("<Cell N='Y' V='0'/>\n");
		shape += indentPP + QString("</Row>\n");
		shape += indentPP + QString("<Row T='RelLineTo' IX='3'>\n");
		shape += indentPPP + QString("<Cell N='X' V='1'/>\n");
		shape += indentPPP + QString("<Cell N='Y' V='1'/>\n");
		shape += indentPP + QString("</Row>\n");
		shape += indentPP + QString("<Row T='RelLineTo' IX='4'>\n");
		shape += indentPPP + QString("<Cell N='X' V='0'/>\n");
		shape += indentPPP + QString("<Cell N='Y' V='1'/>\n");
		shape += indentPP + QString("</Row>\n");
		shape += indentPP + QString("<Row T='RelLineTo' IX='5'>\n");
		shape += indentPPP + QString("<Cell N='X' V='0'/>\n");
		shape += indentPPP + QString("<Cell N='Y' V='0'/>\n");
		shape += indentPP + QString("</Row>\n");
		shape += indentP + QString("</Section>\n");
		shape += indent + QString("</Shape>\n");
	}

	return shape;
}

QString VsdxWriter::writeEllipseItem(DrawingEllipseItem* item)
{
	QString shape;

	if (item)
	{
		QString indent(2 + 2 * mShapeDepth, ' ');
		QString indentP(2 + 2 * mShapeDepth + 2, ' ');
		QString indentPP(2 + 2 * mShapeDepth + 4, ' ');
		QString indentPPP(2 + 2 * mShapeDepth + 6, ' ');
		QSizeF shapeSize;

		mShapeIndex++;
		shape += indent + QString("<Shape ID='%1' Type='Shape' LineStyle='3' FillStyle='3' TextStyle='3'>\n").arg(mShapeIndex);

		shape += writePositionAndSize(item->position(), item->boundingRect(), item->transform(), &shapeSize);
		shape += writeStyle(item->brush(), item->pen());

		shape += indentP + QString("<Section N='Geometry' IX='0'>\n");
		shape += indentPP + QString("<Cell N='NoFill' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoLine' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoShow' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoSnap' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoQuickDrag' V='0'/>\n");
		shape += indentPP + QString("<Row T='Ellipse' IX='1'>\n");
		shape += indentPPP + QString("<Cell N='X' V='%1' F='Width*0.5'/>\n").arg(shapeSize.width() / 2, 0, 'g', 10);
		shape += indentPPP + QString("<Cell N='Y' V='%1' F='Height*0.5'/>\n").arg(shapeSize.height() / 2, 0, 'g', 10);
		shape += indentPPP + QString("<Cell N='A' V='%1' U='DL' F='Width*1'/>\n").arg(shapeSize.width(), 0, 'g', 10);
		shape += indentPPP + QString("<Cell N='B' V='%1' U='DL' F='Height*0.5'/>\n").arg(shapeSize.height() / 2, 0, 'g', 10);
		shape += indentPPP + QString("<Cell N='C' V='%1' U='DL' F='Width*0.5'/>\n").arg(shapeSize.width() / 2, 0, 'g', 10);
		shape += indentPPP + QString("<Cell N='D' V='%1' U='DL' F='Height*1'/>\n").arg(shapeSize.height(), 0, 'g', 10);
		shape += indentPP + QString("</Row>\n");
		shape += indentP + QString("</Section>\n");
		shape += indent + QString("</Shape>\n");
	}

	return shape;
}

QString VsdxWriter::writeLineItem(DrawingLineItem* item)
{
	QString shape;

	if (item)
	{
		QString indent(2 + 2 * mShapeDepth, ' ');
		QString indentP(2 + 2 * mShapeDepth + 2, ' ');
		QString indentPP(2 + 2 * mShapeDepth + 4, ' ');
		QString indentPPP(2 + 2 * mShapeDepth + 6, ' ');
		QSizeF shapeSize;

		mShapeIndex++;
		shape += indent + QString("<Shape ID='%1' Type='Shape' LineStyle='3' FillStyle='3' TextStyle='3'>\n").arg(mShapeIndex);

		shape += writePositionAndSizeForLine(item->mapToScene(item->line().p1()), item->mapToScene(item->line().p2()), &shapeSize);
		shape += writeStyle(Qt::transparent, item->pen());
		shape += writeArrow(item->startArrow(), item->pen(), true);
		shape += writeArrow(item->endArrow(), item->pen(), false);

		shape += indentP + QString("<Section N='Geometry' IX='0'>\n");
		shape += indentPP + QString("<Cell N='NoFill' V='1'/>\n");
		shape += indentPP + QString("<Cell N='NoLine' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoShow' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoSnap' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoQuickDrag' V='0'/>\n");
		shape += indentPP + QString("<Row T='MoveTo' IX='1'>\n");
		shape += indentPPP + QString("<Cell N='X' V='0' F='Width*0'/>\n");
		shape += indentPPP + QString("<Cell N='Y' V='0'/>\n");
		shape += indentPP + QString("</Row>\n");
		shape += indentPP + QString("<Row T='LineTo' IX='2'>\n");
		shape += indentPPP + QString("<Cell N='X' V='%1' F='Width*1'/>\n").arg(shapeSize.width(), 0, 'g', 10);
		shape += indentPPP + QString("<Cell N='Y' V='0'/>\n");
		shape += indentPP + QString("</Row>\n");
		shape += indentP + QString("</Section>\n");
		shape += indent + QString("</Shape>\n");
	}

	return shape;
}

QString VsdxWriter::writeCurveItem(DrawingCurveItem* item)
{
	QString shape;

	if (item)
	{
		QString indent(2 + 2 * mShapeDepth, ' ');
		QString indentP(2 + 2 * mShapeDepth + 2, ' ');
		QString indentPP(2 + 2 * mShapeDepth + 4, ' ');
		QString indentPPP(2 + 2 * mShapeDepth + 6, ' ');
		QRectF boundingRect = item->boundingRect();
		QPointF p1Rel((item->curveStartPosition().x() - boundingRect.left()) / boundingRect.width(),
					  (boundingRect.bottom() - item->curveStartPosition().y()) / boundingRect.height());
		QPointF cp1Rel((item->curveStartControlPosition().x() - boundingRect.left()) / boundingRect.width(),
					  (boundingRect.bottom() - item->curveStartControlPosition().y()) / boundingRect.height());
		QPointF cp2Rel((item->curveEndControlPosition().x() - boundingRect.left()) / boundingRect.width(),
					  (boundingRect.bottom() - item->curveEndControlPosition().y()) / boundingRect.height());
		QPointF p2Rel((item->curveEndPosition().x() - boundingRect.left()) / boundingRect.width(),
					  (boundingRect.bottom() - item->curveEndPosition().y()) / boundingRect.height());

		mShapeIndex++;
		shape += indent + QString("<Shape ID='%1' Type='Shape' LineStyle='3' FillStyle='3' TextStyle='3'>\n").arg(mShapeIndex);

		shape += writePositionAndSize(item->position(), item->boundingRect(), item->transform());
		shape += writeStyle(Qt::transparent, item->pen());
		shape += writeArrow(item->startArrow(), item->pen(), true);
		shape += writeArrow(item->endArrow(), item->pen(), false);

		shape += indentP + QString("<Section N='Geometry' IX='0'>\n");
		shape += indentPP + QString("<Cell N='NoFill' V='1'/>\n");
		shape += indentPP + QString("<Cell N='NoLine' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoShow' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoSnap' V='0'/>\n");
		shape += indentPP + QString("<Cell N='NoQuickDrag' V='0'/>\n");
		shape += indentPP + QString("<Row T='RelMoveTo' IX='1'>\n");
		shape += indentPPP + QString("<Cell N='X' V='%1'/>\n").arg(p1Rel.x(), 0, 'g', 10);
		shape += indentPPP + QString("<Cell N='Y' V='%1'/>\n").arg(p1Rel.y(), 0, 'g', 10);
		shape += indentPP + QString("</Row>\n");
		shape += indentPP + QString("<Row T='RelCubBezTo' IX='2'>\n");
		shape += indentPPP + QString("<Cell N='X' V='%1'/>\n").arg(p2Rel.x(), 0, 'g', 10);
		shape += indentPPP + QString("<Cell N='Y' V='%1'/>\n").arg(p2Rel.y(), 0, 'g', 10);
		shape += indentPPP + QString("<Cell N='A' V='%1'/>\n").arg(cp1Rel.x(), 0, 'g', 10);
		shape += indentPPP + QString("<Cell N='B' V='%1'/>\n").arg(cp1Rel.y(), 0, 'g', 10);
		shape += indentPPP + QString("<Cell N='C' V='%1'/>\n").arg(cp2Rel.x(), 0, 'g', 10);
		shape += indentPPP + QString("<Cell N='D' V='%1'/>\n").arg(cp2Rel.y(), 0, 'g', 10);
		shape += indentPP + QString("</Row>\n");
		shape += indentP + QString("</Section>\n");
		shape += indent + QString("</Shape>\n");
	}

	return shape;
}

QString VsdxWriter::writePolylineItem(DrawingPolylineItem* item)
{
	QString shape;

	if (item)
	{
		QPolygonF polyline = item->polyline();
		QPolygonF mappedPolyline;
		for(auto pointIter = polyline.begin(); pointIter != polyline.end(); pointIter++)
			mappedPolyline.append(mapFromScene(item->mapToScene(*pointIter)));
		QRectF boundingRect = mappedPolyline.boundingRect();
		int rowIndex = 1;
		qreal x, y, xFormulaScale, yFormulaScale;

		mShapeIndex++;

		QString indexStr = QString::number(mShapeIndex);
		QString pinXStr = QString::number(boundingRect.left());
		QString pinYStr = QString::number(boundingRect.bottom());
		QString widthStr = QString::number(boundingRect.width());
		QString heightStr = QString::number(boundingRect.height());
		QString rowIndexStr, xStr, yStr, xFormulaStr, yFormulaStr;

		shape += "    <Shape ID='" + indexStr + "' Type='Shape' LineStyle='3' FillStyle='3' TextStyle='3'>\n";
		shape += "      <Cell N='PinX' V='" + pinXStr + "'/>\n";
		shape += "      <Cell N='PinY' V='" + pinYStr + "'/>\n";
		shape += "      <Cell N='Width' V='" + widthStr + "'/>\n";
		shape += "      <Cell N='Height' V='" + heightStr + "'/>\n";
		shape += "      <Cell N='LocPinX' V='0' F='Width*0'/>\n";
		shape += "      <Cell N='LocPinY' V='" + heightStr + "' F='Height*1'/>\n";
		shape += "      <Cell N='Angle' V='0'/>\n";
		shape += "      <Cell N='FlipX' V='0'/>\n";
		shape += "      <Cell N='FlipY' V='0'/>\n";
		shape += "      <Cell N='ResizeMode' V='0'/>\n";

		shape += writeStyle(Qt::transparent, item->pen());
		shape += writeArrow(item->startArrow(), item->pen(), true);
		shape += writeArrow(item->endArrow(), item->pen(), false);

		shape += "      <Section N='Geometry' IX='0'>\n";
		shape += "    	  <Cell N='NoFill' V='1'/>\n";
		shape += "    	  <Cell N='NoLine' V='0'/>\n";
		shape += "    	  <Cell N='NoShow' V='0'/>\n";
		shape += "    	  <Cell N='NoSnap' V='0'/>\n";
		shape += "    	  <Cell N='NoQuickDrag' V='0'/>\n";

		for(auto pointIter = mappedPolyline.begin(); pointIter != mappedPolyline.end(); pointIter++)
		{
			x = pointIter->x() - boundingRect.left();
			y = pointIter->y() - boundingRect.top();
			xFormulaScale = x / boundingRect.width();
			yFormulaScale = y / boundingRect.height();

			rowIndexStr = QString::number(rowIndex);
			xStr = QString::number(x);
			yStr = QString::number(y);
			xFormulaStr = "Width*" + QString::number(xFormulaScale);
			yFormulaStr = "Height*" + QString::number(yFormulaScale);

			if (rowIndex == 1)
				shape += "    	  <Row T='MoveTo' IX='" + rowIndexStr + "'>\n";
			else
				shape += "    	  <Row T='LineTo' IX='" + rowIndexStr + "'>\n";

			shape += "          <Cell N='X' V='" + xStr + "' F='" + xFormulaStr + "'/>\n";
			shape += "          <Cell N='Y' V='" + yStr + "' F='" + yFormulaStr + "'/>\n";
			shape += "        </Row>\n";

			rowIndex++;
		}

		shape += "      </Section>\n";
		shape += "    </Shape>\n";
	}

	return shape;
}

QString VsdxWriter::writePolygonItem(DrawingPolygonItem* item)
{
	QString shape;

	if (item)
	{
		QPolygonF polygon = item->polygon();
		QPolygonF mappedPolygon;
		for(auto pointIter = polygon.begin(); pointIter != polygon.end(); pointIter++)
			mappedPolygon.append(mapFromScene(item->mapToScene(*pointIter)));
		QRectF boundingRect = mappedPolygon.boundingRect();
		int rowIndex = 1;
		qreal x, y, xFormulaScale, yFormulaScale;

		mShapeIndex++;

		QString indexStr = QString::number(mShapeIndex);
		QString pinXStr = QString::number(boundingRect.left());
		QString pinYStr = QString::number(boundingRect.bottom());
		QString widthStr = QString::number(boundingRect.width());
		QString heightStr = QString::number(boundingRect.height());
		QString rowIndexStr, xStr, yStr, xFormulaStr, yFormulaStr;

		shape += "    <Shape ID='" + indexStr + "' Type='Shape' LineStyle='3' FillStyle='3' TextStyle='3'>\n";
		shape += "      <Cell N='PinX' V='" + pinXStr + "'/>\n";
		shape += "      <Cell N='PinY' V='" + pinYStr + "'/>\n";
		shape += "      <Cell N='Width' V='" + widthStr + "'/>\n";
		shape += "      <Cell N='Height' V='" + heightStr + "'/>\n";
		shape += "      <Cell N='LocPinX' V='0' F='Width*0'/>\n";
		shape += "      <Cell N='LocPinY' V='" + heightStr + "' F='Height*1'/>\n";
		shape += "      <Cell N='Angle' V='0'/>\n";
		shape += "      <Cell N='FlipX' V='0'/>\n";
		shape += "      <Cell N='FlipY' V='0'/>\n";
		shape += "      <Cell N='ResizeMode' V='0'/>\n";

		shape += writeStyle(item->brush(), item->pen());

		shape += "      <Section N='Geometry' IX='0'>\n";
		shape += "    	  <Cell N='NoFill' V='0'/>\n";
		shape += "    	  <Cell N='NoLine' V='0'/>\n";
		shape += "    	  <Cell N='NoShow' V='0'/>\n";
		shape += "    	  <Cell N='NoSnap' V='0'/>\n";
		shape += "    	  <Cell N='NoQuickDrag' V='0'/>\n";

		for(auto pointIter = mappedPolygon.begin(); pointIter != mappedPolygon.end(); pointIter++)
		{
			x = pointIter->x() - boundingRect.left();
			y = pointIter->y() - boundingRect.top();
			xFormulaScale = x / boundingRect.width();
			yFormulaScale = y / boundingRect.height();

			rowIndexStr = QString::number(rowIndex);
			xStr = QString::number(x);
			yStr = QString::number(y);
			xFormulaStr = "Width*" + QString::number(xFormulaScale);
			yFormulaStr = "Height*" + QString::number(yFormulaScale);

			if (rowIndex == 1)
				shape += "    	  <Row T='MoveTo' IX='" + rowIndexStr + "'>\n";
			else
				shape += "    	  <Row T='LineTo' IX='" + rowIndexStr + "'>\n";

			shape += "          <Cell N='X' V='" + xStr + "' F='" + xFormulaStr + "'/>\n";
			shape += "          <Cell N='Y' V='" + yStr + "' F='" + yFormulaStr + "'/>\n";
			shape += "        </Row>\n";

			rowIndex++;
		}

		x = mappedPolygon.first().x() - boundingRect.left();
		y = mappedPolygon.first().y() - boundingRect.top();

		rowIndexStr = QString::number(rowIndex);
		xStr = QString::number(x);
		yStr = QString::number(y);

		shape += "    	  <Row T='LineTo' IX='" + rowIndexStr + "'>\n";
		shape += "          <Cell N='X' V='" + xStr + "' F='Geometry1.X1'/>\n";
		shape += "          <Cell N='Y' V='" + yStr + "' F='Geometry1.Y1'/>\n";
		shape += "        </Row>\n";

		shape += "      </Section>\n";
		shape += "    </Shape>\n";
	}

	return shape;
}

QString VsdxWriter::writeTextItem(DrawingTextItem* item)
{
	QString shape;

	if (item)
	{
		QPointF pinPosition = mapFromScene(item->position());
		QRectF rect(QPointF(0, 0), item->boundingRect().size() * mDrawingScale * 1.25);

		// Change anchor pin location based on alignment
		// bottom-left ones are correct
		qreal locPinX = 0, locPinY = 0;
		Qt::Alignment horizontalAlignment = (item->alignment() & Qt::AlignHorizontal_Mask);
		Qt::Alignment verticalAlignment = (item->alignment() & Qt::AlignVertical_Mask);
		if (horizontalAlignment & Qt::AlignHCenter)
			locPinX = rect.width() / 2;
		else if (horizontalAlignment & Qt::AlignRight)
			locPinX = rect.width();
		if (verticalAlignment & Qt::AlignVCenter)
			locPinY = rect.height() / 2;
		else if (verticalAlignment & Qt::AlignTop)
			locPinY = rect.height();

		// Calculate rotation angle
		qreal angle = 0;
		if (item->transform().m12() != 0) angle = qAsin(item->transform().m12());
		else angle = qAcos(item->transform().m11());

		mShapeIndex++;

		QString indexStr = QString::number(mShapeIndex);
		QString pinXStr = QString::number(pinPosition.x());
		QString pinYStr = QString::number(pinPosition.y());
		QString widthStr = QString::number(rect.width());
		QString heightStr = QString::number(rect.height());
		QString locPinXStr = QString::number(locPinX);
		QString locPinYStr = QString::number(locPinY);
		QString locPinXFormulaStr = "Width*" + QString::number(locPinX / rect.width());
		QString locPinYFormulaStr = "Height*" + QString::number(locPinY / rect.height());
		QString angleStr = QString::number(angle, 'g', 10);

		shape += "    <Shape ID='" + indexStr + "' Type='Shape' LineStyle='3' FillStyle='3' TextStyle='3'>\n";
		shape += "      <Cell N='PinX' V='" + pinXStr + "'/>\n";
		shape += "      <Cell N='PinY' V='" + pinYStr + "'/>\n";
		shape += "      <Cell N='Width' V='" + widthStr + "'/>\n";
		shape += "      <Cell N='Height' V='" + heightStr + "'/>\n";
		shape += "      <Cell N='LocPinX' V='" + locPinXStr + "' F='" + locPinXFormulaStr + "'/>\n";
		shape += "      <Cell N='LocPinY' V='" + locPinYStr + "' F='" + locPinYFormulaStr + "'/>\n";
		shape += "      <Cell N='Angle' V='" + angleStr + "'/>\n";
		shape += "      <Cell N='FlipX' V='0'/>\n";
		shape += "      <Cell N='FlipY' V='0'/>\n";
		shape += "      <Cell N='ResizeMode' V='0'/>\n";
		shape += "      <Cell N='LeftMargin' V='0.01388888888888889' U='PT'/>\n";
		shape += "      <Cell N='RightMargin' V='0.01388888888888889' U='PT'/>\n";
		shape += "      <Cell N='TopMargin' V='0' U='PT'/>\n";
		shape += "      <Cell N='BottomMargin' V='0' U='PT'/>\n";

		shape += writeStyle(Qt::transparent, Qt::NoPen, item->textBrush(), item->font(), item->alignment());

		shape += "      <Section N='Geometry' IX='0'>\n";
		shape += "        <Cell N='NoFill' V='0'/>\n";
		shape += "        <Cell N='NoLine' V='0'/>\n";
		shape += "        <Cell N='NoShow' V='0'/>\n";
		shape += "        <Cell N='NoSnap' V='0'/>\n";
		shape += "        <Cell N='NoQuickDrag' V='0'/>\n";
		shape += "        <Row T='RelMoveTo' IX='1'>\n";
		shape += "          <Cell N='X' V='0'/>\n";
		shape += "          <Cell N='Y' V='0'/>\n";
		shape += "        </Row>\n";
		shape += "        <Row T='RelLineTo' IX='2'>\n";
		shape += "          <Cell N='X' V='1'/>\n";
		shape += "          <Cell N='Y' V='0'/>\n";
		shape += "        </Row>\n";
		shape += "        <Row T='RelLineTo' IX='3'>\n";
		shape += "          <Cell N='X' V='1'/>\n";
		shape += "          <Cell N='Y' V='1'/>\n";
		shape += "        </Row>\n";
		shape += "        <Row T='RelLineTo' IX='4'>\n";
		shape += "          <Cell N='X' V='0'/>\n";
		shape += "          <Cell N='Y' V='1'/>\n";
		shape += "        </Row>\n";
		shape += "        <Row T='RelLineTo' IX='5'>\n";
		shape += "          <Cell N='X' V='0'/>\n";
		shape += "          <Cell N='Y' V='0'/>\n";
		shape += "        </Row>\n";
		shape += "      </Section>\n";
		shape += "      <Text>" + item->caption() + "</Text>\n";
		shape += "    </Shape>\n";
	}

	return shape;
}

QString VsdxWriter::writeTextRectItem(DrawingTextRectItem* item)
{
	QString shape;

	if (item)
	{
		QRectF rect = QRectF(mapFromScene(item->mapToScene(item->rect().topLeft())),
							 mapFromScene(item->mapToScene(item->rect().bottomRight()))).normalized();
		qreal cornerRadius = item->cornerRadius() * mDrawingScale;

		qreal angle = 0;
		if (item->transform().m12() != 0) angle = qAsin(item->transform().m12());
		else angle = qAcos(item->transform().m11());

		QTransform shapeTransform;
		shapeTransform.translate(rect.center().x(), rect.center().y());
		shapeTransform.rotate(-angle * 180 / 3.141592654);
		shapeTransform.translate(-rect.center().x(), -rect.center().y());
		rect = shapeTransform.mapRect(rect);

		mShapeIndex++;

		QString indexStr = QString::number(mShapeIndex);
		QString pinXStr = QString::number(rect.center().x());
		QString pinYStr = QString::number(rect.center().y());
		QString widthStr = QString::number(rect.width());
		QString heightStr = QString::number(rect.height());
		QString locPinXStr = QString::number(rect.width() / 2);
		QString locPinYStr = QString::number(rect.height() / 2);
		QString angleStr = QString::number(angle, 'g', 10);
		QString cornerRadiusStr = QString::number(cornerRadius);

		shape += "    <Shape ID='" + indexStr + "' Type='Shape' LineStyle='3' FillStyle='3' TextStyle='3'>\n";
		shape += "      <Cell N='PinX' V='" + pinXStr + "'/>\n";
		shape += "      <Cell N='PinY' V='" + pinYStr + "'/>\n";
		shape += "      <Cell N='Width' V='" + widthStr + "'/>\n";
		shape += "      <Cell N='Height' V='" + heightStr + "'/>\n";
		shape += "      <Cell N='LocPinX' V='" + locPinXStr + "' F='Width*0.5'/>\n";
		shape += "      <Cell N='LocPinY' V='" + locPinYStr + "' F='Height*0.5'/>\n";
		shape += "      <Cell N='Angle' V='" + angleStr + "'/>\n";
		shape += "      <Cell N='FlipX' V='0'/>\n";
		shape += "      <Cell N='FlipY' V='0'/>\n";
		shape += "      <Cell N='ResizeMode' V='0'/>\n";
		shape += "      <Cell N='LeftMargin' V='0.01388888888888889' U='PT'/>\n";
		shape += "      <Cell N='RightMargin' V='0.01388888888888889' U='PT'/>\n";
		shape += "      <Cell N='TopMargin' V='0.01388888888888889' U='PT'/>\n";
		shape += "      <Cell N='BottomMargin' V='0.01388888888888889' U='PT'/>\n";

		if (cornerRadius != 0)
			shape += "      <Cell N='Rounding' V='" + cornerRadiusStr + "'/>\n";

		shape += writeStyle(item->brush(), item->pen(), item->textBrush(), item->font(), Qt::AlignCenter);

		shape += "      <Section N='Geometry' IX='0'>\n";
		shape += "        <Cell N='NoFill' V='0'/>\n";
		shape += "        <Cell N='NoLine' V='0'/>\n";
		shape += "        <Cell N='NoShow' V='0'/>\n";
		shape += "        <Cell N='NoSnap' V='0'/>\n";
		shape += "        <Cell N='NoQuickDrag' V='0'/>\n";
		shape += "        <Row T='RelMoveTo' IX='1'>\n";
		shape += "          <Cell N='X' V='0'/>\n";
		shape += "          <Cell N='Y' V='0'/>\n";
		shape += "        </Row>\n";
		shape += "        <Row T='RelLineTo' IX='2'>\n";
		shape += "          <Cell N='X' V='1'/>\n";
		shape += "          <Cell N='Y' V='0'/>\n";
		shape += "        </Row>\n";
		shape += "        <Row T='RelLineTo' IX='3'>\n";
		shape += "          <Cell N='X' V='1'/>\n";
		shape += "          <Cell N='Y' V='1'/>\n";
		shape += "        </Row>\n";
		shape += "        <Row T='RelLineTo' IX='4'>\n";
		shape += "          <Cell N='X' V='0'/>\n";
		shape += "          <Cell N='Y' V='1'/>\n";
		shape += "        </Row>\n";
		shape += "        <Row T='RelLineTo' IX='5'>\n";
		shape += "          <Cell N='X' V='0'/>\n";
		shape += "          <Cell N='Y' V='0'/>\n";
		shape += "        </Row>\n";
		shape += "      </Section>\n";
		shape += "      <Text>" + item->caption() + "</Text>\n";
		shape += "    </Shape>\n";
	}

	return shape;
}

QString VsdxWriter::writeTextEllipseItem(DrawingTextEllipseItem* item)
{
	QString shape;

	if (item)
	{
		QRectF ellipse = QRectF(mapFromScene(item->mapToScene(item->ellipse().topLeft())),
								mapFromScene(item->mapToScene(item->ellipse().bottomRight()))).normalized();

		qreal angle = 0;
		if (item->transform().m12() != 0) angle = qAsin(item->transform().m12());
		else angle = qAcos(item->transform().m11());

		QTransform shapeTransform;
		shapeTransform.translate(ellipse.center().x(), ellipse.center().y());
		shapeTransform.rotate(-angle * 180 / 3.141592654);
		shapeTransform.translate(-ellipse.center().x(), -ellipse.center().y());
		ellipse = shapeTransform.mapRect(ellipse);

		mShapeIndex++;

		QString indexStr = QString::number(mShapeIndex);
		QString pinXStr = QString::number(ellipse.center().x());
		QString pinYStr = QString::number(ellipse.center().y());
		QString widthStr = QString::number(ellipse.width());
		QString heightStr = QString::number(ellipse.height());
		QString locPinXStr = QString::number(ellipse.width() / 2);
		QString locPinYStr = QString::number(ellipse.height() / 2);
		QString xStr = QString::number(ellipse.width() / 2);
		QString yStr = QString::number(ellipse.height() / 2);
		QString angleStr = QString::number(angle, 'g', 10);

		shape += "    <Shape ID='" + indexStr + "' Type='Shape' LineStyle='3' FillStyle='3' TextStyle='3'>\n";
		shape += "      <Cell N='PinX' V='" + pinXStr + "'/>\n";
		shape += "      <Cell N='PinY' V='" + pinYStr + "'/>\n";
		shape += "      <Cell N='Width' V='" + widthStr + "'/>\n";
		shape += "      <Cell N='Height' V='" + heightStr + "'/>\n";
		shape += "      <Cell N='LocPinX' V='" + locPinXStr + "' F='Width*0.5'/>\n";
		shape += "      <Cell N='LocPinY' V='" + locPinYStr + "' F='Height*0.5'/>\n";
		shape += "      <Cell N='Angle' V='" + angleStr + "'/>\n";
		shape += "      <Cell N='FlipX' V='0'/>\n";
		shape += "      <Cell N='FlipY' V='0'/>\n";
		shape += "      <Cell N='ResizeMode' V='0'/>\n";
		shape += "      <Cell N='LeftMargin' V='0.01388888888888889' U='PT'/>\n";
		shape += "      <Cell N='RightMargin' V='0.01388888888888889' U='PT'/>\n";
		shape += "      <Cell N='TopMargin' V='0.01388888888888889' U='PT'/>\n";
		shape += "      <Cell N='BottomMargin' V='0.01388888888888889' U='PT'/>\n";

		shape += writeStyle(item->brush(), item->pen(), item->textBrush(), item->font(), Qt::AlignCenter);

		shape += "      <Section N='Geometry' IX='0'>\n";
		shape += "        <Cell N='NoFill' V='0'/>\n";
		shape += "        <Cell N='NoLine' V='0'/>\n";
		shape += "        <Cell N='NoShow' V='0'/>\n";
		shape += "        <Cell N='NoSnap' V='0'/>\n";
		shape += "        <Cell N='NoQuickDrag' V='0'/>\n";
		shape += "        <Row T='Ellipse' IX='1'>\n";
		shape += "          <Cell N='X' V='" + xStr + "' F='Width*0.5'/>\n";
		shape += "          <Cell N='Y' V='" + yStr + "' F='Height*0.5'/>\n";
		shape += "          <Cell N='A' V='" + widthStr + "' U='DL' F='Width*1'/>\n";
		shape += "          <Cell N='B' V='" + yStr + "' U='DL' F='Height*0.5'/>\n";
		shape += "          <Cell N='C' V='" + xStr + "' U='DL' F='Width*0.5'/>\n";
		shape += "          <Cell N='D' V='" + heightStr + "' U='DL' F='Height*1'/>\n";
		shape += "        </Row>\n";
		shape += "      </Section>\n";
		shape += "      <Text>" + item->caption() + "</Text>\n";
		shape += "    </Shape>\n";
	}

	return shape;
}

QString VsdxWriter::writePathItem(DrawingPathItem* item)
{
	QString shape;

	if (item)
	{
		/*qreal halfPenWidth = item->pen().widthF() / 2;
		QRectF itemRect = item->boundingRect().adjusted(halfPenWidth, halfPenWidth, -halfPenWidth, -halfPenWidth);
		QRectF rect = QRectF(mapFromScene(item->mapToScene(itemRect.topLeft())),
							 mapFromScene(item->mapToScene(itemRect.bottomRight()))).normalized();*/

		qreal halfPenWidth = item->pen().widthF() / 2;
		QRectF itemRect = item->boundingRect().adjusted(halfPenWidth, halfPenWidth, -halfPenWidth, -halfPenWidth);

		QPointF pinPosition = mapFromScene(item->position());
		qreal width = itemRect.width() * mDrawingScale;
		qreal height = itemRect.height() * mDrawingScale;
		qreal locPinX = -itemRect.left() * mDrawingScale;
		qreal locPinY = itemRect.bottom() * mDrawingScale;

		qreal angle = 0;
		if (item->transform().m12() != 0) angle = qAsin(item->transform().m12());
		else angle = qAcos(item->transform().m11());

		QTransform transform = item->transform();
		transform.rotate(-angle * 180 / 3.141592654);
		qreal hScale = transform.m11();
		qreal vScale = transform.m22();

		mShapeIndex++;

		QString indexStr = QString::number(mShapeIndex);
		QString pinXStr = QString::number(pinPosition.x());
		QString pinYStr = QString::number(pinPosition.y());
		QString widthStr = QString::number(width);
		QString heightStr = QString::number(height);
		QString locPinXStr = QString::number(locPinX);
		QString locPinYStr = QString::number(locPinY);
		QString locPinXFormulaStr = "Width*" + QString::number(locPinX / width);
		QString locPinYFormulaStr = "Height*" + QString::number(locPinY / height);
		QString angleStr = QString::number(angle, 'g', 10);
		QString flipXStr = (hScale < 0) ? "1" : "0";
		QString flipYStr = (vScale < 0) ? "1" : "0";

		shape += "    <Shape ID='" + indexStr + "' Type='Shape' LineStyle='3' FillStyle='3' TextStyle='3'>\n";
		shape += "      <Cell N='PinX' V='" + pinXStr + "'/>\n";
		shape += "      <Cell N='PinY' V='" + pinYStr + "'/>\n";
		shape += "      <Cell N='Width' V='" + widthStr + "'/>\n";
		shape += "      <Cell N='Height' V='" + heightStr + "'/>\n";
		shape += "      <Cell N='LocPinX' V='" + locPinXStr + "' F='" + locPinXFormulaStr + "'/>\n";
		shape += "      <Cell N='LocPinY' V='" + locPinYStr + "' F='" + locPinYFormulaStr + "'/>\n";
		shape += "      <Cell N='Angle' V='" + angleStr + "'/>\n";
		shape += "      <Cell N='FlipX' V='" + flipXStr + "'/>\n";
		shape += "      <Cell N='FlipY' V='" + flipYStr + "'/>\n";
		shape += "      <Cell N='ResizeMode' V='0'/>\n";

		shape += writeStyle(Qt::transparent, item->pen());

		shape += "      <Section N='Geometry' IX='0'>\n";
		shape += "    	  <Cell N='NoFill' V='1'/>\n";
		shape += "    	  <Cell N='NoLine' V='0'/>\n";
		shape += "    	  <Cell N='NoShow' V='0'/>\n";
		shape += "    	  <Cell N='NoSnap' V='0'/>\n";
		shape += "    	  <Cell N='NoQuickDrag' V='0'/>\n";

		QPainterPath path = item->path();
		QRectF pathRect = item->pathRect();
		int pathIndex = 1;
		QPointF prevPoint, curveEndPoint, curveStartControlPoint, curveEndControlPoint;
		bool curveDataValid = false;

		for(int i = 0; i < path.elementCount(); i++)
		{
			QPainterPath::Element element = path.elementAt(i);

			switch (element.type)
			{
			case QPainterPath::MoveToElement:
				prevPoint.setX((element.x - pathRect.left()) / pathRect.width());
				prevPoint.setY((pathRect.bottom() - element.y) / pathRect.height());
				shape += "		    <Row T=\"RelMoveTo\" IX=\"" + QString::number(pathIndex) + "\">\n";
				shape += "		      <Cell N=\"X\" V=\"" + QString::number(prevPoint.x()) + "\"/>\n";
				shape += "	    	  <Cell N=\"Y\" V=\"" + QString::number(prevPoint.y()) + "\"/>\n";
				shape += "	    	</Row>\n";
				pathIndex++;
				break;
			case QPainterPath::LineToElement:
				prevPoint.setX((element.x - pathRect.left()) / pathRect.width());
				prevPoint.setY((pathRect.bottom() - element.y) / pathRect.height());
				shape += "		    <Row T=\"RelLineTo\" IX=\"" + QString::number(pathIndex) + "\">\n";
				shape += "		      <Cell N=\"X\" V=\"" + QString::number(prevPoint.x()) + "\"/>\n";
				shape += "		      <Cell N=\"Y\" V=\"" + QString::number(prevPoint.y()) + "\"/>\n";
				shape += "		    </Row>\n";
				pathIndex++;
				break;
			case QPainterPath::CurveToElement:
				curveStartControlPoint.setX((element.x - pathRect.left()) / pathRect.width());
				curveStartControlPoint.setY((pathRect.bottom() - element.y) / pathRect.height());
				curveDataValid = false;
				break;
			case QPainterPath::CurveToDataElement:
				if (curveDataValid)
				{
					curveEndPoint.setX((element.x - pathRect.left()) / pathRect.width());
					curveEndPoint.setY((pathRect.bottom() - element.y) / pathRect.height());

					shape += "		    <Row T=\"RelCubBezTo\" IX=\"" + QString::number(pathIndex) + "\">\n";
					shape += "		      <Cell N=\"X\" V=\"" + QString::number(curveEndPoint.x()) + "\"/>\n";
					shape += "		      <Cell N=\"Y\" V=\"" + QString::number(curveEndPoint.y()) + "\"/>\n";
					shape += "		      <Cell N=\"A\" V=\"" + QString::number(curveStartControlPoint.x()) + "\"/>\n";
					shape += "		      <Cell N=\"B\" V=\"" + QString::number(curveStartControlPoint.y()) + "\"/>\n";
					shape += "		      <Cell N=\"C\" V=\"" + QString::number(curveEndControlPoint.x()) + "\"/>\n";
					shape += "		      <Cell N=\"D\" V=\"" + QString::number(curveEndControlPoint.y()) + "\"/>\n";
					shape += "		    </Row>\n";
					pathIndex++;

					prevPoint = curveEndPoint;
					curveDataValid = false;
				}
				else
				{
					curveEndControlPoint.setX((element.x - pathRect.left()) / pathRect.width());
					curveEndControlPoint.setY((pathRect.bottom() - element.y) / pathRect.height());
					curveDataValid = true;
				}
				break;
			}
		}

		shape += "      </Section>\n";
		shape += "    </Shape>\n";
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

QString VsdxWriter::writePositionAndSize(const QPointF& position, const QRectF& boundingRect,
	const QTransform& transform, QSizeF* shapeSize)
{
	QString shape;

	QPointF pinPosition = mapFromScene(position);
	QRectF normalizedRect = boundingRect.normalized();
	qreal width = normalizedRect.width() * mDrawingScale;
	qreal height = normalizedRect.height() * mDrawingScale;
	qreal locPinX = -normalizedRect.left() * mDrawingScale;
	qreal locPinY = normalizedRect.bottom() * mDrawingScale;

	qreal angle = 0;
	if (transform.m12() != 0) angle = qAsin(transform.m12());
	else angle = qAcos(transform.m11());

	QTransform shapeTransform = transform;
	shapeTransform.rotate(-angle * 180 / 3.141592654);
	qreal hScale = shapeTransform.m11();
	qreal vScale = shapeTransform.m22();

	QString indentP(2 + 2 * mShapeDepth + 2, ' ');

	shape += indentP + QString("<Cell N='PinX' V='%1'/>\n").arg(pinPosition.x(), 0, 'g', 10);
	shape += indentP + QString("<Cell N='PinY' V='%1'/>\n").arg(pinPosition.y(), 0, 'g', 10);
	shape += indentP + QString("<Cell N='Width' V='%1'/>\n").arg(width, 0, 'g', 10);
	shape += indentP + QString("<Cell N='Height' V='%1'/>\n").arg(height, 0, 'g', 10);
	shape += indentP + QString("<Cell N='LocPinX' V='%1' F='Width*%2'/>\n").arg(locPinX, 0, 'g', 10).arg(locPinX / width, 0, 'g', 10);
	shape += indentP + QString("<Cell N='LocPinY' V='%1' F='Height*%2'/>\n").arg(locPinY,0,  'g', 10).arg(locPinY / height, 0, 'g', 10);
	shape += indentP + QString("<Cell N='Angle' V='%1'/>\n").arg(angle, 0, 'g', 10);
	shape += indentP + QString("<Cell N='FlipX' V='%1'/>\n").arg((hScale < 0) ? "1" : "0");
	shape += indentP + QString("<Cell N='FlipY' V='%1'/>\n").arg((vScale < 0) ? "1" : "0");
	shape += indentP + QString("<Cell N='ResizeMode' V='0'/>\n");

	if (shapeSize)
	{
		shapeSize->setWidth(width);
		shapeSize->setHeight(height);
	}

	return shape;
}

QString VsdxWriter::writePositionAndSizeForLine(const QPointF& begin, const QPointF& end, QSizeF* shapeSize)
{
	QString shape;

	QLineF line(mapFromScene(begin), mapFromScene(end));
	qreal xSpan = qAbs(line.x2() - line.x1());
	qreal ySpan = qAbs(line.y2() - line.y1());
	qreal width = qSqrt(xSpan * xSpan + ySpan * ySpan);
	qreal angle = qAtan2(line.y2() - line.y1(), line.x2() - line.x1());

	QString indentP(2 + 2 * mShapeDepth + 2, ' ');

	shape += indentP + QString("<Cell N='PinX' V='%1' F='(BeginX+EndX)/2'/>\n").arg(line.center().x(), 0, 'g', 10);
	shape += indentP + QString("<Cell N='PinY' V='%1' F='(BeginY+EndY)/2'/>\n").arg(line.center().y(), 0, 'g', 10);
	shape += indentP + QString("<Cell N='Width' V='%1' F='SQRT((EndX-BeginX)^2+(EndY-BeginY)^2)'/>\n").arg(width, 0, 'g', 10);
	shape += indentP + QString("<Cell N='Height' V='0'/>\n");
	shape += indentP + QString("<Cell N='LocPinX' V='%1' F='Width*0.5'/>\n").arg(width / 2, 0, 'g', 10);
	shape += indentP + QString("<Cell N='LocPinY' V='0' F='Height*0.5'/>\n");
	shape += indentP + QString("<Cell N='Angle' V='%1' F='ATAN2(EndY-BeginY,EndX-BeginX)'/>\n").arg(angle, 0, 'g', 10);
	shape += indentP + QString("<Cell N='FlipX' V='0'/>\n");
	shape += indentP + QString("<Cell N='FlipY' V='0'/>\n");
	shape += indentP + QString("<Cell N='ResizeMode' V='0'/>\n");
	shape += indentP + QString("<Cell N='BeginX' V='%1'/>\n").arg(line.x1(), 0, 'g', 10);
	shape += indentP + QString("<Cell N='BeginY' V='%1'/>\n").arg(line.y1(),0,  'g', 10);
	shape += indentP + QString("<Cell N='EndX' V='%1'/>\n").arg(line.x2(), 0, 'g', 10);
	shape += indentP + QString("<Cell N='EndY' V='%1'/>\n").arg(line.y2(), 0, 'g', 10);

	if (shapeSize)
	{
		shapeSize->setWidth(width);
		shapeSize->setHeight(0);
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
	shape += "      <Cell N='QuickStyleLineMatrix' V='1'/>\n";
	shape += "      <Cell N='QuickStyleFillMatrix' V='1'/>\n";
	shape += "      <Cell N='QuickStyleEffectsMatrix' V='1'/>\n";
	shape += "      <Cell N='QuickStyleFontMatrix' V='1'/>\n";

	// Brush color
	shape += "      <Cell N='FillForegnd' V='" + colorToString(brushColor) + "'/>\n";

	if (brushColor.alpha() == 0)
		shape += "      <Cell N='FillPattern' V='0'/>\n";
	else
	{
		// Brush color alpha
		if (brushColor.alpha() != 255)
		{
			shape += "      <Cell N='FillForegndTrans' V='" + QString::number(1.0 - brushColor.alphaF()) + "'/>\n";
			shape += "      <Cell N='FillBkgndTrans' V='" + QString::number(1.0 - brushColor.alphaF()) + "'/>\n";
		}
	}

	// Pen width of 16.0 = 1 pt.  1 pt = 1/72 in.
	shape += "      <Cell N='LineWeight' V='" + QString::number(pen.widthF() / 16 / 72) + "'/>\n";

	// Pen color
	shape += "      <Cell N='LineColor' V='" + colorToString(penColor) + "'/>\n";

	if (penColor.alpha() == 0)
		shape += "      <Cell N='LinePattern' V='0'/>\n";
	else
	{
		// Pen color alpha
		if (penColor.alpha() != 255)
			shape += "      <Cell N='LineColorTrans' V='" + QString::number(1.0 - penColor.alphaF()) + "'/>\n";

		// Pen style
		switch (pen.style())
		{
		case Qt::NoPen:
			shape += "      <Cell N='LinePattern' V='0'/>\n";
			break;
		case Qt::DotLine:
			shape += "      <Cell N='LinePattern' V='10'/>\n";
			break;
		case Qt::DashLine:
		case Qt::DashDotLine:
		case Qt::DashDotDotLine:
			shape += "      <Cell N='LinePattern' V='9'/>\n";
			break;
		default:
			break;
		}
	}

	return shape;
}

QString VsdxWriter::writeStyle(const QBrush& brush, const QPen& pen, const QBrush& textBrush, const QFont& font, Qt::Alignment alignment)
{
	QString shape;

	// Alignment
	Qt::Alignment verticalAlignment = (alignment & Qt::AlignVertical_Mask);

	int verticalAlignValue = 1;
	if (verticalAlignment & Qt::AlignTop) verticalAlignValue = 0;
	else if (verticalAlignment & Qt::AlignBottom) verticalAlignValue = 2;

	if (verticalAlignValue != 1)
		shape += "      <Cell N='VerticalAlign' V='" + QString::number(verticalAlignValue) + "'/>\n";

	shape += writeStyle(brush, pen);

	Qt::Alignment horizontalAlignment = (alignment & Qt::AlignHorizontal_Mask);

	int horizontalAlignValue = 1;
	if (horizontalAlignment & Qt::AlignLeft) horizontalAlignValue = 0;
	else if (horizontalAlignment & Qt::AlignRight) horizontalAlignValue = 2;

	if (horizontalAlignValue != 1)
	{
		shape += "      <Section N='Paragraph'>\n";
		shape += "        <Row IX='0'>\n";
		shape += "          <Cell N='HorzAlign' V='" + QString::number(horizontalAlignValue) + "'/>\n";
		shape += "        </Row>\n";
		shape += "      </Section>\n";
	}

	// Font and text brush
	QString fontName = font.family();
	qreal fontSize = font.pointSizeF() * mDrawingScale * 96 / 72;		// pts

	if (fontName != "" || fontSize != 0 || font.bold() || font.italic() || font.underline() || font.strikeOut())
	{
		uint fontStyle = 0;

		if (font.bold() && font.italic()) fontStyle = 51;
		else if (font.italic()) fontStyle = 34;
		else if (font.bold()) fontStyle = 17;

		if (font.underline()) fontStyle += 4;

		shape += "      <Section N='Character'>\n";
		shape += "	      <Row IX='0'>\n";
		if (fontName != "")
			shape += "	        <Cell N='Font' V='" + fontName + "'/>\n";

		shape += "	        <Cell N='Color' V='" + colorToString(textBrush.color()) + "'/>\n";

		if (fontSize != 0)
			shape += "	        <Cell N='Size' V='" + QString::number(fontSize) + "' U='PT'/>\n";
		if (fontStyle != 0)
			shape += "	        <Cell N='Style' V='" + QString::number(fontStyle) + "'/>\n";
		if (font.strikeOut())
			shape += "	        <Cell N='Strikethru' V='1'/>\n";

		shape += "	      </Row>\n";
		shape += "	    </Section>\n";
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
			shape += "      <Cell N='BeginArrow' V='" + arrowStr + "'/>\n";
			shape += "      <Cell N='BeginArrowSize' V='" + arrowSizeStr + "'/>\n";
		}
		else
		{
			shape += "      <Cell N='EndArrow' V='" + arrowStr + "'/>\n";
			shape += "      <Cell N='EndArrowSize' V='" + arrowSizeStr + "'/>\n";
		}
	}

	return shape;
}

//==================================================================================================

QPointF VsdxWriter::mapFromScene(const QPointF& position) const
{
	QPointF newPosition;

	if (mShapeDepth == 1)
	{
		newPosition.setX(mPageMargin - mDrawing->sceneRect().left() * mDrawingScale + position.x() * mDrawingScale);
		newPosition.setY(mPageMargin + mDrawing->sceneRect().bottom() * mDrawingScale - position.y() * mDrawingScale);
	}
	else
	{
		newPosition.setX(position.x() * mDrawingScale);
		newPosition.setY(position.y() * mDrawingScale);
	}

	return newPosition;
}

//==================================================================================================

QString VsdxWriter::colorToString(const QColor& color) const
{
	return QString("#%1%2%3").arg(color.red(), 2, 16, QChar('0')).arg(
		color.green(), 2, 16, QChar('0')).arg(color.blue(), 2, 16, QChar('0')).toLower();
}
