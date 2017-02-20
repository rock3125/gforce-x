#include "clientTest/stdafx.h"

#include "system/network/networkClient.h"
#include "system/network/dataPacket.h"

int _tmain(int argc, _TCHAR* argv[])
{
	System::Initialise();

	byte buffer[NETWORK_MAX_BUFFER_SIZE];

	if (argc != 3)
	{
		printf("Gravity Force X client test takes 2 parameters\n");
		printf("reflector \"user name\" server");
		return 0;
	}

	int cntr = 0;
	NetworkClient* client = NetworkClient::Get();
	if (client->StartClient(argv[1], argv[2], GAME_PORT))
	{
		while (true)
		{
			::Sleep(20);
			client->EventLogic();

			// force it as soon as we reach this state - not a real test
			client->AcknowledgeReady(1,1);

			if (cntr % 100 == 0)
			{
				// create a fake packet and send it
				DataPosition pos(client->GetPlayerId());
				if (!client->SendPacketToServer(&pos))
				{
					break;
				}
			}
			cntr++;
		}
	}

	// close down connection
	client->StopClient();

	return 0;
}

