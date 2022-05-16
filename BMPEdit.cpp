// BMPEdit.cpp : implementation file
//

#include "stdafx.h"
#include "02test.h"
#include "BMPEdit.h"

#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBMPEdit
double QuantTbl[8][8] = {
	////  0     1     2     3     4     5     6     7   
		//1,    1,    1,    1,    1,    1,    1,    1,  //7 
		//1,    1,    1,    1,    1,    1,    1,    1,  //6 
		//1,    1,    1,    1,    1,    1,    1,    1,  //5 
		//1,    1,    1,    1,    1,    1,    1,    1,  //4 
		//1,    1,    1,    1,    1,    1,    1,    1,  //3 
		//1,    1,    1,    1,    1,    1,    1,    1,  //2 
		//1,    1,    1,    1,    1,    1,    1,    1,  //1 
		//2,    1,    1,    1,    1,    1,    1,    1   //0 

  // 	 // 0     1     2     3     4     5     6     7   
    	0,    0,    0,    0,    0,    0,    0,    0,  //7 
		0,    0,    0,    0,    0,    0,    0,    0,  //6 
		0,    0,    0,    0,    0,    0,    0,    0,  //5 
		0,    0,    0,    0,    0,    0,    0,    0,  //4 
		1,    1,    1,    1,    0,    0,    0,    0,  //3 
		1,    1,    1,    1,    0,    0,    0,    0,  //2 
		1,    1,    1,    1,    0,    0,    0,    0,  //1 
		2,    1,    1,    1,    0,    0,    0,    0   //0 			


// 0    1     2     3     4     5     6     7   
		//1,    2,    3,    4,    5,    6,    7,    8,  //7 
		//9,   10,   11,   12,   13,   14,   16,   16,  //6 
		//17,  18,   19,   20,   21,   22,   23,   24,  //5 
		//25,  26,   27,   28,   29,   30,   31,   32,  //4 
		//33,  34,   35,   36,   37,   38,   39,   40,  //3 
		//41,  42,   43,   44,   45,   46,   47,   48,  //2 
		//49,  49,   50,   51,   52,   53,   54,   55,  //1 
		//56,  57,   58,   59,   60,   61,   62,   63   //0 
};



//Add your code here


CBMPEdit::CBMPEdit()
{
	//Add your code here


}

CBMPEdit::~CBMPEdit()
{
	//Add your code here

}



/////////////////////////////////////////////////////////////////////////////
// CBMPEdit message handlers


BOOL CBMPEdit::LoadBmpFile(const CString& sBmpFileName)
{
	m_bReady = FALSE;

	//释放之前读取的数据///////
	//释放图像数据
	if (m_hgImageData)
		GlobalFree(m_hgImageData);

	m_sBMPFileFullName = sBmpFileName;
	if (m_sBMPFileFullName.IsEmpty())
		return FALSE;
	CFile BmpFile;
	if (!BmpFile.Open(m_sBMPFileFullName, CFile::modeRead))
	{
		AfxMessageBox("文件打开时出错!");
		return FALSE;
	}
	//格式检查
	if (BmpFile.Read(&m_BmpFileHeader, sizeof(BITMAPFILEHEADER)) < sizeof(BITMAPFILEHEADER))
	{
		AfxMessageBox("文件数据已损坏!");
		BmpFile.Close();
		return FALSE;
	}
	if (m_BmpFileHeader.bfType != 0x4D42)
	{
		AfxMessageBox("非位图文件");
		BmpFile.Close();
		return FALSE;
	}
	if (BmpFile.Read(&m_BmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER)) < sizeof(BITMAPINFOHEADER))
	{
		AfxMessageBox("文件数据已损坏!");
		BmpFile.Close();
		return FALSE;
	}
	//SetDIBitsToDevice()和StretchDIBits()会自行处理调色板，不需要再向当前DC中选入调色板了！！！

	//读取调色板
	if (m_BmpInfo.bmiHeader.biBitCount < 24)
		BmpFile.Read(&m_BmpInfo.bmiColors, GetColorNum(m_BmpInfo.bmiHeader) * sizeof(RGBQUAD));

	//读取图像数据
	long nBytePerLine = (m_BmpInfo.bmiHeader.biWidth * m_BmpInfo.bmiHeader.biBitCount + 31) / 32 * 4;//图像每行所占实际字节数（须是4的倍数）
	m_hgImageData = GlobalAlloc(GHND, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);

	if (m_hgImageData == NULL)
	{
		AfxMessageBox("内存不足");
		BmpFile.Close();
		return FALSE;
	}
	byte* pImageData = (byte*)GlobalLock(m_hgImageData);
	BmpFile.Read(pImageData, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);

	//can add some processing

	GlobalUnlock(m_hgImageData);

	BmpFile.Close();
	m_bReady = TRUE;


	return TRUE;
}

