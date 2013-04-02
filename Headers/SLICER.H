
typedef struct {
   unsigned int  irq;        // IRQ number for COM port
   unsigned int  com_port;   // I/O address for COM port
} SLICER_IN_STR;

typedef struct {
   unsigned short *freqdata; // each freqdata entry is 16 bits
   unsigned char *linedata;  // each linedata entry is 8 bits
   unsigned long *cpstn;     // current offset into freqdata and linedata
   long bufsize;             // size of freqdata and linedata buffers
} SLICER_OUT_STR;

