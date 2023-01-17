//Connor Flenniken
// Devoux Deysel
// CS 4301 - Alg Lang & Compilers
// Stage0

#include <stage0.h>
#include <ctime>
#include <cctype>
#include <iomanip>

static int numErrors = 0;
static int lineCount = 1;

//************Member functions from main.C**************************************************************
Compiler::Compiler(char **argv) // constructor
{
	sourceFile.open(argv[1]);
	listingFile.open(argv[2]);
	objectFile.open(argv[3]);
}
Compiler::~Compiler() // destructor
{
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}
void Compiler::createListingHeader()
{
	time_t now = time (NULL);
	
	 listingFile << "STAGE0:  Connor Flenniken, Devoux Deysel\t" << ctime(&now) << "\n";
	 listingFile << left << setw(22) << "LINE NO." << "SOURCE STATEMENT\n\n";
	 ch = '\n';
	 //line numbers and source statements should be aligned under the headings
}
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
void Compiler::createListingTrailer()
{
	listingFile << setw(29) << left << "\nCOMPILATION TERMINATED" << numErrors;
	if (numErrors == 1)
		listingFile << " ERROR ENCOUNTERED\n";
	else
		listingFile << " ERRORS ENCOUNTERED\n";
}

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

void Compiler::processError(string err)
{
	 numErrors++;
	 listingFile << "\nError: Line " << lineCount << ": ";
	 listingFile << err;
	 createListingTrailer();
	 sourceFile.close();
	 listingFile.close();
	 objectFile.close();
	 exit(1);
}
//****************production prog()**************************************************************
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
//****************production2 progStmt()******************************
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
//******** production3 consts()************************
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

//********** production4 vars()***********************
void Compiler::vars() //token should be "var"
{ 
	 if (token.compare("var") != 0)
		processError("keyword \"var\" expected\n");
	
	 if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"var\"\n");
	 varStmts();
}
//********** production5 beginEndStmt ********************
void Compiler::beginEndStmt() //token should be "begin"
{ 
	 if (token.compare("begin") != 0)
		processError("keyword \"begin\" expected\n");
	 nextToken();
	 if (token.compare("end") != 0)
		processError("keyword \"end\" expected\n");
	nextToken();
	 if (token.compare(".") != 0)
		processError("period expected\n");
	nextToken();
	code("end", ".");
}


//*********production6 constStmts *************************
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

//********* production7 varStmts() ***************************
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
		 listingFile << endl << token << " TEST\n";
		processError("non-keyword identifier or \"begin\" expected\n");
	 }
	
	 if (isNonKeyId(token) && !isKeyword(token))
		varStmts();
}

// *********** production8 ids() ***************************
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


//*******************************INSERT******************************************
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


//*********whichType whichValue**************************************************
storeTypes Compiler::whichType(string name) //tells which data type a name has
{ 
	storeTypes data_type;

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
			processError("reference to undefined constant\n");
		}
	 }
	return data_type;
}

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
			processError("reference to undefined constant\n");
	 }
	return value;
}

//*******code()*******************************************************
void Compiler::code(string op, string operand1, string operand2)
{
	 if (op.compare("program") == 0)
		emitPrologue(operand1);
	 else if (op.compare("end") == 0)
		emitEpilogue();
	 else
		processError("compiler error since function code should not be called with illegal arguments\n");
}


// ****************************************************************
//emit emitPrologue emitEpilogue emitStorage
void Compiler::emit(string label, string instruction, string operands, string comment)
{
	objectFile << left;
	objectFile << setw(8) << label;
	objectFile << setw(8) << instruction;
	objectFile << setw(24) << operands;
	objectFile << comment;
}
void Compiler::emitPrologue(string progName, string operand2)
{
	time_t now = time (NULL);
	
	 objectFile << "; Connor Flenniken, Devoux Deysel\t" << ctime(&now);
	 objectFile << "%INCLUDE \"Along32.inc\"\n" << "%INCLUDE \"Macros_Along.inc\"\n\n";
	 emit("SECTION", ".text", "", "");
	 objectFile << endl;
	 emit("global", "_start", "", "; program " + progName);
	 objectFile << endl << endl;
	 emit("_start:", "", "", "");
	 objectFile << endl;
}

void Compiler::emitEpilogue(string operand1, string operand2)
{
	 emit("","Exit", "{0}", "");
	 objectFile << endl << endl;
	 emitStorage();
}

void Compiler::emitStorage()
{
	 emit("SECTION", ".data", "", "");
	 objectFile << endl;
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
			 objectFile << endl;
		 }
	 }
	 objectFile << endl;
	 emit("SECTION", ".bss", "", "");
	 objectFile << endl;
	 for (auto& entry : symbolTable)
	 {
		 if (entry.second.getMode() == VARIABLE && entry.second.getAlloc() == YES)
		 {
			 emit(entry.second.getInternalName(), "resd", "1", ("; " + entry.first));
			 objectFile << endl;
		 }
	 }
}

//****************nextToken nextChar *************************
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
				listingFile << setw(5) << right << lineCount << "|" << char1;
			 lineCount++;
			 if (ch != '\n')
				listingFile << char1;
		 }
		 else
		 {
			 if (ch == '\n')
				 listingFile << setw(5) << right << lineCount << "|" << char1;
			 else
				listingFile << char1;
		 }
	 }
	
	ch = char1;
	return char1;
}

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
	else if (s == "true")
	{
		return true;
	}
	else if (s == "false")
	{
		return true;
	}
	else if (s == "not")
	{
		return true;
	}
	
	return false;
}

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
	return false;
}

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

	}
	
	return true;
}

bool Compiler::isInteger(string s) const  // determines if s is an integer
{
	char char1;
	for (uint i = 0; i < s.length(); i++)
	{
		char1 = s.at(i);
		if (!isdigit(char1) && char1 != '+' && char1 != '-')
			return false;
	}
	return true;
}

bool Compiler::isBoolean(string s) const  // determines if s is a boolean
{
	if (s.compare("true") == 0 || s.compare("false") == 0)
		return true;
	return false;
}

bool Compiler::isLiteral(string s) const  // determines if s is a literal
{
	if (s.compare("integer") == 0 || s.compare("boolean") == 0)
		return true;
	
	if (isInteger(s) || isBoolean(s))
		return true;
	
	return false;
}