int CBMPEdit::GetColorNum(const BITMAPINFOHEADER& BmpInf)
{
	if (BmpInf.biClrUsed > 0)
		return BmpInf.biClrUsed;
	else
		return 1 << BmpInf.biBitCount;
}

BOOL CBMPEdit::PlayBmp(CDC* pDC, const CPoint& StartPoint)
{
	if (!m_bReady)
		return FALSE;
	byte* pData = (byte*)GlobalLock(m_hgImageData);

	if (!SetDIBitsToDevice(pDC->GetSafeHdc(),
		StartPoint.x, StartPoint.y,
		m_BmpInfo.bmiHeader.biWidth, m_BmpInfo.bmiHeader.biHeight,
		0, 0,
		0, m_BmpInfo.bmiHeader.biHeight,//
		pData,
		//	pBmpInf,
		(BITMAPINFO*)&m_BmpInfo,
		DIB_RGB_COLORS
	))
	{
		GlobalUnlock(m_hgImageData);
		return FALSE;
	}

	GlobalUnlock(m_hgImageData);
	return TRUE;

}

//added by tonglijing
BOOL CBMPEdit::PlayBmpDCT(CDC* pDC, const CPoint& StartPoint)
{
	if (!m_bReady)
		return FALSE;
	byte* pData = (byte*)GlobalLock(m_DCTResultD);

	if (!SetDIBitsToDevice(pDC->GetSafeHdc(),
		StartPoint.x, StartPoint.y,
		m_BmpInfo.bmiHeader.biWidth, m_BmpInfo.bmiHeader.biHeight,
		0, 0,
		0, m_BmpInfo.bmiHeader.biHeight,//
		pData,
		//	pBmpInf,
		(BITMAPINFO*)&m_BmpInfo,
		DIB_RGB_COLORS
	))
	{
		GlobalUnlock(m_DCTResultD);
		return FALSE;
	}

	GlobalUnlock(m_DCTResultD);
	return TRUE;

}

BOOL CBMPEdit::DCTTransform()
{
	//
	//added by tonglijing
	//AfxMessageBox("Test!");
	//图像每行所占实际字节数（须是4的倍数）
	long nBytePerLine = (m_BmpInfo.bmiHeader.biWidth * m_BmpInfo.bmiHeader.biBitCount + 31) / 32 * 4;
	byte* pImageData = (byte*)GlobalLock(m_hgImageData);

	m_DCTResultD = GlobalAlloc(GHND, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);
	byte* pDCTResultD = (byte*)GlobalLock(m_DCTResultD);

	int u, v, x, y, height, width;
	double Fuv;
	height = m_BmpInfo.bmiHeader.biHeight;
	width = m_BmpInfo.bmiHeader.biWidth;
	pFuvArray = new double[height * width];
	pImage8 = new double[64];
	pFuv8 = new double[64];

	for (v = 0; v < height / 8; v++)
	{
		for (u = 0; u < width / 8; u++)
		{
			//prepare pImage8
			for (y = 0; y < 8; y++)
			{
				for (x = 0; x < 8; x++)
				{

					*(pImage8 + y * 8 + x) = *(pImageData + v * 8 * width + y * width + u * 8 + x);

				}
			}

			//do  DCT
			DCTTransform8();

			//get result
			for (y = 0; y < 8; y++)
			{
				for (x = 0; x < 8; x++)
				{

					pFuvArray[v * 8 * width + y * width + u * 8 + x] = pFuv8[y * 8 + x];

					//prepare display 
					Fuv = pFuv8[y * 8 + x];
					if (Fuv < 0)
					{
						Fuv = Fuv * (-1);
					}

					//512 用 1000
					//Fuv = Fuv * 1000.0;

					//128用100
					//Fuv = Fuv * 100.0;

					//8 用
					Fuv = Fuv * 10;

					if (Fuv > 254)
					{
						//AfxMessageBox("Fuv>255");
						Fuv = 255;
					}
					*(pDCTResultD + v * 8 * width + y * width + u * 8 + x) = (byte)(Fuv + 0.5);

				}
			}
		}
	}

	GlobalUnlock(m_hgImageData);
	GlobalUnlock(m_DCTResultD);

	return TRUE;
}

