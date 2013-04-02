#ifndef ACARS_H
#define ACARS_H


extern TCHAR  szACARSPath[MAX_PATH];

 // Structure for managing acars database infornmation.
 
struct dbase_info {
	char label[8];
	char desc[60];          // Label Description
	struct dbase_info *next;
};


class ACARS
{
	private:
		unsigned char ac_sr0, ac_sr1;
		int ac_blk;
		int ac_bits;
		int index;
		int nch;
		bool ac_synced;
		bool ac_inverted;
		char mode[2];
		char address[8];
		char ack;
		char label[3];
		char dbi;
		char stx;
		char msg_nr[5];
		char flight_nr[7];
		char text[221];
//		char etx_etb;

		int mode_parity_error;
		int label_parity_errors;
		int parity_errors;
           
		char dbuf[300]; // debug buffer
		struct dbase_info *db_label;
		struct dbase_info *db_aircraft_type;
		struct dbase_info *db_origin;
		struct dbase_info *db_airline;
		struct dbase_info *db_gnd_station;
		struct dbase_info *db_routes;
           
	public:
		ACARS();
		~ACARS();

		void reset(void);           
		void frame(int bit);
		void ac_out_msg(void);
		void out_label(void);           
		void out_address(void);
		void out_flight_nr(void);
		void out_ground_station(void);
		void out_dir_ack_nak(void);
		void out_message_txt(void);
		int parity_check(char c);

		void read_data(void);
		void free_data(void);
		struct dbase_info *read_db_file(int max_label_size,char *fname);
		void free_db_info(struct dbase_info *p_info);

		bool ac_alive;
};

extern ACARS acars;


#endif
