#include "nal.h"
#include "mvm.h"

/* declare NAL delimiters */
char g_cPreserveChar[] = { ',', '{', '}', '=', '+', '-', '*', '/' };
char g_cPreservePair[] = { '"', '#' };
char g_cSpace[] = { ' ', '\t' };
char g_cCR[] = { '\n' };

/* declare ROT */
char g_strPlain[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
char g_strROT18[] = "NOPQRSTUVWXYZABCDEFGHIJKLMnopqrstuvwxyzabcdefghijklm5678901234";

/* declare mvm */
mvm *g_NalTokens;
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

/*
void PrintError(FILE *file, const char *strMsgType, const char *strSymbol, const char *strMsg){
printf("\n%s !! [%s] %s\n", strMsgType, strSymbol, strMsg);
}
*/

void PrintError(FILE *file, const char *strMsg){
	printf("\n%s\n", strMsg);

	if (file == Null)
		return;
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
	int bMatch;

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
			sprintf(g_strMsgBuffer, "Error : Invalid ROT18 string '%s'\n", strSource);
			PrintError(Null, g_strMsgBuffer);
			return False;
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
	if (strcmp(strToken, "IFGREATER") == 0) return NAL_IFGREATER;
	if (strcmp(strToken, "IFEQUAL") == 0) return NAL_IFEQUAL;
	if (strcmp(strToken, "JUMP") == 0) return NAL_JUMP;
	if (strcmp(strToken, "ABORT") == 0) return NAL_ABORT;

	return NAL_UNKNOW;
}

Bool NextToken(FILE *file, char *strToken){
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
						PrintToken(strToken, "a");
						return True;
					}
					else{
						strToken[iIndex] = ch;
						strToken[iIndex + 1] = Null;
						PrintToken(strToken, "a");
						return True;
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
					PrintToken(strToken, "a");
					return True;
				}
				break;
			case Space:
				if (iPair == 1){
					typeCh = Normal;
				}
				else{
					if (strlen(strToken) != 0){
						strToken[iIndex] = Null;
						PrintToken(strToken, "a");
						return True;
					}
				}
				break;
			case CR:
				if (strlen(strToken) > 0){
					fseek(file, -1, SEEK_CUR);
					PrintToken(strToken, "a");
					return True;
				}
				else{
					sprintf(strToken, "\n");
					PrintToken(strToken, "a");
					return True;
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
		PrintToken(strToken, "a");
		return True;
	}
	else
		return False;
}

void PrintToken(char *strToken, const char *strMode){
	FILE *file;
	file = fopen("debug.txt", strMode);
	fprintf(file, " %s", strToken);
	fclose(file);

	/*mvm_insert(g_NalTokens, strToken, "n/a");*/
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

Bool P_FILE(FILE *file){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	bResult = False;
	if (NextToken(file, strToken) == True){
		sbTokenSymbol = IdentifyNalKeywordType(strToken);

		switch (sbTokenSymbol){
			case NAL_STRING:
				NormalizeString(strToken, '"');
				bResult = P_NAL(strToken);
				break;
			case NAL_STRING_VARIBLE:
				bResult = True;
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
	if (NextToken(file, strToken) == True){
		sbTokenSymbol = IdentifyNalKeywordType(strToken);

		strcpy(strValue, strToken);
		*nsValueType = sbTokenSymbol;

		switch (sbTokenSymbol){
			case NAL_STRING:
			case NAL_NUMBER:
			case NAL_STRING_VARIBLE:
			case NAL_NUMBER_VARIBLE:
				bResult = True;
				break;
			default:
				bResult = False;
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
	if (NextToken(file, strToken) == True){
		sbTokenSymbol = IdentifyNalKeywordType(strToken);

		switch (sbTokenSymbol){
			case NAL_Equal:
				bResult = P_Equal(file, strValue, &sbValueType);
				if (bResult == True){
					if ((nsExpecedtValueType == sbValueType) || (nsExpecedtValueType == NAL_STRING && sbValueType == NAL_STRING_VARIBLE) || (nsExpecedtValueType == NAL_NUMBER && sbValueType == NAL_NUMBER_VARIBLE)){
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

Bool P_PRINT(FILE *file, int bPrintNewLine){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	char strValue[_MAX_TOKEN_SIZE];
	NAL_Symbol nsValueType;

	bResult = False;
	if (NextToken(file, strToken) == True){
		sbTokenSymbol = IdentifyNalKeywordType(strToken);

		switch (sbTokenSymbol){
			case NAL_STRING:
				NormalizeString(strToken, '"');
				if (bPrintNewLine)
					printf("%s\n", strToken);
				else
					printf("%s", strToken);

				bResult = True;
				break;
			case NAL_STRING_VARIBLE:
				bResult = P_GetVaribleValue(file, strToken, strValue, &nsValueType);
				if (bResult == True){
					if (nsValueType == NAL_STRING){
						NormalizeString(strValue, '"');
						if (bPrintNewLine)
							printf("%s\n", strValue);
						else
							printf("%s", strValue);
					}
					else {
						bResult = False;
					}
				}

				if (bResult == False){
					sprintf(g_strMsgBuffer, "Error!! Cannot print value %s of varible %s.", strValue, strToken);
					PrintError(file, g_strMsgBuffer);
				}
				break;
			case NAL_NUMBER_VARIBLE:
				bResult = P_GetVaribleValue(file, strToken, strValue, &nsValueType);
				if (bResult == True){
					if (nsValueType == NAL_NUMBER){
						NormalizeString(strValue, '"');
						if (bPrintNewLine)
							printf("%s\n", strValue);
						else
							printf("%s", strValue);
					}
					else {
						bResult = False;
					}
				}

				if (bResult == False){
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

Bool P_Block(FILE *file){
	char strToken[_MAX_TOKEN_SIZE];
	Bool bResult;
	NAL_Symbol sbTokenSymbol;

	while (NextToken(file, strToken) == True){
		sbTokenSymbol = IdentifyNalKeywordType(strToken);

		switch (sbTokenSymbol){
			case NAL_CR:
				break;
			case NAL_PRINT:
				bResult = P_PRINT(file, True);
				break;
			case NAL_PRINTN:
				bResult = P_PRINT(file, False);
				break;
			case NAL_R_Braces:
				return True;
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
			default:
				sprintf(g_strMsgBuffer, "Error!! Unexpected/Unknowen keyword '%s'.", strToken);
				PrintError(file, g_strMsgBuffer);
				bResult = False;
				break;
		}

		if (bResult == False)
			return False;
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
		while (NextToken(file, strToken) == True){
			sbTokenSymbol = IdentifyNalKeywordType(strToken);

			bResult = True;
			switch (sbTokenSymbol){
				case NAL_CR:
					break;
				case NAL_L_Braces:
					bResult = P_Block(file);
					break;
				default:
					bResult = False;
					PrintError(file, "Error!! NAL Files expect '{'");
					break;
			}

			if (bResult == False)
				break;
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

	g_NalTokens = mvm_init();
	g_NalVaribles = mvm_init();

	PrintToken("Begin debug >>>\n", "w");

	P_NAL(argv[1]);

	/*mvm_print_2_console(g_NalTokens);*/

	mvm_free(&g_NalVaribles);
	mvm_free(&g_NalTokens);

	printf("\n\n");

	return True;
}

