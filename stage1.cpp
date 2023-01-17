//Connor Flenniken
// Devoux Deysel
// CS 4301 - Alg Lang & Compilers
// Stage1

#include <stage1.h>
#include <ctime>
#include <cctype>
#include <iomanip>
#include <iterator>

bool negation = false;
bool notBool = false;

// *******************************
// ********* CONSTRUCTOR *********
// ********* DESTRUCTOR **********
// *******************************

// S0 Compiler()
Compiler::Compiler(char **argv) // constructor
{
	lineNo++;
	sourceFile.open(argv[1]);
	listingFile.open(argv[2]);
	objectFile.open(argv[3]);
}

// S0 ~Compiler()
Compiler::~Compiler() // destructor
{
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}

// ****************************
// *********** MAIN ***********
// ********* ROUTINES *********
// ****************************

// S0 createListingHeader()
void Compiler::createListingHeader()
{
	time_t now = time (NULL);
	
	 listingFile << "STAGE0:  Connor Flenniken, Devoux Deysel\t" << ctime(&now) << "\n";
	 listingFile << left << setw(22) << "LINE NO." << "SOURCE STATEMENT\n\n";
	 ch = '\n';
	 //line numbers and source statements should be aligned under the headings
}

// S0 parser()
void Compiler::parser()
{
	nextChar();
	//ch must be initialized to the first character of the source file
	nextToken();
	if (token != "program")
	{
		processError("keyword \"program\" expected\n");
	}
	prog();
}

// S0 createListingTrailer()
void Compiler::createListingTrailer()
{
	listingFile << setw(29) << left << "\nCOMPILATION TERMINATED" << errorCount;
	if (errorCount == 1)
		listingFile << " ERROR ENCOUNTERED\n";
	else
		listingFile << " ERRORS ENCOUNTERED\n";
}

// *********************************
// ********** PRODUCTIONS **********
// *********************************

// ************ S0 production1 prog() ***************
void Compiler::prog() //token should be "program"
{ 
	 if (token != "program")
		 processError("keyword \"program\" expected\n");
     progStmt();
	 if (token.compare("const") == 0)
		consts();
	 if (token.compare("var") == 0)
		vars();
	 if (token.compare("begin") != 0)
		processError("keyword \"begin\" expected\n");
     beginEndStmt();
	 if (token.compare("$") != 0)
		processError("no text may follow \"end\"\n");
}
// ************ S0 production2 progStmt() ***************
void Compiler::progStmt() //token should be "program"
{ 
	 string x;
	 if (token != "program")
		processError("keyword \"program\" expected\n");
	 x = nextToken();
	 insert(x,PROG_NAME,CONSTANT,x,NO,0);
	 if (!isNonKeyId(x))
		processError("program name expected\n");
	 nextToken();
	 if (token.compare(";") != 0)
		processError("semicolon expected\n");
	 nextToken();
	 code("program", x);
}
// ************ S0 production3 consts() ***************
void Compiler::consts() //token should be "const"
{ 
	string x ;
	if (token.compare("const") != 0)
		processError("keyword \"const\" expected\n");
	
	x = nextToken();
	if (!isNonKeyId(x))
		processError("non-keyword identifier must follow \"const\"\n");
	constStmts();
}

// ************ S0 production4 vars() ***************
void Compiler::vars() //token should be "var"
{ 
	 if (token.compare("var") != 0)
		processError("keyword \"var\" expected\n");
	
	 if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"var\"\n");
	 varStmts();
}
// ************ S1 production1 beginEndStmt() ***************
void Compiler::beginEndStmt() //token should be "begin"
{ 
	 if (token.compare("begin") != 0)
		processError("keyword \"begin\" expected\n");
	 nextToken();
	 execStmts();
	 if (token.compare("end") != 0)
		processError("keyword \"end\" expected\n");
	nextToken();
	 if (token.compare(".") != 0)
		processError("period expected\n");
	nextToken();
	code("end", ".");
}


// ************ S0 production6 constStmts() ***************
void Compiler::constStmts() //token should be NON_KEY_ID
{ 
	 string x,y;
	 if (!isNonKeyId(token))
		processError("non-keyword identifier expected\n");
	 x = token;
	 
	 if (nextToken() != "=")
     processError("\"=\" expected\n");
	 y = nextToken();
		
	 if (y.compare("+") != 0 && y.compare("-") != 0 && y.compare("not") != 0 && 
	 !isNonKeyId(y) && y.compare("true") != 0 && y.compare("false") != 0 && !isInteger(y))
		processError("token to right of \"=\" illegal\n");
	 
	 if (y.compare("+") == 0 || y.compare("-") == 0)
	 {
		if (!isInteger(nextToken()))
			processError("integer expected after sign\n");
		y = y + token;
	 }
	 
	 if (y.compare("not") == 0)
	 {
		 if (isNonKeyId(nextToken()) && symbolTable.find(token) != symbolTable.end() && symbolTable.at(token).getDataType() == BOOLEAN)
		 {
			 if (symbolTable.at(token).getValue().compare("true") == 0)
				y = "false";
			else
				y = "true";
		 }
		 else if (token.compare("true") == 0)
			y = "false";
		 else if (token.compare("false") == 0)
			y = "true";
		
		 else
			processError("boolean expected after \"not\"\n");
	 }
	 
	 nextToken();
	 if (token.compare(";") != 0)
		processError("semicolon expected\n");
	
	 if (whichType(y) != INTEGER && whichType(y) != BOOLEAN)
		 processError("data type of token on the right-hand side must be INTEGER or BOOLEAN\n");
	 insert(x,whichType(y),CONSTANT,whichValue(y),YES,1);
		
	 x = nextToken();		 
	 if (x.compare("begin") != 0 && x.compare("var") != 0 && !isNonKeyId(x))
	 {
		processError("non-keyword identifier, \"begin\", or \"var\" expected\n");
	 }
		
	 if (isNonKeyId(x) && !isKeyword(x))
		constStmts();
}

