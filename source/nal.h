#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

/* define constants */
#define _MAX_FILENAME_LENGTH 255
#define _MAX_TOKEN_SIZE 500
#define Null 0
#define _APPEND "a"
#define _NEW "w"

typedef enum{
	False, True, Abort
} Bool;

/* define enum */
typedef enum{
	Normal,
	PreserveChar,
	PreservePair,
	Space,
	CR
} ChracterType;

typedef enum{
	NAL_UNKNOW,
	NAL_Equal,			/* = */
	NAL_CR,				/* CRLF */
	NAL_L_Braces,		/* { */
	NAL_R_Braces,		/* } */
	NAL_L_Parentheses,	/* ( */
	NAL_R_Parentheses,	/* ) */
	NAL_Comma,			/* , */
	NAL_FILE,			/* FILE */
	NAL_PRINT,			/* PRINT */
	NAL_PRINTN,			/* PRINTN */
	NAL_STRING,			/* "string" or string */
	NAL_STRING_VARIBLE,	/* $A */
	NAL_NUMBER,			/* 123 */
	NAL_NUMBER_VARIBLE,	/* %A */
	NAL_ROT18,			/* #abcd# */
	NAL_RND,			/* RND */
	NAL_INC,			/* INC */
	NAL_INNUM,			/* INNUM */
	NAL_IN2STR,			/* IN2STR */
	NAL_IFGREATER,		/* IFGREATER */
	NAL_IFEQUAL,		/* IFEQUAL */
	NAL_INSTRS,			/* INSTRS */
	NAL_JUMP,			/* JUMP */
	NAL_ABORT			/* ABORT */
} NAL_Symbol;

/* common function */
void PrintError(FILE *file, const char *strMsg);
void Dump(FILE *file);
Bool FileExist(const char *strFilename);
Bool IsNumber(const char *strString);
Bool DecodeROT18(const char *strSource, char *strResult);
ChracterType GetChType(char ch);
NAL_Symbol IdentifyNalKeywordType(const char *strToken);
NAL_Symbol NextToken(FILE *file, char *strToken);
void PrintToken(char *strToken, const char *strMode);
Bool NormalizeString(char *strLine, char chHeadTail);
char *NormalizeValue(char *strValue, int iDecimalPlace);

/* declare NAL function */
Bool P_NAL(char *strFilename);
Bool P_Block(FILE *file, Bool bSkipBlock);
Bool P_FILE(FILE *file);
Bool P_PRINT(FILE *file, Bool bPrintNewLine);
Bool P_AssignValue2Varible(FILE *file, const char *strVaribleName, NAL_Symbol nsExpecedtValueType);
Bool P_GetVaribleValue(FILE *file, const char *strVaribleName, char *strValue, NAL_Symbol *nsValueType);
Bool P_Equal(FILE *file, char *strValue, NAL_Symbol *nsValueType);
Bool P_JUMP(FILE *file);
Bool P_INC(FILE *file);
Bool P_RND(FILE *file);
Bool P_INNUM(FILE *file);
Bool P_IN2STR(FILE *file);
Bool P_IFCOND(FILE *file, NAL_Symbol nsIFCOND);
