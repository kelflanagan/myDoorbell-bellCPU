// misc defines
#define MEM_TIMEOUT 100  // this results in N ms delay

// return values
#define MEM_OK              0x00  // all is well
#define MEM_CANT_MASTER     0x01  // device can't gain master control of MEM
#define MEM_CANT_RELINQUISH 0x02 // cant relinquish control of memory
#define MEM_UNKNOWN_SIZE    0x03  // unknown memory size

// OP codes and commands
#define READ_STATUS_REGISTER        0xd7
#define CONTINUOUS_ARRAY_READ_SLOW  0x03

#define MEM_HEADER_SIZE 6
// define memory header types
#define MEM_NOP            0x00
#define MEM_FRONT_RINGTONE 0x01
#define MEM_REAR_RINGTONE  0x02
#define MEM_FRONT_VOLUME   0x03
#define MEM_REAR_VOLUME    0x04
#define MEM_FRONT_RING     0x05
#define MEM_REAR_RING      0x06
#define MEM_LOST_ASSOC     0x07

struct MEM_header_struct
{
  unsigned char type;
  unsigned char bell;
  unsigned long val;
} MEM_header;


struct MEM_struct
{
  unsigned int page_size;
  unsigned long mem_size;
  unsigned long fbell_offset;
  unsigned long rbell_offset;
} MEM;

extern unsigned char mem_bus_granted;

void printHeader(void);
char MEM_init(void);
char isMEMRdy(void);
void actOnHeader(void);
char MEM_acquireControl(void);
void MEM_relinquishControl(void);
char MEM_readBlockSPI(unsigned long, unsigned long, unsigned char *);
