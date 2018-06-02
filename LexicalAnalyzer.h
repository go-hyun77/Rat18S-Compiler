#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <vector>

using namespace std;

ofstream outfile;
bool commentCheck = false;		//toggle variable to check if characters are enclosed in "!" in order to ignore characters parsed 

struct tokenInfo {
	string token;
	int FSM_ID_Number;
	int lineNumber;
	tokenInfo(string token = "N/A", int FSM_ID_Number = 9, int lineNumber = 0);			//default attributes for new instances of a token
};

string keywords[26] = { "int", "double", "float", "char", "string", "if", "else", "for", "do", "while",			//array of keywords to check if tokens can be matched as a keyword
"return", "break", "const", "class", "struct", "static", "unsigned", "long", "short",
"switch", "case", "get", "put", "function", "true", "false" };

string operators[16] = { "=", ";", "<", ">", "+", "-", "*", "/", "!", ":", "!=", "=>", "=<", ":=", "^=", "==" };		//array of operators 

string separators[11] = { "%%", ",", ":", ";", "|", "(", ")", "[", "]", "{", "}" };		//array of separators

string state[8] = { "Unknown", "Identifier", "Integer", "Keyword", "Seperator", "Operator", "Real", "Decimal" };		//array of states


const int ID_FSM_States[6][3] = {			//Accepting states: 1, 2, 4

	1, 5, 5,			//state 0, starting state, accepts only letter as input
	2, 4, 3,			//state 1, goes to other states based input. state 2 for letters, state4 for digits, state 3 for underscore
	2, 4, 3,			//state 2, accepted state, ends with a letter
	2, 4, 3,			//state 3, no accepted, ending in underscore
	2, 4, 3,			//state 4, accepted state, ending with digit
	5, 5, 5,			//state 5, reject state. 

};


const int DIGIT_FSM_States[5][2] = {		//Accepting states: 1, 3

	1, 4,				//state 0, first input must be integer otherwise rejected
	1, 2,				//state 1, stays in state 1 if input is a integer, state 2 if input is "."
	3, 4,				//state 2, accepts integer as input. goes to state 3, otherwise reject
	3, 4,				//state 3, keeps accepting integers after ".". if not integer reject.
	4, 4				//state 4, reject state

};



class LexicalAnalysis {
public:
	LexicalAnalysis();

	vector<tokenInfo> lexer(string token, int currLine);

	bool isOperator(string token);			//checks if token is operator
	bool isSeparator(string token);			//checks if token is separator
	bool isKeyword(string token);			//checks if token is keyword
	bool isReal(string token);				//checks if token is real
	bool FirstLetterAlpha(string token);	//checks if first character is a letter

	int ID_FSM_COL(char input);							// returns next state of identifiers FSM
	int	DIGIT_FSM_COL(char input);						// return the next state of integer

	void printList(vector<tokenInfo> & tokens);		//prints list of tokens passed in


	enum colID {
		LETTER,
		INTEGER,
		UNDERSCORE
	};

	enum colDIGIT {
		REAL,
		DECIMAL
	};
};


tokenInfo::tokenInfo(string newToken, int new_FSM_ID, int newLine) {
	token = newToken;
	FSM_ID_Number = new_FSM_ID;
	lineNumber = newLine;
}

LexicalAnalysis::LexicalAnalysis() {		//empty constructor


}


bool LexicalAnalysis::isKeyword(string token) {
	for (int i = 0; i < 27; i++) {
		if (token == keywords[i]) {			//if string found in keyword array, return true
			return true;
		}
	}
	return false; //default
}



bool LexicalAnalysis::isOperator(string token) {
	for (int i = 0; i < 18; i++) {
		if (token == operators[i]) {		//if string found in operator array, return true
			return true;
		}
	}
	return false; //default
}



bool LexicalAnalysis::isSeparator(string token) {
	for (int i = 0; i < 12; i++) {
		if (token == separators[i]) {		//if string found in separator array, return true
			return true;
		}
	}
	return false; //default
}


