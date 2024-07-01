#ifndef TRANSIT_DEF_H
#define TRANSIT_DEF_H
#include "mbedtl_def.h"
#include <windows.h>


#define MAX_PACKET_SIZE		(1024 + 512)
#define MUTEX_SIZE			30

#define TCP_CONN_PORT		7116
#define RTP_PORT_SR			7117
#define RTP_PORT_CL			7118

#define TCP_TRNS_PORT		7120

#define TIME_WAIT_CONN			180500
#define TIME_WAIT_RECEIVE		100
#define GPRS_TIME_WAIT			5000

#define QUANTITY_SATEL_IN_OUT		256
#define SATEL_WRK_BUFF_SIZE			256

#define SEND_DELAY		250		/** Send delay - in milliseconds */

#define GET_UNIT_TYPE	34
#define COMMAND 90
#define SN_0x18600001 408944641
//#define RECV_TIMEOUT	2300	/** Receive timeout - in milliseconds */


// typedef union {
//     WORD d16;
//     struct {
//         WORD Reserv		: 10;
//         WORD RunThread	: 1;
//         WORD ConnectUDP : 1;
//         WORD ConnectTCP		: 1;
//         WORD ChannType	: 2;
//         WORD SendData		: 1;
//     } w;
// } MemState_t;

// typedef struct {
//     DWORD host;
//     WORD port;
// } peer_t;

typedef union {
    uint8_t d8;
    struct {
        uint8_t UseFM : 1;
        uint8_t NmbBox : 2;
        uint8_t Reserve : 5;
    } b;
} ConfigMega_t;
/************ Box ***************/
typedef union {
    uint8_t d8;
    struct {
        uint8_t Inp1 : 1;
        uint8_t Inp2 : 1;
        uint8_t Inp3 : 1;
        uint8_t Inp4 : 1;
        uint8_t Inp5 : 1;
        uint8_t Inp6 : 1;
        uint8_t Reserve : 2;
    } b;
} ResDD_t; // входа на БК

typedef union {
    uint8_t d8;
    struct {
        uint8_t LowBatt : 1;
        uint8_t DoorOpen : 1;
        uint8_t Relay1 : 1; // увімкнення реле
        uint8_t Out1 : 1; // увімкнення реле
        uint8_t SupplyDet : 1; // наявністю зовнішнього живлення
        uint8_t TempError : 1; // якщо термометр відвалився
        uint8_t TestON : 1;
        uint8_t RS485Error : 1;
    } b;
} BoxStat_t;

typedef struct {
    uint8_t		BatVoltage;
    uint8_t		BatTemp;
    uint8_t		AmpStat; // стан підсилювача
    BoxStat_t	BoxStat;
} BoxState_t;

#define NMB_BOX				4

typedef struct {
    float Coeff_T;
    float Coeff_V;
    float Coeff_I;
} UPS_Coeff_t; // коефіціенти множення для температури, напруги та струму

typedef struct {
    ConfigMega_t ConfigMega;
    ResDD_t ResDD;
    BoxState_t BoxState[NMB_BOX];
    UPS_Coeff_t UPS_Coeff;
} BoxStatus_t; // статус ящика сирени


// typedef struct tagUnitMem {
//     int		iNmbUnit;
//     SOCKET	socketTCP;
//     //	SOCKET	socketUDP;
//     WORD sin_port;
//     DWORD 	dwSerNmb;
//     DWORD	dwLastTime;
//     peer_t peer;
//     //	SOCKADDR_IN src_sin;
//     //	SOCKADDR_IN dest_sin;
//     MemState_t State;
//     WORD BattVoltage;
//     HANDLE hTerminateTCP;
//     HANDLE hFinishTCP;
//     HANDLE hTerminateChann;
//     HANDLE hReceivePacket;
//     HANDLE	hMutexTCP;
//     //	HANDLE	hMutexUDP;
//     int		bytesSend;
//     BYTE	*TCP_RcvBuff;
//     //	BYTE	*TCP_TransBuff;
//     BYTE	*UDP_RcvBuff;
//     //	BYTE	*UDP_TransBuff;
//     BYTE	IN_Stat[QUANTITY_SATEL_IN_OUT / 8];
//     BYTE	OUT_Stat[QUANTITY_SATEL_IN_OUT / 8];
//     BYTE	IN_Error[QUANTITY_SATEL_IN_OUT / 8];
//     BYTE	OUT_Error[QUANTITY_SATEL_IN_OUT / 8];
//     BYTE	EventBuffer[SATEL_WRK_BUFF_SIZE / 8];
//     BoxStatus_t BoxStat;
//     BoxStatus_t OldBoxStat;
//     struct	tagUnitMem *pNext;    /* next node in the list */
//     struct	tagUnitMem *pPrev;    /* prev node in the list */
// } UnitMem_t; //продовжити досліджувати команду 90 == GET_BOX_PARAM (в VS знайти можна в TreadTrnsServer.cpp рядок 317)

