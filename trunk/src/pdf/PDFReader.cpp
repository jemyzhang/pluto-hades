/*******************************************************************************
**
**  NAME:			PDFReader.cpp	
**	VER: 			1.0
**  CREATE DATE: 	2009/07/06
**  AUTHOR:			Roger.Yi (roger2yi@gmail.com)
**  
**  Copyright (C) 2009 - PlutoWare All Rights Reserved
** 
**	
**	PURPOSE:	Class PDFReader
**
**  --------------------------------------------------------------
**
**	HISTORY:	v1.0, 2009/07/06
**
**
*******************************************************************************/
#include "stdafx.h"
#include "PDFReader.h"

#ifdef  _MSC_VER
#pragma warning(disable : 4244)
#endif  /* _MSC_VER */

#include "../../../include/mupdf_include.h"

namespace pdf {



static int FITZ_GCMEM_SIZE = 1024 * 512;


class PDFEngineInitiator
{
public:
	PDFEngineInitiator()
	{
		fz_cpudetect();
		fz_accelerate();
	}

	~PDFEngineInitiator()
	{
	}
};

static PDFEngineInitiator PDF_ENGINE_INITIATOR;


///< Page margin percentage
struct PageMargin
{
	double left;
	double right;
	double top;
	double bottom;

	int ignorePages;///<ignore first pages, not cut margin
};


struct BookInfo
{
	BookInfo()
		: pageCount(0)
	{
		margin.left = margin.right = margin.top = margin.bottom = 0.0;
		margin.ignorePages = 0;

		bk = QColor("white");
	}


	QString fileName;
	QString fileBaseName;
	QString filePath;

	QString password;

	int pageCount;
	QString bookTitle;

	PageMargin margin;
	QColor bk;
};


struct PDFReader::PDFReaderImpl
{
	PDFReaderImpl()
		: error(0)
		, okay(TRUE)
		, drawgc(NULL)
		, xref(NULL)
		, outline(NULL)
	{
	}

	~PDFReaderImpl()
	{
		clean();
	}

	BookInfo info;

	//mupdf data
	fz_error error;

	BOOL okay;///< okay flag, use by some methods.

	State state;///<state

	fz_renderer *drawgc;
	pdf_xref *xref;
	pdf_outline *outline;

	QHash<int, pdf_page*> pages;

	/************************************************************************/
	/*
	/* Functions
	/*
	/************************************************************************/
	void createRenderer()
	{
		if (drawgc)
		{
			clean();
		}

		error = fz_newrenderer(&drawgc, 
			pdf_devicergb, 
			0, 
			FITZ_GCMEM_SIZE);

		check();
	}


	void load(const QString& pdfFile, const QString& password)
	{
		QFileInfo fi(pdfFile);

		info.filePath = fi.absoluteFilePath();
		info.fileName = fi.fileName();
		info.fileBaseName = fi.baseName();
		info.password = password;

		//load xref
		reloadxref();

		//load root and information
		loadRootAndInfo();

		//page count and title
		error = pdf_getpagecount(xref, &info.pageCount);
		check();

		getTitle();

		//too slow, abandon this
		//loadNameTreesAndOutline();
	}

	pdf_page* getPdfPage(int pageNo)
	{
		pdf_page* page = pages[pageNo];

		if (!page) 
		{
			fz_obj * obj = NULL;
			error = pdf_getpageobject(xref, pageNo, &obj);
			check();

			error = pdf_loadpage(&page, xref, obj);
			check();

			//if (error)
			//{
			//	reloadstore();
			//	
			//	error = pdf_getpageobject(xref, pageNo, &obj);
			//	check();

			//	error = pdf_loadpage(&page, xref, obj);
			//	check();
			//}

			check();
			pages[pageNo] = page;
		}

		return page;
	}


	QImage render(int pageNo, 		
		ZoomLevel level, 
		int screenW,
		int screenH,
		int rotation,
		bool cutMargin)
	{
		pdf_page* page = getPdfPage(pageNo);

		//handle rotation
		if (page->rotate == 90 || page->rotate == 270)
		{
			qSwap(screenW, screenH);

			if (level <= FitWidth)
				level = (ZoomLevel)(level / 10);
			else if (level <= FitHeight && level > FitWidth)
				level = (ZoomLevel)(level * 10);
		}
		
		QSizeF size = pageSize(pageNo);
		double zoom = pageZoomRatio(pageNo, level, screenW, screenH, cutMargin);

		fz_matrix  ctm = viewctm(page, zoom, rotation);

		fz_rect bbox = fz_transformaabb(ctm, page->mediabox);

		if (cutMargin)
		{
			bbox.x0 += size.width() * info.margin.left * zoom;
			bbox.x1 -= size.width() * info.margin.right * zoom;
			bbox.y0 += size.height() * info.margin.top * zoom;
			bbox.y1 -= size.height() * info.margin.bottom * zoom;
		}

		fz_irect ibbox = round(bbox);

		fz_pixmap pixmap;
		pixmap.x = ibbox.x0;
		pixmap.y = ibbox.y0;
		pixmap.w = ibbox.x1 - ibbox.x0;
		pixmap.h = ibbox.y1 - ibbox.y0;
		pixmap.n = 4;

		//preallocate memory
		QImage image(pixmap.w, pixmap.h, QImage::Format_ARGB32);

		if (!image.isNull())
		{
			image.fill(qbswap<int>(info.bk.rgba()));//fill background, need reverse

			pixmap.samples = image.bits();

			error = fz_rendertreeover(drawgc, 
				&pixmap,
				page->tree, 
				ctm);

			check();

			int numInts = image.numBytes() / 4;
			int* src = (int*)image.bits();
			int* dst = (int*)image.bits();

			for (int i = 0; i < numInts; ++i)
			{
				*dst = qbswap<int>(*src);

				++src;
				++dst;
			}
		}

		return image;
	}