// ************ S0 production7 varStmts() ***************
void Compiler::varStmts() //token should be NON_KEY_ID
{ 
	 string x,y;
	 if (!isNonKeyId(token))
		 processError("non-keyword identifier expected\n");
	 x = ids();
		 
	 if (token.compare(":") != 0)
		 processError("\":\" expected\n");
	
	 nextToken();
	 if (token.compare("integer") != 0 && token.compare("boolean") != 0)
		processError("illegal type follows \":\"\n");
	 y = token;
	 insert(x,whichType(y),VARIABLE,"",YES,1);
	 
	 nextToken();
	 if (token.compare(";") != 0)
		 processError("semicolon expected\n");
	 
	 nextToken();
	 if (token.compare("begin") != 0 && !isNonKeyId(token))
	 {
		processError("non-keyword identifier or \"begin\" expected\n");
	 }
	
	 if (isNonKeyId(token) && !isKeyword(token))
		varStmts();
}

// ************ S0 production8 ids() ***************
string Compiler::ids() //token should be NON_KEY_ID
{ 
	 string temp = "", tempString = "";
	 if (!isNonKeyId(token) || isKeyword(token))
	 {
		 processError("non-keyword identifier expected\n");
	 }
	 temp += token;
	 tempString += token;
	 
	 nextToken();
	 if (token.compare(",") == 0)
	 {
		 if (!isNonKeyId(nextToken()))
		 {
			 processError("non-keyword identifier expected\n");
		 }
		 tempString = temp + "," + ids();
	 }
	return tempString;
}


// ************ S1 production2 execStmts() ***************
void Compiler::execStmts()
{
	if (token != "end")
	{
		execStmt();
		execStmts();
	}
}

// ************ S1 production3 execStmt() ***************
void Compiler::execStmt()
{
	if (token == "read")
		readStmt();
	else if (token == "write")
		writeStmt();
	else if (isNonKeyId(token) && !isKeyword(token))
		assignStmt();
	else
		processError("non-keyword identifier, \"read\", or \"write\" expected\n");
}

// ************ S1 production4 assignStmt() ***************
void Compiler::assignStmt()
{
	pushOperand(token);
	nextToken();
	if (token != ":=")
		processError("\":=\" expected\n");
	pushOperator(token);
	nextToken();
	express();
	if (token != ";")
		nextToken();
	if (token != ";")
		processError("semicolon expected\n");
	string operand1 = popOperand();
	string operand2 = popOperand();
	code(popOperator(), operand1, operand2);
	nextToken();
}

// ************ S1 production5 readStmt() ***************
void Compiler::readStmt()
{
	if (token != "read")
		processError("keyword \"read\" expected");
	nextToken();
	if (token != "(")
		processError("\"(\" expected\n");
	nextToken();
	string x = ids();
	if (token != ")")
		processError("\")\" expected\n");
	nextToken();
	if (token != ";")
		processError("semicolon expected\n");
	code("read", x);
	nextToken();
}

// ************ S1 production7 writeStmt() ***************
void Compiler::writeStmt()
{
	if (token != "write")
		processError("keyword \"write\" expected\n");
	nextToken();
	if (token != "(")
		processError("\"(\" expected\n");
	nextToken();
	string x = ids();
	if (token != ")")
		processError("\',\' or \')\' expected after non-keyword identifier\n");
	nextToken();
	if (token != ";")
		processError("semicolon expected\n");
	code ("write", x);
	nextToken();
}


// ************ S1 production9 express() ***************
void Compiler::express()
{
	term();
	expresses();
}


// ************ S1 production10 expresses() ***************
void Compiler::expresses()
{
	if (token == "=" || token == "<" || token == ">" || token == "<=" || token == ">=" || token == "<>")
	{
		pushOperator(token);
		nextToken();
		term();
		string operand1 = popOperand();
		string operand2 = popOperand();
		string operator1 = popOperator();
		code(operator1, operand1, operand2);
		if (token == "=" || token == "<" || token == ">" || token == "<=" || token == ">=" || token == "<>")
			expresses();
		if (isNonKeyId(token))
			processError("one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected");
	}
}

// ************ S1 production11 term() ***************
void Compiler::term()
{
	factor();
	terms();
}

// ************ S1 production12 terms() ***************
void Compiler::terms()
{
	if (token == "+" || token == "-" || token == "or")
	{
		pushOperator(token);
		nextToken();
		factor();
		string operand1 = popOperand();
		string operand2 = popOperand();
		code(popOperator(), operand1, operand2);
		if (token == "+" || token == "-" || token == "or")
			terms();
		if (isNonKeyId(token))
			processError("one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected");
	}
}

// ************ S1 production13 factor() ***************
void Compiler::factor()
{
	part();
	if (!operandStk.empty())
	{
		string op = popOperator();
		if ((negation || notBool) && (op == "not" || op == "neg"))
		{
			code(op, popOperand());
			negation = false;
			notBool = false;
		}
		else
			pushOperator(op);
	}
	factors();
}

// ************ S1 production14 factors() ***************
void Compiler::factors()
{
	if (token == "*" || token == "div" || token == "mod" || token == "and")
	{
		pushOperator(token);
		nextToken();
		part();
		string operand1 = popOperand();
		string operand2 = popOperand();
		code(popOperator(), operand1, operand2);
		if (token == "*" || token == "div" || token == "mod" || token == "and")
			factors();
		if (isNonKeyId(token))
			processError("one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected");
	}
}

