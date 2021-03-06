#pragma once









#include <windows.h>
#include <winioctl.h>





/*





TCHAR szModelNo[48], szSerialNo[24];
if(GetPhyDriveSerial(szModelNo, szSerialNo))
{
_tprintf(_T(" : 0 1 2\n"));
_tprintf(_T(" : 012345678901234567890123456789\n"));
_tprintf(_T("Model No: %s\n"), szModelNo);
_tprintf(_T("Serial No: %s\n"), szSerialNo);
TrimStart(szSerialNo);
_tprintf(_T("Serial No: %s\n"), szSerialNo);
}
else
{
_tprintf(_T("Failed.\n"));
}
getchar();



*/
class GetHDSerial
{
public:
	GetHDSerial();
	~GetHDSerial();

	int GetDiskInfo(int driver = 0);
	CString OutModelNumber;
	CString OutSerialNumber;
	CString GetHDSerialNo() { GetDiskInfo(); return OutSerialNumber; }


	DWORD ParseDiskName(TCHAR *pszDiskName)
	{
		    static TCHAR *pszWordTable = { L"abcdefghijklmnopqrstuvwxyz" };
		
		    DWORD dwDisk;
		    DWORD dwBase = 0x1;
			DWORD dwCount = 0;          // 记录磁盘数量 
		    DWORD dwScanCount = 0;      // 要扫描的有效磁盘数量，用于返回 
		    DWORD dwStyle;
		
			    TCHAR szDiskPath[4] = { 0 };                  // 缓存一个磁盘根目录名 
		    TCHAR szDiskArray[26] = { 0 };                // 记录要扫描的所有磁盘名 
		
			    dwDisk = GetLogicalDrives();
		
			    while (dwDisk && dwCount <= ::lstrlen(pszWordTable))
			    {
			        memset(szDiskPath, 0, sizeof(szDiskPath));
			
				        if (dwDisk & dwBase)
				        {
				            ::lstrcpyn(szDiskPath, pszWordTable + dwCount, 2);
				            ::lstrcat(szDiskPath, TEXT("://"));
				            dwStyle = GetDriveType(szDiskPath);
				
					            // 是否可扫描的 
					            if (DRIVE_REMOVABLE == dwStyle || DRIVE_FIXED == dwStyle)
				           {
					                szDiskArray[dwScanCount] = pszWordTable[dwCount];
					                dwScanCount++;
					            }
				        }
			
				        dwDisk = dwDisk & ~dwBase;
			        dwBase = dwBase * 2;
			        dwCount++;
			    }
	
			    ::lstrcpy(pszDiskName, szDiskArray);
		    return dwScanCount;
	}









	BOOL GetPhyDriveSerial(LPTSTR pModelNo, LPTSTR pSerialNo)
	{
		//-1是因为 SENDCMDOUTPARAMS 的结尾是 BYTE bBuffer[1];
		BYTE IdentifyResult[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
		DWORD dwBytesReturned;
		GETVERSIONINPARAMS get_version;
		SENDCMDINPARAMS send_cmd = { 0 };

		HANDLE hFile = CreateFile(_T("\\\\.\\PHYSICALDRIVE0"), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return FALSE;

		//get version
		DeviceIoControl(hFile, SMART_GET_VERSION, NULL, 0,
			&get_version, sizeof(get_version), &dwBytesReturned, NULL);

		//identify device
		send_cmd.irDriveRegs.bCommandReg = (get_version.bIDEDeviceMap & 0x10) ? ATAPI_ID_CMD : ID_CMD;
		DeviceIoControl(hFile, SMART_RCV_DRIVE_DATA, &send_cmd, sizeof(SENDCMDINPARAMS) - 1,
			IdentifyResult, sizeof(IdentifyResult), &dwBytesReturned, NULL);
		CloseHandle(hFile);

		//adjust the byte order
		PUSHORT pWords = (USHORT*)(((SENDCMDOUTPARAMS*)IdentifyResult)->bBuffer);
		ToLittleEndian(pWords, 27, 46, pModelNo);
		ToLittleEndian(pWords, 10, 19, pSerialNo);
		return TRUE;
	}

	//把WORD数组调整字节序为little-endian，并滤除字符串结尾的空格。
	void  ToLittleEndian(PUSHORT pWords, int nFirstIndex, int nLastIndex, LPTSTR pBuf)
	{
		int index;
		LPTSTR pDest = pBuf;
		for (index = nFirstIndex; index <= nLastIndex; ++index)
		{
			pDest[0] = pWords[index] >> 8;
			pDest[1] = pWords[index] & 0xFF;
			pDest += 2;
		}
		*pDest = 0;

		//trim space at the endof string; 0x20: _T(' ')
		--pDest;
		while (*pDest == 0x20)
		{
			*pDest = 0;
			--pDest;
		}
	}

	//滤除字符串起始位置的空格
	void TrimStart(LPTSTR pBuf)
	{
		if (*pBuf != 0x20)
			return;

		LPTSTR pDest = pBuf;
		LPTSTR pSrc = pBuf + 1;
		while (*pSrc == 0x20)
			++pSrc;

		while (*pSrc)
		{
			*pDest = *pSrc;
			++pDest;
			++pSrc;
		}
		*pDest = 0;
	}



};