bool LexicalAnalysis::isReal(string token) {
	for (int i = 0; i < token.length(); i++) {		//if decimal found in string, return true
		if (token[i] == '.') {
			return true;
		}
	}
	return false; //default
}


bool LexicalAnalysis::FirstLetterAlpha(string token) {
	if (isalpha(token[0])) {			//return true if the first letter in string is alphanumeric
		return true;
	}
	else {
		return false;	//default
	}
}


int LexicalAnalysis::ID_FSM_COL(char input) {	//determines next fsm state 

	int col = -1;

	if (isalpha(input)) {
		col = LETTER;
	}
	else if (isdigit(input)) {
		col = INTEGER;
	}
	else if (input == '_') {
		col = UNDERSCORE;
	}

	return col;
}

int LexicalAnalysis::DIGIT_FSM_COL(char input) {

	int col = -1;

	if (isdigit(input)) {
		col = REAL;
	}
	else if (input == '.') {
		col = DECIMAL;
	}

	return col;
}

void LexicalAnalysis::printList(vector<tokenInfo> & listTokens) {

	string tokenlog = "lexeme.txt";
	outfile.open(tokenlog);					//outputs to lexeme.txt, if file doesn't exist, then creates the file

	outfile << left << setw(15) << "token" << left << setw(5) << "lexeme" << endl << endl;

	for (int i = 0; i < listTokens.size(); i++) {
		outfile << left << setw(15) << listTokens[i].token << left << setw(7) << state[listTokens[i].FSM_ID_Number] << endl;
	}
	outfile.close();
}



