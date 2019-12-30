#include "nal.h"
#include "mvm.h"

/* declare NAL delimiters */
char g_cPreserveChar[] = { ',', '{', '}', '=', '+', '-', '*', '/', '(', ')' };
char g_cPreservePair[] = { '"', '#' };
char g_cSpace[] = { ' ', '\t' };
char g_cCR[] = { '\n' };

/* declare ROT */
char g_strPlain[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
char g_strROT18[] = "NOPQRSTUVWXYZABCDEFGHIJKLMnopqrstuvwxyzabcdefghijklm5678901234";

/* declare mvm */
mvm *g_NalVaribles;

/* declare print message buffer */
char g_strMsgBuffer[_MAX_TOKEN_SIZE];

/* check file is exist or not */
Bool FileExist(const char *strFilename){
	FILE *file;
	if ((file = fopen(strFilename, "r"))) {
		fclose(file);
		return True;
	}
	return False;
}

void NalPrint(char *strString, Bool bNewLine){
	char *p;

	p = strString;
	for (; *p; ++p) {
		if (*p == '\\'){
			++p;
			switch (*p){
				case '0':
					putchar('\0');
					break;
				case 'n':
					putchar('\n');
					break;
				case 'r':
					putchar('\r');
					break;
				case 't':
					putchar('\t');
					break;
				case 'b':
					putchar('\b');
					break;
				case 'a':
					putchar('\a');
					break;
				case '\\':
					putchar('\\');
					break;
				case '\'':
					putchar('\'');
					break;
				case '\"':
					putchar('\"');
					break;
				default:
					putchar(*p);
			}
		}
		else
			putchar(*p);
	}

	if (bNewLine)
		putchar('\n');
}

void PrintError(FILE *file, const char *strMsg){
	printf("\n%s\n", strMsg);

	if (file != Null)
		return;
}

int DecimalPoint(char *strValue){
	int i, iCount;

	for (i = strlen(strValue) - 1, iCount = 0; i >= 0; i--, ++iCount){
		if (strValue[i] == '.')
			return iCount;
	}

	return 0;
}

int GetRandom(){
	int min = 0;
	int max = 99;

	return(rand() % (max - min + 1) + min);
}

void Dump(FILE *file){
	char strBuf[_MAX_TOKEN_SIZE];
	int i;
	char ch;

	if (file != Null){
		i = 0;
		while (fseek(file, -1, SEEK_CUR) == 0){
			ch = fgetc(file);
			fseek(file, -1, SEEK_CUR);
			if (ch != '\n'){
				strBuf[i++] = ch;
			}
			else{
				break;
			}
		}

		printf("\nDump last inteprted statement :\n");
		while (--i >= 0){
			printf("%c", strBuf[i]);
		}
		printf("\n");
	}
}

Bool IsNumber(const char *strString){
	unsigned i;
	char ch;

	if (strlen(strString) <= 0)
		return False;

	for (i = 0; i < strlen(strString); i++){
		ch = strString[i];

		if (!((ch >= '0' && ch <= '9') || (ch == '.')))
			return False;
	}

	return True;
}

Bool DecodeROT18(const char *strSource, char *strResult){
	unsigned i, j;
	char ch;
	Bool bMatch;

	for (i = 0; i < strlen(strSource); i++){
		ch = strSource[i];
		bMatch = False;
		for (j = 0; j < strlen(g_strROT18); j++){
			if (ch == g_strROT18[j]){
				strResult[i] = g_strPlain[j];
				bMatch = True;
				break;
			}
		}

		if (bMatch == False){
			strResult[i] = ch;
		}
	}
	strResult[strlen(strSource)] = Null;

	return True;
}

ChracterType GetChType(char ch){
	unsigned i;

	for (i = 0; i < sizeof(g_cPreserveChar) / sizeof(char); i++){
		if (ch == g_cPreserveChar[i])
			return PreserveChar;
	}

	for (i = 0; i < sizeof(g_cPreservePair) / sizeof(char); i++){
		if (ch == g_cPreservePair[i])
			return PreservePair;
	}

	for (i = 0; i < sizeof(g_cSpace) / sizeof(char); i++){
		if (ch == g_cSpace[i])
			return Space;
	}

	for (i = 0; i < sizeof(g_cCR) / sizeof(char); i++){
		if (ch == g_cCR[i])
			return CR;
	}

	return Normal;
}

NAL_Symbol IdentifyNalKeywordType(const char *strToken){
	if (strcmp(strToken, ",") == 0) return NAL_Comma;
	if (strcmp(strToken, "=") == 0) return NAL_Equal;
	if (strcmp(strToken, "\n") == 0) return NAL_CR;
	if (strcmp(strToken, "{") == 0) return NAL_L_Braces;
	if (strcmp(strToken, "}") == 0) return NAL_R_Braces;
	if (strcmp(strToken, "(") == 0) return NAL_L_Parentheses;
	if (strcmp(strToken, ")") == 0) return NAL_R_Parentheses;
	if (strcmp(strToken, "FILE") == 0) return NAL_FILE;
	if (strcmp(strToken, "PRINT") == 0) return NAL_PRINT;
	if (strcmp(strToken, "PRINTN") == 0) return NAL_PRINTN;
	if (strToken[0] == '"' && strToken[strlen(strToken) - 1] == '"') return NAL_STRING;
	if (strToken[0] == '$') return NAL_STRING_VARIBLE;
	if (IsNumber(strToken) == True) return NAL_NUMBER;
	if (strToken[0] == '%') return NAL_NUMBER_VARIBLE;
	if (strToken[0] == '#' && strToken[strlen(strToken) - 1] == '#') return NAL_ROT18;
	if (strcmp(strToken, "RND") == 0) return NAL_RND;
	if (strcmp(strToken, "INC") == 0) return NAL_INC;
	if (strcmp(strToken, "INNUM") == 0) return NAL_INNUM;
	if (strcmp(strToken, "IN2STR") == 0) return NAL_IN2STR;
	if (strcmp(strToken, "IFGREATER") == 0) return NAL_IFGREATER;
	if (strcmp(strToken, "IFEQUAL") == 0) return NAL_IFEQUAL;
	if (strcmp(strToken, "INSTRS") == 0) return NAL_INSTRS;
	if (strcmp(strToken, "JUMP") == 0) return NAL_JUMP;
	if (strcmp(strToken, "ABORT") == 0) return NAL_ABORT;

	return NAL_UNKNOW;
}

NAL_Symbol NextToken(FILE *file, char *strToken){
	char ch;
	int iIndex;
	int iPair;
	ChracterType typeCh;

	strToken[0] = Null;
	iIndex = 0;
	iPair = 0;

	while ((ch = fgetc(file)) != EOF)	{
		assert(iIndex < _MAX_TOKEN_SIZE);

		typeCh = GetChType(ch);

		if (typeCh == PreservePair)
			iPair++;

		switch (typeCh){
			case PreserveChar:
				if (iPair == 1){
					typeCh = Normal;
				}
				else{
					if (strlen(strToken) > 0){
						fseek(file, -1, SEEK_CUR);
						PrintToken(strToken, _APPEND);
						return IdentifyNalKeywordType(strToken);
					}
					else{
						strToken[iIndex] = ch;
						strToken[iIndex + 1] = Null;
						PrintToken(strToken, _APPEND);
						return IdentifyNalKeywordType(strToken);
					}
				}
				break;
			case PreservePair:
				if (iPair <= 1){
					typeCh = Normal;
				}
				else{
					strToken[iIndex] = ch;
					strToken[iIndex + 1] = Null;
					PrintToken(strToken, _APPEND);
					return IdentifyNalKeywordType(strToken);
				}
				break;
			case Space:
				if (iPair == 1){
					typeCh = Normal;
				}
				else{
					if (strlen(strToken) != 0){
						strToken[iIndex] = Null;
						PrintToken(strToken, _APPEND);
						return IdentifyNalKeywordType(strToken);
					}
				}
				break;
			case CR:
				if (strlen(strToken) > 0){
					fseek(file, -1, SEEK_CUR);
					PrintToken(strToken, _APPEND);
					return IdentifyNalKeywordType(strToken);
				}
				else{
					sprintf(strToken, "\n");
					PrintToken(strToken, _APPEND);
					return IdentifyNalKeywordType(strToken);
				}
				break;
			default:
				break;
		}

		if (typeCh == Normal){
			strToken[iIndex] = ch;
			strToken[iIndex + 1] = Null;
			iIndex++;
		}
	}

	if (strlen(strToken) > 0){
		PrintToken(strToken, _APPEND);
		return IdentifyNalKeywordType(strToken);
	}
	else
		return NAL_UNKNOW;
}

void PrintToken(char *strToken, const char *strMode){
	FILE *file;
	file = fopen("debug.txt", strMode);
	fprintf(file, " %s", strToken);
	fclose(file);
}

/* change "AAA" to AAA  (delete char'"') */
Bool NormalizeString(char *strLine, char chHeadTail){
	unsigned i;

	if (strLine[0] != chHeadTail &&strLine[strlen(strLine) - 1] != chHeadTail)
		return True;

	if (strLine[0] == chHeadTail){
		if (strLine[strlen(strLine) - 1] == chHeadTail){
			for (i = 1; i < strlen(strLine); i++){
				strLine[i - 1] = strLine[i];
			}
			strLine[strlen(strLine) - 2] = Null;
			return True;
		}
		else{
			return False;
		}
	}
	else if (strLine[strlen(strLine) - 1] == chHeadTail){
		return False;
	}

	return False;
}

char *NormalizeValue(char *strValue, int iDecimalPlace){
	float iValue;
	char strFormat[10];

	iValue = (float)(atof(strValue));

	sprintf(strFormat, "%%015.%df", iDecimalPlace);
	sprintf(strValue, strFormat, iValue);

	return strValue;
}

Bool P_FILE(FILE *file){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	bResult = False;
	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_UNKNOW){
		switch (sbTokenSymbol){
			case NAL_STRING:
				NormalizeString(strToken, '"');
				bResult = P_NAL(strToken);
				break;
			case NAL_STRING_VARIBLE:
				PrintError(file, "Error!! No implement yet.");
				bResult = False;
				break;
			case NAL_CR:
				PrintError(file, "Error!! No filename.");
				bResult = False;
				break;
			default:
				sprintf(g_strMsgBuffer, "Error!! '%s is not a valid string.'", strToken);
				PrintError(file, g_strMsgBuffer);
				bResult = False;
				break;
		}
	}
	else{
		PrintError(file, "Error!! No filename.");
		bResult = False;
	}

	return bResult;
}