BOOL CBMPEdit::DCTTransform8()
{
	//added by tonglijing
	//AfxMessageBox("Test!");

	int u, v, x, y;
	double Fuv, pre, post, Cu, Cv, fxy;
	double pi = 3.1415926;

	for (v = 0; v < 8; v++)
	{
		for (u = 0; u < 8; u++)
		{
			post = 0.0;
			if (v == 0)
			{
				Cv = 1 / sqrt(2.0);
			}
			else
			{
				Cv = 1;
			}
			if (u == 0)
			{
				Cu = 1 / sqrt(2.0);
			}
			else
			{
				Cu = 1;
			}
			pre = (double)4 / (8 * 8) * Cu * Cu * Cv * Cv;
			for (y = 0; y < 8; y++)
			{
				for (x = 0; x < 8; x++)
				{
					fxy = *(pImage8 + y * 8 + x);
					post = post + fxy * cos(pi / 2 / 8 * (2 * x + 1) * u) * cos(pi / 2 / 8 * (2 * y + 1) * v);
				}
			}
			Fuv = pre * post;
			pFuv8[v * 8 + u] = Fuv;

		}
	}


	return TRUE;
}


BOOL CBMPEdit::DCTQuantify()
{
	//add your code here============================================================== 
	//added by tonglijing
	int height = m_BmpInfo.bmiHeader.biHeight;
	int width = m_BmpInfo.bmiHeader.biWidth;
	int u, v, x, y;
	double temp;
	pQuantResult = new __int8[height * width];

	for (v = 0; v < height; v++)
	{
		for (u = 0; u < width; u++)
		{
			y = v % 8;
			x = u % 8;
			if (QuantTbl[7 - y][x] != 0)
			{
				temp = pFuvArray[v * width + u] / QuantTbl[7 - y][x];
				if (temp > 0)
				{
					temp = temp + 0.5;
				}
				else
				{
					temp = temp - 0.5;
				}
				pQuantResult[v * width + u] = (__int8)temp;
			}
			else
			{
				pQuantResult[v * width + u] = 0;
			}
		}
	}

	return TRUE;
}

BOOL CBMPEdit::DCTWrite()
{
	//add your code here ===========================================================
	int height = m_BmpInfo.bmiHeader.biHeight;
	int width = m_BmpInfo.bmiHeader.biWidth;
	int u, v, x, y;

	CFile QuantFile("quantify.txt", CFile::modeCreate | CFile::modeReadWrite);

	for (v = 0; v < height; v++)
	{
		for (u = 0; u < width; u++)
		{
			y = v % 8;
			x = u % 8;
			if (QuantTbl[7 - y][x] != 0)
			{
				QuantFile.Write(&pQuantResult[v * width + u], 1);
			}
		}
	}

	QuantFile.Close();

	CFile QuantFileLog("quantify_log.txt", CFile::modeCreate | CFile::modeReadWrite);
	char oneline[80];
	for (v = 0; v < height; v++)
	{
		for (u = 0; u < width; u++)
		{
			y = v % 8;
			x = u % 8;
			sprintf(oneline, "%d, %d, %10d, %10lf, %10lf\r\n", u, v, pQuantResult[v * width + u], pFuvArray[v * width + u], QuantTbl[7 - y][x]);
			QuantFileLog.Write(oneline, lstrlen(oneline));
		}
	}
	QuantFileLog.Close();

	return TRUE;
}

BOOL CBMPEdit::IDCTRead()
{
	//add your code here =================================================================
	int height = m_BmpInfo.bmiHeader.biHeight;
	int width = m_BmpInfo.bmiHeader.biWidth;
	int u, v, x, y;

	CFile QuantFile("quantify.txt", CFile::modeReadWrite);

	for (v = 0; v < height; v++)
	{
		for (u = 0; u < width; u++)
		{
			y = v % 8;
			x = u % 8;
			if (QuantTbl[7 - y][x] != 0)
			{
				QuantFile.Read(&pQuantResult[v * width + u], 1);
			}
			else
			{
				pQuantResult[v * width + u] = 0;
			}
		}
	}

	QuantFile.Close();

	return TRUE;
}

BOOL CBMPEdit::IDCTQuantify()
{
	//add your code here ===============================================
	int width = m_BmpInfo.bmiHeader.biWidth;
	int height = m_BmpInfo.bmiHeader.biHeight;
	int u, v, x, y;

	memset(pFuvArray, 0, sizeof(double) * width * height);

	for (v = 0; v < height; v++)
	{
		for (u = 0; u < width; u++)
		{
			y = v % 8;
			x = u % 8;
			pFuvArray[v * width + u] = pQuantResult[v * width + u] * QuantTbl[7 - y][x];
			double temp = pFuvArray[v * width + u];

		}
	}


	return TRUE;
}


