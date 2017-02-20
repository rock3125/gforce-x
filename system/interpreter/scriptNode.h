#pragma once

/////////////////////////////////////////////////////

class ScriptNode
{
public:
	ScriptNode();

	ScriptNode(std::string str);
	ScriptNode(int i);
	ScriptNode(float f);
	ScriptNode(bool b);
	ScriptNode(void* v);
	ScriptNode(const D3DXVECTOR2& vec2);
	ScriptNode(const D3DXVECTOR3& vec3);
	ScriptNode(const D3DXQUATERNION& q);

	~ScriptNode();
	ScriptNode(const ScriptNode&);
	const ScriptNode& operator=(const ScriptNode&);

	// access the data stored in a while
	void GetWhileData(ScriptNode*& expr,ScriptNode*& statements);
	// access if expression data
	void GetIfData(ScriptNode*& expr,ScriptNode*& compound,std::vector<ScriptNode*>& elseList);
	void GetElseIfData(ScriptNode*& expr,ScriptNode*& compound);
	void GetElseData(ScriptNode*& compound);
	void GetReturnData(ScriptNode*& expr);

	// get assignment info
	void GetAssignment(ScriptNode*& ident,ScriptNode*& expr);

	// get function call info
	void GetFunctionCall(std::string& ident,std::vector<ScriptNode*>& params);

	// get identifier
	void GetIdentifier(std::string& name);

	enum Type
	{
		None,
		SystemFunction,
		ObjectList,
		ParameterList,
		Parameter,
		Float,
		Vec2,
		Vec3,
		Quat,
		Int,
		String,
		Bool,
		Void,
		Object,
		InitEvent,
		KeyDownEvent,
		KeyUpEvent,
		LogicEvent,
		EventRoutine,
		Function,
		StatementList,
		VariableDeclaration,
		Assignment,
		Return,
		While,
		If,
		ElseIf,
		Else,
		CompoundStatement,
		Identifier,
		FunctionCall,
		Not,
		LessThan,
		GreaterThan,
		LessThanOrEqual,
		GreaterThanOrEqual,
		Equal,
		NotEqual,
		Plus,
		Minus,
		Times,
		Divide,
		And,
		Or,
		This,
		Accessor,
		X,
		Y,
		Z,
		W
	};

	// variable declaration
	void GetVariableDeclaration(std::string& ident,Type& type);

	// get accessor of a complex type
	void GetAccessor(std::string& ident,Type& type);

	ScriptNode(Type type);
	ScriptNode(Type type,std::vector<ScriptNode*>);

	void SetData(int d);
	void SetData(float d);
	void SetData(bool d);
	void SetData(void* d);
	void SetData(std::string d);
	void SetData(D3DXVECTOR2 d);
	void SetData(D3DXVECTOR3 d);
	void SetData(D3DXQUATERNION d);
	void SetAccessor(Type type,float value);

	int GetInt();
	float GetFloat();
	bool GetBool();
	void* GetObject();
	const std::string& GetString();
	const D3DXVECTOR2& GetVec2();
	const D3DXVECTOR3& GetVec3();
	const D3DXQUATERNION& GetQuat();
	float GetAccessor(Type type);

public:
	Type						type;
	std::vector<ScriptNode*>	nodes;
	void*						data;
};

