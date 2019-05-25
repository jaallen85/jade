/* VsdxWriter.cpp
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
 
#include "VsdxWriter.h"
#include <QtPrintSupport>
#include <quazip.h>
#include <quazipfile.h>

VsdxWriter::VsdxWriter()
{
	mDiagram = nullptr;
	mPrinter = nullptr;
	mDiagramScale = 1.0;
	mVsdxWidth = 0.0;
	mVsdxHeight = 0.0;
	mVsdxMargin = 0.0;
}

VsdxWriter::~VsdxWriter() { }

//==================================================================================================

bool VsdxWriter::write(DiagramWidget* diagram, QPrinter* printer, const QString& filePath)
{
	mFilePath = filePath;
	mPrinter = printer;
	mDiagram = diagram;

	mErrorMessage.clear();

	if (mPrinter->pageLayout().pageSize().definitionUnits() == QPageSize::Millimeter ||
		mPrinter->pageLayout().pageSize().definitionUnits() == QPageSize::Cicero)
	{
		mDiagramUnits = "mm";
		mDiagramScale = 0.025;
	}
	else
	{
		mDiagramUnits = "in";
		mDiagramScale = 0.001;
	}

	mDiagramTranslate = mDiagram->scene()->sceneRect().topLeft();
	mVsdxWidth = mDiagram->scene()->sceneRect().width() * mDiagramScale;
	mVsdxHeight = mDiagram->scene()->sceneRect().height() * mDiagramScale;
	mVsdxMargin = (mDiagramUnits == "mm") ? 5 : 0.25;

	writeVsdx();

	return mErrorMessage.isEmpty();
}

QString VsdxWriter::errorMessage() const
{
	return mErrorMessage;
}

//==================================================================================================

void VsdxWriter::writeVsdx()
{
	QuaZip vsdxFile(mFilePath);

	if (vsdxFile.open(QuaZip::mdCreate))
	{
		createFileInZip(&vsdxFile, "[Content_Types].xml", writeContentTypes());

		createFileInZip(&vsdxFile, "_rels/.rels", writeRels());

		createFileInZip(&vsdxFile, "docProps/app.xml", writeApp());
		createFileInZip(&vsdxFile, "docProps/core.xml", writeCore());
		createFileInZip(&vsdxFile, "docProps/custom.xml", writeCustom());

		createFileInZip(&vsdxFile, "visio/pages/_rels/pages.xml.rels", writePagesRels());
		createFileInZip(&vsdxFile, "visio/pages/pages.xml", writePages());
		createFileInZip(&vsdxFile, "visio/pages/page1.xml", writePage1());

		createFileInZip(&vsdxFile, "visio/_rels/document.xml.rels", writeDocumentRels());
		createFileInZip(&vsdxFile, "visio/document.xml", writeDocument());
		createFileInZip(&vsdxFile, "visio/windows.xml", writeWindows());

		vsdxFile.close();
	}
	else mErrorMessage = "Error creating file: " + mFilePath;
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
	//core += "  <cp:lastPrinted>2019-05-23T14:39:36Z</cp:lastPrinted>\n";
	//core += "  <dcterms:created xsi:type=\"dcterms:W3CDTF\">2019-05-23T14:39:36Z</dcterms:created>\n";
	//core += "  <dcterms:modified xsi:type=\"dcterms:W3CDTF\">2019-05-23T14:39:47Z</dcterms:modified>\n";
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
	custom += QString("    <vt:bool>") + ((mDiagramUnits == "mm") ? "true" : "false") + "</vt:bool>\n";
	custom += "  </property>\n";
	//custom += "  <property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"6\" name=\"TimeEdited\">\n";
	//custom += "    <vt:filetime>2019-05-23T14:39:36Z</vt:filetime>\n";
	//custom += "  </property>\n";
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

	QString marginStr = QString::number(mVsdxMargin);
	QString pageWidthStr = QString::number(mVsdxWidth + 2 * mVsdxMargin);
	QString pageHeightStr = QString::number(mVsdxHeight + 2 * mVsdxMargin);
	QString pageOrientationStr = (mVsdxHeight > mVsdxWidth) ? "1" : "2";

	pages += "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	pages += "<Pages xmlns=\"http://schemas.microsoft.com/office/visio/2012/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xml:space=\"preserve\">\n";
	pages += " <Page ID=\"0\" NameU=\"Page-1\" Name=\"Page-1\" ViewScale=\"-1\" ViewCenterX=\"4.2551018751844\" ViewCenterY=\"5.5102038671453\">\n";
	pages += "    <PageSheet LineStyle=\"0\" FillStyle=\"0\" TextStyle=\"0\">\n";
	pages += "      <Cell N=\"PageWidth\" V=\"" + pageWidthStr + "\"/>\n";
	pages += "      <Cell N=\"PageHeight\" V=\"" + pageHeightStr + "\"/>\n";
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
	pages += "      <Cell N=\"PrintPageOrientation\" V=\"" + pageOrientationStr + "\"/>\n";
	pages += "      <Cell N=\"PaperKind\" V=\"256\"/>\n";
	pages += "      <Cell N=\"XRulerOrigin\" V=\"" + marginStr + "\" U=\"" + mDiagramUnits + "\"/>\n";
	pages += "      <Cell N=\"YRulerOrigin\" V=\"" + marginStr + "\" U=\"" + mDiagramUnits + "\"/>\n";
	pages += "      <Cell N=\"XGridSpacing\" V=\"0\" U=\"" + mDiagramUnits + "\"/>\n";
	pages += "      <Cell N=\"YGridSpacing\" V=\"0\" U=\"" + mDiagramUnits + "\"/>\n";
	pages += "      <Cell N=\"XGridOrigin\" V=\"" + marginStr + "\" U=\"" + mDiagramUnits + "\"/>\n";
	pages += "      <Cell N=\"YGridOrigin\" V=\"" + marginStr + "\" U=\"" + mDiagramUnits + "\"/>\n";
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

	page += writeItems(mDiagram->scene()->items());

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

//	windows += "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
//	windows += "<Windows ClientWidth=\"2560\" ClientHeight=\"1221\" xmlns=\"http://schemas.microsoft.com/office/visio/2012/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xml:space=\"preserve\">\n";
//	windows += "  <Window ID=\"0\" WindowType=\"Drawing\" WindowState=\"1073741824\" WindowLeft=\"-8\" WindowTop=\"-31\" WindowWidth=\"2576\" WindowHeight=\"1260\" ContainerType=\"Page\" Page=\"0\" ViewScale=\"-1\" ViewCenterX=\"4.2551018751844\" ViewCenterY=\"5.5102038671453\">\n";
//	windows += "    <ShowRulers>1</ShowRulers>\n";
//	windows += "    <ShowGrid>1</ShowGrid>\n";
//	windows += "    <ShowPageBreaks>1</ShowPageBreaks>\n";
//	windows += "    <ShowGuides>1</ShowGuides>\n";
//	windows += "    <ShowConnectionPoints>0</ShowConnectionPoints>\n";
//	windows += "    <GlueSettings>9</GlueSettings>\n";
//	windows += "    <SnapSettings>65847</SnapSettings>\n";
//	windows += "    <SnapExtensions>34</SnapExtensions>\n";
//	windows += "    <SnapAngles/>\n";
//	windows += "    <DynamicGridEnabled>0</DynamicGridEnabled>\n";
//	windows += "    <TabSplitterPos>0.5</TabSplitterPos>\n";
//	windows += "  </Window>\n";
//	windows += "</Windows>\n";

	windows += "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	windows += "<Windows xmlns=\"http://schemas.microsoft.com/office/visio/2012/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xml:space=\"preserve\">\n";
	windows += "</Windows>\n";

	return windows;
}

void VsdxWriter::createFileInZip(QuaZip* zip, const QString& path, const QString& content)
{
	QuaZipFile outputFile(zip);

	if (outputFile.open(QIODevice::WriteOnly, QuaZipNewInfo(path)))
	{
		QTextStream outputStream(&outputFile);
		outputStream << content;
		outputStream.flush();
		outputFile.close();
	}
	else mErrorMessage = "Error creating " + path + " in file: " + zip->getZipName();
}

//==================================================================================================

QString VsdxWriter::writeItems(const QList<DrawingItem*>& items)
{
	QString itemStr;
	int index = 1;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		DrawingLineItem* lineItem = dynamic_cast<DrawingLineItem*>(*itemIter);
		DrawingArcItem* arcItem = dynamic_cast<DrawingArcItem*>(*itemIter);
		DrawingPolylineItem* polylineItem = dynamic_cast<DrawingPolylineItem*>(*itemIter);
		DrawingCurveItem* curveItem = dynamic_cast<DrawingCurveItem*>(*itemIter);
		DrawingRectItem* rectItem = dynamic_cast<DrawingRectItem*>(*itemIter);
		DrawingEllipseItem* ellipseItem = dynamic_cast<DrawingEllipseItem*>(*itemIter);
		DrawingPolygonItem* polygonItem = dynamic_cast<DrawingPolygonItem*>(*itemIter);
		DrawingTextItem* textItem = dynamic_cast<DrawingTextItem*>(*itemIter);
		DrawingTextRectItem* textRectItem = dynamic_cast<DrawingTextRectItem*>(*itemIter);
		DrawingTextEllipseItem* textEllipseItem = dynamic_cast<DrawingTextEllipseItem*>(*itemIter);
		DrawingTextPolygonItem* textPolygonItem = dynamic_cast<DrawingTextPolygonItem*>(*itemIter);
		DrawingPathItem* pathItem = dynamic_cast<DrawingPathItem*>(*itemIter);
		DrawingItemGroup* groupItem = dynamic_cast<DrawingItemGroup*>(*itemIter);

		if (lineItem) itemStr += writeLineItem(lineItem, index);
		else if (arcItem) itemStr += writeArcItem(arcItem, index);
		else if (polylineItem) itemStr += writePolylineItem(polylineItem, index);
		else if (curveItem) itemStr += writeCurveItem(curveItem, index);
		else if (rectItem) itemStr += writeRectItem(rectItem, index);
		else if (ellipseItem) itemStr += writeEllipseItem(ellipseItem, index);
		else if (polygonItem) itemStr += writePolygonItem(polygonItem, index);
		else if (textItem) itemStr += writeTextItem(textItem, index);
		else if (textRectItem) itemStr += writeTextRectItem(textRectItem, index);
		else if (textEllipseItem) itemStr += writeTextEllipseItem(textEllipseItem, index);
		else if (textPolygonItem) itemStr += writeTextPolygonItem(textPolygonItem, index);
		else if (pathItem) itemStr += writePathItem(pathItem, index);
		else if (groupItem) itemStr += writeItemGroup(groupItem, index);
	}

	return itemStr;
}

QString VsdxWriter::writeLineItem(DrawingLineItem* item, int& index)
{
	QString itemStr;

	QPointF startPoint = mapFromScene(item->mapToScene(item->line().p1()));
	QPointF endPoint = mapFromScene(item->mapToScene(item->line().p2()));
	QPointF centerPoint = (startPoint + endPoint) / 2;
	qreal width = qAbs(endPoint.x() - startPoint.x());
	qreal height = qAbs(endPoint.y() - startPoint.y());
	qreal length = qSqrt(width * width + height * height);
	qreal angle = qAtan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	itemStr += "    <Shape ID=\"" + QString::number(index) + "\" Type=\"Shape\" LineStyle=\"3\" FillStyle=\"3\" TextStyle=\"3\">\n";
	itemStr += "      <Cell N=\"PinX\" V=\"" + QString::number(centerPoint.x()) + "\"/>\n";
	itemStr += "      <Cell N=\"PinY\" V=\"" + QString::number(centerPoint.y()) + "\"/>\n";
	itemStr += "      <Cell N=\"Width\" V=\"" + QString::number(length) + "\"/>\n";
	itemStr += "      <Cell N=\"Height\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"LocPinX\" V=\"" + QString::number(length * 0.5) + "\"/>\n";
	itemStr += "      <Cell N=\"LocPinY\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"Angle\" V=\"" + QString::number(angle) + "\"/>\n";
	itemStr += "      <Cell N=\"FlipX\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"FlipY\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"ResizeMode\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"BeginX\" V=\"" + QString::number(startPoint.x()) + "\"/>\n";
	itemStr += "      <Cell N=\"BeginY\" V=\"" + QString::number(startPoint.y()) + "\"/>\n";
	itemStr += "      <Cell N=\"EndX\" V=\"" + QString::number(endPoint.x()) + "\"/>\n";
	itemStr += "      <Cell N=\"EndY\" V=\"" + QString::number(endPoint.y()) + "\"/>\n";
	itemStr += "	  <Cell N=\"FillPattern\" V=\"0\"/>\n";
	itemStr += writeItemStyle(item->style());
	itemStr += "      <Section N=\"Geometry\" IX=\"0\">\n";
	itemStr += "        <Cell N=\"NoFill\" V=\"1\"/>\n";
	itemStr += "		<Cell N=\"NoLine\" V=\"0\"/>\n";
	itemStr += "       <Row T=\"MoveTo\" IX=\"1\">\n";
	itemStr += "         <Cell N=\"X\" V=\"0\"/>\n";
	itemStr += "         <Cell N=\"Y\" V=\"0\"/>\n";
	itemStr += "       </Row>\n";
	itemStr += "       <Row T=\"LineTo\" IX=\"2\">\n";
	itemStr += "         <Cell N=\"X\" V=\"" + QString::number(length) + "\"/>\n";
	itemStr += "         <Cell N=\"Y\" V=\"0\"/>\n";
	itemStr += "       </Row>\n";
	itemStr += "      </Section>\n";
	itemStr += "	</Shape>\n";

	index++;

	return itemStr;
}

QString VsdxWriter::writeArcItem(DrawingArcItem* item, int& index)
{
	QPointF curveStartPoint = mapFromScene(item->mapToScene(item->arc().p1()));
	QPointF curveEndPoint = mapFromScene(item->mapToScene(item->arc().p2()));

	qreal lengthX = (curveEndPoint.x() - curveStartPoint.x()) * 0.45;
	qreal lengthY = (curveEndPoint.y() - curveStartPoint.y()) * 0.45;

	QPointF curveStartControlPoint1 = QPointF(curveEndPoint.x() - lengthX, curveStartPoint.y());
	QPointF curveEndControlPoint1 = QPointF(curveEndPoint.x(), curveStartPoint.y() + lengthY);
	QPointF curveStartControlPoint2 = QPointF(curveStartPoint.x(), curveEndPoint.y() - lengthY);
	QPointF curveEndControlPoint2 = QPointF(curveStartPoint.x() + lengthX, curveEndPoint.y());

	//qreal rotationAngle = (item->transform().m11() == 0) ?
	//	qAsin(item->transform().m21()) * 180 / 3.141593654 :
	//	qAcos(item->transform().m11()) * 180 / 3.141593654;
	bool flipped = (item->transform().m11() == 0) ?
		(item->transform().m21() == item->transform().m12()) :
		(item->transform().m11() != item->transform().m22());

	if (flipped)
	{
		if ((curveStartPoint.x() < curveEndPoint.x() && curveStartPoint.y() < curveEndPoint.y()) ||
			(curveStartPoint.x() > curveEndPoint.x() && curveStartPoint.y() > curveEndPoint.y()))
		{
			return writeCurveItem(item, curveStartPoint, curveStartControlPoint2,
				curveEndControlPoint2, curveEndPoint, index);
		}
		else
		{
			return writeCurveItem(item, curveStartPoint, curveStartControlPoint1,
				curveEndControlPoint1, curveEndPoint, index);
		}
	}
	else
	{
		if ((curveStartPoint.x() < curveEndPoint.x() && curveStartPoint.y() < curveEndPoint.y()) ||
			(curveStartPoint.x() > curveEndPoint.x() && curveStartPoint.y() > curveEndPoint.y()))
		{
			return writeCurveItem(item, curveStartPoint, curveStartControlPoint1,
				curveEndControlPoint1, curveEndPoint, index);
		}
		else
		{
			return writeCurveItem(item, curveStartPoint, curveStartControlPoint2,
				curveEndControlPoint2, curveEndPoint, index);
		}
	}
}

QString VsdxWriter::writePolylineItem(DrawingPolylineItem* item, int& index)
{
	QString itemStr;

	QPolygonF polyline = mapFromScene(item->mapToScene(item->polyline()));
	QPointF topLeft = polyline.boundingRect().topLeft();
	QPointF bottomRight = polyline.boundingRect().bottomRight();
	qreal width = qAbs(bottomRight.x() - topLeft.x());
	qreal height = qAbs(bottomRight.y() - topLeft.y());
	int pointIndex = 1;

	itemStr += "    <Shape ID=\"" + QString::number(index) + "\" Type=\"Shape\" LineStyle=\"3\" FillStyle=\"3\" TextStyle=\"3\">\n";
	itemStr += "	  <Cell N=\"PinX\" V=\"" + QString::number(topLeft.x()) + "\"/>\n";
	itemStr += "	  <Cell N=\"PinY\" V=\"" + QString::number(topLeft.y()) + "\"/>\n";
	itemStr += "	  <Cell N=\"Width\" V=\"" + QString::number(width) + "\"/>\n";
	itemStr += "	  <Cell N=\"Height\" V=\"" + QString::number(height) + "\"/>\n";
	itemStr += "	  <Cell N=\"LocPinX\" V=\"0\" F=\"Width*0\"/>\n";
	itemStr += "	  <Cell N=\"LocPinY\" V=\"0\" F=\"Height*0\"/>\n";
	itemStr += "      <Cell N=\"Angle\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"FlipX\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"FlipY\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"ResizeMode\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"FillPattern\" V=\"0\"/>\n";
	itemStr += writeItemStyle(item->style());
	itemStr += "      <Section N=\"Geometry\" IX=\"0\">\n";
	itemStr += "        <Cell N=\"NoFill\" V=\"1\"/>\n";
	itemStr += "		<Cell N=\"NoLine\" V=\"0\"/>\n";

	for(auto polyIter = polyline.begin(), polyEnd = polyline.end(); polyIter != polyEnd; polyIter++)
	{
		if (pointIndex == 1)
			itemStr += "        <Row T=\"MoveTo\" IX=\"" + QString::number(pointIndex) + "\">\n";
		else
			itemStr += "        <Row T=\"LineTo\" IX=\"" + QString::number(pointIndex) + "\">\n";
		itemStr += "          <Cell N=\"X\" V=\"" + QString::number(polyIter->x() - topLeft.x()) + "\"/>\n";
		itemStr += "          <Cell N=\"Y\" V=\"" + QString::number(height - bottomRight.y() + polyIter->y()) + "\"/>\n";
		itemStr += "        </Row>\n";

		pointIndex++;
	}

	itemStr += "      </Section>\n";
	itemStr += "    </Shape>\n";

	index++;

	return itemStr;
}

QString VsdxWriter::writeCurveItem(DrawingCurveItem* item, int& index)
{
	QPointF curveStartPoint = mapFromScene(item->mapToScene(item->curveStartPos()));
	QPointF curveEndPoint = mapFromScene(item->mapToScene(item->curveEndPos()));
	QPointF curveStartControlPoint = mapFromScene(item->mapToScene(item->curveStartControlPos()));
	QPointF curveEndControlPoint = mapFromScene(item->mapToScene(item->curveEndControlPos()));

	return writeCurveItem(item, curveStartPoint, curveStartControlPoint,
		curveEndControlPoint, curveEndPoint, index);
}

QString VsdxWriter::writeCurveItem(DrawingItem* item, const QPointF& curveStartPoint, const QPointF& curveStartControlPoint,
	const QPointF& curveEndControlPoint, const QPointF& curveEndPoint, int& index)
{
	QString itemStr;

	QPointF centerPoint = (curveStartPoint + curveEndPoint) / 2;
	qreal width = qAbs(curveEndPoint.x() - curveStartPoint.x());
	qreal height = qAbs(curveEndPoint.y() - curveStartPoint.y());
	qreal length = qSqrt(width * width + height * height);
	qreal angle = qAtan2(curveEndPoint.y() - curveStartPoint.y(), curveEndPoint.x() - curveStartPoint.x());

	QPointF curvePos;
	qreal dist, minDist, maxDist;
	for(qreal ratio = 0.0; ratio <= 1.0; ratio += 0.001)
	{
		curvePos.setX((1 - ratio)*(1 - ratio)*(1 - ratio) * curveStartPoint.x() +
				3*ratio*(1 - ratio)*(1 - ratio) * curveStartControlPoint.x() +
				3*ratio*ratio*(1 - ratio) * curveEndControlPoint.x() +
				ratio*ratio*ratio * curveEndPoint.x());
		curvePos.setY((1 - ratio)*(1 - ratio)*(1 - ratio) * curveStartPoint.y() +
			3*ratio*(1 - ratio)*(1 - ratio) * curveStartControlPoint.y() +
			3*ratio*ratio*(1 - ratio) * curveEndControlPoint.y() +
			ratio*ratio*ratio * curveEndPoint.y());

		// From https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
		dist = ((curveEndPoint.y() - curveStartPoint.y()) * curvePos.x() - (curveEndPoint.x() - curveStartPoint.x()) * curvePos.y() +
			curveEndPoint.x() * curveStartPoint.y() - curveStartPoint.x() * curveEndPoint.y()) / length;
		if (ratio == 0.0)
		{
			minDist = dist;
			maxDist = dist;
		}
		else
		{
			minDist = qMin(minDist, dist);
			maxDist = qMax(maxDist, dist);
		}
	}

	qreal curveHeight = maxDist - minDist;

	QTransform transform;
	transform.rotateRadians(-angle);
	transform.translate(-curveStartPoint.x(), -curveStartPoint.y());
	QPointF curveStartControlRel = transform.map(curveStartControlPoint);
	QPointF curveEndControlRel = transform.map(curveEndControlPoint);
	qreal a = curveStartControlRel.x() / length;
	qreal b = curveStartControlRel.y() / curveHeight + 0.5;
	qreal c = curveEndControlRel.x() / length;
	qreal d = curveEndControlRel.y() / curveHeight + 0.5;

	itemStr += "    <Shape ID=\"" + QString::number(index) + "\" Type=\"Shape\" LineStyle=\"3\" FillStyle=\"3\" TextStyle=\"3\">\n";
	itemStr += "      <Cell N=\"PinX\" V=\"" + QString::number(centerPoint.x()) + "\"/>\n";
	itemStr += "      <Cell N=\"PinY\" V=\"" + QString::number(centerPoint.y()) + "\"/>\n";
	itemStr += "      <Cell N=\"Width\" V=\"" + QString::number(length) + "\"/>\n";
	itemStr += "      <Cell N=\"Height\" V=\"" + QString::number(curveHeight) + "\"/>\n";
	itemStr += "      <Cell N=\"LocPinX\" V=\"" + QString::number(length * 0.5) + "\"/>\n";
	itemStr += "      <Cell N=\"LocPinY\" V=\"" + QString::number(curveHeight * 0.5) + "\"/>\n";
	itemStr += "      <Cell N=\"Angle\" V=\"" + QString::number(angle) + "\"/>\n";
	itemStr += "      <Cell N=\"FlipX\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"FlipY\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"ResizeMode\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"BeginX\" V=\"" + QString::number(curveStartPoint.x()) + "\"/>\n";
	itemStr += "      <Cell N=\"BeginY\" V=\"" + QString::number(curveStartPoint.y()) + "\"/>\n";
	itemStr += "      <Cell N=\"EndX\" V=\"" + QString::number(curveEndPoint.x()) + "\"/>\n";
	itemStr += "      <Cell N=\"EndY\" V=\"" + QString::number(curveEndPoint.y()) + "\"/>\n";
	itemStr += "	  <Cell N=\"FillPattern\" V=\"0\"/>\n";
	itemStr += writeItemStyle(item->style());
	itemStr += "      <Section N=\"Geometry\" IX=\"0\">\n";
	itemStr += "        <Cell N=\"NoFill\" V=\"1\"/>\n";
	itemStr += "		<Cell N=\"NoLine\" V=\"0\"/>\n";
	itemStr += "       <Row T=\"MoveTo\" IX=\"1\">\n";
	itemStr += "         <Cell N=\"X\" V=\"0\"/>\n";
	itemStr += "         <Cell N=\"Y\" V=\"" + QString::number(curveHeight * 0.5) + "\"/>\n";
	itemStr += "       </Row>\n";
	itemStr += "       <Row T=\"RelCubBezTo\" IX=\"2\">\n";
	itemStr += "         <Cell N=\"X\" V=\"1\"/>\n";
	itemStr += "         <Cell N=\"Y\" V=\"0.5\"/>\n";
	itemStr += "         <Cell N=\"A\" V=\"" + QString::number(a) + "\"/>\n";
	itemStr += "         <Cell N=\"B\" V=\"" + QString::number(b) + "\"/>\n";
	itemStr += "         <Cell N=\"C\" V=\"" + QString::number(c) + "\"/>\n";
	itemStr += "         <Cell N=\"D\" V=\"" + QString::number(d) + "\"/>\n";
	itemStr += "       </Row>\n";
	itemStr += "      </Section>\n";
	itemStr += "	</Shape>\n";

	index++;

	return itemStr;
}

QString VsdxWriter::writeRectItem(DrawingRectItem* item, int& index)
{
	QString itemStr;

	QRectF rect = mapFromScene(item->mapToScene(item->rect()).boundingRect());
	QPointF topLeft = rect.normalized().topLeft();
	QPointF bottomRight = rect.normalized().bottomRight();
	qreal width = qAbs(bottomRight.x() - topLeft.x());
	qreal height = qAbs(bottomRight.y() - topLeft.y());
	qreal cornerRadius = qMin(item->cornerRadiusX(), item->cornerRadiusY()) * mDiagramScale;

	itemStr += "    <Shape ID=\"" + QString::number(index) + "\" Type=\"Shape\" LineStyle=\"3\" FillStyle=\"3\" TextStyle=\"3\">\n";
	itemStr += "	  <Cell N=\"PinX\" V=\"" + QString::number(topLeft.x()) + "\"/>\n";
	itemStr += "	  <Cell N=\"PinY\" V=\"" + QString::number(topLeft.y()) + "\"/>\n";
	itemStr += "	  <Cell N=\"Width\" V=\"" + QString::number(width) + "\"/>\n";
	itemStr += "	  <Cell N=\"Height\" V=\"" + QString::number(height) + "\"/>\n";
	itemStr += "	  <Cell N=\"LocPinX\" V=\"0\" F=\"Width*0\"/>\n";
	itemStr += "	  <Cell N=\"LocPinY\" V=\"0\" F=\"Height*0\"/>\n";
	itemStr += "	  <Cell N=\"Angle\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"FlipX\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"FlipY\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"ResizeMode\" V=\"0\"/>\n";
	if (cornerRadius != 0)
		itemStr += "	  <Cell N=\"Rounding\" V=\"" + QString::number(cornerRadius) + "\"/>\n";
	itemStr += writeItemStyle(item->style());
	itemStr += "	  <Section N=\"Geometry\" IX=\"0\">\n";
	itemStr += "		<Row T=\"RelMoveTo\" IX=\"1\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"0\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"0\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "		<Row T=\"RelLineTo\" IX=\"2\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"1\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"0\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "		<Row T=\"RelLineTo\" IX=\"3\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"1\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"1\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "		<Row T=\"RelLineTo\" IX=\"4\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"0\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"1\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "		<Row T=\"RelLineTo\" IX=\"5\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"0\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"0\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "	  </Section>\n";
	itemStr += "	</Shape>\n";

	index++;

	return itemStr;
}

QString VsdxWriter::writeEllipseItem(DrawingEllipseItem* item, int& index)
{
	QString itemStr;

	QRectF ellipse = mapFromScene(item->mapToScene(item->ellipse()).boundingRect());
	QPointF topLeft = ellipse.normalized().topLeft();
	QPointF bottomRight = ellipse.normalized().bottomRight();
	qreal width = qAbs(bottomRight.x() - topLeft.x());
	qreal height = qAbs(bottomRight.y() - topLeft.y());

	itemStr += "    <Shape ID=\"" + QString::number(index) + "\" Type=\"Shape\" LineStyle=\"3\" FillStyle=\"3\" TextStyle=\"3\">\n";
	itemStr += "	  <Cell N=\"PinX\" V=\"" + QString::number(topLeft.x()) + "\"/>\n";
	itemStr += "	  <Cell N=\"PinY\" V=\"" + QString::number(topLeft.y()) + "\"/>\n";
	itemStr += "	  <Cell N=\"Width\" V=\"" + QString::number(width) + "\"/>\n";
	itemStr += "	  <Cell N=\"Height\" V=\"" + QString::number(height) + "\"/>\n";
	itemStr += "	  <Cell N=\"LocPinX\" V=\"0\" F=\"Width*0\"/>\n";
	itemStr += "	  <Cell N=\"LocPinY\" V=\"0\" F=\"Height*0\"/>\n";
	itemStr += "      <Cell N=\"Angle\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"FlipX\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"FlipY\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"ResizeMode\" V=\"0\"/>\n";
	itemStr += writeItemStyle(item->style());
	itemStr += "      <Section N=\"Geometry\" IX=\"0\">\n";
	itemStr += "        <Row T=\"Ellipse\" IX=\"1\">\n";
	itemStr += "          <Cell N=\"X\" V=\"" + QString::number(width * 0.5) + "\" F=\"Width*0.5\"/>\n";
	itemStr += "          <Cell N=\"Y\" V=\"" + QString::number(height * 0.5) + "\" F=\"Height*0.5\"/>\n";
	itemStr += "          <Cell N=\"A\" V=\"" + QString::number(width) + "\" U=\"DL\" F=\"Width*1\"/>\n";
	itemStr += "          <Cell N=\"B\" V=\"" + QString::number(height * 0.5) + "\" U=\"DL\" F=\"Height*0.5\"/>\n";
	itemStr += "          <Cell N=\"C\" V=\"" + QString::number(width * 0.5) + "\" U=\"DL\" F=\"Width*0.5\"/>\n";
	itemStr += "          <Cell N=\"D\" V=\"" + QString::number(height) + "\" U=\"DL\" F=\"Height*1\"/>\n";
	itemStr += "        </Row>\n";
	itemStr += "      </Section>\n";
	itemStr += "    </Shape>\n";

	index++;

	return itemStr;
}

QString VsdxWriter::writePolygonItem(DrawingPolygonItem* item, int& index)
{
	QString itemStr;

	QPolygonF polygon = mapFromScene(item->mapToScene(item->polygon()));
	QPointF topLeft = polygon.boundingRect().topLeft();
	QPointF bottomRight = polygon.boundingRect().bottomRight();
	qreal width = qAbs(bottomRight.x() - topLeft.x());
	qreal height = qAbs(bottomRight.y() - topLeft.y());
	int pointIndex = 1;

	itemStr += "    <Shape ID=\"" + QString::number(index) + "\" Type=\"Shape\" LineStyle=\"3\" FillStyle=\"3\" TextStyle=\"3\">\n";
	itemStr += "	  <Cell N=\"PinX\" V=\"" + QString::number(topLeft.x()) + "\"/>\n";
	itemStr += "	  <Cell N=\"PinY\" V=\"" + QString::number(topLeft.y()) + "\"/>\n";
	itemStr += "	  <Cell N=\"Width\" V=\"" + QString::number(width) + "\"/>\n";
	itemStr += "	  <Cell N=\"Height\" V=\"" + QString::number(height) + "\"/>\n";
	itemStr += "	  <Cell N=\"LocPinX\" V=\"0\" F=\"Width*0\"/>\n";
	itemStr += "	  <Cell N=\"LocPinY\" V=\"0\" F=\"Height*0\"/>\n";
	itemStr += "      <Cell N=\"Angle\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"FlipX\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"FlipY\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"ResizeMode\" V=\"0\"/>\n";
	itemStr += writeItemStyle(item->style());
	itemStr += "      <Section N=\"Geometry\" IX=\"0\">\n";

	for(auto polyIter = polygon.begin(), polyEnd = polygon.end(); polyIter != polyEnd; polyIter++)
	{
		if (pointIndex == 1)
			itemStr += "        <Row T=\"MoveTo\" IX=\"" + QString::number(pointIndex) + "\">\n";
		else
			itemStr += "        <Row T=\"LineTo\" IX=\"" + QString::number(pointIndex) + "\">\n";
		itemStr += "          <Cell N=\"X\" V=\"" + QString::number(polyIter->x() - topLeft.x()) + "\"/>\n";
		itemStr += "          <Cell N=\"Y\" V=\"" + QString::number(height - bottomRight.y() + polyIter->y()) + "\"/>\n";
		itemStr += "        </Row>\n";

		pointIndex++;
	}

	itemStr += "        <Row T=\"LineTo\" IX=\"" + QString::number(pointIndex) + "\">\n";
	itemStr += "          <Cell N=\"X\" V=\"" + QString::number(polygon.first().x() - topLeft.x()) + "\"/>\n";
	itemStr += "          <Cell N=\"Y\" V=\"" + QString::number(height - bottomRight.y() + polygon.first().y()) + "\"/>\n";
	itemStr += "        </Row>\n";

	itemStr += "      </Section>\n";
	itemStr += "    </Shape>\n";

	index++;

	return itemStr;
}

QString VsdxWriter::writeTextItem(DrawingTextItem* item, int& index)
{
	QString itemStr;

	Qt::Alignment horizontalAlign = item->style()->hasValue(DrawingItemStyle::TextHorizontalAlignment) ?
		(Qt::Alignment)item->style()->value(DrawingItemStyle::TextHorizontalAlignment).toUInt() : Qt::AlignHCenter;
	Qt::Alignment verticalAlign = item->style()->hasValue(DrawingItemStyle::TextVerticalAlignment) ?
		(Qt::Alignment)item->style()->value(DrawingItemStyle::TextVerticalAlignment).toUInt() : Qt::AlignVCenter;

	QRectF boundingRect = item->boundingRect();

	if (horizontalAlign & Qt::AlignLeft) boundingRect.adjust(0, 0, boundingRect.width() * 0.5, 0);
	else if (horizontalAlign & Qt::AlignRight) boundingRect.adjust(-boundingRect.width() * 0.5, 0, 0, 0);
	else boundingRect.adjust(-boundingRect.width() * 0.25, 0, boundingRect.width() * 0.25, 0);

	if (verticalAlign & Qt::AlignTop) boundingRect.adjust(0, 0, 0, boundingRect.height() * 0.5);
	else if (verticalAlign & Qt::AlignBottom) boundingRect.adjust(0, -boundingRect.height() * 0.5, 0, 0);
	else boundingRect.adjust(0, -boundingRect.height() * 0.25, 0, boundingRect.height() * 0.25);

	QPointF topLeft = mapFromScene(item->mapToScene(boundingRect.topLeft()));
	QPointF bottomRight = mapFromScene(item->mapToScene(boundingRect.bottomRight()));
	qreal width = qAbs(bottomRight.x() - topLeft.x());
	qreal height = qAbs(bottomRight.y() - topLeft.y());

	itemStr += "    <Shape ID=\"" + QString::number(index) + "\" Type=\"Shape\" LineStyle=\"3\" FillStyle=\"3\" TextStyle=\"3\">\n";
	itemStr += "	  <Cell N=\"PinX\" V=\"" + QString::number(topLeft.x()) + "\"/>\n";
	itemStr += "	  <Cell N=\"PinY\" V=\"" + QString::number(bottomRight.y()) + "\"/>\n";
	itemStr += "	  <Cell N=\"Width\" V=\"" + QString::number(width) + "\"/>\n";
	itemStr += "	  <Cell N=\"Height\" V=\"" + QString::number(height) + "\"/>\n";
	itemStr += "	  <Cell N=\"LocPinX\" V=\"0\" F=\"Width*0\"/>\n";
	itemStr += "	  <Cell N=\"LocPinY\" V=\"0\" F=\"Height*0\"/>\n";
	itemStr += "	  <Cell N=\"Angle\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"FlipX\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"FlipY\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"ResizeMode\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"LinePattern\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"FillPattern\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"LeftMargin\" V=\"0.027777778\" U=\"PT\"/>\n";
	itemStr += "	  <Cell N=\"RightMargin\" V=\"0.027777778\" U=\"PT\"/>\n";
	itemStr += "	  <Cell N=\"TopMargin\" V=\"0.0138888889\" U=\"PT\"/>\n";
	itemStr += "	  <Cell N=\"BottomMargin\" V=\"0.0138888889\" U=\"PT\"/>\n";
	itemStr += writeItemStyle(item->style());
	itemStr += "	  <Section N=\"Geometry\" IX=\"0\">\n";
	itemStr += "		<Row T=\"RelMoveTo\" IX=\"1\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"0\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"0\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "		<Row T=\"RelLineTo\" IX=\"2\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"1\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"0\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "		<Row T=\"RelLineTo\" IX=\"3\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"1\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"1\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "		<Row T=\"RelLineTo\" IX=\"4\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"0\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"1\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "		<Row T=\"RelLineTo\" IX=\"5\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"0\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"0\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "	  </Section>\n";
	itemStr += "	  <Text>" + item->caption() + "</Text>\n";
	itemStr += "	</Shape>\n";

	index++;

	return itemStr;
}

QString VsdxWriter::writeTextRectItem(DrawingTextRectItem* item, int& index)
{
	QString itemStr;

	QRectF rect = mapFromScene(item->mapToScene(item->rect()).boundingRect());
	QPointF topLeft = rect.normalized().topLeft();
	QPointF bottomRight = rect.normalized().bottomRight();
	qreal width = qAbs(bottomRight.x() - topLeft.x());
	qreal height = qAbs(bottomRight.y() - topLeft.y());
	qreal cornerRadius = qMin(item->cornerRadiusX(), item->cornerRadiusY()) * mDiagramScale;

	itemStr += "    <Shape ID=\"" + QString::number(index) + "\" Type=\"Shape\" LineStyle=\"3\" FillStyle=\"3\" TextStyle=\"3\">\n";
	itemStr += "	  <Cell N=\"PinX\" V=\"" + QString::number(topLeft.x()) + "\"/>\n";
	itemStr += "	  <Cell N=\"PinY\" V=\"" + QString::number(topLeft.y()) + "\"/>\n";
	itemStr += "	  <Cell N=\"Width\" V=\"" + QString::number(width) + "\"/>\n";
	itemStr += "	  <Cell N=\"Height\" V=\"" + QString::number(height) + "\"/>\n";
	itemStr += "	  <Cell N=\"LocPinX\" V=\"0\" F=\"Width*0\"/>\n";
	itemStr += "	  <Cell N=\"LocPinY\" V=\"0\" F=\"Height*0\"/>\n";
	itemStr += "	  <Cell N=\"Angle\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"FlipX\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"FlipY\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"ResizeMode\" V=\"0\"/>\n";

	if (cornerRadius != 0)
		itemStr += "	  <Cell N=\"Rounding\" V=\"" + QString::number(cornerRadius) + "\"/>\n";

	itemStr += "	  <Cell N=\"LeftMargin\" V=\"0\" U=\"PT\"/>\n";
	itemStr += "	  <Cell N=\"RightMargin\" V=\"0\" U=\"PT\"/>\n";
	itemStr += "	  <Cell N=\"TopMargin\" V=\"0\" U=\"PT\"/>\n";
	itemStr += "	  <Cell N=\"BottomMargin\" V=\"0\" U=\"PT\"/>\n";
	itemStr += writeItemStyle(item->style());
	itemStr += "	  <Section N=\"Geometry\" IX=\"0\">\n";
	itemStr += "		<Row T=\"RelMoveTo\" IX=\"1\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"0\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"0\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "		<Row T=\"RelLineTo\" IX=\"2\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"1\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"0\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "		<Row T=\"RelLineTo\" IX=\"3\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"1\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"1\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "		<Row T=\"RelLineTo\" IX=\"4\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"0\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"1\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "		<Row T=\"RelLineTo\" IX=\"5\">\n";
	itemStr += "		  <Cell N=\"X\" V=\"0\"/>\n";
	itemStr += "		  <Cell N=\"Y\" V=\"0\"/>\n";
	itemStr += "		</Row>\n";
	itemStr += "	  </Section>\n";
	itemStr += "	  <Text>" + item->caption() + "</Text>\n";
	itemStr += "	</Shape>\n";

	index++;

	return itemStr;
}

QString VsdxWriter::writeTextEllipseItem(DrawingTextEllipseItem* item, int& index)
{
	QString itemStr;

	QRectF ellipse = mapFromScene(item->mapToScene(item->ellipse()).boundingRect());
	QPointF topLeft = ellipse.normalized().topLeft();
	QPointF bottomRight = ellipse.normalized().bottomRight();
	qreal width = qAbs(bottomRight.x() - topLeft.x());
	qreal height = qAbs(bottomRight.y() - topLeft.y());

	itemStr += "    <Shape ID=\"" + QString::number(index) + "\" Type=\"Shape\" LineStyle=\"3\" FillStyle=\"3\" TextStyle=\"3\">\n";
	itemStr += "	  <Cell N=\"PinX\" V=\"" + QString::number(topLeft.x()) + "\"/>\n";
	itemStr += "	  <Cell N=\"PinY\" V=\"" + QString::number(topLeft.y()) + "\"/>\n";
	itemStr += "	  <Cell N=\"Width\" V=\"" + QString::number(width) + "\"/>\n";
	itemStr += "	  <Cell N=\"Height\" V=\"" + QString::number(height) + "\"/>\n";
	itemStr += "	  <Cell N=\"LocPinX\" V=\"0\" F=\"Width*0\"/>\n";
	itemStr += "	  <Cell N=\"LocPinY\" V=\"0\" F=\"Height*0\"/>\n";
	itemStr += "      <Cell N=\"Angle\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"FlipX\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"FlipY\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"ResizeMode\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"LeftMargin\" V=\"0\" U=\"PT\"/>\n";
	itemStr += "	  <Cell N=\"RightMargin\" V=\"0\" U=\"PT\"/>\n";
	itemStr += "	  <Cell N=\"TopMargin\" V=\"0\" U=\"PT\"/>\n";
	itemStr += "	  <Cell N=\"BottomMargin\" V=\"0\" U=\"PT\"/>\n";
	itemStr += writeItemStyle(item->style());
	itemStr += "      <Section N=\"Geometry\" IX=\"0\">\n";
	itemStr += "        <Row T=\"Ellipse\" IX=\"1\">\n";
	itemStr += "          <Cell N=\"X\" V=\"" + QString::number(width * 0.5) + "\" F=\"Width*0.5\"/>\n";
	itemStr += "          <Cell N=\"Y\" V=\"" + QString::number(height * 0.5) + "\" F=\"Height*0.5\"/>\n";
	itemStr += "          <Cell N=\"A\" V=\"" + QString::number(width) + "\" U=\"DL\" F=\"Width*1\"/>\n";
	itemStr += "          <Cell N=\"B\" V=\"" + QString::number(height * 0.5) + "\" U=\"DL\" F=\"Height*0.5\"/>\n";
	itemStr += "          <Cell N=\"C\" V=\"" + QString::number(width * 0.5) + "\" U=\"DL\" F=\"Width*0.5\"/>\n";
	itemStr += "          <Cell N=\"D\" V=\"" + QString::number(height) + "\" U=\"DL\" F=\"Height*1\"/>\n";
	itemStr += "        </Row>\n";
	itemStr += "      </Section>\n";
	itemStr += "	  <Text>" + item->caption() + "</Text>\n";
	itemStr += "    </Shape>\n";

	index++;

	return itemStr;
}

QString VsdxWriter::writeTextPolygonItem(DrawingTextPolygonItem* item, int& index)
{
	QString itemStr;

	QPolygonF polygon = mapFromScene(item->mapToScene(item->polygon()));
	QPointF topLeft = polygon.boundingRect().topLeft();
	QPointF bottomRight = polygon.boundingRect().bottomRight();
	qreal width = qAbs(bottomRight.x() - topLeft.x());
	qreal height = qAbs(bottomRight.y() - topLeft.y());
	int pointIndex = 1;

	itemStr += "    <Shape ID=\"" + QString::number(index) + "\" Type=\"Shape\" LineStyle=\"3\" FillStyle=\"3\" TextStyle=\"3\">\n";
	itemStr += "	  <Cell N=\"PinX\" V=\"" + QString::number(topLeft.x()) + "\"/>\n";
	itemStr += "	  <Cell N=\"PinY\" V=\"" + QString::number(topLeft.y()) + "\"/>\n";
	itemStr += "	  <Cell N=\"Width\" V=\"" + QString::number(width) + "\"/>\n";
	itemStr += "	  <Cell N=\"Height\" V=\"" + QString::number(height) + "\"/>\n";
	itemStr += "	  <Cell N=\"LocPinX\" V=\"0\" F=\"Width*0\"/>\n";
	itemStr += "	  <Cell N=\"LocPinY\" V=\"0\" F=\"Height*0\"/>\n";
	itemStr += "      <Cell N=\"Angle\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"FlipX\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"FlipY\" V=\"0\"/>\n";
	itemStr += "      <Cell N=\"ResizeMode\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"LeftMargin\" V=\"0\" U=\"PT\"/>\n";
	itemStr += "	  <Cell N=\"RightMargin\" V=\"0\" U=\"PT\"/>\n";
	itemStr += "	  <Cell N=\"TopMargin\" V=\"0\" U=\"PT\"/>\n";
	itemStr += "	  <Cell N=\"BottomMargin\" V=\"0\" U=\"PT\"/>\n";
	itemStr += writeItemStyle(item->style());
	itemStr += "      <Section N=\"Geometry\" IX=\"0\">\n";

	for(auto polyIter = polygon.begin(), polyEnd = polygon.end(); polyIter != polyEnd; polyIter++)
	{
		if (pointIndex == 1)
			itemStr += "        <Row T=\"MoveTo\" IX=\"" + QString::number(pointIndex) + "\">\n";
		else
			itemStr += "        <Row T=\"LineTo\" IX=\"" + QString::number(pointIndex) + "\">\n";
		itemStr += "          <Cell N=\"X\" V=\"" + QString::number(polyIter->x() - topLeft.x()) + "\"/>\n";
		itemStr += "          <Cell N=\"Y\" V=\"" + QString::number(height - bottomRight.y() + polyIter->y()) + "\"/>\n";
		itemStr += "        </Row>\n";

		pointIndex++;
	}

	itemStr += "        <Row T=\"LineTo\" IX=\"" + QString::number(pointIndex) + "\">\n";
	itemStr += "          <Cell N=\"X\" V=\"" + QString::number(polygon.first().x() - topLeft.x()) + "\"/>\n";
	itemStr += "          <Cell N=\"Y\" V=\"" + QString::number(height - bottomRight.y() + polygon.first().y()) + "\"/>\n";
	itemStr += "        </Row>\n";

	itemStr += "      </Section>\n";
	itemStr += "	  <Text>" + item->caption() + "</Text>\n";
	itemStr += "    </Shape>\n";

	index++;

	return itemStr;
}

QString VsdxWriter::writePathItem(DrawingPathItem* item, int& index)
{
	QString itemStr;

	QRectF rect = mapFromScene(item->mapToScene(item->rect()).boundingRect());
	QPointF topLeft = rect.normalized().topLeft();
	QPointF bottomRight = rect.normalized().bottomRight();
	qreal width = qAbs(bottomRight.x() - topLeft.x());
	qreal height = qAbs(bottomRight.y() - topLeft.y());

	QPainterPath path = item->path();
	QRectF pathRect = item->pathRect();
	int pathIndex = 1;
	QPointF prevPoint, curveEndPoint, curveStartControlPoint, curveEndControlPoint;
	bool curveDataValid = false;

	itemStr += "    <Shape ID=\"" + QString::number(index) + "\" Type=\"Shape\" LineStyle=\"3\" FillStyle=\"3\" TextStyle=\"3\">\n";
	itemStr += "	  <Cell N=\"PinX\" V=\"" + QString::number(topLeft.x()) + "\"/>\n";
	itemStr += "	  <Cell N=\"PinY\" V=\"" + QString::number(topLeft.y()) + "\"/>\n";
	itemStr += "	  <Cell N=\"Width\" V=\"" + QString::number(width) + "\"/>\n";
	itemStr += "	  <Cell N=\"Height\" V=\"" + QString::number(height) + "\"/>\n";
	itemStr += "	  <Cell N=\"LocPinX\" V=\"0\" F=\"Width*0\"/>\n";
	itemStr += "	  <Cell N=\"LocPinY\" V=\"0\" F=\"Height*0\"/>\n";
	itemStr += "	  <Cell N=\"Angle\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"FlipX\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"FlipY\" V=\"0\"/>\n";
	itemStr += "	  <Cell N=\"ResizeMode\" V=\"0\"/>\n";
	itemStr += writeItemStyle(item->style());
	itemStr += "	  <Cell N=\"FillPattern\" V=\"0\"/>\n";
	itemStr += "	  <Section N=\"Geometry\" IX=\"0\">\n";
	itemStr += "        <Cell N=\"NoFill\" V=\"1\"/>\n";
	itemStr += "		<Cell N=\"NoLine\" V=\"0\"/>\n";

	for(int i = 0; i < path.elementCount(); i++)
	{
		QPainterPath::Element element = path.elementAt(i);

		switch (element.type)
		{
		case QPainterPath::MoveToElement:
			prevPoint.setX((element.x - pathRect.left()) / pathRect.width());
			prevPoint.setY((pathRect.bottom() - element.y) / pathRect.height());
			itemStr += "		<Row T=\"RelMoveTo\" IX=\"" + QString::number(pathIndex) + "\">\n";
			itemStr += "		  <Cell N=\"X\" V=\"" + QString::number(prevPoint.x()) + "\"/>\n";
			itemStr += "		  <Cell N=\"Y\" V=\"" + QString::number(prevPoint.y()) + "\"/>\n";
			itemStr += "		</Row>\n";
			pathIndex++;
			break;
		case QPainterPath::LineToElement:
			prevPoint.setX((element.x - pathRect.left()) / pathRect.width());
			prevPoint.setY((pathRect.bottom() - element.y) / pathRect.height());
			itemStr += "		<Row T=\"RelLineTo\" IX=\"" + QString::number(pathIndex) + "\">\n";
			itemStr += "		  <Cell N=\"X\" V=\"" + QString::number(prevPoint.x()) + "\"/>\n";
			itemStr += "		  <Cell N=\"Y\" V=\"" + QString::number(prevPoint.y()) + "\"/>\n";
			itemStr += "		</Row>\n";
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

				qDebug() << prevPoint << curveStartControlPoint << curveEndControlPoint << curveEndPoint;

				QTransform transform;
				transform.rotateRadians(-qAtan2(curveEndPoint.y() - prevPoint.y(), curveEndPoint.x() - prevPoint.x()));
				transform.translate(-prevPoint.x(), -prevPoint.y());
				QPointF curveStartControlRel = transform.map(curveStartControlPoint);
				QPointF curveEndControlRel = transform.map(curveEndControlPoint);
				qreal a = curveStartControlRel.x();
				qreal b = curveStartControlRel.y() + 0.5;
				qreal c = curveEndControlRel.x();
				qreal d = curveEndControlRel.y() + 0.5;

				itemStr += "		<Row T=\"RelCubBezTo\" IX=\"" + QString::number(pathIndex) + "\">\n";
				itemStr += "		  <Cell N=\"X\" V=\"" + QString::number(curveEndPoint.x()) + "\"/>\n";
				itemStr += "		  <Cell N=\"Y\" V=\"" + QString::number(curveEndPoint.y()) + "\"/>\n";
				itemStr += "		  <Cell N=\"A\" V=\"" + QString::number(curveStartControlPoint.x()) + "\"/>\n";
				itemStr += "		  <Cell N=\"B\" V=\"" + QString::number(curveStartControlPoint.y()) + "\"/>\n";
				itemStr += "		  <Cell N=\"C\" V=\"" + QString::number(curveEndControlPoint.x()) + "\"/>\n";
				itemStr += "		  <Cell N=\"D\" V=\"" + QString::number(curveEndControlPoint.y()) + "\"/>\n";
				itemStr += "		</Row>\n";
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

	itemStr += "	  </Section>\n";
	itemStr += "	</Shape>\n";

	index++;

	return itemStr;
}

QString VsdxWriter::writeItemGroup(DrawingItemGroup* item, int& index)
{
	QString itemStr;



	return itemStr;
}

//==================================================================================================

QString VsdxWriter::writeItemStyle(DrawingItemStyle* style)
{
	QString styleStr;

	// Pen style information
	if (style->hasValue(DrawingItemStyle::PenColor))
		styleStr += "	  <Cell N=\"LineColor\" V=\"" + colorToHexString(style->value(DrawingItemStyle::PenColor).value<QColor>()) + "\"/>\n";

	if (style->hasValue(DrawingItemStyle::PenOpacity))
	{
		qreal alphaF = style->value(DrawingItemStyle::PenOpacity).toReal();
		if (alphaF != 1.0)
			styleStr += "	  <Cell N=\"LineColorTrans\" V=\"" + QString::number(1.0 - alphaF) + "\"/>\n";
	}

	if (style->hasValue(DrawingItemStyle::PenStyle))
	{
		Qt::PenStyle penStyle = (Qt::PenStyle)style->value(DrawingItemStyle::PenStyle).toUInt();
		if (penStyle == Qt::DotLine)
			styleStr += "	  <Cell N=\"LinePattern\" V=\"10\"/>\n";

		else if (penStyle == Qt::DashLine || penStyle == Qt::DashDotLine || penStyle == Qt::DashDotDotLine)
			styleStr += "	  <Cell N=\"LinePattern\" V=\"9\"/>\n";
	}

	if (style->hasValue(DrawingItemStyle::PenWidth))
	{
		// Pen width of 16.0 = 1 pt.  1 pt = 1/72 in.
		qreal penWidth = style->value(DrawingItemStyle::PenWidth).toReal();
		styleStr += "	  <Cell N=\"LineWeight\" V=\"" + QString::number(penWidth / 16 / 72) + "\"/>\n";
	}

	// Brush style information
	if (style->hasValue(DrawingItemStyle::BrushColor))
		styleStr += "	  <Cell N=\"FillForegnd\" V=\"" + colorToHexString(style->value(DrawingItemStyle::BrushColor).value<QColor>()) + "\"/>\n";

	if (style->hasValue(DrawingItemStyle::BrushOpacity))
	{
		qreal alphaF = style->value(DrawingItemStyle::BrushOpacity).toReal();
		if (alphaF != 0.0 && alphaF != 1.0)
		{
			styleStr += "	  <Cell N=\"FillForegndTrans\" V=\"" + QString::number(1.0 - alphaF) + "\"/>\n";
			styleStr += "	  <Cell N=\"FillBkgndTrans\" V=\"" + QString::number(1.0 - alphaF) + "\"/>\n";
		}
		else if (alphaF == 0.0)
			styleStr += "	  <Cell N=\"FillPattern\" V=\"0\"/>\n";
	}

	// Text alignment information
	if (style->hasValue(DrawingItemStyle::TextVerticalAlignment))
	{
		Qt::Alignment align = (Qt::Alignment)style->value(DrawingItemStyle::TextVerticalAlignment).toUInt();

		int alignValue = 1;
		if (align & Qt::AlignTop) alignValue = 0;
		else if (align & Qt::AlignBottom) alignValue = 2;

		if (alignValue != 1)
			styleStr += "	  <Cell N=\"VerticalAlign\" V=\"" + QString::number(alignValue) + "\"/>\n";
	}

	if (style->hasValue(DrawingItemStyle::TextHorizontalAlignment))
	{
		Qt::Alignment align = (Qt::Alignment)style->value(DrawingItemStyle::TextHorizontalAlignment).toUInt();

		int alignValue = 1;
		if (align & Qt::AlignLeft) alignValue = 0;
		else if (align & Qt::AlignRight) alignValue = 2;

		if (alignValue != 1)
		{
			styleStr += "      <Section N=\"Paragraph\">\n";
			styleStr += "        <Row IX=\"0\">\n";
			styleStr += "          <Cell N=\"HorzAlign\" V=\"" + QString::number(alignValue) + "\"/>\n";
			styleStr += "        </Row>\n";
			styleStr += "      </Section>\n";

		}
	}

	// Font information and text color
	QString fontName = style->hasValue(DrawingItemStyle::FontName) ?
		style->value(DrawingItemStyle::FontName).toString() : "";
	qreal fontSize = style->hasValue(DrawingItemStyle::FontSize) ?
		style->value(DrawingItemStyle::FontSize).toReal() * mDiagramScale * 96 / 72 : 0;		// pts
	bool fontBold = style->hasValue(DrawingItemStyle::FontBold) ?
		style->value(DrawingItemStyle::FontBold).toBool() : false;
	bool fontItalic = style->hasValue(DrawingItemStyle::FontItalic) ?
		style->value(DrawingItemStyle::FontItalic).toBool() : false;
	bool fontUnderline = style->hasValue(DrawingItemStyle::FontUnderline) ?
		style->value(DrawingItemStyle::FontUnderline).toBool() : false;
	bool fontStrikeThrough = style->hasValue(DrawingItemStyle::FontStrikeThrough) ?
		style->value(DrawingItemStyle::FontStrikeThrough).toBool() : false;

	if (fontName != "" || fontSize != 0 || fontBold || fontItalic || fontUnderline || fontStrikeThrough ||
		style->hasValue(DrawingItemStyle::TextColor))
	{
		uint fontStyle = 0;
		if (fontBold && fontItalic) fontStyle = 51;
		else if (fontItalic) fontStyle = 34;
		else if (fontBold) fontStyle = 17;
		if (fontUnderline) fontStyle += 4;

		styleStr += "	  <Section N=\"Character\">\n";
		styleStr += "	    <Row IX=\"0\">\n";
		if (fontName != "")
			styleStr += "	      <Cell N=\"Font\" V=\"" + fontName + "\"/>\n";
		if (fontSize != 0)
			styleStr += "	      <Cell N=\"Size\" V=\"" + QString::number(fontSize) + "\" U=\"PT\"/>\n";
		if (fontStyle != 0)
			styleStr += "	      <Cell N=\"Style\" V=\"" + QString::number(fontStyle) + "\"/>\n";
		if (fontStrikeThrough)
			styleStr += "	      <Cell N=\"Strikethru\" V=\"1\"/>\n";
		if (style->hasValue(DrawingItemStyle::TextColor))
			styleStr += "	     <Cell N=\"Color\" V=\"" + colorToHexString(style->value(DrawingItemStyle::TextColor).value<QColor>()) + "\"/>\n";
		styleStr += "	    </Row>\n";
		styleStr += "	  </Section>\n";
	}

	// Start and end arrow information (style and size)
	if (style->hasValue(DrawingItemStyle::StartArrowStyle))
	{
		DrawingItemStyle::ArrowStyle arrowStyle = (DrawingItemStyle::ArrowStyle)style->value(DrawingItemStyle::StartArrowStyle).toUInt();
		QString arrowStr = "0";
		QString arrowSize = "2";

		if (arrowStyle == DrawingItemStyle::ArrowNormal)
			arrowStr = "3";
		else if (arrowStyle == DrawingItemStyle::ArrowTriangleFilled || arrowStyle == DrawingItemStyle::ArrowConcaveFilled)
			arrowStr = "4";
		else if (arrowStyle == DrawingItemStyle::ArrowTriangle || arrowStyle == DrawingItemStyle::ArrowConcave)
			arrowStr = "16";
		else if (arrowStyle == DrawingItemStyle::ArrowCircleFilled || arrowStyle == DrawingItemStyle::ArrowDiamondFilled)
			arrowStr = "10";
		else if (arrowStyle == DrawingItemStyle::ArrowCircle || arrowStyle == DrawingItemStyle::ArrowDiamond)
			arrowStr = "20";

		if (style->hasValue(DrawingItemStyle::StartArrowSize) && style->hasValue(DrawingItemStyle::PenWidth))
		{
			qreal size = style->value(DrawingItemStyle::StartArrowSize).toReal();
			qreal penWidth = style->value(DrawingItemStyle::PenWidth).toReal();
			if (size < penWidth * 2) arrowSize = "0";
			else if (size < penWidth * 5) arrowSize = "1";
		}

		if (arrowStr != "0")
		{
			styleStr += "	  <Cell N=\"BeginArrow\" V=\"" + arrowStr + "\"/>\n";
			styleStr += "	  <Cell N=\"BeginArrowSize\" V=\"" + arrowSize + "\"/>\n";
		}
	}

	if (style->hasValue(DrawingItemStyle::EndArrowStyle))
	{
		DrawingItemStyle::ArrowStyle arrowStyle = (DrawingItemStyle::ArrowStyle)style->value(DrawingItemStyle::EndArrowStyle).toUInt();
		QString arrowStr = "0";
		QString arrowSize = "2";

		if (arrowStyle == DrawingItemStyle::ArrowNormal)
			arrowStr = "3";
		else if (arrowStyle == DrawingItemStyle::ArrowTriangleFilled || arrowStyle == DrawingItemStyle::ArrowConcaveFilled)
			arrowStr = "4";
		else if (arrowStyle == DrawingItemStyle::ArrowTriangle || arrowStyle == DrawingItemStyle::ArrowConcave)
			arrowStr = "16";
		else if (arrowStyle == DrawingItemStyle::ArrowCircleFilled || arrowStyle == DrawingItemStyle::ArrowDiamondFilled)
			arrowStr = "10";
		else if (arrowStyle == DrawingItemStyle::ArrowCircle || arrowStyle == DrawingItemStyle::ArrowDiamond)
			arrowStr = "20";

		if (style->hasValue(DrawingItemStyle::EndArrowSize) && style->hasValue(DrawingItemStyle::PenWidth))
		{
			qreal size = style->value(DrawingItemStyle::EndArrowSize).toReal();
			qreal penWidth = style->value(DrawingItemStyle::PenWidth).toReal();
			if (size < penWidth * 2) arrowSize = "0";
			else if (size < penWidth * 5) arrowSize = "1";
		}

		if (arrowStr != "0")
		{
			styleStr += "	  <Cell N=\"QuickStyleLineMatrix\" V=\"1\"/>\n";
			styleStr += "	  <Cell N=\"QuickStyleFillMatrix\" V=\"1\"/>\n";
			styleStr += "	  <Cell N=\"QuickStyleEffectsMatrix\" V=\"1\"/>\n";
			styleStr += "	  <Cell N=\"QuickStyleFontMatrix\" V=\"1\"/>\n";

			styleStr += "	  <Cell N=\"EndArrow\" V=\"" + arrowStr + "\"/>\n";
			styleStr += "	  <Cell N=\"EndArrowSize\" V=\"" + arrowSize + "\"/>\n";
		}
	}

	return styleStr;
}

//==================================================================================================

QPointF VsdxWriter::mapFromScene(const QPointF& pos) const
{
	QPointF newPos = (pos - mDiagramTranslate) * mDiagramScale;
	newPos.setX(newPos.x() + mVsdxMargin);
	newPos.setY(mVsdxHeight - newPos.y() + mVsdxMargin);
	return newPos;
}

QRectF VsdxWriter::mapFromScene(const QRectF& rect) const
{
	return QRectF(mapFromScene(rect.topLeft()), mapFromScene(rect.bottomRight()));
}

QPolygonF VsdxWriter::mapFromScene(const QPolygonF& poly) const
{
	QPolygonF newPoly;
	for(auto polyIter = poly.begin(), polyEnd = poly.end(); polyIter != polyEnd; polyIter++)
		newPoly.append(mapFromScene(*polyIter));
	return newPoly;
}

QString VsdxWriter::colorToHexString(const QColor& color) const
{
	QString str = "#";

	if (color.red() < 16) str += "0";
	str += QString::number(color.red(), 16).toUpper();
	if (color.green() < 16) str += "0";
	str += QString::number(color.green(), 16).toUpper();
	if (color.blue() < 16) str += "0";
	str += QString::number(color.blue(), 16).toUpper();

	return str;
}
