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
#include <DrawingWidget.h>
#include <QTextStream>
#include <quazip.h>
#include <quazipfile.h>

VsdxWriter::VsdxWriter(DrawingWidget* drawing)
{
	mDrawing = drawing;

	mDrawingUnits = "in";
	mDrawingScale = 0.001;
	//mDrawingUnits = "mm";
	//mDrawingScale = 0.025;
}

VsdxWriter::~VsdxWriter()
{
	mDrawing = nullptr;
}

//==================================================================================================

bool VsdxWriter::save(const QString& filePath)
{
	bool errorOccurred = false;

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

	const qreal margin = 0.25;
	QString pageWidth = QString::number(mDrawing->sceneRect().width() * mDrawingScale + 2 * margin);
	QString pageHeight = QString::number(mDrawing->sceneRect().height() * mDrawingScale + 2 * margin);
	QString xRulerOrigin = QString::number(margin - mDrawing->sceneRect().left() * mDrawingScale);
	QString yRulerOrigin = QString::number(margin + mDrawing->sceneRect().bottom() * mDrawingScale);
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
	return QString();
}
