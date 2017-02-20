#include "standardFirst.h"

#include "system/interpreter/script.h"

/////////////////////////////////////////////////////////////////////////////

Script::Script()
{
	// these primitives are used
	EnableInt();
	EnableFloat();
	EnableDoubleQuoteString();
	EnableIdent();

	Add("ObjectScript",true);
	Add(".",false);
	Add("Init",true);
	Add("KeyDown",true);
	Add("KeyUp",true);
	Add("Logic",true);
	Add("this",true);
	Add("return",true);
	Add(",",false);
	Add("=",false);
	Add(";",false);
	Add("while",true);
	Add("if",true);
	Add("else",true);
	Add("{",false);
	Add("}",false);
	Add("<",false);
	Add(">",false);
	Add("<=",false);
	Add(">=",false);
	Add("==",false);
	Add("!=",false);
	Add("+",false);
	Add("-",false);
	Add("/",false);
	Add("*",false);
	Add("&&",false);
	Add("||",false);
	Add("!",false);
	Add("(",false);
	Add(")",false);
	Add("//",false);
	Add("float",true);
	Add("vec2",true);
	Add("vec3",true);
	Add("quat",true);
	Add("int",true);
	Add("string",true);
	Add("bool",true);
	Add("object",true);
	Add("void",true);
	Add("true",true);
	Add("false",true);
}

Script::~Script()
{
}

Script::Script(const Script& s)
{
	operator=(s);
}

const Script& Script::operator=(const Script& s)
{
	Tokeniser::operator =(s);
	return *this;
}

std::vector<ScriptNode*> Script::GetSystemFunctions()
{
	return systemFunctions;
}

Function Script::GetEventFunction(ScriptNode* s)
{
	PreCond(s->type==ScriptNode::KeyDownEvent ||
			s->type==ScriptNode::KeyUpEvent ||
			s->type==ScriptNode::LogicEvent ||
			s->type==ScriptNode::InitEvent);

	Function f;

	PreCond(s->nodes.size()==1);
	f.actions = s->nodes[0]->nodes;

	if (s->type==ScriptNode::KeyDownEvent ||
		s->type==ScriptNode::KeyUpEvent)
	{
		f.symbolTable.Add(SymbolNode("key",ScriptNode::Int));
		f.symbolTable.Add(SymbolNode("shift",ScriptNode::Bool));
		f.symbolTable.Add(SymbolNode("ctrl",ScriptNode::Bool));
	}
	return f;
}

Function Script::GetFunction(ScriptNode* s)
{
	PreCond(s->type==ScriptNode::Function);

	Function f;

	PreCond(s->nodes.size()==4);
	PreCond(s->nodes[1]->type == ScriptNode::String);
	PreCond(s->nodes[2]->type == ScriptNode::ParameterList);
	PreCond(s->nodes[3]->type == ScriptNode::StatementList);

	f.name = s->nodes[1]->GetString();
	f.actions = s->nodes[3]->nodes;
	f.params = s->nodes[2]->nodes;

	std::vector<ScriptNode*> params = s->nodes[2]->nodes;
	for (int i=0; i<params.size(); i++)
	{
		ScriptNode* n = params[i];
		PreCond(n->nodes.size() == 2);
		PreCond(n->type == ScriptNode::Parameter);
		f.symbolTable.Add(SymbolNode(n->nodes[1]->GetString(),n->nodes[0]->type));
	}
	return f;
}

void Script::Parse(const std::string& str)
{
	Tokeniser::Parse(str);
	ScriptFile();
}

void Script::Parse()
{
	ScriptFile();
}

std::vector<ScriptNode*> Script::ParseFunctionsOnly(const std::string& str)
{
	Tokeniser::Parse(str);
	return FunctionList();
}

//ScriptFile ->			'ObjectScript' ident '{' FunctionList '}' ObjectList	|
//						{e}
void Script::ScriptFile()
{
	GetCompulsaryToken(Get("ObjectScript"));
	if (error) return;
	GetCompulsaryToken(TIDENT);
	if (error) return;
	std::string objStr = yyString;

	GetCompulsaryToken(Get("{"));
	if (error) return;
	std::vector<ScriptNode*> functions = FunctionList();
	if (error) return;
	GetCompulsaryToken(Get("}"));
	if (error) return;

	ScriptNode* obj = new ScriptNode();
	obj->nodes = functions;
	obj->type = ScriptNode::ObjectList;
	objectList.push_back(obj);

	token = GetNextToken();
	UngetToken();
	if (token == Get("ObjectScript"))
	{
		ScriptFile();
	}
}

