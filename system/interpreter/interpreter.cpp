#include "standardFirst.h"

#include "system/BaseApp.h"
#include "system/model/worldObject.h"

#include "system/interpreter/interpreter.h"
#include "system/interpreter/systemFacade.h"

//////////////////////////////////////////////////////////////

void Interpreter::ExecuteFunction(WorldObject* obj,Function& f,SymbolTable& globals,ScriptNode& result)
{
	bool isReturn;
	for (int i=0; i<f.actions.size(); i++)
	{
		ExecuteStatement(obj,f.actions[i],globals,f.symbolTable,result,isReturn);
		if (isReturn)
			break;
	}
}

void Interpreter::ExecuteStatementList(WorldObject* obj,ScriptNode* sl,SymbolTable& globals,
									   SymbolTable& locals,ScriptNode& result,bool& isReturn)
{
	if (sl->type != ScriptNode::CompoundStatement)
	{
		throw new Exception("ExecuteStatementList: type must be CompoundStatement");
		return;
	}

	for (int i=0; i<sl->nodes.size(); i++)
	{
		ExecuteStatement(obj,sl->nodes[i],globals,locals,result,isReturn);
		if (isReturn)
			break;
	}
}

void Interpreter::ExecuteFunction(WorldObject* obj, Function* function, SymbolTable& globals,
								  SymbolTable& locals,ScriptNode& result)
{
	bool isReturn;
	std::vector<ScriptNode*> sl = function->actions;
	for (int i=0; i<sl.size(); i++)
	{
		ExecuteStatement(obj,sl[i],globals,locals,result,isReturn);
		if (isReturn)
			break;
	}
}

bool Interpreter::ExecuteFunction(WorldObject* obj,
								  ScriptNode* s,
								  SymbolTable& global,
								  SymbolTable& local,
								  ScriptNode& result)
{
	// or a system function
	std::string name;
	std::vector<ScriptNode*> orgParams;
	std::vector<ScriptNode*> params;
	s->GetFunctionCall(name,orgParams);

	// is it a world-object function?
	Function* f = obj->GetFunction(name);
	if (f != NULL)
	{
		if (orgParams.size() != f->params.size())
		{
			throw new Exception("incorrect number of parameters - function takes " + System::Int2Str(f->params.size()) + " parameters");
		}

		// evaluate and set each parameters
		ScriptNode* r;
		for (int i=0; i<orgParams.size(); i++)
		{
			r = new ScriptNode();
			if (!EvaluateExpression(obj,orgParams[i],global,local,*r))
			{
				safe_delete_stl_array(params);
				return false;
			}
			ScriptNode* p = f->params[i];
			if (r->type != p->nodes[0]->type)
			{
				throw new Exception("function " + name + " parameter " + System::Int2Str(i+1) + " type mismatch");
			}

			switch (r->type)
			{
				case ScriptNode::Object:
				{
					f->symbolTable.SetValue(p->nodes[1]->GetString(),r->GetObject());
					break;
				}
				case ScriptNode::Vec2:
				{
					f->symbolTable.SetValue(p->nodes[1]->GetString(),r->GetVec2());
					break;
				}
				case ScriptNode::Vec3:
				{
					f->symbolTable.SetValue(p->nodes[1]->GetString(),r->GetVec3());
					break;
				}
				case ScriptNode::Quat:
				{
					f->symbolTable.SetValue(p->nodes[1]->GetString(),r->GetQuat());
					break;
				}
				case ScriptNode::String:
				{
					f->symbolTable.SetValue(p->nodes[1]->GetString(),r->GetString());
					break;
				}
				case ScriptNode::Float:
				{
					f->symbolTable.SetValue(p->nodes[1]->GetString(),r->GetFloat());
					break;
				}
				case ScriptNode::Int:
				{
					f->symbolTable.SetValue(p->nodes[1]->GetString(),r->GetInt());
					break;
				}
				default:
				{
					throw new Exception("function " + name + " unknown data-type");
				}
			}
		}
		ExecuteFunction(obj, f, global, f->symbolTable, result);
		safe_delete_stl_array(params);
		return true;
	}
	else
	{
		// evaluate all parameters
		ScriptNode* r;
		for (int i=0; i<orgParams.size(); i++)
		{
			r = new ScriptNode();
			if (!EvaluateExpression(obj,orgParams[i],global,local,*r))
			{
				safe_delete_stl_array(params);
				return false;
			}
			params.push_back(r);
		}

		SystemFacade* facade = BaseApp::GetApp().GetFacade();
		if (facade==NULL)
		{
			throw new Exception("facade does not exist");
			return false;
		}
		bool ran = facade->ExecuteFunction(name,result,params);
		safe_delete_stl_array(params);
		if (!ran)
		{
			throw new Exception("system function " + name + " does not exist");
		}
		return ran;
	}
}