// typedef struct {
//     DWORD 	dwSerNmb;
// } UnitRec_t;

// typedef struct {
//     BYTE	TransBuff[MAX_PACKET_SIZE];
//     UnitMem_t *UnitMem;
// } SendParam_t;

#define SEND_CMD_EVENT				83
#define GET_MEGA_CNF				84
#define SET_MEGA_CNF				85
#define GET_MEGA_INP				86
#define GET_MEGA_BOX_STATE			87
#define SET_MEGA_BOX				88
#define SET_MEGA_OUT_PCB			89
#define GET_BOX_PARAM				90

// #define EVENT_ON			0xf1
// #define EVENT_OFF			0xf2
// #define EVENT_STATE			0xf3

 #define CMD_MAX_DATA_SIZE		(256 * 3)

// #define UNIT_STATE_COMM		0
// #define UNIT_CONTROL_COMM	1
// #define UNIT_CONFIG_COMM	2
// typedef struct {
//     DWORD dwSerNmb;
//     DWORD dwCurrTime;
//     WORD wCommand;
//     WORD wSize;
//     BYTE Data[1024];
// } PacketToServ_t;

// typedef enum {
//     ERR_NO = 0x01,
//     ERR_CRC = 0x80,
//     ERR_NMB_CHANN = 0x81,
//     ERR_CHANN_BUSY = 0x82,
//     ERR_SIZE = 0x83,
//     ERR_COMMAND = 0x84,
//     ERR_NUMBER = 0x85,
//     ERR_NOT_REGISTERED = 0x86,
//     ERR_WRITE_FLASH = 0x87,
//     ERR_TIMEOUT_FLASH = 0x88,
//     ERR_NOT_CONNECT = 0x89,
//     ERR_TIMEOUT_MEGA = 0x8a,
//     ERR_TIMEOUT_DMR = 0x8b,
//     ERR_ENCRYPTION = 0x8c
// } ReturnError_t;

typedef struct {
    uint16_t Size;
    uint8_t Command;
    uint8_t	ReturnCode;
    uint8_t data[CMD_MAX_DATA_SIZE];
} RecvData_t;

typedef struct {
    uint16_t Size;
    uint8_t Command;
    uint8_t data[CMD_MAX_DATA_SIZE];
} TransData_t;

typedef struct {
    SOCKADDR_IN dest_sin;       // Destination socket address
    struct timeval tv;          // Time value for timeout
    DWORD SerNmb;               // Serial number
} ModuleParam_t;


// #define DEV_OK				0
// #define DEV_NOT_FOUND		-1
// #define DEV_NOT_CONNECT		-2
// #define DEV_TIMEOUT			-3
// #define DEV_BAD_PACKET		-4

// typedef struct {
//     DWORD 	dwSerNmb;
//     DWORD	dwCurrTime;
//     MemState_t State;
//     WORD BattVoltage;
//     BYTE RetCode;
// } Return_t;
// /********************** Typedef Unit *****************************/
// #define CHANN_ETHERNET	0
// #define CHANN_GPRS		1
// #define CHANN_433		2

// typedef union {
//     WORD d16;
//     struct {
//         WORD Reserv	: 14;
//         WORD ChannType: 2;
//     } w;
// } State_t;

// typedef struct {
//     DWORD SerNmb;
//     DWORD CurrTime;
//     State_t State;
//     WORD BattVoltage;
// } UnitState_t;

// typedef struct {
//     DWORD CurrTime;
//     WORD Command;
//     WORD Size;
//     BYTE Data[1024];
// } PacketToUnit_t;

typedef struct { 
    uint32_t Time;
    DWORD SerNmb;
    DWORD RandomNumber;
    uint8_t Payload[1024];
} GPRS_Send_t;

