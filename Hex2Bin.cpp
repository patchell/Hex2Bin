//------------------------------------------------
// Convert an Action! Code Blocks to a binary 
// file.
//------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys\stat.h>
#include <sys\types.h>
#include "Hex2Bin.h"

int main(int argc, char* argv[])
{
	CHex2Bin Hex2Bin;
	int rV = 1;

	fprintf(stderr, "Ascii Hex to Binary Converter\n");
	fprintf(stderr, "Ver 0.1.1  Dec 6, 2024\n");

	if (argc == 3)
	{
		Hex2Bin.Create(argc, argv);
		if (Hex2Bin.Run())
			rV = 0;
	}
	else
	{
		fprintf(stderr, "Usage: hex2bin <in file> <out file>\n");
	}
	return rV;
}

CHex2Bin::CHex2Bin()
{
	m_pInFile = 0;
	m_pOutFile = 0;
	m_pOut = 0;
	m_pInFileBuffer = 0;
	m_BufferSize = 0;
	m_InFileSize = 0;
	m_LexBuffIndex = 0;
	for(int i = 0; i < 32;++i)
		m_aLexBuff[i] = 0;
	m_DigitCount = 0;
	m_Index = 0;
	m_LexValue = 0;
	m_BytesSaved = 0;
}

CHex2Bin::~CHex2Bin()
{
	if (m_pInFileBuffer) delete[] m_pInFileBuffer;
}

bool CHex2Bin::Create(int argc, char* argv[])
{
	bool rV = true;
	FILE* pIn;
	struct _stat32 FileStats;
	int BytesRead = 0;

	m_pInFile = argv[1];
	m_pOutFile = argv[2];

	//---------------------------------
	// Open Input File
	//---------------------------------
	_stat32(m_pInFile, &FileStats);
	m_BufferSize = FileStats.st_size;
	fopen_s(&pIn, m_pInFile, "r");
	m_pInFileBuffer = new char[m_BufferSize + 1];
	if (m_pInFileBuffer && pIn)
		BytesRead = fread(m_pInFileBuffer, 1, m_BufferSize, pIn);
	if(pIn) fclose(pIn);
	if (BytesRead)
	{
		m_InFileSize = BytesRead;
		fprintf(stderr, "%d bytes read from %s\n", m_InFileSize, m_pInFile);
	}
	else
		rV = false;

	if (rV)
	{
		fopen_s(&m_pOut, argv[2], "wb");
		if (m_pOut == 0)
		{
			fprintf(stderr, "Unable to open %s for output\n", m_pOutFile);
			rV = false;
		}
	}
	return rV;
}

void CHex2Bin::SaveData()
{
	unsigned Mask = 0;
	unsigned v;
	int i;

	switch (m_DigitCount)
	{
	case 8:
	case 7:
		Mask = 0xFF;
		for (i = 0; i < 4; ++i)
		{
			v = (m_LexValue & Mask) >>( i * 8);
			fputc(v, m_pOut);
			m_BytesSaved++;
			Mask <<= 8;
		}
		break;
	case 6:
	case 5:
		Mask = 0xFF;
		for (i = 0; i < 3; ++i)
		{
			v = (m_LexValue & Mask) >> (i * 8);
			fputc(v, m_pOut);
			m_BytesSaved++;
			Mask <<= 8;
		}
		break;
	case 4:
	case 3:
		Mask = 0xFF;
		for (i = 0; i < 2; ++i)
		{
			v = (m_LexValue & Mask) >> (i * 8);
			fputc(v, m_pOut);
			m_BytesSaved++;
			Mask <<= 8;
		}
		break;
	case 2:
	case 1:
		Mask = 0xFF;
		v = (m_LexValue & Mask);
		fputc(v, m_pOut);
		m_BytesSaved++;
		break;
	default:
		fprintf(stderr, "Too many characters :%d\n", m_DigitCount);
		break;
	}
}

bool CHex2Bin::Run()
{
	bool Loop = true;
	int v;
	long data = 0;

	while (Loop)
	{
		v = Lex();
		switch (v)
		{
		case ENDOFFILE:
			if (m_pOut) fclose(m_pOut);
			Loop = false;
			break;
		case NUMBER:
			SaveData();
			break;
		}
	}
	fprintf(stderr, "Bytes Saved: %d to %s\n", m_BytesSaved, m_pOutFile);
	return false;
}

int CHex2Bin::Lex()
{
	//------------------------------------
	//------------------------------------
	int c;
	bool Loop = true;
	bool LoopIn = true;
	int rV = 0;

	while (Loop)
	{
		c = LexGet();
		switch (c)
		{
		case EOF:
			Loop = false;
			rV = ENDOFFILE;
			break;
		case '[':
			break;
		case ']':
			break;
		case '\n':
			break;
		case ' ':
			break;
		case '$':
			LoopIn = true;
			m_LexBuffIndex = 0;
			while (LoopIn)
			{
				c = LexGet();
				if (IsValidHexNumber(c))
				{
					m_aLexBuff[m_LexBuffIndex++] = c;
				}
				else
				{
					m_Index--;	//unget
					LoopIn = false;
					m_aLexBuff[m_LexBuffIndex] = 0;
					m_DigitCount = m_LexBuffIndex;
					m_LexValue = strtol(m_aLexBuff, NULL, 16);
					rV = NUMBER;
				}
			}
			Loop = false;
//			printf("%2d$%s\n", m_LexBuffIndex,  m_aLexBuff);
			break;
		}
	}
	return rV;
}

int CHex2Bin::LexGet()
{
	int c = EOF;

	if (m_pInFileBuffer && (m_Index < m_InFileSize))
		c = m_pInFileBuffer[m_Index++];
	else if (m_Index == m_InFileSize)
	{
		c = EOF;
	}
	return c;
}

bool CHex2Bin::IsValidHexNumber(int c)
{
	bool IsValid = false;

	switch (c)
	{
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		IsValid = true;
		break;
	}
	return IsValid;
}
