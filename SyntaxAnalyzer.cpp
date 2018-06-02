#include "LexicalAnalyzer.h"
#include <stack>
#include <stdio.h>

#define max_instr 1000
#define max_sym 100
#define null_addr -999

using namespace std;

ifstream input;
ofstream output;
ofstream table; 

int LineNum = 0;
int TokenIndex = 0;             //Index used to parse token vector
bool printSyntax = true;

vector<tokenInfo> tokens;                     //vector to hold tokens as they are being inputted
vector<tokenInfo> tokenList;                  //vector that holds all tokens once they have been read in initially
tokenInfo currentToken;

string instrTable[max_instr][2]; //symbol tables
string symbolTable[max_sym][2];

int instr_address;		//intermediate code generation variables
int current_instr = 1;
char address_array[100];
int addr;
int symbol = 0;
int memory_address = 2000;

bool ICG = true;
bool storeSymbol = true;
bool isNegative = false;

string save;				//variables for saving lexeme types
string type = "int";
string operatorstring;

stack <string> typestack;		//stacks for type checking
stack <int> jumpstack;

void errorMessage(string, int, string);			//logs errors and terminates
void getNextToken();							//gets next token

void generate_instr(string, int);		//creates assembly instructions
bool checkSymTable(string);				//searches symbol table for lexeme
//void checkType(string);					//checks variable type
void insertSymbol(string, string);		//insert string to symbol table

void printSymbolTable();	//prints to terminal
void printInstrTable();

int get_addr(string);
string get_type(string);
void back_patch(int);


void Rat18S();							//syntax rule function prototypes
void OptFunctionDefinitions();
void FunctionDefinitions();
void Function();
void OptParameterList();
void ParameterList();
void Parameter();
void Qualifier();
void Body();
void OptDeclarationList();
void DeclarationList();
void Declaration();
void IDs();
void StatementList();
void Statement();
void Compound();
void Assign();
void If();
void Return();
void Print();
void Scan();
void While();
void Condition();
void Relop();
void Expression();
void ExpressionPrime();
void Term();
void TermPrime();
void Factor();
void Primary();
void Empty();



int main() {

	LexicalAnalysis  test;
	string currentLine = "";
	string	testthis;


	cout << "Enter the test file's name: ";
	cin >> testthis;



	input.open(testthis);
	output.open("output.txt");
	table.open("Tables.txt");

	if (!input) {
		cout << "Error. Unable to read file." << endl;
		//system("pause");
		return -1;
	}


	while (getline(input, currentLine)) {
		LineNum++;
		tokens = test.lexer(currentLine, LineNum);
		tokenList.insert(tokenList.end(), tokens.begin(), tokens.end());
		tokens.clear();
	}


	input.close();
	test.printList(tokenList);


	Rat18S();

	printSymbolTable();
	printInstrTable();
	output.close();


	cout << "Check output file." << endl;
	system("pause");
	return 0;
}


void getNextToken() {
	if (TokenIndex < tokenList.size()) {
		currentToken = tokenList[TokenIndex];
		if (printSyntax) {
			output << endl << "Token: " << left << setw(15) << state[currentToken.FSM_ID_Number] << left << setw(10) << "Lexeme: " << left << setw(20) << currentToken.token << endl;
		}
		TokenIndex++;
	}
}


void Rat18S() {
	getNextToken();
	if (printSyntax) {
		output << "\t<Rat18S> -> <Opt Function Definitions> %% <Opt Declaration List> <Statement List> \n";
	}

	OptFunctionDefinitions();

	if (currentToken.token == "%%") {
		getNextToken();
		storeSymbol = true;
		OptDeclarationList();
		storeSymbol = false;
		StatementList();

		output << "Finish." << endl << endl << endl;
	}
	else {
		errorMessage("'%%'", 1, "<RAT18S>");
	}
}


void OptFunctionDefinitions() {
	if (printSyntax) {
		output << "\t<Opt Function Definitions> ->  <Function Definitions> | <Empty>" << endl;
	}

	if (currentToken.token == "function") {
		FunctionDefinitions();
	}

	else if (currentToken.token == "%%") {
		Empty();
	}

	else {
		errorMessage("'function' or '$$'", 1, "<Opt Function Definitions>");
	}

}


void FunctionDefinitions() {
	if (printSyntax) {
		output << "\t<Function Definitions> -> <Function> | <Function> <Function Definitions>" << endl;
	}

	while (currentToken.token == "function") {
		Function();
	}
}