bool Interpreter::EvaluateExpression(WorldObject* obj,
									 ScriptNode* s,
									 SymbolTable& global,
									 SymbolTable& local,
									 ScriptNode& result)
{
	ScriptNode e1,e2;

	switch (s->type)
	{
		case ScriptNode::And:
		{
			if (EvaluateExpression(obj,s->nodes[0],global,local,e1))
			if (EvaluateExpression(obj,s->nodes[1],global,local,e2))
			{
				if (e1.type == e2.type && e1.type == ScriptNode::Bool)
				{
					result.SetData( e1.GetBool() && e2.GetBool() );
					return true;
				}
				else
				{
					throw new Exception("'&&' takes two Boolean expressions");
				}
			}
			return false;
		}
		case ScriptNode::Or:
		{
			if (EvaluateExpression(obj,s->nodes[0],global,local,e1))
			if (EvaluateExpression(obj,s->nodes[1],global,local,e2))
			{
				if (e1.type == e2.type && e1.type == ScriptNode::Bool)
				{
					result.SetData( e1.GetBool() || e2.GetBool() );
					return true;
				}
				else
				{
					throw new Exception("'||' takes two Boolean expressions");
				}
			}
			return false;
		}
		case ScriptNode::LessThan:
		{
			result.type = ScriptNode::Bool;
			if (EvaluateExpression(obj,s->nodes[0],global,local,e1))
			if (EvaluateExpression(obj,s->nodes[1],global,local,e2))
			{
				if (e1.type == e2.type)
				{
					switch (e1.type)
					{
						case ScriptNode::Int:
						{
							result.SetData( e1.GetInt() < e2.GetInt() );
							return true;
						}
						case ScriptNode::Float:
						{
							result.SetData( e1.GetFloat() < e2.GetFloat() );
							return true;
						}
						default:
						{
							throw new Exception("non comparable type");
						}
					}
				}
				throw new Exception("type mismatch");
			}
			break;
		}
		case ScriptNode::LessThanOrEqual:
		{
			result.type = ScriptNode::Bool;
			if( EvaluateExpression(obj,s->nodes[0],global,local,e1))
			if (EvaluateExpression(obj,s->nodes[1],global,local,e2))
			{
				if (e1.type == e2.type)
				{
					switch (e1.type)
					{
						case ScriptNode::Int:
						{
							result.SetData( e1.GetInt() <= e2.GetInt() );
							return true;
						}
						case ScriptNode::Float:
						{
							result.SetData( e1.GetFloat() <= e2.GetFloat() );
							return true;
						}
						default:
						{
							throw new Exception("non comparable type");
						}
					}
				}
				throw new Exception("type mismatch");
			}
			break;
		}
		case ScriptNode::GreaterThan:
		{
			result.type = ScriptNode::Bool;
			if( EvaluateExpression(obj,s->nodes[0],global,local,e1))
			if (EvaluateExpression(obj,s->nodes[1],global,local,e2))
			{
				if (e1.type == e2.type)
				{
					switch (e1.type)
					{
						case ScriptNode::Int:
						{
							result.SetData( e1.GetInt() > e2.GetInt() );
							return true;
						}
						case ScriptNode::Float:
						{
							result.SetData( e1.GetFloat() > e2.GetFloat() );
							return true;
						}
						default:
						{
							throw new Exception("non comparable type");
						}
					}
				}
				throw new Exception("type mismatch");
			}
			break;
		}
		case ScriptNode::GreaterThanOrEqual:
		{
			result.type = ScriptNode::Bool;
			if( EvaluateExpression(obj,s->nodes[0],global,local,e1))
			if (EvaluateExpression(obj,s->nodes[1],global,local,e2))
			{
				if (e1.type == e2.type)
				{
					switch (e1.type)
					{
						case ScriptNode::Int:
						{
							result.SetData( e1.GetInt() >= e2.GetInt() );
							return true;
						}
						case ScriptNode::Float:
						{
							result.SetData( e1.GetFloat() >= e2.GetFloat() );
							return true;
						}
						default:
						{
							throw new Exception("non comparable type");
						}
					}
				}
				throw new Exception("type mismatch");
			}
			break;
		}
		case ScriptNode::Equal:
		{
			result.type = ScriptNode::Bool;
			if( EvaluateExpression(obj,s->nodes[0],global,local,e1))
			if (EvaluateExpression(obj,s->nodes[1],global,local,e2))
			{
				if (e1.type == e2.type)
				{
					switch (e1.type)
					{
						case ScriptNode::Int:
						{
							result.SetData( e1.GetInt() == e2.GetInt() );
							return true;
						}
						case ScriptNode::Float:
						{
							result.SetData( e1.GetFloat() == e2.GetFloat() );
							return true;
						}
						case ScriptNode::Bool:
						{
							result.SetData( e1.GetBool() == e2.GetBool() );
							return true;
						}
						case ScriptNode::Object:
						{
							result.SetData( e1.GetObject() == e2.GetObject() );
							return true;
						}
						case ScriptNode::String:
						{
							result.SetData( e1.GetString() == e2.GetString() );
							return true;
						}
						case ScriptNode::Vec2:
						{
							result.SetData( e1.GetVec2() == e2.GetVec2() );
							return true;
						}
						case ScriptNode::Vec3:
						{
							result.SetData( e1.GetVec3() == e2.GetVec3() );
							return true;
						}
						case ScriptNode::Quat:
						{
							result.SetData( e1.GetQuat() == e2.GetQuat() );
							return true;
						}
						default:
						{
							throw new Exception("non comparable type");
						}
					}
				}
				throw new Exception("type mismatch");
			}
			break;
		}
		case ScriptNode::NotEqual:
		{
			result.type = ScriptNode::Bool;
			if( EvaluateExpression(obj,s->nodes[0],global,local,e1))
			if (EvaluateExpression(obj,s->nodes[1],global,local,e2))
			{
				if (e1.type == e2.type)
				{
					switch (e1.type)
					{
						case ScriptNode::Int:
						{
							result.SetData( e1.GetInt() != e2.GetInt() );
							return true;
						}
						case ScriptNode::Float:
						{
							result.SetData( e1.GetFloat() != e2.GetFloat() );
							return true;
						}
						case ScriptNode::Bool:
						{
							result.SetData( e1.GetBool() != e2.GetBool() );
							return true;
						}
						case ScriptNode::Object:
						{
							result.SetData( e1.GetObject() != e2.GetObject() );
							return true;
						}
						case ScriptNode::String:
						{
							result.SetData( e1.GetString() != e2.GetString() );
							return true;
						}
						case ScriptNode::Vec2:
						{
							result.SetData( e1.GetVec2() != e2.GetVec2() );
							return true;
						}
						case ScriptNode::Vec3:
						{
							result.SetData( e1.GetVec3() != e2.GetVec3() );
							return true;
						}
						case ScriptNode::Quat:
						{
							result.SetData( e1.GetQuat() != e2.GetQuat() );
							return true;
						}
						default:
						{
							throw new Exception("non comparable type");
						}
					}
				}
				throw new Exception("type mismatch");
			}
			break;
		}
		case ScriptNode::Not:
		{
			if (EvaluateExpression(obj,s->nodes[0],global,local,result))
			{
				if (result.type != ScriptNode::Bool)
					throw new Exception("not of boolean type");
				result.SetData( !result.GetBool() );
				return true;
			}
			break;
		}
		case ScriptNode::Plus:
		{
			if( EvaluateExpression(obj,s->nodes[0],global,local,e1))
			if (EvaluateExpression(obj,s->nodes[1],global,local,e2))
			{
				if (e1.type == e2.type)
				{
					switch (e1.type)
					{
						case ScriptNode::Int:
						{
							result.SetData( e1.GetInt() + e2.GetInt() );
							return true;
						}
						case ScriptNode::Float:
						{
							result.SetData( e1.GetFloat() + e2.GetFloat() );
							return true;
						}
						case ScriptNode::String:
						{
							result.SetData( e1.GetString() + e2.GetString() );
							return true;
						}
						case ScriptNode::Vec2:
						{
							result.SetData( e1.GetVec2() + e2.GetVec2() );
							return true;
						}
						case ScriptNode::Vec3:
						{
							result.SetData( e1.GetVec3() + e2.GetVec3() );
							return true;
						}
						case ScriptNode::Quat:
						{
							result.SetData( e1.GetQuat() + e2.GetQuat() );
							return true;
						}
						default:
						{
							throw new Exception("non comparable type");
						}
					}
				}
				throw new Exception("type mismatch");
			}
			break;
		}
		case ScriptNode::Minus:
		{
			if( EvaluateExpression(obj,s->nodes[0],global,local,e1))
			if (EvaluateExpression(obj,s->nodes[1],global,local,e2))
			{
				if (e1.type == e2.type)
				{
					switch (e1.type)
					{
						case ScriptNode::Int:
						{
							result.SetData( e1.GetInt() - e2.GetInt() );
							return true;
						}
						case ScriptNode::Float:
						{
							result.SetData( e1.GetFloat() - e2.GetFloat() );
							return true;
						}
						case ScriptNode::Vec2:
						{
							result.SetData( e1.GetVec2() - e2.GetVec2() );
							return true;
						}
						case ScriptNode::Vec3:
						{
							result.SetData( e1.GetVec3() - e2.GetVec3() );
							return true;
						}
						case ScriptNode::Quat:
						{
							result.SetData( e1.GetQuat() - e2.GetQuat() );
							return true;
						}
						default:
						{
							throw new Exception("non comparable type");
						}
					}
				}
				throw new Exception("type mismatch");
			}
			break;
		}
		case ScriptNode::Divide:
		{
			if( EvaluateExpression(obj,s->nodes[0],global,local,e1))
			if (EvaluateExpression(obj,s->nodes[1],global,local,e2))
			{
				if (e1.type == e2.type)
				{
					switch (e1.type)
					{
						case ScriptNode::Int:
						{
							result.SetData( e1.GetInt() / e2.GetInt() );
							return true;
						}
						case ScriptNode::Float:
						{
							result.SetData( e1.GetFloat() / e2.GetFloat() );
							return true;
						}
						default:
						{
							throw new Exception("non comparable type");
						}
					}
				}
				throw new Exception("type mismatch");
			}
			break;
		}
		case ScriptNode::Times:
		{
			if( EvaluateExpression(obj,s->nodes[0],global,local,e1))
			if (EvaluateExpression(obj,s->nodes[1],global,local,e2))
			{
				if (e1.type == e2.type)
				{
					switch (e1.type)
					{
						case ScriptNode::Int:
						{
							result.SetData( e1.GetInt() * e2.GetInt() );
							return true;
						}
						case ScriptNode::Float:
						{
							result.SetData( e1.GetFloat() * e2.GetFloat() );
							return true;
						}
						default:
						{
							throw new Exception("non comparable type");
						}
					}
				}
				else if (((e1.type == ScriptNode::Float || e2.type == ScriptNode::Float)) &&
						((e1.type == ScriptNode::Vec2 || e2.type == ScriptNode::Vec2) ||
						 (e1.type == ScriptNode::Vec3 || e2.type == ScriptNode::Vec3) ||
						 (e1.type == ScriptNode::Quat || e2.type == ScriptNode::Quat) ))
				{
					switch (e1.type)
					{
						case ScriptNode::Float:
						{
							switch (e2.type)
							{
								case ScriptNode::Vec2:
								{
									result.SetData( e2.GetVec2() * e1.GetFloat() );
									return true;
								}
								case ScriptNode::Vec3:
								{
									result.SetData( e2.GetVec3() * e1.GetFloat() );
									return true;
								}
								case ScriptNode::Quat:
								{
									result.SetData( e2.GetQuat() * e1.GetFloat() );
									return true;
								}
							}
							break;
						}
						case ScriptNode::Vec2:
						{
							result.SetData( e1.GetVec2() * e2.GetFloat() );
							return true;
						}
						case ScriptNode::Vec3:
						{
							result.SetData( e1.GetVec3() * e2.GetFloat() );
							return true;
						}
						case ScriptNode::Quat:
						{
							result.SetData( e1.GetQuat() * e2.GetFloat() );
							return true;
						}
					}
				}
				throw new Exception("type mismatch");
			}
			break;
		}
		case ScriptNode::Accessor:
		{
			// like e.g.  vec2Var.x
			std::string name;
			ScriptNode::Type acc;
			s->GetAccessor(name,acc);

			// lookup a symbol table value and return it
			SymbolNode* v = global.GetNode(name);
			if (v == NULL)
			{
				v = local.GetNode(name);
			}
			if (v == NULL)
			{
				throw new Exception("variable " + name + " not declared");
			}
			result.SetData( v->value.GetAccessor(acc) );
			return true;
		}
		case ScriptNode::FunctionCall:
		{
			return ExecuteFunction(obj,s,global,local,result);
		}
		case ScriptNode::Int:
		{
			result.SetData( s->GetInt() );
			return true;
		}
		case ScriptNode::Bool:
		{
			result.SetData( s->GetBool() );
			return true;
		}
		case ScriptNode::String:
		{
			result.SetData( s->GetString() );
			return true;
		}
		case ScriptNode::Float:
		{
			result.SetData( s->GetFloat() );
			return true;
		}
		case ScriptNode::Object:
		{
			result.SetData( s->GetObject() );
			return true;
		}
		case ScriptNode::This:
		{
			result.SetData((void*)obj);
			return true;
		}
		case ScriptNode::Vec2:
		{
			result.SetData( s->GetVec2() );
			return true;
		}
		case ScriptNode::Vec3:
		{
			result.SetData( s->GetVec3() );
			return true;
		}
		case ScriptNode::Quat:
		{
			result.SetData( s->GetQuat() );
			return true;
		}
		case ScriptNode::Identifier:
		{
			std::string name;
			s->GetIdentifier(name);
			// lookup a symbol table value and return it
			SymbolNode* v = global.GetNode(name);
			if (v == NULL)
			{
				v = local.GetNode(name);
			}
			if (v == NULL)
			{
				throw new Exception("variable " + name + " not declared");
			}
			else
			{
				result = v->value;
				return true;
			}
			break;
		}
		default:
		{
			throw new Exception("unknown expression");
		}
	}
	return false;
}

