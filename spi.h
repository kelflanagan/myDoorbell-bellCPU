#define MEMCS 1
#define VSXCS 2
#define VSXDCS 3

void SPI_init(int);
void SPI_deselect(void);
void SPI_select(int);
int SPI_adjustSpeed(int);
unsigned char SPI_send(unsigned char);
unsigned char SPI_receive(unsigned char);
