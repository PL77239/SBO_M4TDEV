#include <Windows.h>
#include "..\server.h"

void ServerAuth(CLIENT* client)
{
	Server* server = (Server*)client->server;
	switch (client->inbuf.getSubType())
	{
	case 0x0000: // Server
	{
		if (client->isAuth == FALSE && client->inbuf.getSize() == 0x20)
		{
			uint8_t iv[16];
			client->inbuf.getArray(&iv[0], 16, 0x06);
			int32_t res = server->db->Open();
			if (res == DB_OK)
			{
				std::string ivStr = server->HexString(&iv[0], 16);
				std::stringstream ss;
				server->logger->Log(LOGTYPE_COMM, L"Otrzymany IV od serwera: %s", server->logger->ToWide(ivStr).c_str());
				ss << "SELECT * FROM server_keys WHERE iv = X'" << ivStr << "'";
				res = server->db->Exec(ss.str().c_str());
				if (res)
				{
					// server_keys columns are (key, iv). Prefer named lookup so map iteration order cannot swap them.
					std::string _key = server->db->GetValue(&server->db->results, 0, (char*)"key");
					std::string _iv = server->db->GetValue(&server->db->results, 0, (char*)"iv");
					if (_key.empty() || _iv.empty())
					{
						_key = server->db->GetValue(&server->db->results, 0, 0);
						_iv = server->db->GetValue(&server->db->results, 0, 1);
					}
					
					for (auto& o : server->connections)
					{
						if (!memcmp(o->iv, _iv.c_str(), min((size_t)16, _iv.size())) && !memcmp(o->key, _key.c_str(), min((size_t)32, _key.size())))
						{
							client->Disconnect();
							server->db->Close();
							server->logger->Log(LOGTYPE_COMM, L"Server connecting from %s is already connected", server->logger->ToWide((char*)client->IP_Address).c_str());
							return;
						}
					}
					ZeroMemory(&client->iv[0], sizeof(client->iv));
					ZeroMemory(&client->key[0], sizeof(client->key));
					CopyMemory(&client->iv[0], _iv.c_str(), min((size_t)16, _iv.size()));
					CopyMemory(&client->key[0], _key.c_str(), min((size_t)32, _key.size()));
					client->SetAuth();
					server->logger->Log(LOGTYPE_COMM, L"Server authenticated from %s", server->logger->ToWide((char*)client->IP_Address).c_str());
					client->sendAuth();
				}
				else
				{
					server->logger->Log(LOGTYPE_COMM, L"No key found for server connecting from %s (IV %s). Put matching serverkey.bin in the DB Server working directory and restart, or run: SBOL DB Server.exe /createkey and copy the new file next to the Battle Server exe.",
						server->logger->ToWide((char*)client->IP_Address).c_str(),
						server->logger->ToWide(ivStr).c_str());
					client->Disconnect();
				}
				server->db->Close();
			}
		}
	}
	break;
	case 0x0001: // Heartbeat
	{
		client->lastHeartBeat = timeGetTime() + HEARTBEAT_TIME;
	}
	break;
	}
}