void Interpreter::ExecuteStatement(WorldObject* obj,
								   ScriptNode* s,
								   SymbolTable& global,
								   SymbolTable& local,
								   ScriptNode& result,
								   bool& isReturn)
{
	// best default for return type
	result.type = ScriptNode::Void;
	isReturn = false;

	const int maxLoopCheck = 10000;

	switch (s->type)
	{
		case ScriptNode::FunctionCall:
		{
			ExecuteFunction(obj,s,global,local,result);
			break;
		}
		case ScriptNode::Return:
		{
			ScriptNode* expression;
			s->GetReturnData(expression);
			EvaluateExpression(obj,expression,global,local,result);
			isReturn = true;
			break;
		}
		case ScriptNode::While:
		{
			ScriptNode* expression;
			ScriptNode* statementList;
			s->GetWhileData(expression,statementList);

			if (EvaluateExpression(obj,expression,global,local,result))
			{
				if (result.type != ScriptNode::Bool)
					throw new Exception("while expression not of boolean type");

				// make sure we don't go on forever
				int cntr = 0;
				while (result.GetBool() && cntr<maxLoopCheck)
				{
					bool isReturn;
					ExecuteStatementList(obj,statementList,global,local,result,isReturn);
					if (isReturn)
						break;

					if (!EvaluateExpression(obj,expression,global,local,result))
						break;

					cntr++;
				}
				if (cntr>=maxLoopCheck)
				{
					throw new Exception("while statement exceeded 10,000 iterations?");
				}
			}
			break;
		}
		case ScriptNode::If:
		{
			ScriptNode* expression;
			ScriptNode* statementList;
			std::vector<ScriptNode*> elsePart;
			s->GetIfData(expression,statementList,elsePart);

			ScriptNode result;
			if (EvaluateExpression(obj,expression,global,local,result))
			{
				if (result.type != ScriptNode::Bool)
				{
					throw new Exception("if expression not of boolean type");
				}
				else if (result.GetBool())
				{
					bool isReturn;
					ExecuteStatementList(obj,statementList,global,local,result,isReturn);
				}
				else
				{
					for (int i=0; i<elsePart.size(); i++)
					{
						if (elsePart[i]->type == ScriptNode::ElseIf)
						{
							elsePart[i]->GetElseIfData(expression,statementList);
							if (EvaluateExpression(obj,expression,global,local,result))
							{
								if (result.type != ScriptNode::Bool)
								{
									throw new Exception("else if expression not of boolean type");
								}
								else if (result.GetBool())
								{
									bool isReturn;
									ExecuteStatementList(obj,statementList,global,local,result,isReturn);
									break;
								}
							}
						}
						else if (elsePart[i]->type == ScriptNode::Else)
						{
							elsePart[i]->GetElseData(statementList);
							bool isReturn;
							ExecuteStatementList(obj,statementList,global,local,result,isReturn);
							break;
						}
					}
				}
			}
			break;
		}
		case ScriptNode::VariableDeclaration:
		{
			std::string ident;
			ScriptNode::Type type;
			s->GetVariableDeclaration(ident,type);

			// make sure this variable doesn't exist in the global
			// or local symbol tables
			if (global.GetNode(ident)!=NULL)
				throw new Exception("variable already declared globally");
			if (local.GetNode(ident)!=NULL)
				throw new Exception("variable already declared locally");

			local.Add(SymbolNode(ident,type));
			break;
		}
		case ScriptNode::Assignment:
		{
			ScriptNode::Type accessorType = ScriptNode::Void;
			ScriptNode* ident;
			ScriptNode* expr;
			s->GetAssignment(ident,expr);
		
			std::string name;
			if (ident->type == ScriptNode::String)
			{
				name = ident->GetString();
			}
			else if (ident->type == ScriptNode::Accessor)
			{
				ident->GetAccessor(name,accessorType);
			}

			SymbolNode* n = global.GetNode(name);
			if (n == NULL)
			{
				n = local.GetNode(name);
				if (n == NULL)
					throw new Exception(name + " is an undeclared variable");
			}

			if (n != NULL)
			{
				ScriptNode result;
				if (EvaluateExpression(obj,expr,global,local,result))
				{
					if (accessorType == ScriptNode::Void)
					{
						if (result.type != n->value.type)
						{
							throw new Exception("expression evaluates to the wrong data-type");
						}
						switch (result.type)
						{
							case ScriptNode::Int:
							{
								n->value.SetData(result.GetInt());
								break;
							}
							case ScriptNode::Float:
							{
								n->value.SetData(result.GetFloat());
								break;
							}
							case ScriptNode::Bool:
							{
								n->value.SetData(result.GetBool());
								break;
							}
							case ScriptNode::String:
							{
								n->value.SetData(result.GetString());
								break;
							}
							case ScriptNode::Object:
							{
								n->value.SetData(result.GetObject());
								break;
							}
							case ScriptNode::Vec2:
							{
								n->value.SetData(result.GetVec2());
								break;
							}
							case ScriptNode::Vec3:
							{
								n->value.SetData(result.GetVec3());
								break;
							}
							case ScriptNode::Quat:
							{
								n->value.SetData(result.GetQuat());
								break;
							}
						}
					}
					else
					{
						if (result.type != ScriptNode::Float)
						{
							throw new Exception("expression evaluates to the wrong data-type for accessor\n(must be float)");
						}
						n->value.SetAccessor(accessorType,result.GetFloat());
					}
				}
			}
			break;
		}
		default:
		{
			throw new Exception("unknown statement type " + System::Int2Str(s->type));
		}
	}
}

