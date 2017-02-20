#include "reflector/stdafx.h"

#include "system/network/networkServer.h"
#include "system/xml/xmlparser.h"

//
// start a networks server & handle connection protocols
// also reflect all data to clients
//
int _tmain(int argc, _TCHAR* argv[])
{
	System::Initialise();

	if (argc != 4)
	{
		printf("Gravity Force X reflector takes 3 parameters\n");
		printf("reflector \"level filename\" gameType maxPoints");
		return 0;
	}

	// start a server with the required parameters
	std::string levelName = argv[1];
	std::string fullPathLevelName = System::GetDataDirectory() + levelName;
	int gameType = System::Str2Int(argv[2]);
	int maxPoints = System::Str2Int(argv[3]);

	XmlParser parser;
	if (parser.LoadAndParse(fullPathLevelName))
	{
		if (maxPoints == 0)
		{
			printf("maxPoints must be greater than 0\n");
			return 0;
		}

		// get base count from level
		int numBases = 0;
		XmlNode* root = parser.GetDocumentRoot();
		XmlNode* level = root->GetChild("level");
		std::vector<XmlNode*> objects = level->GetChildren();
		std::vector<XmlNode*>::iterator pos = objects.begin();
		while (pos != objects.end())
		{
			XmlNode* node = *pos;
			if (node->GetTag() == "obj")
			{
				if (node->GetValue("type") == "8")
				{
					numBases++;
				}
			}
			pos++;
		}

		// check num bases add up
		if (numBases == 0)
		{
			printf("level \"%s\" contains no bases - can't be played\n", levelName.c_str());
			return 0;
		}

		// setup server
		NetworkServer server(levelName, gameType, maxPoints);

		// start hosting on this port
		if (server.StartServer(GAME_PORT))
		{
			printf("Gravity Force X Reflector (0.7.0)\n");
			printf("(c) Excession Software Ltd, 2006\n\n\n");
			printf("this level supports %d players\n", numBases);
			printf("started hosting on port %d\n\nlevel %s\ngameType %d\nmaxPoints %d\n\n" ,
						GAME_PORT, levelName.c_str(), gameType, maxPoints);

			// set how many players are allowed to connect
			server.SetMaxNumberOfPlayers(numBases);

			while (true)
			{
				// 200 fps
				Sleep(5);

				// process messages
				server.EventLogic();
			}
			server.StopServers();
		}
		return 0;
	}
	else
	{
		printf("error loading level \"%s\":\n%s\n", levelName.c_str(), parser.GetError().c_str());
	}
}

