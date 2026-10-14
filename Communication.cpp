
#include <stdio.h>
#include <string.h>
#include "stdafx.h"

#include <setupapi.h>

#include "Communication.h"
#include "Base.h"



#define XON            0x11
#define XOFF        0x13

static HANDLE g_shCommPort = INVALID_HANDLE_VALUE;  // 用于保存打开的串口句柄，如果未打开则赋值-1
static int g_siCurPortNo = 0xff;  // 当前端口号，取值1、2、3、4……，不可取0，0xff表示已关闭

extern CONFIG_INFO g_sConfigreInfo;
extern DWORD g_adwBauds[8];

int g_iCommCheckType = 0;  //校验类型标志 0:Lrc, 1:CRC16, 2:CRC32, 其它：错误


#ifdef _COMM_DEBUG_
void SaveRecv(BYTE *str, int iLen)
{
    FILE *fp;

    fp = fopen("RecvData.bin", "r+");
    if (NULL == fp)
    {
        fp = fopen("RecvData.bin", "w+");
        if (NULL == fp)
        {
            return ;
        }
    }
    fseek(fp, 0, SEEK_END);
    fwrite(str, sizeof(BYTE), iLen, fp);
    fclose(fp);
}
#endif

#ifdef _COMM_DEBUG_
void SaveSend(BYTE *str, int iLen)
{
    FILE *fp;

    fp = fopen("SendData.bin", "r+");
    if (NULL == fp)
    {
        fp = fopen("SendData.bin", "w+");
        if (NULL == fp)
        {
            return ;
        }
    }
    fseek(fp, 0, SEEK_END);
    fwrite(str, sizeof(BYTE), iLen, fp);
    fclose(fp);
}
#endif



/**************************

Serial port Communication start

***************************/

            
/****************************************************************************
  函数名     :  
  描述       :  获取当前工作串口端口号
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :  
      修改人     修改时间    修改版本号   修改原因
  1、
****************************************************************************/
int GetCurPortNo()
{
    return g_siCurPortNo;
}

/****************************************************************************
  函数名     :  
  描述       :  关闭当前工作的串口号
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :  
      修改人     修改时间    修改版本号   修改原因
  1、
****************************************************************************/
void SerClose(void)
{
    if (g_shCommPort != INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_shCommPort);
        g_shCommPort = INVALID_HANDLE_VALUE;
    }
    g_siCurPortNo = 0xff;
}


/****************************************************************************
  函数名     :  BYTE SerWrite(BYTE *lpByte, WORD dwBytesToWrite, WORD TimeOut)
  描述       :  向串口写数据
  输入参数   :  1、BYTE *lpByte : 要写入的数据包
                2、WORD dwBytesToWrite : 要写入的数据包lpByte的长度
  输出参数   :  无
  返回值     :  0:成功发送 / 1:发送的数据包长度为0 / 0xff:发送失败
****************************************************************************/
BYTE SerWrite(BYTE *lpByte, WORD wBytesToWrite)
{
    CString MsgStr;
    BOOL        fWriteStat;
    DWORD       dwBytesWritten ;
    DWORD       dwErrorFlags;
    COMSTAT        ComStat;

    if (wBytesToWrite == 0)
        return TRUE;
    fWriteStat = WriteFile(g_shCommPort, lpByte, 1, &dwBytesWritten, NULL); 
    if (!fWriteStat)
    {
        (void)ClearCommError(g_shCommPort, &dwErrorFlags, &ComStat );
        return 0xff;
    }
    if (wBytesToWrite > 1)
    {
        fWriteStat = WriteFile(g_shCommPort, lpByte+1, wBytesToWrite-1,
                               &dwBytesWritten, NULL); 
        if (!fWriteStat)
        {
            // some other error occurred
            (void)ClearCommError(g_shCommPort, &dwErrorFlags, &ComStat );
            return 0xff;
        }
    } 
#ifdef _COMM_DEBUG_
    SaveSend(lpByte, wBytesToWrite);
#endif
    return 0;
}

/****************************************************************************
  函数名     :  
  描述       :  读取串口数据
  输入参数   :  
  输出参数   :    
  返回值     :  0表示成功， 其他值表示失败
****************************************************************************/
BYTE SerRead(BYTE *lpByte , WORD wLength , DWORD dwTimeoutMs)
{
    CString        MsgStr;
    BOOL        fReadStat;
    DWORD        dwReaded, dwValue;
    DWORD       dwErrorFlags;
    COMSTAT        ComStat;
    DWORD       dwBeginTick, dwCurTick;
    BYTE        i=0;

    dwBeginTick = GetTickCount();
    while (1)
    {
        ClearCommError(g_shCommPort, &dwErrorFlags, &ComStat);
        fReadStat = ReadFile(g_shCommPort, lpByte+i, wLength-i, &dwReaded, NULL);
        if (!fReadStat)
        {
            MessageBox(NULL, "Failed to receive data!", "ERROR", MB_OK);
            return 0xfe;
        }

        if(dwReaded ==0)
        {
            dwCurTick = GetTickCount();
            dwValue = dwCurTick - dwBeginTick;
            if (dwValue >= dwTimeoutMs)
                return 0xfe;

            Sleep(10);
        }
        else
        {
            i +=(BYTE)dwReaded;    
            if (i >= wLength)
            {
#ifdef _COMM_DEBUG_
                SaveRecv(lpByte, wLength);
#endif
                return 0;
            }
        }
    } //while
}

/****************************************************************************
  函数名     :  
  描述       :  清除串口缓冲
  输入参数   :  １、DWORD mode　: 清除方式，PURGE_TXABORT | PURGE_RXABORT |
                        PURGE_TXCLEAR | PURGE_RXCLEAR
  输出参数   :  
  返回值     :  
  修改历史   :  
      修改人     修改时间    修改版本号   修改原因
  1、
****************************************************************************/
void PurgeCommBuf(DWORD mode)
{
    PurgeComm(g_shCommPort, mode);
}

