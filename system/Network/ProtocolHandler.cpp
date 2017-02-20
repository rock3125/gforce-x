//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "reflector/stdafx.h"

#include "system/network/networkServer.h"
#include "system/network/ProtocolHandler.h"

//////////////////////////////////////////////////////////////////////

ProtocolHandler::ProtocolHandler()
{
}

ProtocolHandler::~ProtocolHandler()
{
}

bool ProtocolHandler::IsProtocolPacket(BasePacket* packet)
{
	if (packet == NULL)
	{
		throw new Exception("packet can not be null");
	}
	// these packets needs special handling by the server
	// and shouldn't be passed on
	switch (packet->id)
	{
		case PID_CLIENT_CANIJOIN:
		case PID_CLIENT_IWILLJOIN:
		case PID_CLIENT_STARTED:
		{
			return true;
		}
	}
	return false;
}

void ProtocolHandler::ProcessPacket(NetworkServer* server, byte playerId, BasePacket* basePacket)
{
	byte buffer[NETWORK_MAX_BUFFER_SIZE];

	if (!ProtocolHandler::IsProtocolPacket(basePacket))
	{
		throw new Exception("only protocol packets are allowed to be processed by the protocol handler");
	}

	// these packets needs special handling by the server
	// and shouldn't be passed on
	switch (basePacket->id)
	{
		// check a player joining (right protocol/name etc).
	case PID_CLIENT_CANIJOIN:
		{
			PacketClientCanIJoin* packet = dynamic_cast<PacketClientCanIJoin*>(basePacket);

			// check the name is unique and the version is correct
			std::string errStr;
			if (server->CheckNewPlayer(packet->name, packet->version, errStr))
			{
				// the player can join (since they have an id)
				PacketServerYouCan packetServerYouCan(true, server->LevelName(), playerId, "welcome");
				int size = packetServerYouCan.Write(buffer);
				server->SendDataToPlayer(playerId, buffer, size);

				printf("player %d (%s) is joining, waiting for confirmation\n", playerId, packet->name.c_str());

				server->SetName(playerId, packet->name);
			}
			else
			{
				// the player can join (since they have an id)
				PacketServerYouCan packetServerYouCan(false, "", playerId, errStr);
				int size = packetServerYouCan.Write(buffer);
				server->SendDataToPlayer(playerId, buffer, size);

				printf("player %d (%s) can not join (%s)\n", playerId, packet->name.c_str(), errStr.c_str());
			}
			break;
		}

		// client confirms joining with more details
	case PID_CLIENT_IWILLJOIN:
		{
			std::string name = server->GetName(playerId);

			PacketClientIWillJoin* packet = dynamic_cast<PacketClientIWillJoin*>(basePacket);
			server->SetClientDetails(playerId, packet->accept, packet->shipId, packet->team);

			if (packet->accept)
			{
				printf("player %d (%s) has joined, waiting for activation\n", playerId, name.c_str());

				// announce this to all other players
				PacketServerOtherPlayerJoined packet2(playerId, name, packet->team, packet->shipId);
				int size = packet2.Write(buffer);
				server->Reflect(playerId, buffer, size);

				// send welcome message to this player with other player's details
				PacketServerWelcome serverWelcome(server->GetGameType(), server->GetMaxPoints());
				// get server details
				server->SetupWelcomePacket(&serverWelcome);
				size = serverWelcome.Write(buffer);
				server->SendDataToPlayer(playerId, buffer, size);
			}
			else
			{
				printf("player %d (%s) will not join us afterall\n", playerId, name.c_str());
			}
			break;
		}

		// client moves to active state
	case PID_CLIENT_STARTED:
		{
			std::string name = server->GetName(playerId);

			server->ActivatePlayer(playerId);
			printf("player %d (%s) is now active\n", playerId, name.c_str());

			// reflect the same packet to all existing connected players
			// send welcome message to this player with other player's details
			PacketServerWelcome serverWelcome(server->GetGameType(), server->GetMaxPoints());
			// get server details
			server->SetupWelcomePacket(&serverWelcome);
			int size = serverWelcome.Write(buffer);
			server->Reflect(-1, buffer, size);

			break;
		}
	default:
		{
			throw new Exception("unknown protocol packet");
		}
	}
}