//SystemFunctionList ->	SystemFunctionDefn SystemFunctionList	|
//						{e}
void Script::SystemFunctionList()
{
	do
	{
		token = GetNextToken();
		UngetToken();
		if (IsVarType(token) || token == Get("//"))
		{
			SystemFunctionDefn();
		}
		else
		{
			break;
		}
	}
	while (!error);
}

//SystemFunctionDefn ->	VarType ident '(' ParameterList ')' ';'	|
//						'//' {eol}
void Script::SystemFunctionDefn()
{
	token = GetNextToken();
	if (token == Get("//"))
	{
		ScanToEol();
	}
	else
	{
		UngetToken();
		ScriptNode::Type type = VarType();
		if (error) return;
		GetCompulsaryToken(TIDENT);
		if (error) return;
		std::string id = yyString;
		GetCompulsaryToken(Get("("));
		if (error) return;
		std::vector<ScriptNode*> parameterList = ParameterList();
		if (error) return;
		GetCompulsaryToken(Get(")"));
		if (error) return;
		GetCompulsaryToken(Get(";"));
		if (error) return;

		ScriptNode* obj = new ScriptNode();
		obj->type = ScriptNode::SystemFunction;
		obj->nodes.push_back(new ScriptNode(type));
		obj->nodes.push_back(new ScriptNode(id));
		obj->nodes.push_back(new ScriptNode(ScriptNode::ParameterList,parameterList));

		systemFunctions.push_back(obj);
	}
}

//ParameterList ->		VarType ident ',' ParameterList		|
//						VarType ident						|
//						{e}
std::vector<ScriptNode*> Script::ParameterList()
{
	std::vector<ScriptNode*> list;
	bool hasComma = false;
	do
	{
		token = GetNextToken();
		UngetToken();
		if (IsVarType(token))
		{
			ScriptNode::Type type = VarType();
			if (error) break;
			GetCompulsaryToken(TIDENT);
			if (error) break;
			std::string id = yyString;

			ScriptNode* sn = new ScriptNode(ScriptNode::Parameter);
			sn->nodes.push_back(new ScriptNode(type));
			sn->nodes.push_back(new ScriptNode(id));
			list.push_back(sn);

			hasComma = false;
			token = GetNextToken();
			if (token != Get(","))
			{
				UngetToken();
				break;
			}
			hasComma = true;
		}
		else
		{
			if (hasComma)
			{
				SetError("')' expected, found ',' instead");
			}
			break;
		}
	}
	while (!error);

	if (error) { safe_delete_stl_array(list); }

	return list;
}

//FunctionList ->		Function FunctionList				|
//						{e}
std::vector<ScriptNode*> Script::FunctionList()
{
	std::vector<ScriptNode*> list;

	do
	{
		token = GetNextToken();
		UngetToken();
		if (IsVarType(token) || IsEventRoutine(token))
		{
			ScriptNode* n = ParseFunction();
			if (error) break;
			list.push_back(n);
		}
		else
		{
			break;
		}
	}
	while (!error);

	if (error) { safe_delete_stl_array(list); }

	return list;
}