	//calculation the zoom ratio according to - page no, zoom level, screen size
	double pageZoomRatio(int pageNo, 
		ZoomLevel level, 
		int screenW,
		int screenH, 
		bool cutMargin)
	{
		if (level < 0)
		{
			//virtual zoom ratio, need calculation on demand
			QSizeF size = cutMargin ? pageSizeWithoutMargin(pageNo) :
				pageSize(pageNo);

			QSizeF scSize(screenW, screenH);

			if (level == FitPage)
			{
				QSizeF fitSize = size;
				fitSize.scale(scSize, Qt::KeepAspectRatio);

				return fitSize.width() / size.width();
			}
			else if (level <= FitWidth)//fit width
			{
				return screenW * level * 1.0 / (FitWidth * size.width());
			}
			else//fit height
			{
				return screenH * level * 1.0 / (FitHeight * size.height());
			}
		}
		else
		{
			return 1.0 * level / 100;
		}
	}


	QSizeF pageSizeWithoutMargin(int pageNo)
	{
		QSizeF size = pageSize(pageNo);

		QSizeF realSize(size.width() - size.width() * (info.margin.left + info.margin.right),
			size.height() - size.height() * (info.margin.top + info.margin.bottom));

		return realSize;
	}

	QSizeF pageSize(int pageNo)
	{
		QSizeF size;

		fz_obj *dict = NULL;
		error = pdf_getpageobject(xref, pageNo, &dict);
		check();

		fz_obj* obj = fz_dictgets(dict, "CropBox");
		if (!obj)
			obj = fz_dictgets(dict, "MediaBox");

		if (fz_isarray(obj))
		{
			fz_rect bbox = pdf_torect(obj);
			size = QSizeF(qAbs(bbox.x1 - bbox.x0), qAbs(bbox.y1 - bbox.y0));
		}

		return size;
	}


	int pageRotation(int pageNo)
	{
		int rotation = -1;

		fz_obj *dict = NULL;
		error = pdf_getpageobject(xref, pageNo, &dict);
		check();

		fz_obj* obj = fz_dictgets(dict, "Rotate");

		if (fz_isint(obj))
			rotation = fz_toint(obj);

		return rotation;
	}


	fz_matrix viewctm(pdf_page *page, float zoom, int rotate)
	{
		fz_matrix ctm = fz_identity();

		ctm = fz_concat(ctm, fz_translate(-page->mediabox.x0,
			-page->mediabox.y1));
		
		ctm = fz_concat(ctm, fz_scale(zoom, -zoom));
		
		rotate += page->rotate;
		
		if (rotate != 0)
			ctm = fz_concat(ctm, fz_rotate(rotate));
		
		return ctm;
	}


	fz_irect round(fz_rect f)
	{
		fz_irect i;
		i.x0 = fz_floor(f.x0 + 0.5);
		i.y0 = fz_floor(f.y0 + 0.5);
		i.x1 = fz_floor(f.x1 + 0.5);
		i.y1 = fz_floor(f.y1 + 0.5);
		return i;
	}

	void check()
	{
		if (error || !okay)
		{
			QString errMsg;

			if (error)
			{
				errMsg = QString("Engine error - %1.").arg(error);
			}

			if (!okay)
			{
				errMsg = QString("Set password error.");
			}

			__THROW_L(PDFException, errMsg);
		}
	}


	void clean()
	{
		droppages();

		dropoutline();

		dropxref();

		if (drawgc)
		{
			fz_droprenderer(drawgc);
			drawgc = NULL;
		}

		error = 0;
		okay = TRUE;

		info = BookInfo();
	}


	void dropoutline()
	{
		if (outline)
		{
			pdf_dropoutline(outline);
			outline = NULL;
		}
	}


	void droppage(int pageNo)
	{
		if (pages[pageNo])
		{
			pdf_droppage(pages[pageNo]);

			pages[pageNo] = NULL;
		}
	}


	void droppages()
	{
		foreach(pdf_page* page, pages.values())
		{
			if (page)
			{
				pdf_droppage(page);
			}
		}

		pages.clear();
	}


	void dropxref()
	{
		if (xref) 
		{
			if (xref->store)
				pdf_dropstore(xref->store);

			xref->store = 0;
			pdf_closexref(xref);

			xref = NULL;
		}
	}

