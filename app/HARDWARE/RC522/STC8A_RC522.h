#include	"config.h"

sbit RC522_RESET = P2^1;        //RC522��λ�ܽţ�

#define RC522_NSS  SPI_SS_2     //���ʹӻ� SS �ܽ�




/////////////////////////////////////////////////////////////////////
//MF522������
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
#define PCD_CALCCRC           0x03               //CRC����

/////////////////////////////////////////////////////////////////////
//Mifare_One��Ƭ������
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
// #define PICC_DECREMENT        0xC0               //�ۿ�
// #define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����

/////////////////////////////////////////////////////////////////////
//MF522 FIFO���ȶ���
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

/////////////////////////////////////////////////////////////////////
//MF522�Ĵ�������
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01    // ������ֹͣ�����ִ��
#define     ComIEnReg             0x02    // �ж����󴫵ݵ�ʹ�ܺͽ��ܿ���λ
#define     DivlEnReg             0x03    // �ж����󴫵ݵ�ʹ�ܺͽ��ܿ���λ
#define     ComIrqReg             0x04    // �����ж������־
#define     DivIrqReg             0x05    // �����ж������־
#define     ErrorReg              0x06    // �����־��ָʾִ�е��ϸ�����Ĵ���״̬
#define     Status1Reg            0x07    // ����ͨ�ŵ�״̬��־
#define     Status2Reg            0x08    // �����������ͷ�������״̬��־
#define     FIFODataReg           0x09    // 64�ֽ�FIFO����������������
#define     FIFOLevelReg          0x0A    // ָʾFIFO�еĴ洢���ֽ���
#define     WaterLevelReg         0x0B    // ����FIFO��������籨����FIFO���
#define     ControlReg            0x0C    // ��ͬ�Ŀ��ƼĴ���
#define     BitFramingReg         0x0D    // ����λ��֡�ĵ���
#define     CollReg               0x0E    // RF�ӿ��ϼ�⵽�ĵ�һ��λ��ͻ��λ��λ��
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10      
#define     ModeReg               0x11      // ���巢�ͺͽ��յĳ���ģʽ
#define     TxModeReg             0x12      // ���巢�͹��̵����ݴ�������
#define     RxModeReg             0x13      // ������չ��̵����ݴ�������
#define     TxControlReg          0x14      // ���������������ܽ�TX1��TX2���߼�����
#define     TxAutoReg             0x15      // ��������������������
#define     TxSelReg              0x16      // ѡ���������������ڲ�Դ
#define     RxSelReg              0x17      // ѡ���ڲ��Ľ���������
#define     RxThresholdReg        0x18      // ѡ��λ�������ķ�ֵ
#define     DemodReg              0x19      // ��������������
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C      // ����ISO14443/MIFAREģʽ��106Kbit/s ��ͨ��
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F      // ѡ����UART�ӿڵ�����
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21      // ��ʾCRC�����ʵ��MSB��LSBֵ
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24      // ����ModWidth������
#define     RFU25                 0x25
#define     RFCfgReg              0x26      // ���ý���������
#define     GsNReg                0x27      // ѡ�������������ܽ�TX1��TX2�ĵ��Ƶ絼
#define     CWGsCfgReg            0x28      // ѡ�������������ܽ�TX1��TX2�ĵ��Ƶ絼
#define     ModGsCfgReg           0x29      // ѡ�������������ܽ�TX1��TX2�ĵ��Ƶ絼
#define     TModeReg              0x2A      // �����ڲ���ʱ��������
#define     TPrescalerReg         0x2B      // �����ڲ���ʱ��������
#define     TReloadRegH           0x2C      // ����16λ���Ķ�ʱ����װֵ
#define     TReloadRegL           0x2D      // ����16λ���Ķ�ʱ����װֵ
#define     TCounterValueRegH     0x2E      // ��ʾ16λ����ʵ�ʶ�ʱ��ֵ
#define     TCounterValueRegL     0x2F      // ��ʾ16λ����ʵ�ʶ�ʱ��ֵ
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31      // ���ò����źŵ�����
#define     TestSel2Reg           0x32      // ���ò����ºŵ����ú�PRBS����
#define     TestPinEnReg          0x33      // �����������ʹ�ܹܽţ�ע�������ڴ��нӿڣ�
#define     TestPinValueReg       0x34      // ����D1-D7����I/O����ʱ��ֵ
#define     TestBusReg            0x35      // ��ʾ�ڲ��������ߵ�״̬
#define     AutoTestReg           0x36      // ���������Բ���
#define     VersionReg            0x37      // ��ʾ�汾
#define     AnalogTestReg         0x38      // ���ƹܽ�AUX1��AUX2
#define     TestDAC1Reg           0x39      // ����TestDAC1�Ĳ���ֵ
#define     TestDAC2Reg           0x3A      // ����TestDAC2�Ĳ���ֵ  
#define     TestADCReg            0x3B      // ��ʾADC I��Q ͨ����ʵ��ֵ
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  0x3F

/////////////////////////////////////////////////////////////////////
//��MF522ͨѶʱ���صĴ������
/////////////////////////////////////////////////////////////////////
#define MI_OK                          0
#define MI_NOTAGERR                    1 //(-1)
#define MI_ERR                         2 //(-2)
//ͨѶ������󳤶�
#define MAXRLEN                       64  //ԭ 18

void RC522_Init(void);

void GetCard(u8 Reqcode, u8* pTagType, u8* pSnr);
char PcdRequest(unsigned char req_code,unsigned char *pTagType);
char PcdAnticoll(unsigned char *pSnr);
char PcdSelect(unsigned char *pSnr);
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);
char PcdRead(unsigned char addr,unsigned char *pData);
char PcdWrite(unsigned char addr,unsigned char *pData);
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue);
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr);
char PcdHalt(void);
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData);
char PcdReset(void);
unsigned char ReadRawRC(unsigned char Address);
void WriteRawRC(unsigned char Address, unsigned char value);
void SetBitMask(unsigned char reg,unsigned char mask);
void ClearBitMask(unsigned char reg,unsigned char mask);
char PcdComMF522(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit);
void PcdAntennaOn(void);
void PcdAntennaOff(void);
//CPU������
u16 CardReset(u8 * Data_Out,u8 *  Len);
char PcdRats(u8 * DataOut,u8 * Len);
u8 Pcd_Cmd(u8* pDataIn, u8  In_Len, u8* pDataOut,u8 * Out_Len);
void PcdSwitchPCB(void);


//void RC522_Config(unsigned char Card_Type);
//void RC522_SPI_Check(void); 





