/****************************************************************************
  函数名     :  
  描述       :  打开串口
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :  
      修改人     修改时间    修改版本号   修改原因
  1、
****************************************************************************/
static BOOL OpenSerialPort(LPCTSTR DevName,DWORD baud,BYTE databit,BYTE parity,BYTE stopbit)
{
    CString MsgStr;
    DCB            dcb;
    BOOL        fRetVal;
    DWORD        dwError;
    COMMTIMEOUTS  to;
    int i;

    if ((g_shCommPort=CreateFile(DevName,
            GENERIC_READ | GENERIC_WRITE,
            0,                  // exclusive access
            NULL,               // no security attrs
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, // | FILE_FLAG_OVERLAPPED, 
            NULL )) == (HANDLE) -1 )
    {
        g_shCommPort = INVALID_HANDLE_VALUE;
        return FALSE;
    }
    
    for(i=0; i<5; i++)
    {
        fRetVal = SetupComm(g_shCommPort, 8192, 8192);
        if (fRetVal)
            break;
        Sleep(10);
    }
    if (fRetVal == 0)
    {
        dwError = GetLastError();
        // an error occurred, try to recover
        MsgStr.Format("<CE-%u>", dwError);
        MessageBox(NULL, MsgStr, "SetupComm", MB_OK);
        CloseHandle(g_shCommPort);
        g_shCommPort = INVALID_HANDLE_VALUE;
        return FALSE;
    }

    // purge any information in the buffer
    PurgeComm(g_shCommPort, PURGE_TXABORT | PURGE_RXABORT |
            PURGE_TXCLEAR | PURGE_RXCLEAR );

    // set up for overlapped I/O

    GetCommTimeouts(g_shCommPort, &to ) ;
    to.ReadIntervalTimeout = 0xFFFFFFFF;
    to.ReadTotalTimeoutMultiplier = 0 ;
    to.ReadTotalTimeoutConstant = 0 ;
    to.WriteTotalTimeoutMultiplier = 10;
    to.WriteTotalTimeoutConstant = 5000 ;
    SetCommTimeouts(g_shCommPort, &to ) ;
    
    // set up DCB
    dcb.DCBlength = sizeof( DCB ) ;
    
    for(i=0; i<5; i++)
    {
        fRetVal =GetCommState(g_shCommPort, &dcb);
        if (fRetVal)
            break;
        Sleep(10);
    }
    dcb.BaudRate=baud;
    dcb.ByteSize=databit;
    dcb.Parity = parity;
    dcb.StopBits =stopbit;

    // setup hardware flow control
    dcb.fOutxDsrFlow = false ;
        dcb.fDtrControl = DTR_CONTROL_ENABLE ;

    dcb.fOutxCtsFlow = false ;
        dcb.fRtsControl = RTS_CONTROL_ENABLE ;

    // setup software flow control

    dcb.fInX = dcb.fOutX = false ;
    dcb.XonLim = 100 ;
    dcb.XoffLim = 100 ;
    dcb.XonChar = XON;
    dcb.XoffChar = XOFF;

    // other various settings
    dcb.fBinary = TRUE;
    if (dcb.Parity == 0)
        dcb.fParity = FALSE;
    else
        dcb.fParity = TRUE;

    // set up DCB
    dcb.DCBlength = sizeof(DCB);
    for(i=0; i<5; i++)
    {
        fRetVal = SetCommState(g_shCommPort, &dcb);
        if (fRetVal)
            break;
        Sleep(10);
    }
    if (fRetVal == 0)
    {
        dwError = GetLastError();
        // an error occurred, try to recover
        MsgStr.Format("<CE-%u>", dwError);
        MessageBox(NULL, MsgStr, "SetCommState", MB_OK);
        CloseHandle(g_shCommPort);
        g_shCommPort = INVALID_HANDLE_VALUE;
        return FALSE;
    }

    GetCommState(g_shCommPort, &dcb);
    PurgeComm(g_shCommPort, PURGE_TXABORT | PURGE_RXABORT |
                        PURGE_TXCLEAR | PURGE_RXCLEAR) ;
    return TRUE;
}


/****************************************************************************
  函数名     :  BYTE SerOpen(int iPortNo, DWORD BaudRate)
  描述       :  打开串口
  输入参数   :  1、int iPortNo : 串口端口号，取值１、２、３、４……
                2、DWORD BaudRate : 串口工作波特率
  输出参数   :  无
  返回值     :  0 : 成功　/ 0xff : 失败
  修改历史   :  
      修改人     修改时间    修改版本号   修改原因
  1、
****************************************************************************/
BYTE SerOpen(int iPortNo, DWORD BaudRate)
{
    char DevName[10];
    
    sprintf(DevName,"\\\\.\\COM%d", iPortNo);

    if (OpenSerialPort(DevName, BaudRate, 8, NOPARITY,ONESTOPBIT) ==  FALSE)
        return 0xff;

    g_siCurPortNo = iPortNo;
    return 0;
}

HANDLE GetSerialHandle(void)
{
    return g_shCommPort;
}

// 串口清缓冲
void SerReset(void)
{
    PurgeComm(g_shCommPort, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
}


/**************************

Serial port Communication end

***************************/




/**************************

USB Communication start

***************************/

//00873fdf-61a8-11d1-aa5e-00c04fb1728b
DEFINE_GUID(GUID_CLASS_I82930_BULK, 
0x00873fdf, 0x61a8, 0x11d1, 0xaa, 0x5e, 0x00, 0xc0, 0x4f, 0xb1, 0x72, 0x8b);


char g_inPipe[32] = "PIPE01";     // pipe name for bulk input pipe on our test board
char g_outPipe[32] = "PIPE00";    // pipe name for bulk output pipe on our test board
char g_completeDeviceName[256] = "";  //generated from the GUID registered by the driver itself

HANDLE g_hWrite = INVALID_HANDLE_VALUE;
HANDLE g_hRead  = INVALID_HANDLE_VALUE;
HANDLE g_hFile;

int  g_UsbOpenStatus=0;

DWORD ToreadNum,ReadPtr;
unsigned char Rcvbuf[8192];

#define TRYNUM 32

BOOL gfStopDownload=FALSE, gfStopSysDownload=FALSE;
//static BOOL OpenUsbPipe();
void ResetUsbDevice(void);


/*BYTE UsbRecvPack(BYTE *buff,DWORD toRecvlen, DWORD *Recvlen, DWORD Timeoutms)
{
    *Recvlen=ReadUsbDevice(buff, toRecvlen, Timeoutms);
    return TRUE;
}
*/

/*++
Routine Description:

    Given the HardwareDeviceInfo, representing a handle to the plug and
    play information, and deviceInfoData, representing a specific usb device,
    open that device and fill in all the relevant information in the given
    USB_DEVICE_DESCRIPTOR structure.

Arguments:

    HardwareDeviceInfo:  handle to info obtained from Pnp mgr via SetupDiGetClassDevs()
    DeviceInfoData:      ptr to info obtained via SetupDiEnumInterfaceDevice()

Return Value:

    return HANDLE if the open and initialization was successfull,
    else INVLAID_HANDLE_VALUE.

--*/
static HANDLE OpenOneDevice (IN HDEVINFO HardwareDeviceInfo, 
                      IN PSP_INTERFACE_DEVICE_DATA DeviceInfoData,
                      IN char *devName)
{
    PSP_INTERFACE_DEVICE_DETAIL_DATA     functionClassDeviceData = NULL;
    ULONG                                predictedLength = 0;
    ULONG                                requiredLength = 0;
    HANDLE                               hOut = INVALID_HANDLE_VALUE;
    
    //
    // allocate a function class device data structure to receive the
    // goods about this particular device.
    //
    SetupDiGetInterfaceDeviceDetail( HardwareDeviceInfo,
        DeviceInfoData, NULL, // probing so no output buffer yet
        0, // probing so output buffer length of zero
        &requiredLength, NULL); // not interested in the specific dev-node
    
    predictedLength = requiredLength;
    // sizeof (SP_FNCLASS_DEVICE_DATA) + 512;
    
    functionClassDeviceData = (SP_INTERFACE_DEVICE_DETAIL_DATA *)(malloc (predictedLength));
    functionClassDeviceData->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);
    
    //
    // Retrieve the information from Plug and Play.
    //
    if (! SetupDiGetInterfaceDeviceDetail(HardwareDeviceInfo, DeviceInfoData,
           functionClassDeviceData, predictedLength, &requiredLength, NULL))
    {
        free( functionClassDeviceData );
        return INVALID_HANDLE_VALUE;
    }
    
    strcpy( devName,functionClassDeviceData->DevicePath) ;
    
    hOut = CreateFile ( functionClassDeviceData->DevicePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, // no SECURITY_ATTRIBUTES structure
        OPEN_EXISTING, // No special create flags
        0, // No special attributes
        NULL); // No template file
    
    free( functionClassDeviceData );
    return hOut;
}