//Function ->			EventRoutine '{' StatementList '}'	|
//						DeclarativeStatement				|
//						VarType ident '(' ParameterList ')' '{' StatementList '}'
ScriptNode* Script::ParseFunction()
{
	token = GetNextToken();
	UngetToken();

	if (IsEventRoutine(token))
	{
		ScriptNode::Type type = EventRoutine();
		if (error) return NULL;
		GetCompulsaryToken("{");
		if (error) return NULL;
		std::vector<ScriptNode*> sl = StatementList();
		if (error) return NULL;
		GetCompulsaryToken("}");
		if (error) return NULL;

		ScriptNode* n = new ScriptNode(type);
		ScriptNode* n2 = new ScriptNode(ScriptNode::StatementList,sl);
		n->nodes.push_back(n2);
		return n;
	}
	else if (IsVarType(token))
	{
		int rewind = GetMarker();

		ScriptNode::Type type = VarType();
		if (error) return NULL;
		GetCompulsaryToken(TIDENT);
		if (error) return NULL;
		std::string id = yyString;

		// is it a function call or a variable decl
		token = GetNextToken();
		if (token == Get(";"))
		{
			ScriptNode* n = new ScriptNode(ScriptNode::VariableDeclaration);
			n->nodes.push_back(new ScriptNode(type));
			n->nodes.push_back(new ScriptNode(id));
			return n;
		}

		UngetToken();
		GetCompulsaryToken("(");
		if (error) return NULL;
		std::vector<ScriptNode*> pl = ParameterList();
		if (error) return NULL;
		GetCompulsaryToken(")");
		if (error) return NULL;
		GetCompulsaryToken("{");
		if (error) return NULL;
		std::vector<ScriptNode*> sl = StatementList();
		if (error) return NULL;
		GetCompulsaryToken("}");
		if (error) return NULL;

		ScriptNode* n = new ScriptNode(ScriptNode::Function);
		n->nodes.push_back(new ScriptNode(type));
		n->nodes.push_back(new ScriptNode(id));
		n->nodes.push_back(new ScriptNode(ScriptNode::ParameterList,pl));
		n->nodes.push_back(new ScriptNode(ScriptNode::StatementList,sl));
		return n;
	}
	else
	{
		SetError("expected EventRoutine or Function");
	}
	return NULL;
}

bool Script::IsEventRoutine(Token token)
{
	if (token == Get("Init") ||
		token == Get("KeyDown") ||
		token == Get("KeyUp") ||
		token == Get("Logic"))
	{
		return true;
	}
	return false;
}

//EventRoutine ->		'Init' '(' ')'					|
//						'KeyDown' '(' KeyParams ')'		|
//						'KeyUp' '(' KeyParams ')'		|
//						'Logic' '(' ')'
ScriptNode::Type Script::EventRoutine()
{
	token = GetNextToken();
	if (token == Get("Init"))
	{
		GetCompulsaryToken(Get("("));
		if (error) return ScriptNode::None;
		GetCompulsaryToken(Get(")"));
		if (error) return ScriptNode::None;
		return ScriptNode::InitEvent;
	}
	if (token == Get("KeyDown"))
	{
		GetCompulsaryToken(Get("("));
		if (error) return ScriptNode::None;
		KeyParams();
		if (error) return ScriptNode::None;
		GetCompulsaryToken(Get(")"));
		if (error) return ScriptNode::None;
		return ScriptNode::KeyDownEvent;
	}
	if (token == Get("KeyUp"))
	{
		GetCompulsaryToken(Get("("));
		if (error) return ScriptNode::None;
		KeyParams();
		if (error) return ScriptNode::None;
		GetCompulsaryToken(Get(")"));
		if (error) return ScriptNode::None;
		return ScriptNode::KeyUpEvent;
	}
	if (token == Get("Logic"))
	{
		GetCompulsaryToken(Get("("));
		if (error) return ScriptNode::None;
		GetCompulsaryToken(Get(")"));
		if (error) return ScriptNode::None;
		return ScriptNode::LogicEvent;
	}
	SetError("Expected event routine name");
	return ScriptNode::None;
}

//KeyParams ->			'int' 'key' ',' 'bool' 'shift' ',' 'bool' 'ctrl'
void Script::KeyParams()
{
	GetCompulsaryToken(Get("int"));
	if (error) return;
	GetCompulsaryToken(TIDENT);
	if (error) return;
	if (yyString != "key")
	{
		SetError("first parameter in key-event routine must be 'key'");
		return;
	}
	GetCompulsaryToken(Get(","));
	if (error) return;
	GetCompulsaryToken(Get("bool"));
	if (error) return;
	GetCompulsaryToken(TIDENT);
	if (error) return;
	if (yyString != "shift")
	{
		SetError("second parameter in key-event routine must be 'shift'");
		return;
	}
	GetCompulsaryToken(Get(","));
	if (error) return;
	GetCompulsaryToken(Get("bool"));
	if (error) return;
	GetCompulsaryToken(TIDENT);
	if (error) return;
	if (yyString != "ctrl")
	{
		SetError("third parameter in key-event routine must be 'ctrl'");
		return;
	}
}

