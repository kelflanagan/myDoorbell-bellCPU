// Definititons for use with VS1011e mp3 decoder 

// SCI Instructions
#define  SCI_WRITE  0x02
#define  SCI_READ   0x03

// SCI Registers
#define  MODE_REG        0x0
#define  STATUS_REG      0x1
#define  BASS_REG        0x2
#define  CLOCKF_REG      0x3
#define  DECODE_TIME_REG 0x4
#define  AUDATA_REG      0x5
#define  VOL_REG         0xb

// Bit positions within Mode register
#define  SM_RESET  2
#define  SM_TESTS  5
#define  SM_SDINEW 11

// Bit positions in Status register
#define  SS_AVOL0    0
#define  SS_AVOL1    1
#define  SS_APDOWN1  2
#define  SS_APDOWN2  3
#define  SS_VER0     4
#define  SS_VER1     5
#define  SS_VER2     6

// Bit positions in Bass register
#define  SB_FREQLIMIT0  0
#define  SB_FREQLIMIT1  1
#define  SB_FREQLIMIT2  2
#define  SB_FREQLIMIT3  3
#define  SB_AMPLITUDE0  4
#define  SB_AMPLITUDE1  5
#define  SB_AMPLITUDE2  6
#define  SB_AMPLITUDE3  7
#define  ST_FREQLIMIT0  8
#define  ST_FREQLIMIT1  9
#define  ST_FREQLIMIT2  10
#define  ST_FREQLIMIT3  11
#define  ST_AMPLITUDE0  12
#define  ST_AMPLITUDE1  13
#define  ST_AMPLITUDE2  14
#define  ST_AMPLITUDE3  15

char VS_init(void);
void VS_remotePlayMEM(int, unsigned long);
void VS_playFileMEM(int);
char VS_writeData(unsigned char *);
unsigned int VS_readRegister(unsigned char);
char VS_writeRegister(unsigned char, unsigned int);
void VS_sineTest(unsigned char, int);
char VS_isReady(void);