/*++
Routine Description:

   Do the required PnP things in order to find
   the next available proper device in the system at this time.

Arguments:

    pGuid:      ptr to GUID registered by the driver itself
    outNameBuf: the generated name for this device

Return Value:

    return HANDLE if the open and initialization was successful,
    else INVLAID_HANDLE_VALUE.
--*/
static HANDLE OpenUsbDeviceSub(LPGUID pGuid, char *outNameBuf)
{
    ULONG NumberDevices;
    HANDLE hOut = INVALID_HANDLE_VALUE;
    HDEVINFO                 hardwareDeviceInfo;
    SP_INTERFACE_DEVICE_DATA deviceInfoData;
    ULONG                    i;
    BOOLEAN                  done;
    PUSB_DEVICE_DESCRIPTOR   usbDeviceInst;
    PUSB_DEVICE_DESCRIPTOR   *UsbDevices = &usbDeviceInst;
    
    *UsbDevices = NULL;
    NumberDevices = 0;
    
    //
    // Open a handle to the plug and play dev node.
    // SetupDiGetClassDevs() returns a device information set that contains info on all
    // installed devices of a specified class.
    //
    hardwareDeviceInfo = SetupDiGetClassDevs (
        pGuid,
        NULL, // Define no enumerator (global)
        NULL, // Define no
        (DIGCF_PRESENT | // Only Devices present
        DIGCF_INTERFACEDEVICE)); // Function class devices.
    
    //
    // Take a wild guess at the number of devices we have;
    // Be prepared to realloc and retry if there are more than we guessed
    //
    NumberDevices = 4;
    done = FALSE;
    deviceInfoData.cbSize = sizeof (SP_INTERFACE_DEVICE_DATA);
    
    i=0;
    while (!done)
    {
        NumberDevices *= 2;
        if (*UsbDevices)
        {
            *UsbDevices = (PUSB_DEVICE_DESCRIPTOR )(realloc (*UsbDevices, 
                (NumberDevices * sizeof (USB_DEVICE_DESCRIPTOR))));
        }
        else
        {
            *UsbDevices = (PUSB_DEVICE_DESCRIPTOR )(calloc (NumberDevices, 
                sizeof (USB_DEVICE_DESCRIPTOR)));
        }
        
        if (NULL == *UsbDevices)
        {
            // SetupDiDestroyDeviceInfoList destroys a device information set
            // and frees all associated memory.
            SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
            return INVALID_HANDLE_VALUE;
        }
        
        usbDeviceInst = *UsbDevices + i;
        
        for (; i < NumberDevices; i++)
        {
            // SetupDiEnumDeviceInterfaces() returns information about device interfaces
            // exposed by one or more devices. Each call returns information about one interface;
            // the routine can be called repeatedly to get information about several interfaces
            // exposed by one or more devices.
            
            if (SetupDiEnumDeviceInterfaces(hardwareDeviceInfo, 0, // We don't care about specific PDOs
                pGuid, i, &deviceInfoData))
            {
                hOut = OpenOneDevice(hardwareDeviceInfo, &deviceInfoData, outNameBuf);
                if ( hOut != INVALID_HANDLE_VALUE )
                {
                    done = TRUE;
                    break;
                }
            }
            else
            {
                if (ERROR_NO_MORE_ITEMS == GetLastError())
                {
                    done = TRUE;
                    break;
                }    
            }    
        }
    }
    
    NumberDevices = i;
    
    // SetupDiDestroyDeviceInfoList() destroys a device information set
    // and frees all associated memory.
    
    SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
    free ( *UsbDevices );
    return hOut;
}

/*++
Routine Description:

    Given a ptr to a driver-registered GUID, give us a string with the device name
    that can be used in a CreateFile() call.
    Actually briefly opens and closes the device and sets outBuf if successfull;
    returns FALSE if not

Arguments:

    pGuid:      ptr to GUID registered by the driver itself
    outNameBuf: the generated zero-terminated name for this device

Return Value:

    TRUE on success else FALSE

--*/
static BOOL GetUsbDeviceFileName(LPGUID  pGuid, char *outNameBuf)
{
    HANDLE hDev = OpenUsbDeviceSub( pGuid, outNameBuf );
    if ( hDev != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hDev );
        return TRUE;
    }
    return FALSE;
}

