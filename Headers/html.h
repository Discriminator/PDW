//---------------------------------------------------------------------------------
//
//						html_file header file
//						(c) Andreas Verhoeven 2003
//						versie 1 (13 April 2003 / 19:31)				
//
//---------------------------------------------------------------------------------

#ifndef INCLUDE_PROTECTION_HTML_FILE_HPP_29021901
#define INCLUDE_PROTECTION_HTML_FILE_HPP_29021901

#pragma warning(disable : 4786) 
#include <vector>
#pragma warning(default : 4786) 

#include <string>

class html
{
	private:
	
	#ifdef PDW_HTML_SET_COMPILE
	public:
	#endif

	std::vector<std::string>	Lines;

	#ifdef PDW_HTML_SET_COMPILE
	private:
	#endif
		unsigned long	MaxLines;
		std::string		CurrentLine;
		std::string		FileName;

		std::string		HTMLHead;
		std::string		HTMLStart;
		std::string		HTMLEnd;
		std::string		HTMLStartLine;
		std::string		HTMLEndLine;

		bool			IsActive;

		void ResizeBuffer(unsigned long NewSize);
		
	public:
		html(){};
		~html(){};

		bool Load(std::string Buffer);
		bool Save(std::string Buffer);
		void Clear();
		bool UpdateFile();
		void Statistics(PPROFILE pProfile) ;


		//set, get functions
		std::string	GetFileName(){return(FileName);};
		void SetFileName(std::string val){FileName=val;};
		unsigned long GetMaxLines(){return(MaxLines);};
		void SetMaxLines(unsigned long val){ResizeBuffer(val);MaxLines=val;};

		std::string GetHTMLHead(){return(HTMLHead);};
		void SetHTMLHead(std::string val){HTMLHead=val;};
		
		std::string GetHTMLStart(){return(HTMLStart);};
		void SetHTMLStart(std::string val){HTMLStart=val;};
		std::string GetHTMLEnd(){return(HTMLEnd);};
		void SetHTMLEnd(std::string val){HTMLEnd=val;};

		std::string GetHTMLStartLine(){return(HTMLStartLine);};
		void SetHTMLStartLine(std::string val){HTMLStartLine=val;};
		std::string GetHTMLEndLine(){return(HTMLEndLine);};
		void SetHTMLEndLine(std::string val){HTMLEndLine=val;};

		bool GetActive(){return(IsActive);};
		void SetActive(bool val){IsActive=val;};

		html& AddText(std::string val);
		html& EndLine();
};


#endif
//INCLUDE_PROTECTION_HTML_FILE_HPP_29021901