// ************ S1 production15 part() ***************
void Compiler::part()
{
	bool negative = false;
	if (token == "+")
	{
		nextToken();
		if ((!isInteger(token) && token != "(" && !isNonKeyId(token)) || isKeyword(token))
			processError("expected \'(\', integer, or non-keyword id; found " + token);
	}
	if (token == "-")
	{
		negative = true;
		nextToken();
		if ((!isInteger(token) && token != "(" && !isNonKeyId(token)) || isKeyword(token))
			processError("expected \'(\', integer, or non-keyword id; found " + token);
		
		if (token == "(")
		{
			nextToken();
			express();
			if (token != ")")
			processError("\")\" expected");
			nextToken();
			negation = true;
			pushOperator("neg");
		}
		if (isNonKeyId(token))
		{
			negation = true;
			pushOperator("neg");
		}
	}
	if (token == "not")
	{
		nextToken();
		if ((!isBoolean(token) && token != "(" && !isNonKeyId(token)) || isKeyword(token))
			processError("expected \'(\', boolean, or non-keyword id; found " + token);
		if (token == "(")
		{
			nextToken();
			express();
			if (token != ")")
			processError("\")\" expected");
			nextToken();
		}
		else if (isBoolean(token) || isNonKeyId(token))
		{
			notBool = true;
			pushOperator("not");
			pushOperand(token);
			nextToken();
			return;
		}
		notBool = true;
		pushOperator("not");
	}
	if (token == "(")
	{
		nextToken();
		express();
		if (token != ")")
			processError("\")\" expected");
		nextToken();
	}
	if (isInteger(token))
	{
		if (negative && !negation)
		{
			string tempToken = token;
			token = "-";
			token.append(tempToken);
		}
		
		if (symbolTable.find(token) == symbolTable.end())
		{
			bool found = false;
			map<string, SymbolTableEntry>::iterator itr;
			for (itr = symbolTable.begin(); itr != symbolTable.end(); itr++)
			{
				if (itr->second.getValue() == token)
				{
					found = true;
					break;
				}
			}
			if (found)
				token = itr->first;
			else
			{
				insert(token, INTEGER, CONSTANT, token, YES, 1);
			}
		}
		pushOperand(token);
		nextToken();
	}
	if (isBoolean(token))
	{
		if (symbolTable.find(token) == symbolTable.end())
		{
			if (token == "false")
			{
				insert("false", UNKNOWN, CONSTANT, "0", YES, 1);
				symbolTable.at("false").setInternalName("FALSE");
				symbolTable.at("false").setDataType(BOOLEAN);
			}
			if (token == "true")
			{
				insert("true", UNKNOWN, CONSTANT, "-1", YES, 1);
				symbolTable.at("true").setInternalName("TRUE");
				symbolTable.at("true").setDataType(BOOLEAN);
			}
		}
		pushOperand(token);
		nextToken();
	}
	if (isNonKeyId(token) && !isKeyword(token) && token != "+")
	{
		pushOperand(token);
		nextToken();
	}
}

// ****************************
// ********** HELPER **********
// ******** FUNCTIONS *********
// ****************************

// S1 isKeyword()
// DONE
bool Compiler::isKeyword(string s) const  // determines if s is a keyword
{
	if (s == "program")
	{
		return true;
	}
	else if (s == "begin")
	{
		return true;
	}
	else if (s == "end")
	{
		return true;
	}
	else if (s == "var")
	{
		return true;
	}
	else if (s == "const")
	{
		return true;
	}
	else if (s == "var")
	{
		return true;
	}
	else if (s == "boolean")
	{
		return true;
	}
	else if (s == "not")
	{
		return true;
	}
	else if (s == "mod")
	{
		return true;
	}
	else if (s == "div")
	{
		return true;
	}
	else if (s == "and")
	{
		return true;
	}
	else if (s == "or")
	{
		return true;
	}
	else if (s == "read")
	{
		return true;
	}
	else if (s == "write")
	{
		return true;
	}
	
	return false;
}

// S1 isSpecialSymbol()
// DONE
bool Compiler::isSpecialSymbol(char c) const // determines if c is a special symbol
{
	if (c == '=')
		return true;
	if (c == ':')
		return true;
	if (c == ',')
		return true;
	if (c == ';')
		return true;
	if (c == '.')
		return true;
	if (c == '+')
		return true;
	if (c == '-')
		return true;
	if (c == '*')
		return true;
	if (c == '(')
		return true;
	if (c == ')')
		return true;
	if (c == '<')
		return true;
	if (c == '>')
		return true;
	return false;
}

// S0 isNonKeyId()
bool Compiler::isNonKeyId(string s) const // determines if s is a non_key_id
{
	if (s.empty())
		return false;
	if (s.at(0) == '_' || s.at(s.length() - 1) == '_' || isdigit(s.at(0)) || (isalnum(s.at(0)) && isupper(s.at(0))))
		return false;
	char char1;
	for (uint i = 0; i < s.length(); i++) 
	{
		char1 = s.at(i);
		if (!isalnum(char1) && char1 != '-' && char1 != '+' && char1 != '_')
		{
			return false;
		}
		else if (isalnum(char1) && isupper(char1))
		{
			return false;
		}
		else if (isSpecialSymbol(char1))
			return false;

	}
	
	return true;
}

// S0 isInteger()
bool Compiler::isInteger(string s) const  // determines if s is an integer
{
	if ((s[0] == '+' || s[0] == '-') && s.length() == 1)
		return false;
	char char1;
	for (uint i = 0; i < s.length(); i++)
	{
		char1 = s.at(i);
		if (!isdigit(char1) && s[0] != '+' && s[0] != '-')
			return false;
	}
	return true;
}

// S0 isBoolean()
bool Compiler::isBoolean(string s) const  // determines if s is a boolean
{
	if (s.compare("true") == 0 || s.compare("false") == 0)
		return true;
	return false;
}

// S0 isLiteral()
bool Compiler::isLiteral(string s) const  // determines if s is a literal
{
	if (s.compare("integer") == 0 || s.compare("boolean") == 0)
		return true;
	
	if (isInteger(s) || isBoolean(s))
		return true;
	
	return false;
}


// ****************************
// ********** ACTION **********
// ********* ROUTINES *********
// ****************************