/*++
Routine Description:

    Called by main() to open an instance of our device after obtaining its name

Arguments:

    None

Return Value:

    Device handle on success else NULL

--*/
static HANDLE OpenUsbPipeSub(char *filename)
{
    HANDLE h=0;
    int Trycnt;
    
    for(Trycnt=0; Trycnt<TRYNUM; Trycnt++)
    {
        if ( !GetUsbDeviceFileName((LPGUID)&GUID_CLASS_I82930_BULK,
            g_completeDeviceName))
        {
            Sleep(1);
            continue;
        }
        strcat(g_completeDeviceName, "\\");
        strcat(g_completeDeviceName, filename);
        h = CreateFile(g_completeDeviceName, GENERIC_WRITE | GENERIC_READ, 0, NULL, 
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL);
        if (h != INVALID_HANDLE_VALUE)
        {
            break;
        } 
        Sleep(1);
    }
    if(Trycnt==TRYNUM)
        return  INVALID_HANDLE_VALUE;
    
    return h;
}

static BOOL OpenUsbPipe()
{
    CString MsgStr;
    if(g_UsbOpenStatus>1)
        return TRUE;
    g_UsbOpenStatus=2;
    g_hWrite = OpenUsbPipeSub( g_outPipe);
    if(g_hWrite==INVALID_HANDLE_VALUE)
    {
        MsgStr.Format("Can't open USB device");
        MessageBox(NULL, MsgStr, "ERROR", MB_OK);
        CloseHandle(g_hWrite);
        return FALSE;
    }
    g_hRead = OpenUsbPipeSub(g_inPipe);
    if(g_hRead==INVALID_HANDLE_VALUE)
    {
        MsgStr.Format("Can't open USB device");
        MessageBox(NULL, MsgStr, "ERROR", MB_OK);
        CloseHandle(g_hRead);
        return FALSE;
    }
    ResetUsbDevice();
    return TRUE;
}

//extern
BOOL WriteUsbDevice(BYTE * lpByte, DWORD dwBytesToWrite)
{
    DWORD dwWrite=0,i=0;
    DWORD dwTimeout=500;
    OVERLAPPED ovInternal;
    DWORD   dwErrorFlags;
    COMSTAT ComStat;
    CString MsgStr;
    
    memset(&ovInternal,0,sizeof(OVERLAPPED));
    ovInternal.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
    if(ovInternal.hEvent==NULL)
    {
        return FALSE;
    }
    LPOVERLAPPED lpOverlapped;
    lpOverlapped=&ovInternal;
    while(1)
    {    
        if (gfStopDownload ==TRUE || gfStopSysDownload ==TRUE)
        {
            CloseHandle(ovInternal.hEvent);
            ResetUsbDevice();
            return 0;
        }
        
        ClearCommError(g_hWrite, &dwErrorFlags, &ComStat);
        if(!WriteFile(g_hWrite,lpByte+i,dwBytesToWrite-i,&dwWrite,lpOverlapped))
        {
            long lLastError=GetLastError();
            if(lLastError!=ERROR_IO_PENDING)
            {
                CloseHandle(ovInternal.hEvent);
                return FALSE;
            }
            switch(WaitForSingleObject(ovInternal.hEvent,dwTimeout))
            {
            case WAIT_OBJECT_0:
                if(!GetOverlappedResult(g_hWrite,lpOverlapped ,&dwWrite,TRUE))
                {
                    lLastError=GetLastError();
                    CloseHandle(ovInternal.hEvent);
                    return FALSE;
                }
                break;
            case WAIT_TIMEOUT:
                CloseHandle(ovInternal.hEvent);
                return FALSE;
            default:
                break;
            }
        }
        if(dwWrite>0)
        {
            i+=dwWrite;
            if(i>=dwBytesToWrite)
            {
                CloseHandle(ovInternal.hEvent);
                return TRUE;
            }
        }
    }
}

/*BYTE UsbSendPack(BYTE *buff,DWORD  sendlen)
{
    BOOL bret;
    ResetUsbDevice();
    bret=WriteUsbDevice(buff, sendlen);
    if(bret)
        return 0;
    return 0xff;
}
*/
//extern
DWORD ReadUsbDevice(BYTE * lpByte, DWORD dwLength, DWORD timeout)
{    
    DWORD dwRead=0,i=0,j,dwReaded;
    DWORD dwTimeout=timeout;  // ms
    OVERLAPPED ovInternal;    
    DWORD dwErrorFlags;
    COMSTAT ComStat;    
    CString MsgStr;    
    
    BYTE  Receivebuff[2048];
    
    i=0;    
    dwRead=dwLength;
    
    if(ToreadNum>0)
    {
        if(dwLength>ToreadNum)
        {
            memcpy(lpByte,Rcvbuf+ReadPtr,ToreadNum);
            i=ToreadNum;
            dwRead=dwLength-ToreadNum;
            ReadPtr=0;
            ToreadNum =0;
        }
        else
        {
            memcpy(lpByte,Rcvbuf+ReadPtr,dwLength);
            ReadPtr +=dwLength;
            ToreadNum -=dwLength;
            if(!ToreadNum)
                ReadPtr=0;
            return dwLength;             
        }
    }
    
    j=0;    
    
    memset(&ovInternal,0,sizeof(OVERLAPPED));
    ovInternal.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
    if(ovInternal.hEvent==NULL)
    {
        return -1;
    }
    LPOVERLAPPED lpOverlapped;
    lpOverlapped=&ovInternal;
    
    while(1)
    {
        if (gfStopDownload ==TRUE || gfStopSysDownload ==TRUE)
        {
            CloseHandle(ovInternal.hEvent);
            ResetUsbDevice();
            return 0;
        }
        
        ClearCommError( g_hRead, &dwErrorFlags, &ComStat );
        DWORD dwBytesRead;
        dwBytesRead =dwRead-j;
        if(dwBytesRead<512)
            dwBytesRead=512;
        memset(Receivebuff,0x0,sizeof(Receivebuff));  
        if(!ReadFile(g_hRead,Receivebuff,dwBytesRead,&dwReaded,lpOverlapped))
        {
            long lLastError=GetLastError();
            if(lLastError!=ERROR_IO_PENDING)
            {
                CloseHandle(ovInternal.hEvent);
                return 0;
            }
            switch(WaitForSingleObject(ovInternal.hEvent,dwTimeout))
            {
            case WAIT_OBJECT_0:
                if(!GetOverlappedResult(g_hRead,lpOverlapped ,&dwReaded,TRUE))
                {       
                    lLastError = GetLastError();
                    PurgeComm( g_hRead,  PURGE_RXCLEAR | PURGE_RXABORT );
                    CloseHandle(ovInternal.hEvent);
                    return -1;
                }
                break;
            case WAIT_TIMEOUT:
                CloseHandle(ovInternal.hEvent);
                return 0;
            default:
                break;
            }
            
        }
        
        memcpy(Rcvbuf+j,Receivebuff,dwReaded);
        ToreadNum += dwReaded; 
        j+=dwReaded;
        
        if(j>=dwRead)
        {
            CloseHandle(ovInternal.hEvent);
            break;
        }      
        
    }
    
    memcpy(lpByte+i,Rcvbuf+ReadPtr,dwRead);
    ReadPtr +=dwRead;
    ToreadNum -=dwRead;
    if(!ToreadNum)
        ReadPtr=0;
    return dwLength;
    
}