void Function() {
	if (printSyntax) {
		output << "\t<Function> ->  function <Identifier> [ <Opt Parameter List> ] <Opt Declaration List> <Body>" << endl;
	}

	getNextToken();

	if (currentToken.FSM_ID_Number == 1) {
		getNextToken();
		if (currentToken.token == "[") {
			getNextToken();

			OptParameterList();

			if (currentToken.token == "]") {
				getNextToken();

				OptDeclarationList();
				Body();
			}
			else {
				errorMessage("']'", 1, "<Identifier>");
			}
		}

		else {
			errorMessage("'['", 1, "<Identifier>");
		}
	}

	else {
		errorMessage("<Identifer>", 1, "<Identifier>");
	}
}


void OptParameterList() {
	if (printSyntax) {
		output << "\t<Opt Parameter List> -> <Parameter List> | <Empty>";
	}

	if (currentToken.FSM_ID_Number == 1) {
		ParameterList();
	}

	else if (currentToken.token == "]") {
		Empty();
	}

	else {
		errorMessage("<Identifer>", 1, "<Opt Parameter List>");
	}
}


void ParameterList() {
	if (printSyntax) {
		output << "\t<Parameter List> -> <Parameter> | <Parameter>, <Parameter List>" << endl;
	}

	if (currentToken.FSM_ID_Number == 1) {
		Parameter();

		if (currentToken.token == ",") {
			getNextToken();
			ParameterList();
		}
	}
}


void Parameter() {
	if (printSyntax) {
		output << "\t<Parameter> -> <IDs> : <Qualifier> \n";
	}

	IDs();

	if (currentToken.token == ":") {
		getNextToken();
		Qualifier();
	}

	else {
		errorMessage(":", 1, "<Parameter>");
	}
}



void Qualifier() {
	if (printSyntax) {
		output << "\t<Qualifier> ->  Int | Boolean | Real\n";
	}

	type = currentToken.token;

	if (currentToken.token == "int") {
		if (printSyntax) {
			output << "\t<Qualifier> -> Int\n";
		}
	}

	else if (currentToken.token == "boolean") {
		if (printSyntax) {
			output << "\t<Qualifier> -> Boolean\n";
		}
	}

	else if (currentToken.token == "real") {
		if (printSyntax) {
			output << "\t<Qualifier> -> Real\n";
		}
	}

	else if (currentToken.token == "true") {
		if (printSyntax) {
			output << "\t<Qualifier> ->	Boolean \n";
		}
	}

	else if (currentToken.token == "false") {
		if (printSyntax) {
			output << "\t<Qualifier> -> Boolean \n";
		}
	}

	else {
		errorMessage("'int', 'boolean', or 'real'", 1, "<Qualifier>");
	}

	getNextToken();
}


void Body() {
	if (printSyntax) {
		output << "\t<Body> -> { <Statement List> }\n";
	}

	if (currentToken.token == "{") {
		getNextToken();
		StatementList();

		if (currentToken.token == "}") {
			getNextToken();
		}

		else {
			errorMessage("'}'", 1, "<Body>");
		}
	}

	else {
		errorMessage("'{'", 1, "<Body>");
	}
}


void OptDeclarationList() {
	if (printSyntax) {
		output << "\t<Opt Declaration List> -> <Declaration List> | <Empty>\n";
	}

	if (currentToken.token == "{") {
		Empty();
	}
	else if (currentToken.token == "int" || currentToken.token == "boolean" || currentToken.token == "real") {
		DeclarationList();
	}

	else {
		errorMessage("'int', 'boolean', 'real'", 1, "<Opt Declaration List>");
	}
}


void DeclarationList() {
	if (printSyntax) {
		output << "\t<Declaration List> -> <Declaration>; | <Declaration>; <Declaration List>\n";
	}


	Declaration();

	if (currentToken.token == ";") {
		getNextToken();

		if (currentToken.token == "int" || currentToken.token == "boolean" || currentToken.token == "real") {
			DeclarationList();
		}
	}

	else {
		errorMessage("';'", 2, "<Declaration List>");
	}
}


void Declaration() {
	if (printSyntax) {
		output << "\t<Declaration> -> <Qualifier> <IDs>";
	}

	Qualifier();
	IDs();
}