//StatementList ->		Statement					|
//						Statement StatementList		|
//						{e}
std::vector<ScriptNode*> Script::StatementList()
{
	std::vector<ScriptNode*> list;

	do
	{
		token = GetNextToken();
		UngetToken();
		if (IsStatement(token) || IsEventRoutine(token))
		{
			ScriptNode* f = Statement();
			if (error) break;
			if (f!=NULL)
			{
				list.push_back(f);
			}
		}
		else
		{
			break;
		}
	}
	while (!error);

	if (error) { safe_delete_stl_array(list); }

	return list;
}

bool Script::IsStatement(Token token)
{
	if (token==Get("//")		||
		token==Get("if")		||
		token==Get("while")		||
		token==Get("return")	||
		token==TIDENT			||
		IsVarType(token))
	{
		return true;
	}
	return false;
}

//Statement ->			ConditionalStatement	|
//						IterativeStatement		|
//						DeclarativeStatement	|
//						ExpressionStatement		|
//						CompoundStatement		|
//						ReturnStatement			|
//						'//' {eol}
ScriptNode* Script::Statement()
{
	token = GetNextToken();
	UngetToken();
	if (token==Get("if"))
	{
		return ConditionalStatement();
	}
	else if (token==Get("while"))
	{
		return IterativeStatement();
	}
	else if (IsVarType(token))
	{
		return DeclarativeStatement();
	}
	else if (token==TIDENT)
	{
		return ExpressionStatement();
	}
	else if (token==Get("{"))
	{
		return CompoundStatement();
	}
	else if (token == Get("return"))
	{
		return ReturnStatement();
	}
	else if (token == Get("//"))
	{
		ScanToEol();
		return NULL;
	}
	else
	{
		SetError("expected Statement");
	}
	return NULL;
}

//ExpressionStatement ->	ident '=' Expression ';'						|
//							ident '(' ExpressionList ')' ';'				|
//							ident '.' ['x'|'y'|'z'|'w'] = Expression ';'
ScriptNode* Script::ExpressionStatement()
{
	GetCompulsaryToken(TIDENT);
	if (error) return NULL;
	std::string id = yyString;

	token = GetNextToken();
	if (token == Get("("))
	{
		std::vector<ScriptNode*> params = ExpressionList();
		if (error) return NULL;
		GetCompulsaryToken(Get(")"));
		if (error) { safe_delete_stl_array(params); return NULL; }
		GetCompulsaryToken(Get(";"));
		if (error) { safe_delete_stl_array(params); return NULL; }

		ScriptNode* expr1 = new ScriptNode(ScriptNode::FunctionCall);
		expr1->nodes.push_back(new ScriptNode(id));
		expr1->nodes.push_back(new ScriptNode(ScriptNode::ParameterList,params));
		return expr1;
	}
	else if (token == Get("="))
	{
		if (error) return NULL;
		ScriptNode* expr = Expression();
		if (error) return NULL;
		GetCompulsaryToken(Get(";"));
		if (error) return NULL;

		ScriptNode* n = new ScriptNode(ScriptNode::Assignment);
		n->nodes.push_back(new ScriptNode(id));
		n->nodes.push_back(expr);
		return n;
	}
	else if (token == Get("."))
	{
		GetCompulsaryToken(TIDENT);
		if (error) return NULL;
		std::string id2 = yyString;

		if (id2 != "x" && id2 != "y" && id2 !="z" && id2 != "w")
		{
			SetError("accessors can only be one of 'x', 'y', 'z', or 'w'");
			return NULL;
		}

		ScriptNode* accessor = new ScriptNode(ScriptNode::Accessor);
		accessor->nodes.push_back(new ScriptNode(id));
		if (id2 == "x")
		{
			accessor->nodes.push_back(new ScriptNode(ScriptNode::X));
		}
		else if (id2 == "y")
		{
			accessor->nodes.push_back(new ScriptNode(ScriptNode::Y));
		}
		else if (id2 == "z")
		{
			accessor->nodes.push_back(new ScriptNode(ScriptNode::Z));
		}
		else if (id2 == "w")
		{
			accessor->nodes.push_back(new ScriptNode(ScriptNode::W));
		}

		GetCompulsaryToken(Get("="));
		if (error) { safe_delete(accessor); return NULL; }
		ScriptNode* expr = Expression();
		if (error) { safe_delete(accessor); return NULL; }
		GetCompulsaryToken(Get(";"));
		if (error) { safe_delete(accessor); return NULL; }

		ScriptNode* n = new ScriptNode(ScriptNode::Assignment);
		n->nodes.push_back(accessor);
		n->nodes.push_back(expr);
		return n;
	}
	else
	{
		SetError("expected ident to be followed by '=' or '('");
		return NULL;
	}
}