//extern
void ResetUsbDevice(void)
{
    ToreadNum=0;
    ReadPtr=0;
    memset(Rcvbuf,0x0,sizeof(Rcvbuf));
    PurgeComm( g_hWrite, PURGE_TXCLEAR | PURGE_TXABORT );
    PurgeComm( g_hRead,  PURGE_RXCLEAR | PURGE_RXABORT );    
}

//extern
void CloseUsbDevice()
{
    int i, nRet;

    if (g_hWrite !=INVALID_HANDLE_VALUE)
    {
        for(i=0; i<3; i++)
        {
            nRet =CloseHandle(g_hWrite);
            if (nRet !=FALSE)
                break;
        }
        g_hWrite =INVALID_HANDLE_VALUE;
    }     
    if (g_hRead !=INVALID_HANDLE_VALUE)
    {
        for(i=0; i<3; i++)
        {
            nRet =CloseHandle(g_hRead);
            if (nRet !=FALSE)
                break;
        }
        g_hRead =INVALID_HANDLE_VALUE;
    }    
    
    g_UsbOpenStatus=1;
}
//extern
BOOL SetupUsbPara()
{
    BOOL fRetVal;
    fRetVal= OpenUsbPipe();
    return fRetVal;
}
//extern

// 打开USB设备，对32个usb设备进行轮流打开，直到有成功打开的就返回
BOOL OpenUsbDevice()
{
    int Trycnt;
    CString MsgStr;
    
    if(g_UsbOpenStatus)
        return TRUE;
    
    for(Trycnt=0; Trycnt<TRYNUM; Trycnt++)
    {
        // 使用USB设备ID号打开USB设备
        g_hFile = OpenUsbDeviceSub( (LPGUID)&GUID_CLASS_I82930_BULK, 
            g_completeDeviceName);
        if (g_hFile == INVALID_HANDLE_VALUE)
        {
            Sleep(20);
            continue;
        }
        else
            break;
    }
    if (Trycnt == TRYNUM)
    {
        MsgStr.Format("Failed to open USB Device:(%s) = %d", 
            g_completeDeviceName, GetLastError());
        MessageBox(NULL, MsgStr, "ERROR", MB_OK);
        return FALSE;
    }
    g_UsbOpenStatus=1;
    return TRUE;
}

/**************************

USB Communication end

***************************/




WORD g_awhalfCrc16CCITT[16]={ /* CRC 半字节余式表 */ 
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};

/****************************************************************************
  函数名     :  void Crc16CCITT(BYTE *pbyDataIn, DWORD dwDataLen, BYTE abyCrcOut[2]) 
  描述       :  用移位的方法计算一组数字的16位CRC-CCITT校验值
  输入参数   :  1、BYTE *pbyDataIn : 要进行16位CRC-CCITT计算的数
                2、WORD dwDataLen : pbyDataIn数组的长度最大值65535
  输出参数   :  1、BYTE abyCrcOut[2] : 16位CRC-CCITT计算的结果
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-08-30  V1.0         创建
****************************************************************************/
void Crc16CCITT(BYTE *pbyDataIn, DWORD dwDataLen, BYTE abyCrcOut[2])
{
    WORD wCrc = 0;
    BYTE byTemp;
    while (dwDataLen-- != 0)
    {
        byTemp = ((BYTE)(wCrc>>8))>>4;
        wCrc <<= 4; 
        wCrc ^= g_awhalfCrc16CCITT[byTemp^(*pbyDataIn/16)]; 
        byTemp = ((BYTE)(wCrc>>8))>>4;
        wCrc <<= 4;
        wCrc ^= g_awhalfCrc16CCITT[byTemp^(*pbyDataIn&0x0f)]; 
        pbyDataIn++; 
    }
    abyCrcOut[0] = wCrc/256;
    abyCrcOut[1] = wCrc%256;    
}

BYTE LocalWrite(BYTE *pbySendData, WORD wDataLen)
{
    if (0 == g_sConfigreInfo.m_byCommType)
    {
        return SerWrite(pbySendData, wDataLen);
    }
    else
    {
        if (TRUE == WriteUsbDevice(pbySendData, wDataLen))
        {
            return 0;
        }
        else
        {
            return 0xff;
        }
    }
}

/****************************************************************************
  函数名     :  int Crc16SendPacket(BYTE *pbySendData, WORD wDataLen, WORD wCmd)
  描述       :  数据包发送函数，带CRC16-CCITT校验
  输入参数   :  1、BYTE *pbySendData：要发送的数据包
                2、WORD wDataLen：数据包长度
                3、WORD wCmd：命令码
  输出参数   :  无
  返回值     :  SUCCESS：发送成功 / SEND_PACKET_ERROR：发送错误 
                / PACKET_LEN_TOO_LONG
  修改历史   :  
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-08-30  V1.0         创建
****************************************************************************/
int Crc16SendPacket(BYTE *pbySendData, WORD wDataLen, WORD wCmd)
{
    static BYTE abyBuff[9000];
    BYTE byRet;

    if (wDataLen > 8900)
    {
        return PACKET_LEN_TOO_LONG;
    }
    abyBuff[0] = 0x02;
    abyBuff[1] = (BYTE)(wCmd/256);
    abyBuff[2] = (BYTE)(wCmd%256);
    abyBuff[3] = wDataLen/256;
    abyBuff[4] = wDataLen%256;
    memcpy(&abyBuff[5], pbySendData, wDataLen);
    Crc16CCITT(&abyBuff[1], wDataLen+4, &abyBuff[wDataLen+5]);

    byRet = LocalWrite(abyBuff, wDataLen+7);
    if (0 != byRet)
    {
        return SEND_PACKET_ERROR;
    }
    return SUCCESS;
}

// ReadUsbDevice

BYTE LocalRead(BYTE *lpByte , WORD wLength , DWORD dwTimeoutMs)
{
    if (0 == g_sConfigreInfo.m_byCommType)
    {
        return SerRead(lpByte, wLength, dwTimeoutMs);
    }
    else
    {
        if (ReadUsbDevice(lpByte, wLength, dwTimeoutMs) > 0)
        {
            return 0;  // 成功
        }
        else
        {
            return 0xff;  // 失败
        }
    }
}

