#include <stdio.h>
#include <string.h>

#include <common/ipc.cpp>
#include <common/socket.cpp>
#include <client/window.cpp>
#include <client/cursor.cpp>
#include <client/interface.cpp>

int main(int argc, char *argv[]) {
	Interface interface;
	interface.run();
	return 0;
}