//DeclarativeStatement ->	VarType ident ';'
ScriptNode* Script::DeclarativeStatement()
{
	token = GetNextToken();
	UngetToken();
	if (!IsVarType(token))
	{
		SetError("expected variable declaration");
		return NULL;
	}
	ScriptNode::Type type = VarType();
	if (error) return NULL;
	GetCompulsaryToken(TIDENT);
	if (error) return NULL;
	std::string id = yyString;
	GetCompulsaryToken(Get(";"));
	if (error) return NULL;

	ScriptNode* n = new ScriptNode(ScriptNode::VariableDeclaration);
	n->nodes.push_back(new ScriptNode(type));
	n->nodes.push_back(new ScriptNode(id));
	return n;
}

//IterativeStatement ->	'while' '(' Expression ')' CompoundStatement
ScriptNode* Script::IterativeStatement()
{
	GetCompulsaryToken(Get("while"));
	if (error) return NULL;
	GetCompulsaryToken(Get("("));
	if (error) return NULL;

	ScriptNode* expr = Expression();
	if (error) return NULL;

	GetCompulsaryToken(Get(")"));
	if (error) { safe_delete(expr); return NULL; }

	ScriptNode* compound = CompoundStatement();
	if (error) { safe_delete(expr); return NULL; }

	ScriptNode* n = new ScriptNode(ScriptNode::While);
	n->nodes.push_back(expr);
	n->nodes.push_back(compound);
	return n;
}

// ReturnStatement ->		'return' Expression ';'
ScriptNode* Script::ReturnStatement()
{
	GetCompulsaryToken(Get("return"));
	if (error) return NULL;
	ScriptNode* expr = Expression();
	if (error) return NULL;
	GetCompulsaryToken(Get(";"));
	if (error) { safe_delete(expr); return NULL; }

	ScriptNode* sn = new ScriptNode(ScriptNode::Return);
	sn->nodes.push_back(expr);
	return sn;
}

//ConditionalStatement ->	'if' '(' Expression ')' CompoundStatement
//							ConditionalSubstatementList
ScriptNode* Script::ConditionalStatement()
{
	GetCompulsaryToken(Get("if"));
	if (error) return NULL;
	GetCompulsaryToken(Get("("));
	if (error) return NULL;

	ScriptNode* expr = Expression();
	if (error) return NULL;

	GetCompulsaryToken(Get(")"));
	if (error) { safe_delete(expr); return NULL; }

	ScriptNode* compound = CompoundStatement();
	if (error) { safe_delete(expr); return NULL; }

	token = GetNextToken();
	UngetToken();
	std::vector<ScriptNode*> list;
	if (token == Get("else"))
	{
		list = ConditionalSubstatementList();
		if (error) { safe_delete(compound); safe_delete(expr); return NULL; }
	}

	ScriptNode* n = new ScriptNode(ScriptNode::If);
	n->nodes.push_back(expr);
	n->nodes.push_back(compound);

	for (int i=0; i<list.size(); i++)
	{
		n->nodes.push_back(list[i]);
	}

	return n;
}

//ConditionalSubstatement ->	'else' 'if' '(' Expression ')' CompoundStatement
ScriptNode* Script::ConditionalSubstatement()
{
	GetCompulsaryToken(Get("else"));
	if (error) return NULL;
	GetCompulsaryToken(Get("if"));
	if (error) return NULL;
	GetCompulsaryToken(Get("("));
	if (error) return NULL;

	ScriptNode* expr = Expression();
	if (error) return NULL;

	GetCompulsaryToken(Get(")"));
	if (error) { safe_delete(expr); return NULL; }

	ScriptNode* compound = CompoundStatement();
	if (error) { safe_delete(expr); return NULL; }

	ScriptNode* n = new ScriptNode(ScriptNode::ElseIf);
	n->nodes.push_back(expr);
	n->nodes.push_back(compound);
	return n;
}

