#pragma once

int main(int argc, char* argv[]);

class CHex2Bin
{
	enum  {
		ENDOFFILE = -1,
		NUMBER = 256
	};
	char* m_pInFile;
	char* m_pOutFile;
	FILE* m_pOut;
	char* m_pInFileBuffer;
	int m_BufferSize;
	int m_InFileSize;
	int m_Index;
	char m_aLexBuff[32];
	int m_LexBuffIndex;
	int m_DigitCount;
	unsigned m_LexValue;
	int m_BytesSaved;
public:
	CHex2Bin();
	virtual ~CHex2Bin();
	bool Create(int argc, char* argv[]);
	void SaveData();
	bool Run();
	int Lex();
	int LexGet();
	bool IsValidHexNumber(int c);
};


