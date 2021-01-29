#pragma once
#include "NetworkBase.h"
#include <stdint.h>
#include <thread>
#include <atomic>

namespace NCL {
	namespace CSC8503 {
		class GameObject;
		class GameClient : public NetworkBase {
		public:
			GameClient();
			~GameClient();

			bool Connect(uint8_t a, uint8_t b, uint8_t c, uint8_t d, int portNum);

			void SendPacket(GamePacket&  payload);

			void UpdateClient();
		protected:	
			//void ThreadedUpdate();

			ENetPeer*	netPeer;
			//std::atomic<bool>	threadAlive;
			//std::thread			updateThread;
		};
	}
}