//ConditionalSubstatementList ->ConditionalSubstatement ConditionalSubstatementList |
//								'else' CompoundStatement |
//								{e}
std::vector<ScriptNode*> Script::ConditionalSubstatementList()
{
	std::vector<ScriptNode*> list;

	do
	{
		int rewind = GetMarker();

		token = GetNextToken();
		UngetToken();
		if (token == Get("else"))
		{
			GetNextToken();
			token = GetNextToken();

			if (token == Get("if"))
			{
				SetMarker(rewind);
				ScriptNode* n = ConditionalSubstatement();
				if (error) break;
				list.push_back(n);
			}
			else if (token == Get("{"))
			{
				UngetToken();

				ScriptNode* compound = CompoundStatement();
				if (error) break;

				ScriptNode* n2 = new ScriptNode(ScriptNode::Else);
				n2->nodes.push_back(compound);

				list.push_back(n2);
			}
			else
			{
				SetError("unknown token following 'else'");
				break;
			}
		}
		else
		{
			break;
		}
	}
	while (!error);

	if (error) { safe_delete_stl_array(list); }

	return list;
}

//CompoundStatement ->	'{' StatementList '}'
ScriptNode* Script::CompoundStatement()
{
	GetCompulsaryToken(Get("{"));
	if (error) return NULL;

	std::vector<ScriptNode*> list = StatementList();
	if (error) return NULL;

	GetCompulsaryToken(Get("}"));
	if (error) return NULL;

	return new ScriptNode(ScriptNode::CompoundStatement,list);
}

bool Script::IsExpression(Token token)
{
	if (token == TINT			||
		token == TFLOAT			||
		token == TSTRING		||
		token == TIDENT			||
		token == Get("!")		||
		token == Get("(")		||
		token == Get("true")	||
		token == Get("false")	||
		token == Get("vec2")	||
		token == Get("vec3")	||
		token == Get("quat")	||
		token == Get("this"))
	{
		return true;
	}
	return false;
}

ScriptNode::Type Script::TokenToType(Tokeniser::Token token)
{
	if (token == Get("<"))
		return ScriptNode::LessThan;
	if (token == Get(">"))
		return ScriptNode::LessThanOrEqual;
	if (token == Get("<="))
		return ScriptNode::GreaterThanOrEqual;
	if (token == Get(">="))
		return ScriptNode::GreaterThanOrEqual;
	if (token == Get("=="))
		return ScriptNode::Equal;
	if (token == Get("!="))
		return ScriptNode::NotEqual;
	if (token == Get("+"))
		return ScriptNode::Plus;
	if (token == Get("-"))
		return ScriptNode::Minus;
	if (token == Get("/"))
		return ScriptNode::Divide;
	if (token == Get("*"))
		return ScriptNode::Times;
	if (token == Get("&&"))
		return ScriptNode::And;
	if (token == Get("||"))
		return ScriptNode::Or;

	SetError("unknown binary token");
	return ScriptNode::None;
}

//Expression ->		Expression '&&' Expression		|
//					Expression '||' Expression
//					SubExpression
ScriptNode* Script::Expression()
{
	ScriptNode* expr1 = SubExpression();
	if (error) return NULL;

	token = GetNextToken();
	if (token == Get("&&") || token == Get("||"))
	{
		ScriptNode::Type type = TokenToType(token);

		ScriptNode* expr2 = SubExpression();
		if (error) { safe_delete(expr1); return NULL; }

		ScriptNode* expr = new ScriptNode(type);
		expr->nodes.push_back(expr1);
		expr->nodes.push_back(expr2);
		return expr;
	}
	else
	{
		UngetToken();
		return expr1;
	}
}

