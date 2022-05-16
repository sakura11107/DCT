#if !defined(AFX_BMPEDIT_H__CAD01C85_3CBA_4D33_AFDF_14A746B4BD41__INCLUDED_)
#define AFX_BMPEDIT_H__CAD01C85_3CBA_4D33_AFDF_14A746B4BD41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BMPEdit.h : header file
//

//BITMAPINFO����չ
struct EXT_BITMAPINFO
{
	BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[256];//��ɫ�����Ϊ256
};


/////////////////////////////////////////////////////////////////////////////
// CBMPEdit window

class CBMPEdit
{
// Construction
public:
	CBMPEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBMPEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	int  GetColorNum(const BITMAPINFOHEADER &BmpInf);
	BOOL LoadBmpFile(const CString &sBMPFileName);
	BOOL PlayBmp(CDC *pDC, const CPoint &StartPoint);

	BOOL DCTTransform();
	BOOL DCTTransform8();
	BOOL DCTQuantify();
	BOOL DCTWrite();
	BOOL PlayBmpDCT(CDC *pDC, const CPoint &StartPoint);

	BOOL IDCTRead();	
	BOOL IDCTQuantify();
	BOOL IDCTTransform();
	BOOL IDCTTransform8();
	BOOL PlayBmpIDCT(CDC *pDC, const CPoint &StartPoint);

	BOOL Error();
	BOOL PlayBmpError(CDC *pDC, const CPoint &StartPoint);

	virtual ~CBMPEdit();

private:
	BOOL m_bReady;  //�����Ƿ��Ѵ��ļ���ȡ
	CString m_sBMPFileFullName;  //����·����
	BITMAPFILEHEADER m_BmpFileHeader;
	EXT_BITMAPINFO m_BmpInfo;//��� BITMAPINFOHEADER�͵�ɫ��(�����ɫ)
	HGLOBAL m_hgImageData; //ͼ�����ݾ��

	//added by tonglijing
	HGLOBAL m_DCTResultD; //DCT�任�����ʾ������
	HGLOBAL m_IDCTResult; //IDCT���任���������
	HGLOBAL m_Error;      //IDCT���任���-ԭͼ
	double  *pFuvArray;    //DCT�任���������
	double  *pImage8;     //8*8�任ͼ�񻺳���
	double  *pFuv8;      //8*8�任Ƶ�׻�����
	__int8  *pQuantResult;  //DCT �������

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPEDIT_H__CAD01C85_3CBA_4D33_AFDF_14A746B4BD41__INCLUDED_)