void IDs() {
	if (currentToken.FSM_ID_Number == 1) {
		if (printSyntax) {
			output << "\t<IDs> -> <Identifier> | <Identifier>, <IDs>\n";
		}

		if (storeSymbol) {
			if (checkSymTable(currentToken.token)) {
				output << "Error: Creating duplicate symbol: " << currentToken.token << endl;
				exit(1);
			}
			else {
				insertSymbol(type, currentToken.token);
			}
		}
		else {
			if (checkSymTable(currentToken.token)) {
				generate_instr("STDIN", null_addr);
				addr = get_addr(currentToken.token);
				generate_instr("POPM", addr);
			}
			else {
				output << "Error: Unknown identifier: " << currentToken.token << endl;
				exit(1);
			}
		}

		getNextToken();

		if (currentToken.token == ",") {			//detects identifier list
			getNextToken();
			IDs();
		}
		else {
			Empty();
		}

	}
	else {
		errorMessage("<Identifer>", 1, "<IDs>");
	}
}


void StatementList() {
	if (printSyntax) {
		output << "\t<Statement List> -> <Statement> | <Statement> <Statement List>\n";
	}
	while (currentToken.token == "if" || currentToken.token == "put" || currentToken.token == "get" || currentToken.token == "while" || currentToken.token == "return" || currentToken.FSM_ID_Number == 1) {
		Statement();
	}
}



void Statement() {
	if (printSyntax) {
		output << "\t<Statement> -> <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While> \n";
	}

	if (currentToken.token == "{") {
		Compound();
	}

	else if (currentToken.FSM_ID_Number == 1) {
		Assign();
	}

	else if (currentToken.token == "if") {
		If();
	}

	else if (currentToken.token == "return") {
		Return();
	}

	else if (currentToken.token == "put") {
		Print();
	}

	else if (currentToken.token == "get") {
		Scan();
	}

	else if (currentToken.token == "while") {
		While();
	}

	else {
		errorMessage("<Statement>", 1, "<Statement>");
	}
}


void Compound() {
	if (printSyntax) {
		output << "\t<Compound> -> { <Statement List> } \n ";
	}

	if (currentToken.token == "{") {
		getNextToken();
		StatementList();

		if (currentToken.token == "}") {
			getNextToken();
		}

		else {
			errorMessage("}", 2, "<Compound>");
		}
	}
}


void Assign() {
	if (printSyntax) {
		output << "\t<Assign> -> <Identifier> = <Expression>;\n";
	}

	if (currentToken.FSM_ID_Number == 1) {
		save = currentToken.token;
		getNextToken();

		if (currentToken.token == "=") {
			getNextToken();
			Expression();
			addr = get_addr(save);
			type = get_type(save);
			typestack.push(type);
			//checkType("assignment");
			generate_instr("POPM", addr);


			if (currentToken.token == ";") {
				getNextToken();
			}

			else {
				errorMessage("';'", 2, "<Assign>");
			}
		}

		else {
			errorMessage("'='", 1, "<Assign>");
		}
	}

	else {
		errorMessage("Identifier", 2, "<Assign>");
	}
}


void If() {
	if (printSyntax) {
		output << "\t<If> -> if ( <Condition> ) <Statement> endif | if ( <Condition> ) <Statement> else <Statement> endif\n";
	}

	if (currentToken.token == "if") {
		getNextToken();
		if (currentToken.token == "(") {
			getNextToken();
			Condition();
			if (currentToken.token == ")") {
				getNextToken();
				Statement();
				if (currentToken.token == "endif") {
					getNextToken();
				}
				else if (currentToken.token == "else") {
					getNextToken();
					Statement();

					if (currentToken.token == "endif") {
						getNextToken();
					}
					else {
						errorMessage("'endif'", 2, "<Condition>");
					}
				}
				else {
					errorMessage("'endif' or 'else'", 2, "<Condition>");
				}
			}
			else {
				errorMessage("')'", 2, "<Condition>");
			}
		}
		else {
			errorMessage("'('", 2, "<Condition>");
		}

	}

}


void Return() {
	if (printSyntax) {
		output << "\t<Return> -> return; |  return <Expression>;\n";
	}

	getNextToken();

	if (currentToken.token == ";") {
		getNextToken();
	}

	else {
		Expression();
		if (currentToken.token == ";") {
			getNextToken();
		}
		else {
			errorMessage("';'", 1, "<Return>");
		}
	}
}


