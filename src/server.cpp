#include <uWS.h>

#include <iostream> // DEBUG
#include <string> 
#include <nlohmann/json.hpp>
using nlohmann::json;

#pragma comment (lib, "uWS.lib")
#pragma comment (lib, "libeay32.lib")
#pragma comment (lib, "ssleay32.lib")
#pragma comment (lib, "libuv.lib")
#pragma comment (lib, "zlib.lib")

using namespace std;

struct room;
struct player {
	uWS::WebSocket<uWS::SERVER>* ws;
	string user;
	string pw;
	room* room{nullptr};
};
struct chart {
	string title;
	string subtitle;
	string artist;
	string chartkey;
	int meter;
	int difficulty;
	void fromJson(json j) {}
	json asJson() { json j; return j; }
};
struct room {
	string title;
	string desc;
	string pw;
	vector<player*> players;
	json asJson() {
		json j;
		j["title"] = title.c_str();
		j["desc"] = title.c_str();
		json players = json::array({});
		for (auto& player : this->players)
			players.push_back(player->user.c_str());
		j["players"] = players;
		return j;
	}
};
json roomList(vector<room*> rooms) {
	json j = json::array({});
	for (auto& room : rooms)
		j.push_back(room.asJson());
	return j;
}
int main(int argc, char* argv[])
{
  const int port = 8765;

  std::cout << "Sever starting up...\n";

  uWS::Hub h;
  vector<room*> rooms;
  vector<player*> players;
  map<uWS::WebSocket<uWS::SERVER>*, player*> playersBySocket;

  json hello;
  hello["type"] = "hi";
  h.onMessage([&](uWS::WebSocket<uWS::SERVER>* ws, char* message, size_t length, uWS::OpCode) {
	  string msg(message, length);
	  json j = parse(msg);
	  j["type"];
  });
  h.onConnection([&](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req) {
	  std::cout << "connection\n";
	  ws->send(hello.dump().c_str());
	  json roomMsg;
	  roomMsg["type"] = "roomlist";
	  roomMsg["rooms"] = roomList(rooms);
	  ws->send(roomMsg.dump().c_str());
	  player* p = new player;
	  p->ws = ws;
	  playersBySocket[ws] = p;
  });
  h.onDisconnection([&](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length) {
	  std::cout << "disconnection\n";
	  auto it = playersBySocket.find(ws);
	  if (it != playersBySocket.end()) {
		  auto itt = find(players.begin(), players.end(), it->second);
		  if (itt != players.end())
			  players.erase(itt);
		  delete it->second;
		  playersBySocket.erase(it);
	  }
  });

  // connect to port and exit if blocked
  if (!h.listen(port)) return 1;
  std::cout << "Listening on port "<< port <<"\n";

  h.run();
}