Bool P_Equal(FILE *file, char *strValue, NAL_Symbol *nsValueType){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	strValue[0] = Null;

	bResult = False;
	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_UNKNOW){
		switch (sbTokenSymbol){
			case NAL_STRING:
			case NAL_NUMBER:
				strcpy(strValue, strToken);
				*nsValueType = sbTokenSymbol;
				bResult = True;
				break;
			case NAL_STRING_VARIBLE:
			case NAL_NUMBER_VARIBLE:
				if ((bResult = P_GetVaribleValue(file, strToken, strValue, nsValueType)) == True){
					bResult = True;
				}
				else{
					sprintf(g_strMsgBuffer, "Error!! Cannot get varible '%s' value.", strToken);
					PrintError(file, g_strMsgBuffer);
				}
				break;
			case NAL_ROT18:
				NormalizeString(strToken, '#');
				if ((bResult = DecodeROT18(strToken, strValue)) != True){
					sprintf(g_strMsgBuffer, "Error!! Cannot decode ROT18 string #%s#.", strToken);
					PrintError(file, g_strMsgBuffer);
				}
				*nsValueType = NAL_STRING;
				break;
			default:
				sprintf(g_strMsgBuffer, "Error!! '= %s' Syntax error.", strToken);
				PrintError(file, g_strMsgBuffer);
				break;
		}
	}
	else{
		strcpy(strValue, "Null");
		bResult = False;
	}

	return bResult;
}

