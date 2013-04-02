#ifndef PDW_ERMES_H
#define PDW_ERMES_H

class ERMES
{
	private:
		char db_buf[80];         // general purpose buffer, also used for debug purposes.
		int db_index;            // counter for above buffer.

		char block[300];
		long int local_addr;     // local pager address.
		short int pcat;          // paging category.
		short int aii;           // Additional information indicator.
		int batch_len;           // number of codewords per batch.
		short int syndlup[4096]; // syndrome lookup table, gives location of bat bits.
		short int fin[300];      // holds bits for de-interleaving.
		int shown;               // indicates we have shown address/message
		int done;                // indicates endof message.
		int err_cw;              // indicates if current codeword has errors.
		int header_pos;          // Bit position in message header.

		long int reverse_cw(long int cw, int nbits);
//		int nOnes(int k);
		int lOnes(long int k);
		long int checkecc(long int col);
		void showme(long int l, int c);
		void docw(long int col, int blkc);
		void rawbit(int c);
		void setupecc(void);

	public:
		ERMES(void);
		~ERMES(void);
		void frame(int gin);

		int timer;
};

extern ERMES em;

#endif