/****************************************************************************
  函数名     :  BYTE PortOpen()
  描述       :  打开设备端口
  输入参数   :  无
  输出参数   :  无
  返回值     :  0 : 成功　/ 0xff : 失败
  修改历史   :  
      修改人     修改时间    修改版本号   修改原因
  1、
****************************************************************************/
BYTE PortOpen()
{
    if (0 == g_sConfigreInfo.m_byCommType)
    {
        return SerOpen(g_sConfigreInfo.m_iSerialPortNumber+1, 
            g_adwBauds[g_sConfigreInfo.m_iSerialPortBaudsSelect]);
    }
    else
    {
        if (TRUE == OpenUsbDevice())
        {
            return 0;
        }
        else
        {
            return 0xff;
        }
    }
}

void PortClose()
{
    if (0 == g_sConfigreInfo.m_byCommType)
    {
        SerClose();
    }
    else
    {
        CloseUsbDevice();
    }

}
/****************************************************************************
  函数名     :  int Crc16RecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs)
  描述       :  数据包接收函数，带CRC16-CCITT校验
  输入参数   :  1、DWORD dwTimeoutMs：接收超时时间，时间单位是毫秒
  输出参数   :  1、BYTE *pbyRecvData：接收到的所有数据，包括包头和校验，传入
                   的pbyRecvData指向的内存不得小于256字节
                2、WORD *pwPacketetLen：接收到的所有数据的长度
  返回值     :  SUCCESS：接收成功 / CRC_CHECK_ERROR：数据包CRC校验错误
                / PACKET_LEN_ERROR：数据包长度错误 / RECV_TIMEOUT：超时 
                / RECV_PACKET_ERROR：接收错误 
  修改历史   :  
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-08-30  V1.0         创建
****************************************************************************/
int Crc16RecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs)
{
    BYTE byRet, abyCrc[2];
    int i;
    WORD wPacketLen = 0;
    DWORD   dwBegintick, dwCurtick;

    dwBegintick = GetTickCount();  // 时间单位是毫秒
    while (1)
    {
        dwCurtick = GetTickCount();
        if ((dwCurtick-dwBegintick) > dwTimeoutMs)
        {
            return RECV_TIMEOUT;
        }
        byRet = LocalRead(pbyRecvData, 1, 3); // timeout = 3ms
        if ((0==byRet) && (0x02==pbyRecvData[0]))
        {
            break;
        }
        Sleep(1);
    }
    byRet = LocalRead(&pbyRecvData[1], 4, 100); // timeout = 100ms
    if (0 != byRet)
    {
        return RECV_PACKET_ERROR;
    }
    wPacketLen = pbyRecvData[3]*256 + pbyRecvData[4];
    *pwPacketetLen = wPacketLen + 7;
    if (wPacketLen > RECV_PACKET_MAX_LEN)
    {
        return PACKET_LEN_ERROR;
    }
    for (i=0; i<wPacketLen+2; i++)
    {
        byRet = LocalRead(&pbyRecvData[i+5], 1, 100);
        if (0 != byRet)
        {
            return RECV_PACKET_ERROR;
        }
    }
    Crc16CCITT(&pbyRecvData[1], wPacketLen+4, abyCrc);
    if (0 != memcmp(abyCrc, &pbyRecvData[5+wPacketLen], 2))
    {
        return CRC_CHECK_ERROR;
    }

    return SUCCESS;
}


/****************************************************************************
  函数名     :  int LrcSendPacket(BYTE *pbySendData, WORD wDataLen, WORD wCmd)
  描述       :  数据包发送函数，带Lrc校验
  输入参数   :  1、BYTE *pbySendData：要发送的数据包
                2、WORD wDataLen：数据包长度
                3、WORD wCmd：命令码
  输出参数   :  无
  返回值     :  SUCCESS：发送成功 / SEND_PACKET_ERROR：发送错误 
                / PACKET_LEN_TOO_LONG
  修改历史   :  
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-08-30  V1.0         创建
****************************************************************************/
int LrcSendPacket(BYTE *pbySendData, WORD wDataLen, BYTE byCmd)
{
    static BYTE abyBuff[4500];
    int i;
    BYTE byLrc = 0;

    BYTE byRet;

    if (wDataLen > 4096)
    {
        return PACKET_LEN_TOO_LONG;
    }
    abyBuff[0] = 0x02;
    abyBuff[1] = byCmd;
    abyBuff[2] = wDataLen/256;
    abyBuff[3] = wDataLen%256;
    memcpy(&abyBuff[4], pbySendData, wDataLen);
    for (i=1; i<(wDataLen+4); i++)
    {
        byLrc ^= abyBuff[i];
    }
    abyBuff[wDataLen+4] = byLrc;

    byRet = SerWrite(abyBuff, wDataLen+5);
    if (0 != byRet)
    {
        return SEND_PACKET_ERROR;
    }
    return SUCCESS;
}

/****************************************************************************
  函数名     :  int LrcRecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs)
  描述       :  TMS数据包接收函数，带Lrc校验
  输入参数   :  1、DWORD dwTimeoutMs：接收超时时间，时间单位是毫秒
  输出参数   :  1、BYTE *pbyRecvData：接收到的所有数据，包括包头和校验，传入
                   的pbyRecvData指向的内存不得小于256字节
                2、WORD *pwPacketetLen：接收到的所有数据的长度
  返回值     :  SUCCESS：接收成功 / CRC_CHECK_ERROR：数据包Lrc校验错误
                / PACKET_LEN_ERROR：数据包长度错误 / RECV_TIMEOUT：超时 
                / RECV_PACKET_ERROR：接收错误 
  修改历史   :  
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-08-30  V1.0         创建
****************************************************************************/
int LrcRecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs)
{
    BYTE byRet, byLrc = 0;
    int i;
    WORD wPacketLen = 0;
    DWORD dwBegintick, dwCurtick;

    dwBegintick = GetTickCount();  // 时间单位是毫秒
    while (1)
    {
        dwCurtick = GetTickCount();
        if ((dwCurtick-dwBegintick) > dwTimeoutMs)
        {
            return RECV_TIMEOUT;
        }
        byRet = SerRead(pbyRecvData, 1, 3); // timeout = 3ms
        if ((0==byRet) && (0x02==pbyRecvData[0]))
        {
            break;
        }
        Sleep(1);
    }
    byRet = SerRead(&pbyRecvData[1], 3, 100); // timeout = 100ms
    if (0 != byRet)
    {
        return RECV_PACKET_ERROR;
    }
    byLrc = pbyRecvData[1];
    byLrc ^= pbyRecvData[2];
    byLrc ^= pbyRecvData[3];
    wPacketLen = pbyRecvData[2]*256 + pbyRecvData[3];
    *pwPacketetLen = wPacketLen + 5;
    if (wPacketLen > RECV_PACKET_MAX_LEN)
    {
        return PACKET_LEN_ERROR;
    }
    for (i=0; i<wPacketLen+1; i++)
    {
        byRet = SerRead(&pbyRecvData[i+4], 1, 100);
        if (0 != byRet)
        {
            return RECV_PACKET_ERROR;
        }
        byLrc ^= pbyRecvData[i+4];
    }
    if (0 != byLrc)
    {
        return LRC_CHECK_ERROR;
    }

    return SUCCESS;
}