//SubExpression ->	SubExpression '<'  SubExpression		|
//					SubExpression '>'  SubExpression		|
//					SubExpression '<=' SubExpression		|
//					SubExpression '>=' SubExpression		|
//					SubExpression '==' SubExpression		|
//					SubExpression '!=' SubExpression		|
//					SubExpression '+'  SubExpression		|
//					SubExpression '-'  SubExpression		|
//					SubExpression '/'  SubExpression		|
//					SubExpression '*'  SubExpression		|
//					'!' Expression							|
//					'(' Expression ')'						|
//					SimpleExpression
ScriptNode* Script::SubExpression()
{
	token = GetNextToken();

	ScriptNode* expr1 = NULL;

	if (token == Get("!"))
	{
		expr1 = new ScriptNode(ScriptNode::Not);
		expr1->nodes.push_back(Expression());
		if (error) { safe_delete(expr1); return NULL; }
	}
	else if (token == Get("("))
	{
		expr1 = Expression();
		if (error) return NULL;
		GetCompulsaryToken(")");
		if (error) { safe_delete(expr1); return NULL; }
	}
	else
	{
		UngetToken();
		expr1 = SimpleExpression();
		if (error) return NULL;
	}

	// second operator?
	token = GetNextToken();
	if (token == Get("<") ||
		token == Get(">") ||
		token == Get("<=") ||
		token == Get(">=") ||
		token == Get("==") ||
		token == Get("!=") ||
		token == Get("+") ||
		token == Get("-") ||
		token == Get("/") ||
		token == Get("*"))
	{
		ScriptNode::Type type = TokenToType(token);
		if (error) { safe_delete(expr1); return NULL; }
		ScriptNode* expr2 = SubExpression();
		if (error) { safe_delete(expr1); return NULL; }

		ScriptNode* expr = new ScriptNode(type);
		expr->nodes.push_back(expr1);
		expr->nodes.push_back(expr2);
		return expr;
	}
	else
	{
		UngetToken();
		return expr1;
	}
}

//ExpressionList -> Expression ',' ExpressionList	|
//				  Expression					|
//				  {e}
std::vector<ScriptNode*> Script::ExpressionList()
{
	std::vector<ScriptNode*> list;
	bool hasComma = false;

	do
	{
		token = GetNextToken();
		UngetToken();
		if (IsExpression(token))
		{
			ScriptNode* n = Expression();
			if (error) break;
			list.push_back(n);
		}
		else
		{
			if (hasComma)
			{
				SetError("expression expected after ','");
			}
			break;
		}

		hasComma = false;
		token = GetNextToken();
		if (token == Get(","))
		{
			hasComma = true;
		}
		else
		{
			UngetToken();
		}
	}
	while (!error && hasComma);

	if (error) { safe_delete_stl_array(list); }
	return list;
}

