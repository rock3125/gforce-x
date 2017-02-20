#include "standardFirst.h"

#include "system/converters/vrml/VRMLExport.h"
#include "system/model/mesh.h"
#include "system/model/model.h"

///////////////////////////////////////////////////////////////////////////////////

VRMLExport::VRMLExport()
{
}

VRMLExport::~VRMLExport()
{
}

void VRMLExport::Export(FileStreamer& fs,Model* model)
{
	StringBuilder sb;
	sb.Append(Prolog(model));
	fs.WriteData("",sb.GetString(),sb.Length());

	sb.Clear();
	sb.Append(ModelToString(model));
	fs.WriteData("",sb.GetString(),sb.Length());

	sb.Clear();
	sb.Append(Epilog());
	fs.WriteData("",sb.GetString(),sb.Length());
}

std::string VRMLExport::ModelToString(Model* model)
{
	StringBuilder sb;

	if (model == NULL)
		return "";

	sb.Append(Geometry(model));

	std::vector<PRS*> children = model->GetChildren();
	if (children.size()>0)
	{
		for (int i=0; i<children.size(); i++)
		{
			sb.Append(ModelToString(dynamic_cast<Model*>(children[i])));
		}
	}
	return sb.ToString();
}

std::string VRMLExport::Geometry(Model* model)
{
	StringBuilder sb;
	Mesh* mesh = model->GetMesh();

	// can't have empty meshes
	if (mesh == NULL || mesh->GetNumTriangles() == 0 || mesh->GetNumVertices() == 0)
		return sb.ToString();

	sb.Append("    Shape \n");
	sb.Append("	{\n");
	sb.Append("      appearance Appearance \n");
	sb.Append("	  {\n");
	sb.Append("        material Material \n");
	sb.Append("		{\n");
	D3DXCOLOR colour = model->GetColour();
	sb.Append("          diffuseColor " + System::Float2Str(colour.r) + " " + System::Float2Str(colour.g) + " " + System::Float2Str(colour.b) + "\n");
	sb.Append("          ambientIntensity 0.2988\n");
	sb.Append("          specularColor 0.045 0.045 0.045\n");
	sb.Append("          shininess 0.2875\n");
	sb.Append("          transparency 0\n");
	sb.Append("        }\n");
	sb.Append("      }\n");
	sb.Append("      geometry DEF " + model->GetName() + "-FACES IndexedFaceSet\n");
	sb.Append("	  {\n");
	sb.Append("        ccw TRUE\n");
	sb.Append("        solid TRUE\n");
	sb.Append("        coord DEF " + model->GetName() + "-COORD Coordinate { point [\n");

	D3DPVERTEX* v = mesh->GetVertices();
	D3DPINDEX* index = mesh->GetIndices();

	for (int i=0; i<mesh->GetNumVertices(); i++)
	{
		sb.Append("          ");
		sb.Append(System::Float2Str(v[i].position.x) + " ");
		sb.Append(System::Float2Str(v[i].position.y) + " ");
		sb.Append(System::Float2Str(v[i].position.z));
		if ((i+1) < mesh->GetNumVertices())
		{
			sb.Append(", \n");
		}
		else
		{
			sb.Append("]\n");
		}
	}
	sb.Append("        }\n");

	sb.Append("        normal Normal { vector [\n");
	for (int i=0; i<mesh->GetNumVertices(); i++)
	{
		sb.Append("          ");
		sb.Append(System::Float2Str(v[i].normal.x) + " ");
		sb.Append(System::Float2Str(v[i].normal.y) + " ");
		sb.Append(System::Float2Str(v[i].normal.z));
		if ((i+1) < mesh->GetNumVertices())
		{
			sb.Append(", \n");
		}
		else
		{
			sb.Append("]\n");
		}
	}
	sb.Append("         }\n");
	sb.Append("         normalPerVertex TRUE\n");
	sb.Append("         coordIndex [\n");

	for (int i=0; i<mesh->GetNumTriangles(); i++)
	{
		sb.Append("          ");
		sb.Append(System::Int2Str(index[i].v1) + ", ");
		sb.Append(System::Int2Str(index[i].v2) + ", ");
		sb.Append(System::Int2Str(index[i].v3) + ", -1");
		if ((i+1) < mesh->GetNumTriangles())
		{
			sb.Append(", \n");
		}
		else
		{
			sb.Append("]\n");
		}
	}

	sb.Append("         normalIndex [\n");
	for (int i=0; i<mesh->GetNumTriangles(); i++)
	{
		sb.Append("          ");
		sb.Append(System::Int2Str(index[i].v1) + ", ");
		sb.Append(System::Int2Str(index[i].v2) + ", ");
		sb.Append(System::Int2Str(index[i].v3) + ", -1");
		if ((i+1) < mesh->GetNumTriangles())
		{
			sb.Append(", \n");
		}
		else
		{
			sb.Append("]\n");
		}
	}

	sb.Append("		texCoord DEF " + model->GetName() + "-TEXCOORD TextureCoordinate { point [\n");
	for (int i=0; i<mesh->GetNumVertices(); i++)
	{
		sb.Append("          ");
		sb.Append(System::Float2Str(v[i].texture.x) + " ");
		sb.Append(System::Float2Str(v[i].texture.y));
		if ((i+1) < mesh->GetNumVertices())
		{
			sb.Append(", \n");
		}
		else
		{
			sb.Append("]\n");
		}
	}
	sb.Append("        }\n");
	sb.Append("        coordIndex [\n");

	for (int i=0; i<mesh->GetNumTriangles(); i++)
	{
		sb.Append("          ");
		sb.Append(System::Int2Str(index[i].v1) + ", ");
		sb.Append(System::Int2Str(index[i].v2) + ", ");
		sb.Append(System::Int2Str(index[i].v3) + ", -1");
		if ((i+1) < mesh->GetNumTriangles())
		{
			sb.Append(", \n");
		}
		else
		{
			sb.Append("]\n");
		}
	}
	sb.Append("      }\n");
	sb.Append("    }\n");

	return sb.ToString();
}

std::string VRMLExport::Prolog(Model* model)
{
	StringBuilder sb;

	sb.Append("#VRML V2.0 utf8\n\n");

	sb.Append("DEF " + model->GetName() + " Transform\n");
	sb.Append("{\n");
	D3DXMATRIXA16 mat = model->GetLocalTransform();
	sb.Append("  translation " + System::Float2Str(mat._41) + " " + System::Float2Str(mat._42) + " " + System::Float2Str(mat._43) + "\n");
	sb.Append("  children [\n");

	return sb.ToString();
}

std::string VRMLExport::Epilog()
{
	StringBuilder sb;
	sb.Append("  ]\n");
	sb.Append("}\n");
	return sb.ToString();
}