Bool P_GetVaribleValue(FILE *file, const char *strVaribleName, char *strValue, NAL_Symbol *nsValueType){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	char *strSearchResult;

	strValue[0] = Null;

	bResult = False;

	strSearchResult = mvm_search(g_NalVaribles, (char*)strVaribleName);

	if (strSearchResult != Null){
		strcpy(strValue, strSearchResult);

		sbTokenSymbol = IdentifyNalKeywordType(strSearchResult);
		*nsValueType = sbTokenSymbol;

		switch (sbTokenSymbol){
			case NAL_STRING:
				bResult = True;
				break;
			case NAL_NUMBER:
				bResult = True;
				break;
			case NAL_STRING_VARIBLE:
				strcpy(strToken, strValue);
				bResult = P_GetVaribleValue(file, strToken, strValue, nsValueType);
				break;
			case NAL_NUMBER_VARIBLE:
				strcpy(strToken, strValue);
				bResult = P_GetVaribleValue(file, strToken, strValue, nsValueType);
				break;
			default:
				bResult = False;
				break;
		}
	}
	else{
		sprintf(g_strMsgBuffer, "Error!! Varible '%s' has not been assigned.", strVaribleName);
		PrintError(file, g_strMsgBuffer);
	}

	return bResult;
}

Bool P_AssignValue2Varible(FILE *file, const char *strVaribleName, NAL_Symbol nsExpecedtValueType){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	NAL_Symbol sbValueType;

	char strValue[_MAX_TOKEN_SIZE];

	bResult = False;
	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_UNKNOW){
		switch (sbTokenSymbol){
			case NAL_Equal:
				bResult = P_Equal(file, strValue, &sbValueType);
				if (bResult == True){
					if ((nsExpecedtValueType == sbValueType) || (nsExpecedtValueType == NAL_STRING && sbValueType == NAL_STRING_VARIBLE) || (nsExpecedtValueType == NAL_NUMBER && sbValueType == NAL_NUMBER_VARIBLE)){
						if (mvm_update(g_NalVaribles, (char*)strVaribleName, strValue) == Null)
							mvm_insert(g_NalVaribles, (char*)strVaribleName, strValue);
					}
					else {
						bResult = False;
					}
				}

				if (bResult == False){
					sprintf(g_strMsgBuffer, "Error!! Cannot assign value %s to %s.", strValue, strVaribleName);
					PrintError(file, g_strMsgBuffer);
				}
				break;
			default:
				sprintf(g_strMsgBuffer, "Error!! Missing '=' for %s.", strVaribleName);
				PrintError(file, "Error!! Missing '='.");
				bResult = False;
				break;
		}
	}
	else{
		sprintf(g_strMsgBuffer, "Error!! Missing '=' for %s.", strVaribleName);
		PrintError(file, g_strMsgBuffer);
		bResult = False;
	}

	return bResult;
}

