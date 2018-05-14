// Project1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Project1.h"
#include "Project1Dlg.h"

#include "graymap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#import "progid:BarcodeReader.BarcodeDecoder" no_namespace




CProject1Dlg::CProject1Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProject1Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProject1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BARCODEDECODER1, m_BarcodeDecoder1);
	DDX_Control(pDX, IDC_EDIT1, m_Edit1);
}

BEGIN_MESSAGE_MAP(CProject1Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CProject1Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CProject1Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CProject1Dlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CProject1Dlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CProject1Dlg message handlers

BOOL CProject1Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_Edit1.SetWindowText( _T("barcodes.jpg") );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProject1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProject1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CProject1Dlg::ShowResults()
{
	CComPtr<IDispatch> pIDispatch;
	pIDispatch = m_BarcodeDecoder1.get_Barcodes();
	_ASSERTE( pIDispatch );

	CComPtr<IBarcodeList> pIBarcodeList;
	HRESULT hr = pIDispatch.QueryInterface( &pIBarcodeList );
	_ASSERTE( pIBarcodeList );

	long len;
	hr = pIBarcodeList->get_length( &len );
	_ASSERTE( pIBarcodeList );

	for( long i=0; i < len; ++i )
	{
		IBarcodePtr pBarcode;
		pBarcode = pIBarcodeList->item( i );
		_ASSERTE( pBarcode );

		CString sResult, sBarcode;

		switch( pBarcode->BarcodeType )
		{
		case Code128:				sBarcode = "Code128";				break;
		case Code39:				sBarcode = "Code39";				break;
		case Interl25:				sBarcode = "Interl25";				break;
		case EAN13:					sBarcode = "EAN13";					break;
		case EAN8:					sBarcode = "EAN8";					break;
		case Codabar:				sBarcode = "Codabar";				break;
		case Code11:				sBarcode = "Code11";				break;
		case UPCA:					sBarcode = "UPCA";					break;
		case UPCE:					sBarcode = "UPCE";					break;
		case Code93:				sBarcode = "Code93";				break;
		case LinearUnrecognized:	sBarcode = "Linear Unrecognized";	break;
		case PDF417Unrecognized:	sBarcode = "PDF417 Unrecognized";	break;
		case PDF417:				sBarcode = "PDF417";				break;
		}

		sResult.Format( _T("%s\n%s\n(%d,%d),(%d,%d),(%d,%d),(%d,%d)"), sBarcode, (LPCTSTR)pBarcode->Text, pBarcode->x1, pBarcode->y1, pBarcode->x2, pBarcode->y2, pBarcode->x3, pBarcode->y3, pBarcode->x4, pBarcode->y4 );
		AfxMessageBox( sResult );
	}
}

void CProject1Dlg::OnBnClickedButton1()
{
	CString sFileName;
	m_Edit1.GetWindowText( sFileName );
	m_BarcodeDecoder1.put_LinearFindBarcodes(7);
	m_BarcodeDecoder1.DecodeFile( sFileName );
	ShowResults();
}

void CProject1Dlg::OnBnClickedButton2()
{
	//prepare the image stream
	CString sFileName;
	m_Edit1.GetWindowText( sFileName );
	HANDLE hFile = ::CreateFile( sFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox( _T("Cant open the file") );
		return;
	}

	DWORD dwFileSize = ::GetFileSize( hFile, NULL );

	BYTE* pBuf = new BYTE[dwFileSize];

	DWORD dwNumberOfBytesRead ;
	if( !::ReadFile(hFile, pBuf, dwFileSize, &dwNumberOfBytesRead, NULL) )
	{
		AfxMessageBox( _T("Cant read the file") );
		return;
	}
	::CloseHandle( hFile );

	//create safe array
	CComVariant var;
	var.vt = VT_ARRAY | VT_UI1;
	var.parray = SafeArrayCreateVector( VT_UI1, 0, dwFileSize );
	if( !var.parray ) return;

	//lock safe array
	BYTE* pSaBuf=NULL;
	if( SUCCEEDED(SafeArrayAccessData(var.parray, (void HUGEP**)&pSaBuf)) )
	{
		//copy image into the safe array
		memcpy( pSaBuf, pBuf, dwFileSize );
		//unlock safe array
		SafeArrayUnaccessData( var.parray );
	}

	delete [] pBuf;

	m_BarcodeDecoder1.put_LinearFindBarcodes(7);

	//find barcodes in the image stream (doesnt use any file)
	HRESULT hr = m_BarcodeDecoder1.DecodeStream( var );

	ShowResults();
}

void CProject1Dlg::OnBnClickedButton3()
{
	//create safe array
	CComVariant var;
	var.vt = VT_ARRAY | VT_UI1;
	var.parray = SafeArrayCreateVector( VT_UI1, 0, sizeof(pgm) );
	if( !var.parray ) return;

	//lock safe array
	BYTE* pSaBuf=NULL;
	if( SUCCEEDED(SafeArrayAccessData(var.parray, (void HUGEP**)&pSaBuf)) )
	{
		//скопировать в safe array картинку
		memcpy( pSaBuf, pgm, sizeof(pgm) );
		//unlock safe array
		SafeArrayUnaccessData( var.parray );
	}

	HRESULT hr = m_BarcodeDecoder1.DecodeGrayMap( var, 105, 60 );

	ShowResults();
}

void CProject1Dlg::OnBnClickedButton4()
{
	m_BarcodeDecoder1.AboutBox();
}