BOOL CBMPEdit::IDCTTransform()
{
	int u, v, x, y, height, width;

	long nBytePerLine = (m_BmpInfo.bmiHeader.biWidth * m_BmpInfo.bmiHeader.biBitCount + 31) / 32 * 4;//图像每行所占实际字节数（须是4的倍数）
	m_IDCTResult = GlobalAlloc(GHND, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);
	byte* pIDCTResult = (byte*)GlobalLock(m_IDCTResult);

	height = m_BmpInfo.bmiHeader.biHeight;
	width = m_BmpInfo.bmiHeader.biWidth;

	for (y = 0; y < height / 8; y++)
	{
		for (x = 0; x < width / 8; x++)
		{
			//prepare pFuv8
			for (v = 0; v < 8; v++)
			{
				for (u = 0; u < 8; u++)
				{
					pFuv8[v * 8 + u] = pFuvArray[y * 8 * width + v * width + x * 8 + u];
				}
			}

			//do IDCT
			IDCTTransform8();

			//get result
			for (v = 0; v < 8; v++)
			{
				for (u = 0; u < 8; u++)
				{
					*(pIDCTResult + y * 8 * width + v * width + x * 8 + u) = (byte)pImage8[v * 8 + u];

				}
			}
		}
	}

	delete[] pFuvArray;
	delete[] pImage8;
	delete[] pFuv8;

	GlobalUnlock(m_IDCTResult);
	return TRUE;
}


BOOL CBMPEdit::IDCTTransform8()
{
	int u, v, x, y;
	double Fuv, post;
	double pi = 3.1415926;

	for (y = 0; y < 8; y++)
	{
		for (x = 0; x < 8; x++)
		{
			post = 0.0;
			for (v = 0; v < 8; v++)
			{
				for (u = 0; u < 8; u++)
				{
					Fuv = pFuv8[v * 8 + u];
					post = post + Fuv * cos(pi / 2 / 8 * (2 * x + 1) * u) * cos(pi / 2 / 8 * (2 * y + 1) * v);
				}
			}

			post = post + 0.5;
			if (post > 255)
			{
				post = 255.0;
			}
			if (post < 0)
			{
				post = 0.0;
			}
			*(pImage8 + y * 8 + x) = (byte)post;
		}
	}


	GlobalUnlock(m_IDCTResult);
	return TRUE;
}



BOOL CBMPEdit::PlayBmpIDCT(CDC* pDC, const CPoint& StartPoint)
{
	if (!m_bReady)
		return FALSE;
	byte* pData = (byte*)GlobalLock(m_IDCTResult);

	if (!SetDIBitsToDevice(pDC->GetSafeHdc(),
		StartPoint.x, StartPoint.y,
		m_BmpInfo.bmiHeader.biWidth, m_BmpInfo.bmiHeader.biHeight,
		0, 0,
		0, m_BmpInfo.bmiHeader.biHeight,//
		pData,
		//	pBmpInf,
		(BITMAPINFO*)&m_BmpInfo,
		DIB_RGB_COLORS
	))
	{
		GlobalUnlock(m_IDCTResult);
		return FALSE;
	}

	GlobalUnlock(m_IDCTResult);
	return TRUE;
}

BOOL CBMPEdit::Error()
{
	int x, y, height, width;

	long nBytePerLine = (m_BmpInfo.bmiHeader.biWidth * m_BmpInfo.bmiHeader.biBitCount + 31) / 32 * 4;//图像每行所占实际字节数（须是4的倍数）
	m_Error = GlobalAlloc(GHND, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);
	byte* pError = (byte*)GlobalLock(m_Error);

	byte* pImageData = (byte*)GlobalLock(m_hgImageData);
	byte* pIDCTResult = (byte*)GlobalLock(m_IDCTResult);

	height = m_BmpInfo.bmiHeader.biHeight;
	width = m_BmpInfo.bmiHeader.biWidth;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			*(pError + y * width + x) = abs(*(pIDCTResult + y * width + x) - *(pImageData + y * width + x)) * 10;
			if (*(pError + y * width + x) != 0)
			{
				//AfxMessageBox("Not Zero!");
			}

		}
	}


	GlobalUnlock(m_Error);
	GlobalUnlock(m_IDCTResult);
	GlobalUnlock(m_hgImageData);
	return TRUE;
}

BOOL CBMPEdit::PlayBmpError(CDC* pDC, const CPoint& StartPoint)
{
	if (!m_bReady)
		return FALSE;
	byte* pData = (byte*)GlobalLock(m_Error);

	if (!SetDIBitsToDevice(pDC->GetSafeHdc(),
		StartPoint.x, StartPoint.y,
		m_BmpInfo.bmiHeader.biWidth, m_BmpInfo.bmiHeader.biHeight,
		0, 0,
		0, m_BmpInfo.bmiHeader.biHeight,//
		pData,
		//	pBmpInf,
		(BITMAPINFO*)&m_BmpInfo,
		DIB_RGB_COLORS
	))
	{
		GlobalUnlock(m_IDCTResult);
		return FALSE;
	}

	GlobalUnlock(m_IDCTResult);
	return TRUE;
}