// ************************* S0 INSERT() ****************************
//create symbol table entry for each identifier in list of external names
//Multiply inserted names are illegal
void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
	 string name = "";
	 int comma = externalName.find(',');
	 if (comma != -1)
	 {
		 name = externalName.substr(0, comma);
		 externalName = externalName.substr((comma + 1), externalName.length());
	 }
	 else
	 {
		 name = externalName;
		 externalName = "";
	 }
	 
	 while (!name.empty())
	 {
		 if (symbolTable.size() > 256)
			processError("symbol table overflow\n");
		 if (symbolTable.find(name) != symbolTable.end())
			processError("symbol " + name + " is multiply defined\n");
		 else if (isKeyword(name))
			processError("illegal use of keyword\n");
		 else //create table entry
		 {
			if (isupper(name.at(0)))
				symbolTable.insert(pair<string, SymbolTableEntry>(name, SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)));
			else
				symbolTable.insert(pair<string, SymbolTableEntry>(name, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)));
		 }
		 
		 comma = externalName.find(',');
		 if (comma != -1)
		 {
			 name = externalName.substr(0, comma);
			 externalName = externalName.substr((comma + 1), externalName.length());
		 }
		 else
		 {
			 name = externalName;
			 externalName = "";
		 }
	 }
}


// ************************* S0 whichType() ****************************
storeTypes Compiler::whichType(string name) //tells which data type a name has
{ 
	storeTypes data_type;
	
	//listingFile << name;
	 if (isLiteral(name))
	 {
		if (name == "boolean" || name == "true" || name == "false")
			data_type = BOOLEAN;
		else
			data_type = INTEGER;
	 }
	 else //name is an identifier and hopefully a constant
	 {
		if (symbolTable.find(name) != symbolTable.end())
			data_type = symbolTable.find(name)->second.getDataType(); 
		else
		{
			processError("reference to undefined symbol " + name + "\n");
		}
	 }
	return data_type;
}

// ************************* S0 whichValue() ****************************
string Compiler::whichValue(string name) //tells which value a name has
{ 
	string value;
	 if (isLiteral(name))
		value = name;	
	 else //name is an identifier and hopefully a constant
	 {
		if (symbolTable.find(name) != symbolTable.end() && !(symbolTable.find(name)->second.getValue().empty()))
			value = symbolTable.find(name)->second.getValue();
		else
			processError("reference to undefined symbol " + name + "\n");
	 }
	return value;
}

// ************ S1 code() ***************
void Compiler::code(string op, string operand1, string operand2)
{
	 if (op.compare("program") == 0)
		emitPrologue(operand1);
	 else if (op.compare("end") == 0)
		emitEpilogue();
	 else if (op == "read")
		 emitReadCode(operand1);
	 else if (op == "write")
		 emitWriteCode(operand1);
	 else if (op == ":=")
		 emitAssignCode(operand1, operand2);
	 else if (op == "+")
		 emitAdditionCode(operand1, operand2);
	 else if (op == "-")
		 emitSubtractionCode(operand1, operand2);
	 else if (op == "neg")
		 emitNegationCode(operand1);
	 else if (op == "not")
		 emitNotCode(operand1);
	 else if (op == "*")
		 emitMultiplicationCode(operand1, operand2);
	 else if (op == "div")
		 emitDivisionCode(operand1, operand2);
	 else if (op == "mod")
		 emitModuloCode(operand1, operand2);
	 else if (op == "and")
		 emitAndCode(operand1, operand2);
	 else if (op == "or")
		 emitOrCode(operand1, operand2);
	 else if (op == "=")
		 emitEqualityCode(operand1, operand2);
	 else if (op == "<>")
		 emitInequalityCode(operand1, operand2);
	 else if (op == "<")
		 emitLessThanCode(operand1, operand2);
	 else if (op == "<=")
		 emitLessThanOrEqualToCode(operand1, operand2);
	 else if (op == ">")
		 emitGreaterThanCode(operand1, operand2);
	 else if (op == ">=")
		 emitGreaterThanOrEqualToCode(operand1, operand2);
	 else
		processError("compiler error since function code should not be called with illegal arguments\n");
}

// ************ S1 pushOperator() ***************
void Compiler::pushOperator(string op)
{
	operatorStk.push(op);
}

// ************ S1 popOperator() ***************
string Compiler::popOperator()
{
	if (!operatorStk.empty())
	{
		string value = operatorStk.top();
		operatorStk.pop();
		return value;
	}
	else
		processError("operator stack underflow\n");
	return "";
}

// ************ S1 pushOperand() ***************
void Compiler::pushOperand(string operand)
{
	if (isLiteral(operand) && symbolTable.find(operand) == symbolTable.end())
	{
		insert(operand, whichType(operand), VARIABLE, operand, YES, 1);
	}
	operandStk.push(operand);
}

// ************ S1 popOperand() ***************
string Compiler::popOperand()
{
	if (!operandStk.empty())
	{
		string value = operandStk.top();
		operandStk.pop();
		return value;
	}
	else
		processError("operand stack underflow\n");
	return "";
}


// ****************************
// *********** EMIT ***********
// ********* FUNCTIONS ********
// ****************************

// ************ S0 emit() ***************
void Compiler::emit(string label, string instruction, string operands, string comment)
{
	objectFile << left;
	objectFile << setw(8) << label;
	objectFile << setw(8) << instruction;
	objectFile << setw(24) << operands;
	objectFile << comment;
	objectFile << endl;
}

// ************ S0 emitPrologue() ***************
void Compiler::emitPrologue(string progName, string operand2)
{
	time_t now = time (NULL);
	
	 objectFile << "; Connor Flenniken, Devoux Deysel\t" << ctime(&now);
	 objectFile << "%INCLUDE \"Along32.inc\"\n" << "%INCLUDE \"Macros_Along.inc\"\n\n";
	 emit("SECTION", ".text", "", "");
	 emit("global", "_start", "", "; program " + progName);
	 objectFile << endl;
	 emit("_start:", "", "", "");
}

