#include "GameServer.h"
#include "GameWorld.h"
#include <iostream>

using namespace NCL;
using namespace CSC8503;

GameServer::GameServer(int onPort, int maxClients)	{
	port		= onPort;
	clientMax	= maxClients;
	clientCount = 0;
	netHandle	= nullptr;
	//threadAlive = false;

	Initialise();
}

GameServer::~GameServer()	{
	Shutdown();
}

void GameServer::Shutdown() {
	SendGlobalPacket(BasicNetworkMessages::Shutdown);

	//threadAlive = false;
	//updateThread.join();

	enet_host_destroy(netHandle);
	netHandle = nullptr;
}

bool GameServer::Initialise() {
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;

	netHandle = enet_host_create(&address, clientMax, 1, 0, 0);

	if (!netHandle) {
		std::cout << __FUNCTION__ << " failed to create network handle!" << std::endl;
		return false;
	}
	//threadAlive		= true;
	//updateThread	= std::thread(&GameServer::ThreadedUpdate, this);

	return true;
}

bool GameServer::SendGlobalPacket(int msgID) {
	GamePacket packet;
	packet.type = msgID;

	return SendGlobalPacket(packet);
}

bool GameServer::SendGlobalPacket(GamePacket& packet) {
	ENetPacket* dataPacket = enet_packet_create(&packet, packet.GetTotalSize(), 0);
	enet_host_broadcast(netHandle, 0, dataPacket);
	return true;
}

void GameServer::UpdateServer() {
	if (!netHandle) {
		return;
	}

	ENetEvent event;
	while (enet_host_service(netHandle, &event, 0) > 0)	{
		int type	= event.type;
		ENetPeer* p = event.peer;

		int peer = p->incomingPeerID;

		if (type == ENetEventType::ENET_EVENT_TYPE_CONNECT) {
			std::cout << "Server: New client connected" << std::endl;
			NewPlayerPacket player(peer);
			SendGlobalPacket(player);
		}
		else if (type == ENetEventType::ENET_EVENT_TYPE_DISCONNECT) {
			std::cout << "Server: A client has disconnected" << std::endl;
			PlayerDisconnectPacket player(peer);
			SendGlobalPacket(player);
		}
		else if (type == ENetEventType::ENET_EVENT_TYPE_RECEIVE) {
			GamePacket* packet = (GamePacket*)event.packet->data;
			ProcessPacket(packet, peer);
		}
		enet_packet_destroy(event.packet);
	}
}

//void GameServer::ThreadedUpdate() {
//	while (threadAlive) {
//		UpdateServer();
//	}
//}

//Second networking tutorial stuff

void GameServer::SetGameWorld(GameWorld &g) {
	gameWorld = &g;
}