void Print() {
	if (printSyntax) {
		output << "\t<Print> -> put ( <Expression> );\n";
	}

	getNextToken();

	if (currentToken.token == "(") {
		getNextToken();
		Expression();
		generate_instr("STDOUT", null_addr);

		if (!typestack.empty()) {
			typestack.pop();
		}
		else {
			output << "Error: Stack is empty." << endl;
			exit(1);
		}

		if (currentToken.token == ")") {
			getNextToken();

			if (currentToken.token == ";") {
				getNextToken();
			}
			else {
				errorMessage("';'", 2, "<Expression>");
			}
		}
		else {
			errorMessage("')'", 1, "<Expression>");
		}
	}
	else {
		errorMessage("'('", 1, "<Expression>");
	}
}


void Scan() {
	if (printSyntax) {
		output << "\t<Scan> -> get ( <IDs> );\n";
	}

	getNextToken();

	if (currentToken.token == "(") {
		getNextToken();
		IDs();

		if (currentToken.token == ")") {
			getNextToken();

			if (currentToken.token == ";") {
				getNextToken();
			}
			else {
				errorMessage("';'", 2, "<Scan>");
			}
		}
		else {
			errorMessage("')'", 1, "<Scan>");
		}
	}

	else {
		errorMessage("'('", 1, "<Scan>");
	}
}


void While() {
	if (printSyntax) {
		output << "\t<While> -> while ( <Condition> )  <Statement>\n";
	}

	if (currentToken.token == "while") {
		instr_address = current_instr;
		generate_instr("LABEL", null_addr);
		getNextToken();


		if (currentToken.token == "(") {
			getNextToken();
			Condition();
			if (currentToken.token == ")") {
				getNextToken();
				Statement();
				generate_instr("JUMP", instr_address);
				back_patch(current_instr);
			}
			else {
				errorMessage("')'", 1, "<While>");
			}
		}
		else {
			errorMessage("'('", 1, "<While>");
		}
	}

	else {
		errorMessage("'('", 1, "<While>");
	}
}


void Condition() {
	if (printSyntax) {
		output << "\t<Condition> -> <Expression>  <Relop>  <Expression>\n";
	}

	Expression();
	Relop();
	Expression();

	//checkType("condition");
	if (operatorstring == "<") {				//generates the appropriate instruction based on the operator parsed and adds to the stack
		generate_instr("LES", null_addr);
	}
	else if (operatorstring == ">") {
		generate_instr("GRE", null_addr);
	}
	else if (operatorstring == "==") {
		generate_instr("EQU", null_addr);
	}
	else if (operatorstring == "^=") {
		generate_instr("NEQ", null_addr);
	}
	else if (operatorstring == "=>") {
		generate_instr("GEQ", null_addr);
	}
	else if (operatorstring == "=<") {
		generate_instr("LEQ", null_addr);
	}
	else {
		errorMessage("Legal Operator", 2, "<Condition>");
	}

	jumpstack.push(current_instr);
	generate_instr("JUMPZ", null_addr);
}


void Relop() {
	
	operatorstring = currentToken.token;
	if (currentToken.token == "==") {				//checks for "=="
		if (printSyntax) {
			output << "\t<Relop> -> ==" << endl;
		}
	}
	else if (currentToken.token == "^=") {			//checks for "^="
		if (printSyntax) {
			output << "\t<Relop> -> ^=" << endl;
		}
	}
	else if (currentToken.token == ">") {			//checks for ">"
		if (printSyntax) {
			output << "\t<Relop> -> >" << endl;
		}
	}
	else if (currentToken.token == "<") {			//checks for "<"
		if (printSyntax) {
			output << "\t<Relop> -> <" << endl;
		}
	}
	else if (currentToken.token == ">=") {			//checks for ">="
		if (printSyntax) {
			output << "\t<Relop> -> >=" << endl;
		}
	}
	else if (currentToken.token == "<=") {			//checks for "<="
		if (printSyntax) {
			output << "\t<Relop> -> <=" << endl;
		}
	}
	else {
		errorMessage("valid relational operator (== | ^= | > | < | >= | <=)", 1, "<Relop>");
	}

	getNextToken();
}


void Expression() {
	if (printSyntax) {
		output << "\t<Expression> -> <Term> <ExpressionPrime>\n";
	}

	Term();
	ExpressionPrime();
}