//SimpleExpression ->	ident							|
//						ident '(' ExpressionList ')'	|
//						ident '.' 'x'					|
//						ident '.' 'y'					|
//						ident '.' 'z'					|
//						ident '.' 'w'					|
//						int								|
//						'this'							|
//						float							|
//						string							|
//						boolean							|
//						vec3(Expr,Expr,Expr)			|
//						vec2(Expr,Expr)					|
//						quat(Expr,Expr,Expr,Expr)
ScriptNode* Script::SimpleExpression()
{
	token = GetNextToken();
	if (token==TIDENT)
	{
		std::string id = yyString;

		token = GetNextToken();
		if (token == Get("("))
		{
			std::vector<ScriptNode*> params = ExpressionList();
			if (error) return NULL;
			GetCompulsaryToken(Get(")"));
			if (error) { safe_delete_stl_array(params); return NULL; }

			ScriptNode* expr1 = new ScriptNode(ScriptNode::FunctionCall);
			expr1->nodes.push_back(new ScriptNode(id));
			expr1->nodes.push_back(new ScriptNode(ScriptNode::ParameterList,params));
			return expr1;
		}
		else if (token == Get("."))
		{
			GetCompulsaryToken(TIDENT);
			std::string accessor = yyString;
			ScriptNode* expr1 = new ScriptNode(ScriptNode::Accessor);
			expr1->nodes.push_back(new ScriptNode(id));
			if (accessor == "x")
			{
				expr1->nodes.push_back(new ScriptNode(ScriptNode::X));
				return expr1;
			}
			else if (accessor == "y")
			{
				expr1->nodes.push_back(new ScriptNode(ScriptNode::Y));
				return expr1;
			}
			else if (accessor == "z")
			{
				expr1->nodes.push_back(new ScriptNode(ScriptNode::Z));
				return expr1;
			}
			else if (accessor == "w")
			{
				expr1->nodes.push_back(new ScriptNode(ScriptNode::W));
				return expr1;
			}
			else
			{
				SetError("unknown variable accessor");
				return NULL;
			}
		}
		else
		{
			UngetToken();

			ScriptNode* n = new ScriptNode(ScriptNode::Identifier);
			n->nodes.push_back(new ScriptNode(id));
			return n;
		}
	}
	else if (token==TINT)
	{
		return new ScriptNode(yyInt);
	}
	else if (token==TFLOAT)
	{
		return new ScriptNode(yyFloat);
	}
	else if (token==TSTRING)
	{
		return new ScriptNode(yyString);
	}
	else if (token == Get("true") || token == Get("false"))
	{
		return new ScriptNode((token == Get("true")));
	}
	else if (token == Get("this"))
	{
		return new ScriptNode(ScriptNode::This);
	}
	else if (token == Get("vec2"))
	{
		GetCompulsaryToken(Get("("));
		if (error) return NULL;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		float v1 = yyFloat;
		if (error) return NULL;
		GetCompulsaryToken(Get(","));
		if (error) return NULL;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		if (error) return NULL;
		float v2 = yyFloat;
		GetCompulsaryToken(Get(")"));
		if (error) return NULL;

		return new ScriptNode(D3DXVECTOR2(v1,v2));
	}
	else if (token == Get("vec3"))
	{
		GetCompulsaryToken(Get("("));
		if (error) return NULL;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		float v1 = yyFloat;
		if (error) return NULL;
		GetCompulsaryToken(Get(","));
		if (error) return NULL;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		float v2 = yyFloat;
		if (error) return NULL;
		GetCompulsaryToken(Get(","));
		if (error) return NULL;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		float v3 = yyFloat;
		if (error) return NULL;
		GetCompulsaryToken(Get(")"));
		if (error) return NULL;

		return new ScriptNode(D3DXVECTOR3(v1,v2,v3));
	}
	else if (token == Get("quat"))
	{
		GetCompulsaryToken(Get("("));
		if (error) return NULL;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		float v1 = yyFloat;
		if (error) return NULL;
		GetCompulsaryToken(Get(","));
		if (error) return NULL;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		float v2 = yyFloat;
		if (error) return NULL;
		GetCompulsaryToken(Get(","));
		if (error) return NULL;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		float v3 = yyFloat;
		if (error) return NULL;
		GetCompulsaryToken(Get(","));
		if (error) return NULL;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		float v4 = yyFloat;
		if (error) return NULL;
		GetCompulsaryToken(Get(")"));
		if (error) return NULL;

		return new ScriptNode(D3DXQUATERNION(v1,v2,v3,v4));
	}
	else
	{
		UngetToken();
		SetError("unknown simpleExpression");
	}
	return NULL;
}

bool Script::IsVarType(Token token)
{
	if (token==Get("float")		||
		token==Get("vec2")		||
		token==Get("vec3")		||
		token==Get("quat")		||
		token==Get("int")		||
		token==Get("string")	||
		token==Get("bool")		||
		token==Get("void")		||
		token==Get("object"))
	{
		return true;
	}
	return false;
}

//VarType ->			'float'		|
//						'vec2'		|
//						'vec3'		|
//						'quat'		|
//						'int'		|
//						'string'	|
//						'bool'		|
//						'void'		|
//						'object'
ScriptNode::Type Script::VarType()
{
	token = GetNextToken();
	if (token==Get("float"))
		return ScriptNode::Float;
	if (token==Get("vec2"))
		return ScriptNode::Vec2;
	if (token==Get("vec3"))
		return ScriptNode::Vec3;
	if (token==Get("quat"))
		return ScriptNode::Quat;
	if (token==Get("int"))
		return ScriptNode::Int;
	if (token==Get("string"))
		return ScriptNode::String;
	if (token==Get("bool"))
		return ScriptNode::Bool;
	if (token==Get("void"))
		return ScriptNode::Void;
	if (token==Get("object"))
		return ScriptNode::Object;
	SetError("unknown VarType");
	return ScriptNode::None;
}
