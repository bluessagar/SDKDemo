#include <windows.h> 

#define _USE_MATH_DEFINES // for C++
#include <cmath>

#define MYTIMER 101

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
float DegreesToRadions(INT degrees);
DWORD WINAPI SerialThreadProc(LPVOID);


//Global Variables
HANDLE hComm;  // Handle to the Serial port
static SHORT	VerticesOfPolygon = 3;	// 4 = Square, 3 = Tringle, 5 = Pentagon
static DWORD	TimerCountMS = 1000;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX  WndCls;
	static char szAppName[] = "GDISuite";
	MSG         Msg;

	WndCls.cbSize = sizeof(WndCls);
	WndCls.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	WndCls.lpfnWndProc = WndProc;
	WndCls.cbClsExtra = 0;
	WndCls.cbWndExtra = 0;
	WndCls.hInstance = hInstance;
	WndCls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndCls.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndCls.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndCls.lpszMenuName = NULL;
	WndCls.lpszClassName = szAppName;
	WndCls.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
	RegisterClassEx(&WndCls);

	CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
		szAppName,
		"GDI Accessories and Tools",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	if(NULL!= hComm)
		CloseHandle(hComm);//Closing the Serial Port

	return static_cast<int>(Msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	static RECT		rc;

	// Common Math variables
	static INT		Radius = 100;
	static POINT	CircleCenter;
	static INT		AngleInDegrees = 90;	// initial value
	CONST INT		RotationAngleIncrement = 15;	
	TCHAR			str[255];
	static LONG		Height = 100, Width = 100; 
	POINT			PolygonCoordinates[7];	
	INT				AngleIncrementOfPolygon = 120;

	//variables for serail communication
//	HANDLE hComm;  // Handle to the Serial port -- make this global so that it can be used inside the task as well
	HANDLE hSerialThread = NULL;
	DCB	   dcbSerialParams = { 0 };  // Initializing DCB structure
	BOOL   Status; // Status
	COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure

	switch (Msg)
	{
	case WM_CREATE:
		SetTimer(hWnd, MYTIMER, TimerCountMS, NULL);	// 4th parameter -> Timer Callback function


		//Open the serial com port
		hComm = CreateFile("\\\\.\\COM8", //friendly name
			GENERIC_READ  | GENERIC_WRITE ,      // Read/Write Access
			0,                                 // No Sharing, ports cant be shared
			NULL,                              // No Security
			OPEN_EXISTING,                     // Open existing port only
			0,                                 // Non Overlapped I/O
			NULL);                             // Null for Comm Devices

		if (hComm == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, TEXT("Failed to open the serial Port"), TEXT("Error Message"), MB_OK);
		}
		else
		{
			//Setting the Parameters for the SerialPort
			dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

			Status = GetCommState(hComm, &dcbSerialParams); //retreives  the current settings
			if (Status == FALSE)
			{
				MessageBox(hWnd, TEXT("Failed to get current settings of serial Port"), TEXT("Error Message"), MB_OK);
			}
			else
			{ 
				dcbSerialParams.BaudRate = CBR_9600;      //BaudRate = 9600
				dcbSerialParams.ByteSize = 8;             //ByteSize = 8
				dcbSerialParams.StopBits = ONESTOPBIT;    //StopBits = 1
				dcbSerialParams.Parity = NOPARITY;      //Parity = None

				Status = SetCommState(hComm, &dcbSerialParams);
				if (Status == FALSE)
				{
					MessageBox(hWnd, TEXT("Failed to set serial port params."), TEXT("Error Message"), MB_OK);
				}
				else
				{
					//Setting Timeouts
					timeouts.ReadIntervalTimeout = 50;
					timeouts.ReadTotalTimeoutConstant = 50;
					timeouts.ReadTotalTimeoutMultiplier = 10;
					timeouts.WriteTotalTimeoutConstant = 50;
					timeouts.WriteTotalTimeoutMultiplier = 10;
					if (SetCommTimeouts(hComm, &timeouts) == FALSE)
					{
						MessageBox(hWnd, TEXT("Failed to set Serial Port Timeouts."), TEXT("Error Message"), MB_OK);
					}

					// At this point we have sucessfully opened and configured a serial port for communication
					// Lets create a thread to monitor and receive the data
					hSerialThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SerialThreadProc, (LPVOID)hWnd, 0, NULL);
				}				
			}
		}

		break;
	case WM_PAINT:


		hDC = BeginPaint(hWnd, &Ps);

		SelectObject(hDC, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hDC, RGB(255, 0, 0));			// red

		SelectObject(hDC, GetStockObject(DC_PEN));		// blue
		SetDCPenColor(hDC, RGB(0, 0, 255));

		// Step 1: Get Client Rect to know bounderies 
		GetClientRect(hWnd, &rc);

		// Calculate the co-ordinates if center of the Rect
		Width = rc.right - rc.left;
		Height = rc.top - rc.bottom;

		CircleCenter.x = rc.left + (Width / 2);
		CircleCenter.y = rc.bottom + (Height / 2);

		if (abs(Height) < abs(Width))
			Radius = (Height / 2) - (Height / 5);
		else
			Radius = (Width / 2) - (Width / 5);

		AngleIncrementOfPolygon = 360 / VerticesOfPolygon;

		for (short i = 0; i < VerticesOfPolygon; i++)
		{
			PolygonCoordinates[i].x = CircleCenter.x + (Radius * cos(DegreesToRadions((float) (AngleInDegrees + AngleIncrementOfPolygon*i) )));
			PolygonCoordinates[i].y = CircleCenter.y + (Radius * sin(DegreesToRadions((float) (AngleInDegrees + AngleIncrementOfPolygon*i) )));
		}

		Polygon(hDC, (const POINT *)PolygonCoordinates, VerticesOfPolygon);

		EndPaint(hWnd, &Ps);
		break;

	case WM_DESTROY:
		CloseHandle(hComm);//Closing the Serial Port
		PostQuitMessage(WM_QUIT);
		break;

	case WM_TIMER:
		KillTimer(hWnd, MYTIMER);
		
		AngleInDegrees = AngleInDegrees + RotationAngleIncrement;
		if (AngleInDegrees > 360)
			AngleInDegrees -= 360;
		InvalidateRect(hWnd, &rc, TRUE);
		SetTimer(hWnd, MYTIMER, TimerCountMS, NULL);
		break;
	case WM_KEYDOWN:
		TimerCountMS /= 2;
		if (TimerCountMS < 10)
			TimerCountMS = 1000;
		break;
	case WM_LBUTTONDOWN:
		VerticesOfPolygon++;
		if (VerticesOfPolygon == 8 )
			VerticesOfPolygon = 3;
		break;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}