void ExpressionPrime() {
	if (printSyntax) {
		output << "\t<ExpressionPrime> -> + <Term> <ExpressionPrime> | - <Term> <ExpressionPrime> | <Empty>   \n";
	}

	if (currentToken.token == "+" || currentToken.token == "-") {
		string operator_exprPrime = currentToken.token;
		getNextToken();
		Term();

		if (operator_exprPrime == "+") {
			generate_instr("ADD", null_addr);
		}
		else if (operator_exprPrime == "-") {
			generate_instr("SUB", null_addr);
		}

		typestack.push(type);
		ExpressionPrime();
	}

	else if (currentToken.FSM_ID_Number == 0) {
		errorMessage("'+', '-', or blankspace", 1, "<ExpressionPrime>");
	}

	else {
		Empty();
	}
}


void Term() {
	if (printSyntax) {
		output << "\t<Term> -> <Factor> <TermPrime>\n";
	}

	Factor();
	TermPrime();
}



void TermPrime() {
	if (printSyntax) {
		output << "\t<TermPrime> -> * <Factor> <TermPrime> | / <Factor> <TermPrime> | <Empty>\n";
	}

	if (currentToken.token == "*" || currentToken.token == "/") {
		string op_check = currentToken.token;
		getNextToken();
		Factor();

		if (op_check == "*") {
			generate_instr("MUL", null_addr);
		}
		else if (op_check == "/") {
			generate_instr("DIV", null_addr);
		}

		typestack.push(type);
		TermPrime();
	}

	else if (currentToken.FSM_ID_Number == 0) {
		errorMessage("'*', '/', or blankspace", 1, "<TermPrime>");
	}

	else {
		Empty();
	}
}



void Factor() {
	if (printSyntax) {
		output << "\t<Factor> -> - <Primary> | <Primary>\n";
	}

	if (currentToken.token == "-") {
		getNextToken();
		Primary();
	}

	else if (currentToken.FSM_ID_Number != 0) {
		Primary();
	}

	else {
		errorMessage("VALID INPUT", 2, "<Factor>");
	}
}


void Primary() {
	if (printSyntax) {
		output << "\t<Primary> -> <Identifier> | <Integer> | <Identifier> ( <IDs> ) | ( <Expression> ) | <Real> | true | false\n";
	}

	if (currentToken.FSM_ID_Number == 1) {						//if FSM state = 1
		generate_instr("PUSHM", addr);

		if (isNegative) {
			generate_instr("PUSHI", -1);
			generate_instr("MUL", null_addr);
			isNegative = false;
		}

		type = get_type(currentToken.token);
		typestack.push(type);
		getNextToken();

		if (currentToken.token == "(") {
			getNextToken();
			IDs();		

			if (currentToken.token == ")") {
				getNextToken();
			}
			else {
				errorMessage("')'", 1, "<Primary>");
			}
		}
		else {
			Empty();
		}
	}



	else if (currentToken.FSM_ID_Number == 2) {						//if FSM state = 2
		generate_instr("PUSHI", stoi(currentToken.token));
		if (isNegative) {
			generate_instr("PUSHI", -1);
			generate_instr("MUL", null_addr);
			isNegative = false;
		}

		typestack.push("int");
		getNextToken();
	}



	else if (currentToken.token == "(") {
		getNextToken();
		Expression();

		if (currentToken.token == ")") {
			getNextToken();
		}
		else {
			errorMessage("')'", 2, "<Primary>");
		}
	}


	else if (currentToken.FSM_ID_Number == 6) {				//if FSM state = 6
		getNextToken();
	}

	else if (currentToken.token == "false") {
		generate_instr("PUSHI", 0);
		if (isNegative) {
			output << "Error: Boolean cannot be positive or negative." << endl;
			exit(1);
		}

		typestack.push("boolean");
		getNextToken();
	}

	else if (currentToken.token == "true") {
		generate_instr("PUSHI", 1);
		if (isNegative) {
			output << "Error: Boolean cannot be positive or negative." << endl;
			exit(1);
		}

		typestack.push("boolean");
		getNextToken();
	}

	else {
		errorMessage("'<Identifer>', 'Integer', '<Expression>', or '<Qualifier>'", 1, "<Primary>");
	}
}


void Empty() {
	if (printSyntax) {
		output << "\t<Empty> -> Epsilon\n";
	}
}