// ************ S0 emitEpilogue() ***************
void Compiler::emitEpilogue(string operand1, string operand2)
{
	 emit("","Exit", "{0}", "");
	 objectFile << endl;
	 emitStorage();
}

// ************ S0 emitStorage() ***************
void Compiler::emitStorage()
{
	 emit("SECTION", ".data", "", "");
	 for (auto& entry : symbolTable)
	 {
		 if (entry.second.getMode() == CONSTANT && entry.second.getAlloc() == YES)
		 {
			 if (entry.second.getValue() == "false")
				 emit(entry.second.getInternalName(), "dd", "0", ("; " + entry.first));
			 else if (entry.second.getValue() == "true")
				 emit(entry.second.getInternalName(), "dd", "-1", ("; " + entry.first));
			 else
				emit(entry.second.getInternalName(), "dd", entry.second.getValue(), ("; " + entry.first));
		 }
	 }
	 objectFile << endl;
	 emit("SECTION", ".bss", "", "");
	 for (auto& entry : symbolTable)
	 {
		 if (entry.second.getMode() == VARIABLE && entry.second.getAlloc() == YES)
		 {
			 emit(entry.second.getInternalName(), "resd", "1", ("; " + entry.first));
		 }
	 }
}


// ************ emitReadCode() ***************
void Compiler::emitReadCode(string operand, string operand2)
{
	string name = "";
	int comma = operand.find(',');
	 if (comma != -1)
	 {
		 name = operand.substr(0, comma);
		 operand = operand.substr((comma + 1), operand.length());
	 }
	 else
	 {
		 name = operand;
		 operand = "";
	 }
	 
	 while (name != "")
	 {
		 if (symbolTable.find(name) == symbolTable.end())
			 processError("reference to undefined symbol " + name + "\n");
		 if (whichType(name) != INTEGER)
			 processError("Can't read variables of this type\n");
		 if (symbolTable.at(name).getMode() != VARIABLE)
			 processError("attempting to read to a read-only location\n");
		 emit("", "call", "ReadInt", "; read int; value placed in eax");
		 emit("", "mov", ("[" + symbolTable.at(name).getInternalName() + "],eax"), ("; store eax at " + name));
		 contentsOfAReg = name;
		 
		 comma = operand.find(',');
		 if (comma != -1)
		 {
			 name = operand.substr(0, comma);
			 operand = operand.substr((comma + 1), operand.length());
		 }
		 else
		 {
			 name = operand;
			 operand = "";
		 }
	 }
}

// ************ emitWriteCode() ***************
void Compiler::emitWriteCode(string operand, string operand2)
{
	string name = "";
	int comma = operand.find(',');
	if (comma != -1)
	 {
		 name = operand.substr(0, comma);
		 operand = operand.substr((comma + 1), operand.length());
	 }
	 else
	 {
		 name = operand;
		 operand = "";
	 }
	 
	 while (name != "")
	 {
		 if (symbolTable.find(name) == symbolTable.end())
			 processError("reference to undefined symbol " + name + "\n");
		 if (contentsOfAReg != name)
		 {
			 emit("", "mov", ("eax,[" + symbolTable.at(name).getInternalName() + "]"), ("; load " + name + " in eax"));
			 contentsOfAReg = name;
		 }
		 if (whichType(name) == INTEGER || whichType(name) == BOOLEAN)
			 emit("", "call", "WriteInt", "; write int in eax to standard out");
		 
		 emit("", "call", "Crlf", "; write \\r\\n to standard out");
		
		 comma = operand.find(',');
		 if (comma != -1)
		 {
			 name = operand.substr(0, comma);
			 operand = operand.substr((comma + 1), operand.length());
		 }
		 else
		 {
			 name = operand;
			 operand = "";
		 }
	 }
	
}