Bool P_PRINT(FILE *file, Bool bPrintNewLine){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	char strValue[_MAX_TOKEN_SIZE];
	NAL_Symbol nsValueType;

	bResult = False;
	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_UNKNOW){
		switch (sbTokenSymbol){
			case NAL_ROT18:
				NormalizeString(strToken, '#');
				if ((bResult = DecodeROT18(strToken, strValue)) == True){
					NalPrint(strValue, bPrintNewLine);
				}
				else{
					sprintf(g_strMsgBuffer, "Error!! Cannot decode ROT18 string #%s#.", strToken);
					PrintError(file, g_strMsgBuffer);
				}
				break;
			case NAL_STRING:
				NormalizeString(strToken, '"');
				NalPrint(strToken, bPrintNewLine);
				bResult = True;
				break;
			case NAL_STRING_VARIBLE:
			case NAL_NUMBER_VARIBLE:
				if ((bResult = P_GetVaribleValue(file, strToken, strValue, &nsValueType)) == True){
					NormalizeString(strValue, '"');
					NalPrint(strValue, bPrintNewLine);
				}
				else{
					sprintf(g_strMsgBuffer, "Error!! Cannot print value %s of varible %s.", strValue, strToken);
					PrintError(file, g_strMsgBuffer);
				}
				break;
			case NAL_CR:
				PrintError(file, "Error!! No string can print.");
				break;
			default:
				sprintf(g_strMsgBuffer, "Error!! '%s; is not a valid string.", strToken);
				PrintError(file, g_strMsgBuffer);
				break;
		}
	}
	else{
		PrintError(file, "Error!! No string can print.");
	}

	return bResult;
}