void errorMessage(string missing, int type, string function) {
	switch (type) {
	case 1: {
		output << "\nError: Expected " << missing << " before '" << currentToken.token << "' on line: " << currentToken.lineNumber << endl;
		break;
	}
	case 2: {
		output << "\nError: Expected " << missing << " on line: " << currentToken.lineNumber << endl;
		break;
	}
	case 3: {
		output << "\nError: Expected " << missing << " between list on line: " << currentToken.lineNumber << endl;
		break;
	}

			output << endl << endl << "Errpr: Exit at line: " << currentToken.lineNumber << endl;
			output << endl << endl << "Error: Exit at function: " << function << endl;
			output << endl << endl << "Error: Exit at token: " << currentToken.token << endl;

			getNextToken();

			output << endl << "Next Token: " << currentToken.token;
			system("pause");
			exit(1);
	}

}



void generate_instr(string instruction, int address) {		//creates instruction to put in instruction table
	instrTable[current_instr][1] = instruction;

	if (address != null_addr) {
		instrTable[current_instr][2] = to_string(address);
	}

	current_instr++;
}


bool checkSymTable(string lexeme) {				//accepts a string and checks if it already exists in the symbol table
	bool found = 0;

	int i = 0;

	while (i < symbol) {
		if (symbolTable[i][1] == lexeme) {
			found = 1;
		}
		i++;
	}

	return found;
}



//void checkType(string check) {				//checks for matching types in stack, type is saved to push into typestack after generate_instr() of
//											//an arithmetic operation
//	if (!typestack.empty()) {
//		//save for future use for top/pop purposes
//		type = typestack.top();
//		typestack.pop();
//	}
//	else {
//		output << "Error: typestack is empty." << endl;
//		output.close();
//		exit(1);
//	}
//
//	if (!typestack.empty()) {
//		if (type == typestack.top()) {
//			typestack.pop();
//		}
//		else {
//			output << "Error: Type mismatch error." << endl;
//			output.close();
//			exit(1);
//		}
//	}
//	else {
//		output << "Error: typestack is empty." << endl;
//		output.close();
//		exit(1);
//	}
//
//													//check for booleans (for add/sub, mul/div or condition)
//	if (check != "assignment") {
//		if (type == "boolean") {
//			output << "Error: Booleans not allowed in arithmetic/conditional expressions." << endl;
//			output.close();
//			exit(1);
//		}
//	}
//}


void insertSymbol(string type, string lexeme) {						//accepts a lexeme and type to add to symbol table
	if (type != "int" & type != "boolean" & type != "real") {
		output << "Error: Expected type 'int', 'bool', 'real'" << endl;
		output << "Inserting symbol type: " << type << endl;
		output << "Inserting symbol type: " << lexeme << endl;
		output.close();
		exit(1);
	}

	symbolTable[symbol][1] = lexeme;
	symbolTable[symbol][2] = type;
	symbol++;
}



void printSymbolTable() {				//prints symbol table
	table << left << setw(15) << "__Symbol__" << left << setw(15) << "__Type__" << "__Memory Address__" << endl;

	int i = 0;

	while (i < symbol) {
		table << left << setw(15) << symbolTable[i][1] << left << setw(15) << symbolTable[i][2] << i + memory_address << endl;
		i++;
	}

	output << endl;
}


void printInstrTable() {					//prints instruction table
	table << left << setw(15) << "__Index__" << left << setw(15) << "__Operation__" << "__Operand__" << endl;

	int i = 1;

	while (i < current_instr) {
		table << left << setw(15) << i << left << setw(15) << instrTable[i][1] << instrTable[i][2] << endl;
		i++;
	}

	output << endl;
}



int get_addr(string lexeme) {			//takes a string and returns the address from the symbol table
	int addr = 0;
	int i = 0;

	while (i < symbol) {
		if (symbolTable[i][1] == lexeme) {
			addr = i + memory_address;
		}

		i++;
	}
	return addr;
}

string get_type(string lexeme) {		//takes a string and returns the type from the symbol table
	string type = "Error: Not found.";
	int i = 0;

	while (i < symbol) {
		if (symbolTable[i][1] == lexeme) {
			type = symbolTable[i][2];
		}
		i++;
	}

	if (type == "Error: not found.") {
		output << "Error: Symbol not found in the symbol table." << endl;
		output.close();
		exit(1);
	}

	return type;
}



void back_patch(int jump_addr) {		//gets address from top of stack, pops, and replaces it with a JUMP operand to current instr_addr
	if (!jumpstack.empty()) {
		addr = jumpstack.top();
		jumpstack.pop();
		instrTable[addr][2] = to_string(jump_addr);
	}
	else {
		output << "Error: Stack is empty." << endl;
		output.close();
		exit(1);
	}
}
