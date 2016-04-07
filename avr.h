#define SetPin(Port, Bit) (Port |= (1 << Bit))
#define ClearPin(Port, Bit) (Port &= ~(1 << Bit))
#define ReadPin(Pin, Bit) ((Pin >> Bit) & 0x1)

#define everyMinutes(t) ((avr_millis % (t*60000)) == 0)
#define everySeconds(t) ((avr_millis % (t*1000)) == 0)

#define MEM_GRANT PB0
#define MEM_SEL PB1
#define SS PB2
#define BELL_MOSI PB3
#define BELL_MISO PB4
#define BELL_SCK PB5

#define MEM_SB PC0
#define MEM_RDY PC1
#define MUTE PC2
#define VS_RESET PC3
#define VS_DREQ PC4
#define MEM_RESET PC5

#define VS_XDCS PD0
#define VS_XCS PD1
#define FBELL_BUTTON PD2
#define RBELL_BUTTON PD3
#define LED PD4
#define FBELL_EVENT PD5
#define RBELL_EVENT PD6
#define MEM_REQ PD7

#define FBELL  1
#define RBELL  2
#define CPU_FREQ 8000000    //  8 MHz

#define DEBOUNCE_TIME 10  //  ms for switch to stabalize
#define RELEASED 0
#define PRESSED 1

// eeprom offsets
#define FBELL_VOLUME_OFFSET 0
#define FBELL_LENGTH_OFFSET 1
#define RBELL_VOLUME_OFFSET 6
#define RBELL_LENGTH_OFFSET 7

void AVR_init_mcu(void);