// typedef struct {
//     WORD Comm;
//     BYTE Payload[1024];
// } GPRS_PacketInOut_t;

// typedef struct {
//     DWORD SerNmb;
// } GPRS_PacketState_t;

// typedef union {
//     uint16_t d16;
//     struct {
//         uint16_t Reserve : 1;
//         uint16_t UseCrypto : 1;
//         uint16_t UseVoiceInp : 1;
//         uint16_t UseDTMF : 1;
//         uint16_t UseShortRTP : 1;
//         uint16_t UseModBusTCP : 1;
//         uint16_t UseOwnPCB : 1;
//         uint16_t UseMCP41 : 1;
//         uint16_t UseOwnRelay : 1;
//         uint16_t UseGPRS_Modem : 1;
//         uint16_t UseMegaInOut : 1;
//         uint16_t UseLineInp : 1;
//         uint16_t UseUPS : 1;
//         uint16_t UseFM : 1;
//         uint16_t UsePLC : 1;
//         uint16_t Use433 : 1;
//     } w;
// } ChannelsEx_t;
// typedef union {
//     uint8_t d8;
//     struct {
//         uint8_t Reserv : 7;
//         uint8_t GeteWayMode : 1;
//     } b;
// } SX1278_Mode_t;

// #define CONFIRMATION_PASSW_SIZE			9

// typedef struct {
//     uint32_t Frequency;
//     uint16_t ScaningCycle;		/* In second */
//     uint16_t WaitAnswer;		/* In second */
//     uint16_t NmbLoraUnit;
//     CryptoSet_t CryptoSet;
//     SX1278_Mode_t Mode;
//     uint8_t Power;
//     uint8_t SF;
//     uint8_t BW;
//     uint8_t FEC;
// } SX1278_Rec_t;

// typedef struct {
//     uint32_t ipaddr;
//     uint16_t Port;
//     uint16_t StatePort;
//     CryptoSet_t CryptoSet;
// } GPRS_Modem_Rec_t;

// typedef struct {
//     UPS_Coeff_t UPS_Coeff;
//     uint16_t Freq100hz;
//     ChannelsEx_t Channels;
//     uint16_t PasswordMenu;
//     uint8_t FmVolume;
//     uint8_t NmbLcdTextRec;
//     SX1278_Rec_t SX1278;
//     GPRS_Modem_Rec_t GPRS_Modem;
//     uint8_t ConfirmPass[CONFIRMATION_PASSW_SIZE];
//     uint8_t LinInpVolume;
//     uint8_t SpVolume;
//     uint8_t NmbModBusUnit;
//     uint8_t NmbRadioUnit;
//     uint16_t TimeOfConfirmation;
//     uint8_t ErrorBitOffs;
//     uint8_t LinOutVolume;
//     uint16_t wCRC;
// } UnitSetingEx_t;
// /*****************************************************************/
typedef BYTE (*PRGETUSER)(const char *username, BYTE *auth_algo, BYTE *auth_key, BYTE *priv_algo,
                          BYTE *priv_key, CryptoSet_t *CryptoSet);
typedef BYTE (*PRDECRIPTIONPROC)(Packet_t *Packet, uint8_t auth_algo, uint8_t *auth_key, uint8_t priv_algo,
                                 uint8_t *priv_key, CryptoSet_t *CryptoSet);
typedef BYTE (*PRENCRIPTIONPROC)(Packet_t *Packet, uint8_t auth_algo, uint8_t *auth_key, uint8_t priv_algo,
                                 uint8_t *priv_key, CryptoSet_t *CryptoSet);
// UnitMem_t *RemoveFromUnitList(UnitMem_t *pNode);
// UnitMem_t *UnitAddNewNode(UnitRec_t *TagFileRecord);
// UnitMem_t *FindeUnitNode(DWORD 	dwSerNmb);
// DWORD WINAPI ThreadUDPServer(int iNmbPort);
// DWORD WINAPI ThreadMainServer(int iNmbPort);
// DWORD WINAPI ThreadTransServer(int iNmbPort);
// DWORD WINAPI ThreadUnitTCPPulling(void *par);
// DWORD WINAPI ThreadSendTCPPacket(SendParam_t *pSendUnit);
// DWORD WINAPI ThreadSendUDPPacket(void *par);
#endif // TRANSIT_DEF_H
