#pragma once

#include "system/tokeniser.h"
#include "system/interpreter/scriptNode.h"
#include "system/interpreter/function.h"

////////////////////////////////////////////////////////////////////
/*

ScriptFile ->			'ObjectScript' ident '{' FunctionList '}' ObjectList	|
						{e}

SystemFunctionList ->	SystemFunctionDefn SystemFunctionList	|
						{e}

SystemFunctionDefn ->	VarType ident '(' ParameterList ')' ';'	|
						'//' {eol}

ParameterList ->		VarType ident ',' ParameterList		|
						VarType ident						|
						{e}

FunctionList ->			Function FunctionList				|
						{e}

Function ->				EventRoutine '{' StatementList '}'	|
						DeclarativeStatement				|
						VarType ident '(' ParameterList ')' '{' StatementList '}'

EventRoutine ->			'Init' '(' ')'					|
						'KeyDown' '(' KeyParams ')'		|
						'KeyUp' '(' KeyParams ')'		|
						'Logic' '(' ')'

KeyParams ->			'int' 'key' ',' 'bool' 'shift' ',' 'bool' 'ctrl'

StatementList ->		Statement					|
						Statement StatementList		|
						{e}

Statement ->			ConditionalStatement	|
						IterativeStatement		|
						DeclarativeStatement	|
						ExpressionStatement		|
						CompoundStatement		|
						ReturnStatement			|
						'//' {eol}

ExpressionStatement ->	ident '=' Expression ';'

ReturnStatement ->		'return' Expression ';'

DeclarativeStatement ->	VarType ident ';'

IterativeStatement ->	'while' '(' Expression ')' CompoundStatement

ConditionalStatement ->	'if' '(' Expression ')' CompoundStatement
						ConditionalSubstatementList

ConditionalSubstatement ->	'else' 'if' '(' Expression ')' CompoundStatement

ConditionalSubstatementList ->	ConditionalSubstatement ConditionalSubstatementList |
								'else' CompoundStatement |
								{e}

CompoundStatement ->	'{' StatementList '}'

Expression ->		Expression '&&' Expression		|
					Expression '||' Expression
					SubExpression

SubExpression ->	SubExpression '<'  SubExpression		|
					SubExpression '>'  SubExpression		|
					SubExpression '<=' SubExpression		|
					SubExpression '>=' SubExpression		|
					SubExpression '==' SubExpression		|
					SubExpression '!=' SubExpression		|
					SubExpression '+'  SubExpression		|
					SubExpression '-'  SubExpression		|
					SubExpression '/'  SubExpression		|
					SubExpression '*'  SubExpression		|
					'!' Expression							|
					'(' Expression ')'						|
					SimpleExpression

ExpressionList -> Expression ',' ExpressionList	|
				  Expression					|
				  {e}

SimpleExpression ->		ident							|
						ident '(' ExpressionList ')'	|
						ident '.' 'x'					|
						ident '.' 'y'					|
						ident '.' 'z'					|
						ident '.' 'w'					|
						int								|
						float							|
						string							|
						boolean							|
						'this'							|
						vec3(Expr,Expr,Expr)			|
						vec2(Expr,Expr)					|
						quat(Expr,Expr,Expr,Expr)		|


VarType ->				'float'		|
						'vec2'		|
						'vec3'		|
						'quat'		|
						'int'		|
						'string'	|
						'bool'		|
						'void'		|
						'object'

*/

class Script : public Tokeniser
{
public:
	Script();
	~Script();
	Script(const Script&);
	const Script& operator=(const Script&);

	// parser assist grammar functions
	virtual void Parse();
	virtual void Parse(const std::string& str);

	// from code window compilation
	virtual std::vector<ScriptNode*> ParseFunctionsOnly(const std::string& str);

	// get the event functions from a scriptNode
	static Function GetEventFunction(ScriptNode* s);
	static Function GetFunction(ScriptNode* s);

	// get a list of all the system defined functions parsed
	std::vector<ScriptNode*> GetSystemFunctions();

private:
	void ScriptFile();
	void SystemFunctionList();
	void SystemFunctionDefn();
	std::vector<ScriptNode*> ParameterList();
	std::vector<ScriptNode*> FunctionList();
	ScriptNode* ParseFunction();
	bool IsEventRoutine(Token token);
	ScriptNode::Type EventRoutine();
	void KeyParams();
	std::vector<ScriptNode*> StatementList();
	bool IsStatement(Token token);
	ScriptNode* Statement();
	ScriptNode* ExpressionStatement();
	ScriptNode* DeclarativeStatement();
	ScriptNode* IterativeStatement();
	ScriptNode* ConditionalStatement();
	ScriptNode* ConditionalSubstatement();
	ScriptNode* ReturnStatement();
	std::vector<ScriptNode*> ConditionalSubstatementList();
	ScriptNode* CompoundStatement();
	bool IsExpression(Token token);
	ScriptNode* Expression();
	ScriptNode* SubExpression();
	std::vector<ScriptNode*> ExpressionList();
	ScriptNode* SimpleExpression();
	bool IsVarType(Token token);
	ScriptNode::Type VarType();
	ScriptNode::Type TokenToType(Tokeniser::Token token);

private:
	std::vector<ScriptNode*>	systemFunctions;
	std::vector<ScriptNode*>	objectList;
};

