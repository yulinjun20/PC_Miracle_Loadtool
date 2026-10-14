
#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_


#include <initguid.h>



#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif



#define LRC_CHECK_TYPE     0
#define CRC16_CHECK_TYPE   1
#define CRC32_CHECK_TYPE   2

#define FLASH_BLOCK 4096

#define RECV_PACKET_MAX_LEN 5050

#define SUCCESS              0

#define RECV_PACKET_ERROR       101
#define SEND_PACKET_ERROR       102
#define PACKET_LEN_ERROR        103
#define RECV_TIMEOUT            104
#define PACKET_LEN_TOO_LONG     107
#define PACKET_CHECK_TYPE_ERROR 108


#define CRC_CHECK_ERROR             120
#define LRC_CHECK_ERROR             121



extern HANDLE GetSerialHandle(void);
//extern int GetCurPortNo();
extern BYTE SerOpen(int iPortNo, DWORD BaudRate);
extern void SerClose(void);
extern BYTE SerWrite(BYTE *pszBuf , WORD SendCnt);
extern BYTE SerRead(BYTE *pszBuf , WORD RecvCnt , DWORD TimeOut);

extern void PurgeCommBuf(DWORD mode);

extern int CmdSendPacket(BYTE *pbySendData, WORD wDataLen, BYTE byCmd);
extern int CmdRecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs);

extern int LrcSendPacket(BYTE *pbySendData, WORD wDataLen, BYTE byCmd);
extern int LrcRecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs);
extern int Crc16SendPacket(BYTE *pbySendData, WORD wDataLen, WORD wCmd);
extern int Crc16RecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs);
extern int Crc32SendPacket(BYTE *pbySendData, WORD wDataLen, BYTE byCmd);
extern int Crc32RecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs);







typedef struct _USB_DEVICE_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT bcdUSB;
    UCHAR bDeviceClass;
    UCHAR bDeviceSubClass;
    UCHAR bDeviceProtocol;
    UCHAR bMaxPacketSize0;
    USHORT idVendor;
    USHORT idProduct;
    USHORT bcdDevice;
    UCHAR iManufacturer;
    UCHAR iProduct;
    UCHAR iSerialNumber;
    UCHAR bNumConfigurations;
} USB_DEVICE_DESCRIPTOR, *PUSB_DEVICE_DESCRIPTOR;

BOOL OpenUsbDevice();

void CloseUsbDevice();

BOOL SetupUsbPara();

void ResetUsbDevice(void);

BOOL WriteUsbDevice(BYTE * lpByte, DWORD dwBytesToWrite);

DWORD ReadUsbDevice(BYTE * lpByte, DWORD dwLength, DWORD timeout);



BYTE LocalWrite(BYTE *pbySendData, WORD wDataLen);
BYTE LocalRead(BYTE *lpByte , WORD wLength , DWORD dwTimeoutMs);


#endif