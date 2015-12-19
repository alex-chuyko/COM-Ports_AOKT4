#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <thread>
#include <string>

using namespace std;

HANDLE hSerial;
string userName;
bool myFlag = true;
static int TIMEOUT = 10;

void CreatePort();
void InitialPort();
void readFromCOMPort();
void writeInCOMPort();

int main()
{
	CreatePort();
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			cout << "Serial port does not exist.\n";
		}
		cout << "Some other error occurred.\n";
	}
	else
	{
		InitialPort();
		thread one(readFromCOMPort);
		while (1)
		{
			writeInCOMPort();
		}
	}
	system("pause");
}

void writeInCOMPort()
{
	char data[255] = { "\0" };
	DWORD dwSize = strlen(data); 
	DWORD dwBytesWritten;
	cout << "\r" << userName << ": ";
	gets(data);
	dwSize = strlen(data);
	WriteFile(hSerial, data, dwSize, &dwBytesWritten, NULL);
}

void readFromCOMPort()
{
	while (1)
	{
		bool flag = true;
		DWORD iSize;
		char sReceivedChar;
		do
		{
			if ((ReadFile(hSerial, &sReceivedChar, 1, &iSize, NULL) != 0) && (iSize > 0))
			{
				if (flag)
				{
					cout << "\rMESSAGE:       ";
					flag = false;
					cout << "\rMESSAGE: ";
				}
				cout << sReceivedChar;

			}
		} while (iSize > 0);
		if (!flag)
			myFlag = false;
		if (myFlag == false)
		{
			cout << "\n" << userName << ": ";
			myFlag = true;
		}
	}
}

void CreatePort()
{
	int num = 0;
	LPCTSTR sPortNameOne = L"COM1", sPortNameTwo = L"COM2";
	printf("Input number pot COM: ");
	scanf("%d", &num);
	if (num == 1)
	{
		hSerial = CreateFile(sPortNameOne, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, NULL, 0);
		userName = "<COM1>";
	}
	else
		if (num == 2)
		{
			hSerial = CreateFile(sPortNameTwo, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, NULL, 0);
			userName = "<COM2>";
		}
}

void InitialPort()
{
	SetCommMask(hSerial, EV_RXCHAR);
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = TIMEOUT;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = TIMEOUT;

	if (!SetCommTimeouts(hSerial, &CommTimeOuts))
	{
		cout << "Error!\n";
	}

	DCB ComDCB;
	memset(&ComDCB, 0, sizeof(ComDCB));
	ComDCB.DCBlength = sizeof(DCB);

	if (!GetCommState(hSerial, &ComDCB))
	{
		cout << "Getting state error!\n";
	}

	ComDCB.BaudRate = DWORD(9600);
	ComDCB.ByteSize = 8;
	ComDCB.Parity = NOPARITY;
	ComDCB.StopBits = ONESTOPBIT;
	ComDCB.fAbortOnError = TRUE;
	ComDCB.fDtrControl = DTR_CONTROL_DISABLE;
	ComDCB.fRtsControl = RTS_CONTROL_DISABLE;
	ComDCB.fBinary = TRUE;
	ComDCB.fParity = FALSE;
	ComDCB.fInX = ComDCB.fOutX = FALSE;
	ComDCB.XonChar = 0;
	ComDCB.XoffChar = (unsigned char)0xFF;
	ComDCB.fErrorChar = FALSE;
	ComDCB.fNull = FALSE;
	ComDCB.fOutxCtsFlow = FALSE;
	ComDCB.fOutxDsrFlow = FALSE;
	ComDCB.XonChar = ComDCB.XonLim = 128;

	if (!SetCommState(hSerial, &ComDCB))
	{
		cout << "Error setting serial port state.\n";
	}
}