float DegreesToRadions(INT degrees)
{
	return(degrees * (M_PI / 180));
}


DWORD WINAPI SerialThreadProc(LPVOID param)
{
	DWORD dwEventMask;     // Event mask to trigger
	BOOL Status;
	char  ReadData;        //temperory Character
	DWORD NoBytesRead;     // Bytes read by ReadFile()
	char SerialBuffer[64] = { 0 }; //Buffer to send and receive data
	unsigned char loop = 0;
	TCHAR str[255];
	static char lastRx;


	while (1) 
	{
		// Wait here till we receive an serial port interrupt
		WaitCommEvent(hComm, &dwEventMask, NULL);
		
		if (dwEventMask && EV_RXCHAR)
		{
			//Read data and store in a buffer
			do
			{
				Status = ReadFile(hComm, &ReadData, sizeof(ReadData), &NoBytesRead, NULL);
				SerialBuffer[loop] = ReadData;
				++loop;
			} while (NoBytesRead > 0);

			--loop; //Get Actual length of received data

			if (lastRx != SerialBuffer[0])
			{
				lastRx = SerialBuffer[0];
				
				switch (SerialBuffer[0])
				{
				case '1':
					TimerCountMS = 1000;
					break;
				case '2':
					TimerCountMS = 500;
					break;
				case '3':
					TimerCountMS = 250;
					break;
				case '4':
					TimerCountMS = 125;
					break;
				case '5':
					TimerCountMS = 62;
					break;
				case '6':
					TimerCountMS = 31;
					break;
				case '7':
					TimerCountMS = 15;
					break;
				case 'B':
					VerticesOfPolygon++;
					if (VerticesOfPolygon == 8)
						VerticesOfPolygon = 3;
					break;
				default:
					break;

			}

		}
			loop = 0;
			
			memset(SerialBuffer, 0, sizeof SerialBuffer);
		}
		Sleep(1000);
	}
	return 0;
}