Bool P_JUMP(FILE *file){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	long iJumpIndex, iJumpCount;

	bResult = False;
	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_UNKNOW){
		switch (sbTokenSymbol){
			case NAL_NUMBER:
				bResult = True;
				iJumpCount = strtol(strToken, NULL, 10);
				rewind(file);
				iJumpIndex = 0;
				while (iJumpIndex < iJumpCount){
					if (NextToken(file, strToken) == NAL_UNKNOW){
						bResult = False;
						break;
					}

					if (IdentifyNalKeywordType(strToken) != NAL_CR)
						iJumpIndex++;
				}
				break;
			case NAL_NUMBER_VARIBLE:
				PrintError(file, "Error!! No implement yet.");
				bResult = False;
				break;
			case NAL_CR:
				PrintError(file, "Error!! No JUMP number.");
				bResult = False;
				break;
			default:
				sprintf(g_strMsgBuffer, "Error!! '%s is not a valid number.'", strToken);
				PrintError(file, g_strMsgBuffer);
				bResult = False;
				break;
		}
	}
	else{
		PrintError(file, "Error!! No filename.");
		bResult = False;
	}

	return bResult;
}

Bool P_INC(FILE *file){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	char strValue[_MAX_TOKEN_SIZE];
	NAL_Symbol nsValueType;

	float iValue;
	int iDecimalPlace;
	char strFormat[10];

	bResult = False;

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_L_Parentheses){
		PrintError(file, "Error!! INC expect '('.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strToken)) == NAL_NUMBER_VARIBLE){
		if (P_GetVaribleValue(file, strToken, strValue, &nsValueType) == True){
			if (nsValueType == NAL_NUMBER){
				iDecimalPlace = DecimalPoint(strValue);
				iValue = (float)(atof(strValue));
				iValue = iValue + 1;
				sprintf(strFormat, "%%0.%df", iDecimalPlace);
				sprintf(strValue, strFormat, iValue);
				mvm_update(g_NalVaribles, (char*)strToken, strValue);
				bResult = True;
			}
		}
	}
	else{
		PrintError(file, "Error!! INC expect a number varible.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_R_Parentheses){
		PrintError(file, "Error!! INC expect ')'.");
		return False;
	}

	return bResult;
}

Bool P_RND(FILE *file){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	char strValue[_MAX_TOKEN_SIZE];

	int iValue;

	bResult = False;

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_L_Parentheses){
		PrintError(file, "Error!! RND expect '('.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strToken)) == NAL_NUMBER_VARIBLE){
		iValue = GetRandom();
		sprintf(strValue, "%i", iValue);
		if (mvm_update(g_NalVaribles, (char*)strToken, strValue) == NULL)
			mvm_insert(g_NalVaribles, (char*)strToken, strValue);
		bResult = True;
	}
	else{
		PrintError(file, "Error!! RND expect a number varible.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_R_Parentheses){
		PrintError(file, "Error!! RND expect ')'.");
		return False;
	}

	return bResult;
}

Bool P_INNUM(FILE *file){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	char strVar[_MAX_TOKEN_SIZE];
	char strValue[_MAX_TOKEN_SIZE];

	bResult = True;

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_L_Parentheses){
		PrintError(file, "Error!! INNUM expect '('.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strToken)) == NAL_NUMBER_VARIBLE){
		strcpy(strVar, strToken);
	}
	else{
		PrintError(file, "Error!! INNUM expect a number varible.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_R_Parentheses){
		PrintError(file, "Error!! INNUM expect ')'.");
		return False;
	}

	do{
		scanf("%s", strValue);
		if (IsNumber(strValue)){
			if (mvm_update(g_NalVaribles, (char*)strVar, strValue) == Null)
				mvm_insert(g_NalVaribles, (char*)strVar, strValue);
			break;
		}
		else{
			sprintf(g_strMsgBuffer, "Sorry!! '%s' is not a number. Please input again.", strValue);
			PrintError(file, g_strMsgBuffer);
		}
	} while (1);

	return bResult;
}

Bool P_IN2STR(FILE *file){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	char strVar1[_MAX_TOKEN_SIZE], strVar2[_MAX_TOKEN_SIZE];
	char strBuf[_MAX_TOKEN_SIZE];

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_L_Parentheses){
		PrintError(file, "Error!! IN2STR expect '('.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strVar1)) != NAL_STRING_VARIBLE){
		PrintError(file, "Error!! IN2STR param1 must be string varible.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_Comma){
		PrintError(file, "Error!! IN2STR expect ','.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strVar2)) != NAL_STRING_VARIBLE){
		PrintError(file, "Error!! IN2STR param2 must be string varible.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_R_Parentheses){
		PrintError(file, "Error!! IN2STR expect ')'.");
		return False;
	}

	printf("Please input string 1 :\n");
	fgets(strToken, _MAX_TOKEN_SIZE, stdin);
	if (strToken[strlen(strToken) - 1] == '\n')
		strToken[strlen(strToken) - 1] = Null;
	sprintf(strBuf, "\"%s\"", strToken);
	if (mvm_update(g_NalVaribles, (char*)strVar1, strBuf) == Null)
		mvm_insert(g_NalVaribles, (char*)strVar1, strBuf);

	printf("Please input string 2 :\n");
	fgets(strToken, _MAX_TOKEN_SIZE, stdin);
	if (strToken[strlen(strToken) - 1] == '\n')
		strToken[strlen(strToken) - 1] = Null;
	sprintf(strBuf, "\"%s\"", strToken);
	if (mvm_update(g_NalVaribles, (char*)strVar2, strBuf) == Null)
		mvm_insert(g_NalVaribles, (char*)strVar2, strBuf);

	bResult = True;

	return bResult;
}

Bool P_IFCOND(FILE *file, NAL_Symbol nsIFCOND){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	char strValue1[_MAX_TOKEN_SIZE], strValue2[_MAX_TOKEN_SIZE];
	NAL_Symbol nsValueType1, nsValueType2;

	Bool bDo;

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_L_Parentheses){
		PrintError(file, "Error!! IFCOND expect '('.");
		return False;
	}

	if (P_Equal(file, strValue1, &nsValueType1) == False){
		PrintError(file, "Error!! IFCOND param1 is not valid.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_Comma){
		PrintError(file, "Error!! IFCOND expect ','.");
		return False;
	}

	if (P_Equal(file, strValue2, &nsValueType2) == False){
		PrintError(file, "Error!! IFCOND param2 is not valid.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_R_Parentheses){
		PrintError(file, "Error!! IFCOND expect ')'.");
		return False;
	}

	if (nsValueType1 != nsValueType2){
		PrintError(file, "Error!! IFCOND param1 and param2 type are diffrent.");
		return False;
	}

	if ((sbTokenSymbol = NextToken(file, strToken)) != NAL_L_Braces){
		PrintError(file, "Error!! IFCOND expect '{'.");
		return False;
	}

	if (nsValueType1 == NAL_NUMBER){
		NormalizeValue(strValue1, 5);
		NormalizeValue(strValue2, 5);
	}
	else{
		NormalizeString(strValue1, '"');
		NormalizeString(strValue2, '"');
	}

	bDo = True;
	switch (nsIFCOND){
		case NAL_IFGREATER:
			bDo = False;
			if (strcmp(strValue1, strValue2) > 0)
				bDo = True;
			break;
		case NAL_IFEQUAL:
			bDo = False;
			if (strcmp(strValue1, strValue2) == 0)
				bDo = True;
			break;
		case NAL_INSTRS:
			bDo = False;
			if (strstr(strValue1, strValue2) != NULL)
				bDo = True;
			break;
		default:
			break;
	}

	if (bDo == True)
		bResult = P_Block(file, False);
	else
		bResult = P_Block(file, True);

	return bResult;
}

Bool P_Block(FILE *file, Bool bSkipBlock){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	while (1){
		sbTokenSymbol = NextToken(file, strToken);

		bResult = False;

		switch (sbTokenSymbol){
			case NAL_L_Braces:
				bResult = P_Block(file, bSkipBlock);
				break;
			case NAL_R_Braces:
				bResult = True;
				return True;
				break;
			default:
				break;
		}

		if (bSkipBlock == False){
			switch (sbTokenSymbol){
				case NAL_CR:
					bResult = True;
					break;
				case NAL_PRINT:
					bResult = P_PRINT(file, True);
					break;
				case NAL_PRINTN:
					bResult = P_PRINT(file, False);
					break;
				case NAL_FILE:
					bResult = P_FILE(file);
					break;
				case NAL_STRING_VARIBLE:
					bResult = P_AssignValue2Varible(file, strToken, NAL_STRING);
					break;
				case NAL_NUMBER_VARIBLE:
					bResult = P_AssignValue2Varible(file, strToken, NAL_NUMBER);
					break;
				case NAL_JUMP:
					bResult = P_JUMP(file);
					break;
				case NAL_ABORT:
					bResult = Abort;
					break;
				case NAL_INC:
					bResult = P_INC(file);
					break;
				case NAL_RND:
					bResult = P_RND(file);
					break;
				case NAL_IFGREATER:
				case NAL_IFEQUAL:
				case NAL_INSTRS:
					bResult = P_IFCOND(file, sbTokenSymbol);
					break;
				case NAL_INNUM:
					bResult = P_INNUM(file);
					break;
				case NAL_IN2STR:
					bResult = P_IN2STR(file);
					break;
				default:
					sprintf(g_strMsgBuffer, "Error!! Unexpected/Unknowen keyword '%s'.", strToken);
					PrintError(file, g_strMsgBuffer);
					bResult = False;
					break;
			}

			if (bResult == False || bResult == Abort)
				return bResult;
		}
	}

	return False;
}

Bool P_NAL(char *strFilename){
	char strToken[_MAX_TOKEN_SIZE];
	FILE *file = Null;
	NAL_Symbol sbTokenSymbol;
	Bool bResult;

	if (FileExist(strFilename) == False){
		sprintf(g_strMsgBuffer, "Error!! Cannot open file %s", strFilename);
		PrintError(file, g_strMsgBuffer);
		return False;
	}

	file = fopen(strFilename, "rb");
	{
		while ((sbTokenSymbol = NextToken(file, strToken)) != NAL_UNKNOW){
			bResult = True;
			switch (sbTokenSymbol){
				case NAL_CR:
					break;
				case NAL_L_Braces:
					bResult = P_Block(file, False);
					break;
				default:
					bResult = False;
					PrintError(file, "Error!! NAL Files expect '{'");
					break;
			}

			if (bResult == False){
				Dump(file);
				break;
			}

			if (bResult == Abort){
				break;
			}
		}
	}

	fclose(file);

	return bResult;
}


int main(int argc, char *argv[]){

	if (argc != 2) {
		printf("Error !! No input file.\n\n");
		return False;
	}

	if (!FileExist(argv[1])){
		printf("Error !! file %s is not exist.\n\n", argv[1]);
		return False;
	}

	g_NalVaribles = mvm_init();

	srand((unsigned)time(NULL));

	PrintToken("Begin debug >>>\n", _NEW);

	P_NAL(argv[1]);

	mvm_free(&g_NalVaribles);

	printf("\n\n");

	return True;
}