	void reloadxref()
	{
		//drop first
		dropxref();

		//new
		error = pdf_newxref(&xref);
		check();


		//load (repair)
		error = pdf_loadxref(xref, (char*)qPrintable(info.filePath));

		if (error)
		{
			error = pdf_repairxref(xref, (char*)qPrintable(info.filePath));
			check();
		}

		//decrypt
		error = pdf_decryptxref(xref);
		check();

		//set password
		if (xref->crypt)
		{
			okay = pdf_setpassword(xref->crypt, (char *)qPrintable(info.password));
			check();
		}
	}

	void getTitle()
	{
		if(xref->info)
		{
			fz_obj* obj = fz_dictgets(xref->info ,  "Title");

			if(obj)	
			{
				char* title = NULL;
				error = pdf_toutf8(&title , obj);

				if(title)
				{
					info.bookTitle = QString::fromUtf8(title);
					fz_free(title);
				}
			}
		}
	}


	void loadRootAndInfo()
	{
		fz_obj *obj = fz_dictgets(xref->trailer, "Root");
		xref->root = fz_resolveindirect(obj);
		if (xref->root)
			fz_keepobj(xref->root);

		obj = fz_dictgets(xref->trailer, "Info");
		xref->info = fz_resolveindirect(obj);
		if (xref->info)
			fz_keepobj(xref->info);
	}


	void loadNameTreesAndOutline()
	{
		//load name trees
		error = pdf_loadnametrees(xref);
		check();

		//load outline
		error = pdf_loadoutline(&outline, xref);
		check();
	}

	void reloadstore()
	{
		pdf_dropstore(xref->store);
		pdf_newstore(&xref->store);

		droppages();
	}
};


PDFReader::PDFReader(int gcBufSize)
: impl_(new PDFReaderImpl())
{
	FITZ_GCMEM_SIZE = gcBufSize;
}


PDFReader::~PDFReader(void)
{
	SAFE_DELETE(impl_);
}


bool 
PDFReader::isNull() const
{
	return impl_->drawgc == NULL;
}


int 
PDFReader::pageCount() const
{
	return impl_->info.pageCount;
}


QString 
PDFReader::fileName() const
{
	return impl_->info.fileName;
}


QString 
PDFReader::fileBaseName() const
{
	return impl_->info.fileBaseName;
}


QString 
PDFReader::filePath() const
{
	return impl_->info.filePath;
}


QString 
PDFReader::bookTitle() const
{
	return impl_->info.bookTitle;
}


QImage 
PDFReader::render(int pageNo, 
				  ZoomLevel level, 
				  int screenW,
				  int screenH,
				  int rotation) const
{
	int realPageNo = qMin(qMax(pageNo, 0), this->pageCount() - 1);
	bool cutMargin = pageNo >= impl_->info.margin.ignorePages;

	return impl_->render(realPageNo, level, screenW, screenH, rotation, cutMargin);
}


QImage 
PDFReader::renderFitWidth(int pageNo, 
						  int width,
						  ZoomLevel level,
						  int rotation) const
{
	__PRECOND_E(level <= FitWidth, "Not fit width zoom level");

	return this->render(pageNo, level, width, 0, rotation);
}


void 
PDFReader::open(const QString& pdfFile, const QString& password)
{
	QFileInfo fi(pdfFile);

	if (fi.exists() &&
		QString::compare(fi.suffix(), QString("pdf"), Qt::CaseInsensitive) == 0)
	{
		impl_->createRenderer();

		impl_->load(pdfFile, password);
	}
	else
	{
		throw PDFException();
	}
}


QSizeF 
PDFReader::pageSize(int pageNo) const
{
	return impl_->pageSize(pageNo);
}


QSizeF 
PDFReader::pageSizeWithoutMargin(int pageNo) const
{
	return impl_->pageSizeWithoutMargin(pageNo);
}


int 
PDFReader::pageRotation(int pageNo) const
{
	return impl_->pageRotation(pageNo);
}


void 
PDFReader::setMargin(double leftPercent /*= 0.05*/,
					 double rightPercent /*= 0.05*/, 
					 double topPercent /*= 0.0*/, 
					 double bottomPercent /*= 0.0*/,
					 int ignorePages)
{
	impl_->info.margin.left = leftPercent;
	impl_->info.margin.right = rightPercent;
	impl_->info.margin.top = topPercent;
	impl_->info.margin.bottom = bottomPercent;

	impl_->info.margin.ignorePages = ignorePages;
}


void 
PDFReader::setBackground(QColor bk)
{
	impl_->info.bk = bk;
}


void 
PDFReader::clearEngineBuffer()
{
	impl_->reloadstore();
}


double 
PDFReader::leftMargin() const
{
	return impl_->info.margin.left;
}


double 
PDFReader::rightMargin() const
{
	return impl_->info.margin.right;
}


double 
PDFReader::topMargin() const
{
	return impl_->info.margin.top;
}


double 
PDFReader::bottomMargin() const
{
	return impl_->info.margin.bottom;
}


//namespace pdf
}
