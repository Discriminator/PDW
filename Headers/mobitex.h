#ifndef MOBITEX_H
#define MOBITEX_H


class MOBITEX
{
	private: 
			int mb_nOnes(int h);
			int mb_nOnes_int(int h);
			int mb_bs(int st);
			unsigned int mb_crc(signed int bit);
			void barfrog(void);
			char *GetBaseID(char *szBaseID, char *szBaseIdName);
			void GetLinkControlInfo(void);
			void GetFrameHeader(void);
			bool GetMpakHeader();
			bool GetNeighbourChannels();
			bool GetSlaveChannels();

			int h3;  // matrix variables for block encoding
			int h2;
			int h1; 
			int h0;
			int live_frsync;
			int frsync_chk;
			int iUnfixedError;	// Holds unfixed FEC error.
			int blk_err[20];	// Holds block of FEX errors.
			int nBitCount;

			struct
			{
				unsigned int Minute;
				unsigned int Hour;
				unsigned int Day;
				unsigned int Month;
				unsigned int Year;
				unsigned int MD;
				unsigned int MT;
			} MOBITEX_TIME;

			struct
			{
				unsigned int BaseID;
				unsigned int AreaID;
				unsigned int CFlags;
				unsigned int Parity;
			} FRAME_HEADER;

			struct
			{
				char         BaseID[8];
				unsigned int FrameID;
				unsigned int BlockLength;
				unsigned int SeqNumber;
				unsigned int BytesLastBlock;
				unsigned int Destination;
			} LINK_CONTROL_INFO;

			struct
			{
				unsigned int Sender;
				unsigned int Destination;
				unsigned int MpakType;
				unsigned int MpakClass;
				unsigned int AddressList;
				unsigned int HPID;
				unsigned int Flags;
				unsigned int lTime;
				unsigned int TrafficState;
			} MPAK_HEADER;

			struct
			{
				unsigned int type;
				unsigned int fbi;
				unsigned int channels;
				bool channellist;
			} SWEEP;
	public:
			MOBITEX();
			~MOBITEX();           

			int  get_fec(int *goi);
			void frame_sync(int bit);
			void reset(void);

			int cfs;			// system specific frame sync checking status
			int frsync;			// system specific frame sync
			int frsync_rev;
			int bitsync;		// bit sync
			int bitsync_rev;	// reverse bit sync
			int bitscr;			// bit scrambling in use ?
			int ramnet;			// ramnet flag - 1 means it's ram
			int min_msg_len;	// Display message if > min_msg_len
			int show;			// Messages types to be shown
			int timer; 
			int bitrate;		// For reading baudrate from ini file
			int crc_check;
};

extern MOBITEX mb;


#endif