const DWORD gc_dwCrc32Table[256]={
    0x00000000,0x77073096,0xee0e612c,0x990951ba,
    0x076dc419,0x706af48f,0xe963a535,0x9e6495a3,
    0x0edb8832,0x79dcb8a4,0xe0d5e91e,0x97d2d988,
    0x09b64c2b,0x7eb17cbd,0xe7b82d07,0x90bf1d91,
    0x1db71064,0x6ab020f2,0xf3b97148,0x84be41de,
    0x1adad47d,0x6ddde4eb,0xf4d4b551,0x83d385c7,
    0x136c9856,0x646ba8c0,0xfd62f97a,0x8a65c9ec,
    0x14015c4f,0x63066cd9,0xfa0f3d63,0x8d080df5,
    0x3b6e20c8,0x4c69105e,0xd56041e4,0xa2677172,
    0x3c03e4d1,0x4b04d447,0xd20d85fd,0xa50ab56b,
    0x35b5a8fa,0x42b2986c,0xdbbbc9d6,0xacbcf940,
    0x32d86ce3,0x45df5c75,0xdcd60dcf,0xabd13d59,
    0x26d930ac,0x51de003a,0xc8d75180,0xbfd06116,
    0x21b4f4b5,0x56b3c423,0xcfba9599,0xb8bda50f,
    0x2802b89e,0x5f058808,0xc60cd9b2,0xb10be924,
    0x2f6f7c87,0x58684c11,0xc1611dab,0xb6662d3d,
    0x76dc4190,0x01db7106,0x98d220bc,0xefd5102a,
    0x71b18589,0x06b6b51f,0x9fbfe4a5,0xe8b8d433,
    0x7807c9a2,0x0f00f934,0x9609a88e,0xe10e9818,
    0x7f6a0dbb,0x086d3d2d,0x91646c97,0xe6635c01,
    0x6b6b51f4,0x1c6c6162,0x856530d8,0xf262004e,
    0x6c0695ed,0x1b01a57b,0x8208f4c1,0xf50fc457,
    0x65b0d9c6,0x12b7e950,0x8bbeb8ea,0xfcb9887c,
    0x62dd1ddf,0x15da2d49,0x8cd37cf3,0xfbd44c65,
    0x4db26158,0x3ab551ce,0xa3bc0074,0xd4bb30e2,
    0x4adfa541,0x3dd895d7,0xa4d1c46d,0xd3d6f4fb,
    0x4369e96a,0x346ed9fc,0xad678846,0xda60b8d0,
    0x44042d73,0x33031de5,0xaa0a4c5f,0xdd0d7cc9,
    0x5005713c,0x270241aa,0xbe0b1010,0xc90c2086,
    0x5768b525,0x206f85b3,0xb966d409,0xce61e49f,
    0x5edef90e,0x29d9c998,0xb0d09822,0xc7d7a8b4,
    0x59b33d17,0x2eb40d81,0xb7bd5c3b,0xc0ba6cad,
    0xedb88320,0x9abfb3b6,0x03b6e20c,0x74b1d29a,
    0xead54739,0x9dd277af,0x04db2615,0x73dc1683,
    0xe3630b12,0x94643b84,0x0d6d6a3e,0x7a6a5aa8,
    0xe40ecf0b,0x9309ff9d,0x0a00ae27,0x7d079eb1,
    0xf00f9344,0x8708a3d2,0x1e01f268,0x6906c2fe,
    0xf762575d,0x806567cb,0x196c3671,0x6e6b06e7,
    0xfed41b76,0x89d32be0,0x10da7a5a,0x67dd4acc,
    0xf9b9df6f,0x8ebeeff9,0x17b7be43,0x60b08ed5,
    0xd6d6a3e8,0xa1d1937e,0x38d8c2c4,0x4fdff252,
    0xd1bb67f1,0xa6bc5767,0x3fb506dd,0x48b2364b,
    0xd80d2bda,0xaf0a1b4c,0x36034af6,0x41047a60,
    0xdf60efc3,0xa867df55,0x316e8eef,0x4669be79,
    0xcb61b38c,0xbc66831a,0x256fd2a0,0x5268e236,
    0xcc0c7795,0xbb0b4703,0x220216b9,0x5505262f,
    0xc5ba3bbe,0xb2bd0b28,0x2bb45a92,0x5cb36a04,
    0xc2d7ffa7,0xb5d0cf31,0x2cd99e8b,0x5bdeae1d,
    0x9b64c2b0,0xec63f226,0x756aa39c,0x026d930a,
    0x9c0906a9,0xeb0e363f,0x72076785,0x05005713,
    0x95bf4a82,0xe2b87a14,0x7bb12bae,0x0cb61b38,
    0x92d28e9b,0xe5d5be0d,0x7cdcefb7,0x0bdbdf21,
    0x86d3d2d4,0xf1d4e242,0x68ddb3f8,0x1fda836e,
    0x81be16cd,0xf6b9265b,0x6fb077e1,0x18b74777,
    0x88085ae6,0xff0f6a70,0x66063bca,0x11010b5c,
    0x8f659eff,0xf862ae69,0x616bffd3,0x166ccf45,
    0xa00ae278,0xd70dd2ee,0x4e048354,0x3903b3c2,
    0xa7672661,0xd06016f7,0x4969474d,0x3e6e77db,
    0xaed16a4a,0xd9d65adc,0x40df0b66,0x37d83bf0,
    0xa9bcae53,0xdebb9ec5,0x47b2cf7f,0x30b5ffe9,
    0xbdbdf21c,0xcabac28a,0x53b39330,0x24b4a3a6,
    0xbad03605,0xcdd70693,0x54de5729,0x23d967bf,
    0xb3667a2e,0xc4614ab8,0x5d681b02,0x2a6f2b94,
    0xb40bbe37,0xc30c8ea1,0x5a05df1b,0x2d02ef8d,
};