// ************ emitAssignCode() ***************
// op2 = op1
void Compiler::emitAssignCode(string operand1, string operand2)
{
	if (whichType(operand1) != whichType(operand2))
	{
		processError("incompatible types for operator \':=\'\n");
	}
	if (symbolTable.at(operand2).getMode() != VARIABLE)
		processError("symbol on left-hand side of assignment must have a storage mode of VARIABLE\n");
	if (operand1 == operand2)
		return;
	if (contentsOfAReg != operand1)
	{
		emit("", "mov", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; AReg = " + operand1));
		contentsOfAReg = operand1;
	}
	emit("", "mov", ("[" + symbolTable.at(operand2).getInternalName() + "],eax"), ("; " + operand2 + " = AReg"));
	contentsOfAReg = operand2;
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
}

// ************ emitAdditionCode() ***************
// op2 + op1
void Compiler::emitAdditionCode(string operand1, string operand2)
{
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER)
		processError("binary \'+\' requires integer operands\n");
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";
		
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand2));
		contentsOfAReg = operand2;
	}
	if (contentsOfAReg == operand1)
		emit("", "add", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand1 + " + " + operand2));
	else if (contentsOfAReg == operand2)
		emit("", "add", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; AReg = " + operand2 + " + " + operand1));
	// Deassign all temps and free the names for use.
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitSubtractionCode() ***************
// op2 - op1
void Compiler::emitSubtractionCode(string operand1, string operand2)
{
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER)
		processError("binary \'-\' requires integer operands\n");
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
	{
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
		contentsOfAReg = "";
	
	if (contentsOfAReg != operand2)
		emit("", "mov", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand2));
	emit("", "sub", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; AReg = " + operand2 + " - " + operand1));
	// Deassign all temps and free the names for use.
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitMultiplicationCode() ***************
// op2 * op1
void Compiler::emitMultiplicationCode(string operand1, string operand2)
{
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER)
		processError("binary \'*\' requires integer operands\n");
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";
		
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand2));
		contentsOfAReg = operand2;
	}
	if (contentsOfAReg == operand2)
		emit("", "imul", ("dword [" + symbolTable.at(operand1).getInternalName() + "]"), ("; AReg = " + operand2 + " * " + operand1));
	else if (contentsOfAReg == operand1)
		emit("", "imul", ("dword [" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand1 + " * " + operand2));
	// Deassign all temps and free the names for use.
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitDivisionCode() ***************
// op2 / op1
void Compiler::emitDivisionCode(string operand1, string operand2)
{
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER)
		processError("binary \'div\' requires integer operands\n");
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
	{
		// emit(); to store temp in memory
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		// change allocate in temp to YES
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		// deassign it
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
		contentsOfAReg = "";
	
	if (contentsOfAReg != operand2)
		emit("", "mov", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand2));
	emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");
	emit("", "idiv", ("dword [" + symbolTable.at(operand1).getInternalName() + "]"), ("; AReg = " + operand2 + " div " + operand1));
	
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitModuloCode() ***************
// op2 % op1
void Compiler::emitModuloCode(string operand1, string operand2)
{
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER)
		processError("binary \'mod\' requires integer operands\n");
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
	{
		// emit(); to store temp in memory
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		// change allocate in temp to YES
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		// deassign it
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand2)
		contentsOfAReg = "";
	
	if (contentsOfAReg != operand2)
		emit("", "mov", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand2));
	emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");
	emit("", "idiv", ("dword [" + symbolTable.at(operand1).getInternalName() + "]"), ("; AReg = " + operand2 + " div " + operand1));
	emit("", "xchg", "eax,edx", "; exchange quotient and remainder");
	
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitNegationCode() ***************
// -op1
void Compiler::emitNegationCode(string operand1, string)
{
	if (whichType(operand1) != INTEGER)
		processError("unary \'-\' requires an integer operand\n");
	
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
	{
		// emit(); to store temp in memory
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		// change allocate in temp to YES
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		// deassign it
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
		contentsOfAReg = "";
	
	if (contentsOfAReg != operand1)
		emit("", "mov", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; AReg = " + operand1));
	emit("", "neg", "eax", "; AReg = -AReg");
	
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitNotCode() ***************
// !op1
void Compiler::emitNotCode(string operand1, string)
{
	if (whichType(operand1) != BOOLEAN)
		processError("unary \'not\' requires a boolean operand\n");
	
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
	{
		// emit(); to store temp in memory
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		// change allocate in temp to YES
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		// deassign it
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
		contentsOfAReg = "";

	if (operand1 != "true" && operand1 != "false")
	{
		if (contentsOfAReg != operand1)
			emit("", "mov", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; AReg = " + operand1));
		emit("", "not", "eax", "; AReg = !AReg");
	}
	
	else if (operand1 == "true")
	{
		emit("", "mov", "eax,[FALSE]", "; AReg = false");
		if (symbolTable.find("false") == symbolTable.end())
		{	
			insert("false", UNKNOWN, CONSTANT, "0", YES, 1);
			symbolTable.at("false").setInternalName("FALSE");
			symbolTable.at("false").setDataType(BOOLEAN);
		}
	}
	else if (operand1 == "false")
	{
		emit("", "mov", "eax,[TRUE]", "; AReg = true");
		if (symbolTable.find("true") == symbolTable.end())
		{	
			insert("true", UNKNOWN, CONSTANT, "-1", YES, 1);
			symbolTable.at("true").setInternalName("TRUE");
			symbolTable.at("true").setDataType(BOOLEAN);
		}
	}
	
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitAndCode() ***************
// op2 && op1
void Compiler::emitAndCode(string operand1, string operand2)
{
	if (whichType(operand1) != BOOLEAN || whichType(operand2) != BOOLEAN)
		processError("binary \'and\' requires boolean operands\n");
	
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		// emit(); to store temp in memory
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		// change allocate in temp to YES
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		// deassign it
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";
	
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; AReg = " + operand1));
		contentsOfAReg = operand1;
	}
	
	if (contentsOfAReg == operand1)
		emit("", "and", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand1 + " and " + operand2));
	else if (contentsOfAReg == operand2)
		emit("", "and", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; AReg = " + operand2 + " and " + operand1));
	
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitOrCode() ***************
// op2 || op1
void Compiler::emitOrCode(string operand1, string operand2)
{
	if (whichType(operand1) != BOOLEAN || whichType(operand2) != BOOLEAN)
		processError("binary \'or\' requires boolean operands\n");
	
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		// emit(); to store temp in memory
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		// change allocate in temp to YES
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		// deassign it
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";
	
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand2));
		contentsOfAReg = operand2;
	}
	
	if (contentsOfAReg == operand1)
		emit("", "or", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand1 + " or " + operand2));
	else if (contentsOfAReg == operand2)
		emit("", "or", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; AReg = " + operand2 + " or " + operand1));
	
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitEqualityCode() ***************
// op2 == op1
void Compiler::emitEqualityCode(string operand1, string operand2)
{
	if (whichType(operand1) != whichType(operand2))
		processError("binary \'=\' requires operands of the same type\n");
	
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		// emit(); to store temp in memory
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		// change allocate in temp to YES
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		// deassign it
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";
	
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand2));
		contentsOfAReg = operand2;
	}
	
	string label1 = getLabel();
	if (contentsOfAReg == operand1)
	{
		emit("", "cmp", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; compare " + operand1 + " and " + operand2));
		emit("", "je", label1, ("; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE"));
	}
	else if (contentsOfAReg == operand2)
	{
		emit("", "cmp", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; compare " + operand2 + " and " + operand1));
		emit("", "je", label1, ("; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE"));
	}
	
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
	if (symbolTable.find("false") == symbolTable.end())
	{	
		insert("false", UNKNOWN, CONSTANT, "0", YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
		symbolTable.at("false").setDataType(BOOLEAN);
	}
	
	string label2 = getLabel();
	emit("", "jmp", label2, "; unconditionally jump");
	
	emit((label1 + ":"), "", "", "");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
	if (symbolTable.find("true") == symbolTable.end())
	{
		insert("true", UNKNOWN, CONSTANT, "-1", YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
		symbolTable.at("true").setDataType(BOOLEAN);
	}
	
	emit((label2 + ":"), "", "", "");
	
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitInequalityCode() ***************
// op2 != op1
void Compiler::emitInequalityCode(string operand1, string operand2)
{
	if (whichType(operand1) != whichType(operand2))
		processError("binary \'<>\' requires operands of the same type\n\n");
	
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		// emit(); to store temp in memory
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		// change allocate in temp to YES
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		// deassign it
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";
	
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		emit("", "mov", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; AReg = " + operand1));
		contentsOfAReg = operand1;
	}
	
	string label1 = getLabel();
	if (contentsOfAReg == operand1)
	{
		emit("", "cmp", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; compare " + operand1 + " and " + operand2));
		emit("", "jne", label1, ("; if " + operand1 + " <> " + operand2 + " then jump to set eax to TRUE"));
	}
	else if (contentsOfAReg == operand2)
	{
		emit("", "cmp", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; compare " + operand2 + " and " + operand1));
		emit("", "jne", label1, ("; if " + operand1 + " <> "  + operand2 + " then jump to set eax to TRUE"));
	}
	
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
	if (symbolTable.find("false") == symbolTable.end())
	{	
		insert("false", UNKNOWN, CONSTANT, "0", YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
		symbolTable.at("false").setDataType(BOOLEAN);
	}
	
	string label2 = getLabel();
	emit("", "jmp", label2, "; unconditionally jump");
	
	emit((label1 + ":"), "", "", "");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
	if (symbolTable.find("true") == symbolTable.end())
	{
		insert("true", UNKNOWN, CONSTANT, "-1", YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
		symbolTable.at("true").setDataType(BOOLEAN);
	}
	
	emit((label2 + ":"), "", "", "");
	
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitLessThanCode() ***************
// op2 < op1
void Compiler::emitLessThanCode(string operand1, string operand2)
{
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER)
		processError("binary \'<\' requires integer operands\n");
	
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		// emit(); to store temp in memory
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		// change allocate in temp to YES
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		// deassign it
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";
	
	if (contentsOfAReg != operand2)
		emit("", "mov", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand2));
	
	string label1 = getLabel();
	emit("", "cmp", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; compare " + operand2 + " and " + operand1));
	emit("", "jl", label1, ("; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE"));
	
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
	if (symbolTable.find("false") == symbolTable.end())
	{	
		insert("false", UNKNOWN, CONSTANT, "0", YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
		symbolTable.at("false").setDataType(BOOLEAN);
	}
	
	string label2 = getLabel();
	emit("", "jmp", label2, "; unconditionally jump");
	
	emit((label1 + ":"), "", "", "");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
	if (symbolTable.find("true") == symbolTable.end())
	{
		insert("true", UNKNOWN, CONSTANT, "-1", YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
		symbolTable.at("true").setDataType(BOOLEAN);
	}
	
	emit((label2 + ":"), "", "", "");
	
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitLessThanOrEqualToCode() ***************
// op2 <= op1
void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2)
{
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER)
		processError("binary \'<=\' requires integer operands\n");
	
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		// emit(); to store temp in memory
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		// change allocate in temp to YES
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		// deassign it
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";
	
	if (contentsOfAReg != operand2)
		emit("", "mov", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand2));
	
	string label1 = getLabel();
	emit("", "cmp", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; compare " + operand2 + " and " + operand1));
	emit("", "jle", label1, ("; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE"));
	
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
	if (symbolTable.find("false") == symbolTable.end())
	{	
		insert("false", UNKNOWN, CONSTANT, "0", YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
		symbolTable.at("false").setDataType(BOOLEAN);
	}
	
	string label2 = getLabel();
	emit("", "jmp", label2, "; unconditionally jump");
	
	emit((label1 + ":"), "", "", "");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
	if (symbolTable.find("true") == symbolTable.end())
	{
		insert("true", UNKNOWN, CONSTANT, "-1", YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
		symbolTable.at("true").setDataType(BOOLEAN);
	}
	
	emit((label2 + ":"), "", "", "");
	
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitGreaterThanCode() ***************
// op2 > op1
void Compiler::emitGreaterThanCode(string operand1, string operand2)
{
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER)
		processError("binary \'>\' requires integer operands\n");
	
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		// emit(); to store temp in memory
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		// change allocate in temp to YES
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		// deassign it
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";
	
	if (contentsOfAReg != operand2)
		emit("", "mov", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand2));
	
	string label1 = getLabel();
	emit("", "cmp", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; compare " + operand2 + " and " + operand1));
	emit("", "jg", label1, ("; if " + operand2 + " > " + operand1 + " then jump to set eax to TRUE"));
	
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
	if (symbolTable.find("false") == symbolTable.end())
	{	
		insert("false", UNKNOWN, CONSTANT, "0", YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
		symbolTable.at("false").setDataType(BOOLEAN);
	}
	
	string label2 = getLabel();
	emit("", "jmp", label2, "; unconditionally jump");
	
	emit((label1 + ":"), "", "", "");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
	if (symbolTable.find("true") == symbolTable.end())
	{
		insert("true", UNKNOWN, CONSTANT, "-1", YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
		symbolTable.at("true").setDataType(BOOLEAN);
	}
	
	emit((label2 + ":"), "", "", "");
	
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}

// ************ emitGreaterThanOrEqualToCode() ***************
// op2 >= op1
void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2)
{
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER)
		processError("binary \'>=\' requires integer operands\n");
	
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
	{
		// emit(); to store temp in memory
		emit("", "mov", ("[" + contentsOfAReg + "],eax"), ("; deassign AReg"));
		// change allocate in temp to YES
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		// deassign it
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2)
		contentsOfAReg = "";
	
	if (contentsOfAReg != operand2)
		emit("", "mov", ("eax,[" + symbolTable.at(operand2).getInternalName() + "]"), ("; AReg = " + operand2));
	
	string label1 = getLabel();
	emit("", "cmp", ("eax,[" + symbolTable.at(operand1).getInternalName() + "]"), ("; compare " + operand2 + " and " + operand1));
	emit("", "jge", label1, ("; if " + operand2 + " >= " + operand1 + " then jump to set eax to TRUE"));
	
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
	if (symbolTable.find("false") == symbolTable.end())
	{	
		insert("false", UNKNOWN, CONSTANT, "0", YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
		symbolTable.at("false").setDataType(BOOLEAN);
	}
	
	string label2 = getLabel();
	emit("", "jmp", label2, "; unconditionally jump");
	
	emit((label1 + ":"), "", "", "");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
	if (symbolTable.find("true") == symbolTable.end())
	{
		insert("true", UNKNOWN, CONSTANT, "-1", YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
		symbolTable.at("true").setDataType(BOOLEAN);
	}
	
	emit((label2 + ":"), "", "", "");
	
	if (isTemporary(operand1))
	{
		symbolTable.at(operand1).setValue("");
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		symbolTable.at(operand2).setValue("");
		freeTemp();
	}
	
	// AReg = next available temp name and change type of whatever temp it is to integer.
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	// Push name of result onto stack.
	pushOperand(contentsOfAReg);
}


// ****************************
// ********* LEXICAL **********
// ********* ROUTINES *********
// ****************************

// S1 nextToken()
// DONE
string Compiler::nextToken() //returns the next token or end of file marker
{
	 token = "";
	 int tokenCounter = 0;
	 while (token.empty())
	 {
		 
		if (ch == '{')
		{
			while (nextChar() != '$' && ch != '}')
			{
			}
			if (ch == '$')
				processError("unexpected end of file\n");
			else
				nextChar();
		}
		else if (ch == '}')
		{
			processError("'}' cannot begin token\n");
		}
		else if (isspace(ch))
		{
			nextChar();
		}
		else if (isSpecialSymbol(ch))
		{
			token += ch;
			nextChar();
			if (token == ":")
			{
				if (ch == '=')
				{
					token += ch;
					nextChar();
				}
			}
			else if (token == "<")
			{
				if (ch == '>' || ch == '=')
				{
					token += ch;
					nextChar();
				}
			}
			else if (token == ">")
			{
				if (ch == '=')
				{
					token += ch;
					nextChar();
				}
			}
		}
		else if ((ch == '_' || islower(ch)) && tokenCounter < 15)
		{
			token += ch;
			tokenCounter++;
			while (((isalpha(nextChar()) && islower(ch)) || isdigit(ch) || (ch == '_' && token.at(token.length() - 1) != '_')) && ch != ' ' && ch != '.' && ch != '$') 
			{
				if (tokenCounter < 15)
					token += ch;
				tokenCounter++;
			}
			if (ch == '$')
				processError("unexpected end of file\n");
		}
		else if (isdigit(ch) && tokenCounter < 15)
		{
			token += ch;
			tokenCounter++;
			while (isdigit(nextChar()) && ch != '$')
			{
				if (tokenCounter < 15)
					token += ch;
				tokenCounter++;
			}
			if (ch == '$')
				processError("unexpected end of file\n");
		}
		else if (ch == '$')
		{
			token = ch;
		}
		else
		{
			processError("illegal symbol \"" + string(1, ch) + "\"\n");
		}
	}
	return token;
}

// S0: nextChar()
char Compiler::nextChar() //returns the next character or end of file marker
{
	 char char1;
	 char temp = sourceFile.get();
	 if (sourceFile.eof())
		char1 = '$'; //use a special character to designate end of file
	 else
	 {
		 char1 = temp;
		 if (char1 == '\n')
		 {
			 if (ch == '\n')
				listingFile << setw(5) << right << lineNo << "|" << char1;
			 lineNo++;
			 if (ch != '\n')
				listingFile << char1;
		 }
		 else
		 {
			 if (ch == '\n')
				 listingFile << setw(5) << right << lineNo << "|" << char1;
			 else
				listingFile << char1;
		 }
	 }
	
	ch = char1;
	return char1;
}

// ****************************
// ********** OTHER ***********
// ********* ROUTINES *********
// ****************************

// S0 genInternalName()
string Compiler::genInternalName(storeTypes stype) const
{
	static int iCount = -1;
	static int bCount = -1;
	
	if (stype == INTEGER)
	{
		iCount++;
		return "I" + to_string(iCount);
	}
	else if (stype == BOOLEAN)
	{
		bCount++;
		return "B" + to_string(bCount);
	}
	else
		return "";
}

// S0 processError()
void Compiler::processError(string err)
{
	 errorCount++;
	 listingFile << "\nError: Line " << lineNo << ": ";
	 listingFile << err;
	 createListingTrailer();
	 sourceFile.close();
	 listingFile.close();
	 objectFile.close();
	 exit(1);
}


// S1 freeTemp()
void Compiler::freeTemp()
{
	currentTempNo--;
	if (currentTempNo < -1)
	{
		processError("compiler error, currentTempNo should be ≥ –1\n");
	}
}

// S1 getTemp()
string Compiler::getTemp()
{
	string temp;
	currentTempNo++;
	temp = "T";
	temp.append(to_string(currentTempNo));
	if (currentTempNo > maxTempNo)
	{
		insert(temp, UNKNOWN, VARIABLE, "", NO, 1);
		maxTempNo++;
	}
	return temp;
}

// S1 getLabel()
string Compiler::getLabel()
{
	static int labelNo = -1;
	string label = ".L";
	labelNo++;
	label.append(to_string(labelNo));
	return label;
}

// S1 isTemporary()
// determines if s represents a temporary
bool Compiler::isTemporary(string s) const
{
	if (s[0] == 'T')
		return true;
	return false;
}