#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <common/ipc.cpp>
#include <common/file.cpp>
#include <common/socket.cpp>

#include <server/page.cpp>
#include <server/buffer.cpp>
#include <server/point.cpp>
#include <server/client.cpp>

Client *clients[1024] = {};
Buffer scratch("scratch");

void new_connection(int fd) {
	if (clients[fd]) {
		delete clients[fd];
	}
	clients[fd] = new Client(scratch, fd);
}

void existing_connection(int fd) {
	clients[fd]->parse_message();
}

int main() {
	scratch.read_file("test.txt");

	Socket listener;
	listener.listen(new_connection, existing_connection);
}