vector<tokenInfo> LexicalAnalysis::lexer(string token, int currentLine) {			//lexer function
	bool endofstate = false;
	char currentcharacter;
	int	currentstate = 0;	//current id of state in fsm
	int	column = 0;
	string currentToken = "";

	tokenInfo temp;
	vector<tokenInfo> tokenList;

	for (int i = 0; i < token.length(); i++) {				//Parses line by line

		currentcharacter = token[i];
		endofstate = false;

		if (currentcharacter == '!' || commentCheck) {		//checks for comment character '!'

			do {
				i++;
				currentcharacter = token[i];
				currentToken += currentcharacter;
			} while (currentcharacter != '!' && currentcharacter != '\0');		// loop continues until the 2nd '!' is found

			if (currentcharacter == '\0') {			//break when found
				commentCheck = true;
				break;
			}
			else {
				commentCheck = false;
			}

			column = 0;				//sets fsm column back to starting state
			currentstate = 0;
			i++;
			currentcharacter = token[i];
			currentToken.clear();			//empties current token string for next token
		}

		// Checks if identifier, first character in the string is a letter
		if ((isalpha(token[i])) || (token[i] == '_') || (token[i] == '$') || (isdigit(token[i]) && FirstLetterAlpha(currentToken))) {

			currentToken += currentcharacter;			//adds new character to token
			column = ID_FSM_COL(currentcharacter);				//sets column to the fsm state of current character
			currentstate = ID_FSM_States[currentstate][column];		//gets next state

																	//if end of token, end of string, next space is space, non letter reached, non digit reached, non udnerscore reached
			if (i == token.length() - 1 || isspace(token[i + 1]) || (token[i + 1] == '$') || !isalpha(token[i + 1]) && !isdigit(token[i + 1]) && token[i + 1] != '_') {

				if (currentstate == 1 || currentstate == 2 || currentstate == 4) {		//if fsm is in accepting state

					if (isKeyword(currentToken)) {
						temp.FSM_ID_Number = 3;
					}

					else {
						if (token[i + 1] == '$') {
							i++;
							currentcharacter = token[i];
							currentToken += currentcharacter;
						}
						temp.FSM_ID_Number = 1;
					}
				}

				else {
					temp.FSM_ID_Number = 0;
				}

				temp.token = currentToken;
				temp.lineNumber = currentLine;
				tokenList.insert(tokenList.end(), temp);

				currentToken.clear();			//clears the currentToken string for next token
				column = 0;						//sets column and current state back to starting state
				currentstate = 0;
			}
		}





		//if character is a digit, use the digit fsm
		else if (((currentToken.empty() && isdigit(token[i])) || (!currentToken.empty() && isdigit(currentToken[0]) && isdigit(token[i]))
			|| (!currentToken.empty() && isdigit(currentToken[0]) && token[i] == '.'))) {

			currentToken += currentcharacter;				//adds new character to token string
			column = DIGIT_FSM_COL(currentcharacter);
			currentstate = DIGIT_FSM_States[currentstate][column];	//gets next state

																	//if end of token, or next token is space, or is not digit or decimal
			if (i == token.length() - 1 || ispunct(token[i + 1]) || isspace(token[i + 1]) || (!isdigit(token[i + 1]) && token[i + 1] != '.')) {

				if ((i != token.length() - 1) && !isspace(token[i + 1]) && isalpha(token[i + 1])) { //if end is nondigit


																									//reads until end of string
					while ((token[i] != '\0' && !isspace(token[i]) && (isdigit(token[i + 1]) || isalpha(token[i + 1]) || token[i + 1] == '_'))) {
						i++;
						currentcharacter = token[i];
						currentToken += currentcharacter;
					}

					currentstate = 2;
				}

				if (currentstate == 1 || currentstate == 3) {		//if is in accepting state
					if (isReal(currentToken)) {
						temp.FSM_ID_Number = 3; 			//sets fsm state as a Real
					}
					else {
						temp.FSM_ID_Number = 2;			//sets fsm state as a Digit
					}
				}
				else {
					temp.FSM_ID_Number = 0;		//sets fsm state as unknown
				}
				temp.token = currentToken;
				temp.lineNumber = currentLine;
				tokenList.insert(tokenList.end(), temp);
				currentToken.clear();
				column = 0;
				currentstate = 0;
			}
		}



		else if (isspace(token[i]) || token[i] == '\t' || token[i] == '\n') {	//checks token for space

			currentToken.clear();		//clears for next token
			column = 0;					//sets back to starting state
			currentstate = 0;

		}

		else {
			currentToken += currentcharacter;
			//checks for operators or separators
			if (i == token.length() - 1 || (i < token.length() - 1 && (!isspace(token[i + 1]) || !isalnum(token[i + 1])))) {


				for (int k = 0; k < 17; k++) {		//find operators

					if (currentToken == operators[k]) {
						for (int j = 0; j < 17; j++) {	//checks if 2nd charcter is an operator
							if ((currentToken + token[i + 1] == operators[j])) {
								currentToken = currentToken + token[i + 1];
								i++;
								break;
							}
						}

						temp.token = currentToken;
						temp.FSM_ID_Number = 5;
						temp.lineNumber = currentLine;
						tokenList.insert(tokenList.end(), temp);
						currentToken.clear();
						endofstate = true;
						break;

					}

				}

				if (!endofstate && i <= token.length()) {
					if (isSeparator(currentToken + token[i + 1])) {
						temp.token = currentToken + token[i + 1];
						temp.FSM_ID_Number = 5;
						temp.lineNumber = currentLine;
						tokenList.insert(tokenList.end(), temp);
						currentToken.clear();
						i++;

						endofstate = true;
					}
				}



				for (int k = 0; k < 11; k++) {		//checks for separators
					if (currentToken == separators[k]) {
						temp.token = currentToken;
						temp.FSM_ID_Number = 4;
						temp.lineNumber = currentLine;
						tokenList.insert(tokenList.end(), temp);
						currentToken.clear();

						endofstate = true;
						break;
					}
				}

				if (!endofstate && i <= token.length()) {

					if (isSeparator(currentToken + token[i + 1])) {
						temp.FSM_ID_Number = 4;
						temp.token = currentToken + token[i + 1];
						temp.lineNumber = currentLine;
						tokenList.insert(tokenList.end(), temp);
						currentToken.clear();
						i++;

						endofstate = true;
					}
				}

				if (!endofstate) {
					temp.FSM_ID_Number = 0;
					temp.token = currentToken;
					temp.lineNumber = currentLine;
					tokenList.insert(tokenList.end(), temp);
					currentToken.clear();
					i++;
				}
			}
		}
	}
	return tokenList;
}	//end of lexer