/****************************************************************************
  函数名     :  void QuickCrc32(BYTE *buf, DWORD len, BYTE crcout[4])
  描述       :  查表方式计算CRC
                速度快，占用空间大
  输入参数   :  1、BYTE *buf : 要进行32位CRC计算的数
                2、ushort len : buf数组的长度
  输出参数   :  1、BYTE crcout[4] : 32位CRC计算的结果
  返回值     :  无BYTE *pbyDataIn, DWORD dwDataLen, BYTE abyCrcOut[2]
  修改历史   :
      1、修改人     修改时间    修改版本号   修改原因

****************************************************************************/
void QuickCrc32(BYTE *pbyDataIn, DWORD dwDataLen, BYTE abyCrcOut[4])
{
    DWORD dwCrc;
    DWORD i;
    dwCrc = 0xffffffff;
    for (i = 0; i < dwDataLen; i++)
    {
        dwCrc = gc_dwCrc32Table[((BYTE)dwCrc^pbyDataIn[i])&0xff] ^ (dwCrc>>8);
    }
    dwCrc = dwCrc^0xffffffff;
    abyCrcOut[0] = (BYTE )(dwCrc>>24);
    abyCrcOut[1] = (BYTE )(dwCrc>>16);
    abyCrcOut[2] = (BYTE )(dwCrc>>8);
    abyCrcOut[3] = (BYTE )(dwCrc);
}



/****************************************************************************
  函数名     :  int Crc32SendPacket(BYTE *pbySendData, WORD wDataLen, WORD wCmd)
  描述       :  数据包发送函数，带Crc32校验
  输入参数   :  1、BYTE *pbySendData：要发送的数据包
                2、WORD wDataLen：数据包长度
                3、WORD wCmd：命令码
  输出参数   :  无
  返回值     :  SUCCESS：发送成功 / SEND_PACKET_ERROR：发送错误 
                / PACKET_LEN_TOO_LONG
  修改历史   :  
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-08-30  V1.0         创建
****************************************************************************/
int Crc32SendPacket(BYTE *pbySendData, WORD wDataLen, BYTE byCmd)
{
    static BYTE abyBuff[4500];
    BYTE byRet;

    if (wDataLen > 4096)
    {
        return PACKET_LEN_TOO_LONG;
    }
    abyBuff[0] = 0x02;
    abyBuff[1] = byCmd;
    abyBuff[2] = wDataLen/256;
    abyBuff[3] = wDataLen%256;
    memcpy(&abyBuff[4], pbySendData, wDataLen);
    QuickCrc32(&abyBuff[1], wDataLen+3, &abyBuff[wDataLen+4]);
    byRet = SerWrite(abyBuff, wDataLen+8);
    if (0 != byRet)
    {
        return SEND_PACKET_ERROR;
    }
    return SUCCESS;
}

/****************************************************************************
  函数名     :  int Crc32RecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs)
  描述       :  TMS数据包接收函数，带Crc32校验
  输入参数   :  1、DWORD dwTimeoutMs：接收超时时间，时间单位是毫秒
  输出参数   :  1、BYTE *pbyRecvData：接收到的所有数据，包括包头和校验，传入
                   的pbyRecvData指向的内存不得小于256字节
                2、WORD *pwPacketetLen：接收到的所有数据的长度
  返回值     :  SUCCESS：接收成功 / CRC_CHECK_ERROR：数据包CRC校验错误
                / PACKET_LEN_ERROR：数据包长度错误 / RECV_TIMEOUT：超时 
                / RECV_PACKET_ERROR：接收错误 
  修改历史   :  
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-08-30  V1.0         创建
****************************************************************************/
int Crc32RecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs)
{
    BYTE byRet, abyCrc32[5];
    int i;
    WORD wPacketLen = 0;
    DWORD dwBegintick, dwCurtick;

    dwBegintick = GetTickCount();  // 时间单位是毫秒
    while (1)
    {
        dwCurtick = GetTickCount();
        if ((dwCurtick-dwBegintick) > dwTimeoutMs)
        {
            return RECV_TIMEOUT;
        }
        byRet = SerRead(pbyRecvData, 1, 3); // timeout = 3ms
        if ((0==byRet) && (0x02==pbyRecvData[0]))
        {
            break;
        }
        Sleep(1);
    }
    byRet = SerRead(&pbyRecvData[1], 3, 100); // timeout = 100ms
    if (0 != byRet)
    {
        return RECV_PACKET_ERROR;
    }
    wPacketLen = pbyRecvData[2]*256 + pbyRecvData[3];
    *pwPacketetLen = wPacketLen + 8;
    if (wPacketLen > RECV_PACKET_MAX_LEN)
    {
        return PACKET_LEN_ERROR;
    }
    for (i=0; i<wPacketLen+4; i++)
    {
        byRet = SerRead(&pbyRecvData[i+4], 1, 100);
        if (0 != byRet)
        {
            return RECV_PACKET_ERROR;
        }
    }
    QuickCrc32(&pbyRecvData[1], wPacketLen+3, abyCrc32);
    if (0 != memcmp(&pbyRecvData[wPacketLen+4],abyCrc32,4))
    {
        return LRC_CHECK_ERROR;
    }

    return SUCCESS;
}

int CmdSendPacket(BYTE *pbySendData, WORD wDataLen, BYTE byCmd)
{
    switch (g_iCommCheckType)
    {
    case LRC_CHECK_TYPE:
        return LrcSendPacket(pbySendData, wDataLen, byCmd);
    case CRC16_CHECK_TYPE:
        return Crc16SendPacket(pbySendData, wDataLen, byCmd);
    case CRC32_CHECK_TYPE:
        return Crc32SendPacket(pbySendData, wDataLen, byCmd);
    default:
        return PACKET_CHECK_TYPE_ERROR;
    }
}

int CmdRecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs)
{
    switch (g_iCommCheckType)
    {
    case LRC_CHECK_TYPE:
        return LrcRecvPacket(pbyRecvData, pwPacketetLen, dwTimeoutMs);
    case CRC16_CHECK_TYPE:
        return Crc16RecvPacket(pbyRecvData, pwPacketetLen, dwTimeoutMs);
    case CRC32_CHECK_TYPE:
        return Crc32RecvPacket(pbyRecvData, pwPacketetLen, dwTimeoutMs);
    default:
        return PACKET_CHECK_TYPE_ERROR;
    }
}
