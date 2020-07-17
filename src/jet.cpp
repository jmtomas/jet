#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "common/ipc.cpp"
#include "server/page.cpp"
#include "server/buffer.cpp"
#include "server/point.cpp"
#include "server/client.cpp"

#define PORT 6969
#define MAX_EVENTS 10

int create_listener() {
	int s = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in addr = { AF_INET, htons(PORT), htonl(INADDR_LOOPBACK)};
	int opt = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	bind(s, (sockaddr *) &addr, sizeof(sockaddr_in));
	listen(s, MAX_EVENTS);
	return s;
}

int main() {
	Buffer scratch("scratch");
	scratch.read_file("test.txt");

	int listener = create_listener();

	int epollfd = epoll_create1(0);

	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = listener;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev);

	epoll_event events[MAX_EVENTS];
	Client *clients[1024] = {};

	while (true) {
		int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

		for (int i = 0; i < nfds; i++) {
			if (events[i].data.fd == listener) {
				int clientfd = accept(listener, 0, 0);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = clientfd;
				if (clients[clientfd]) {
					delete clients[clientfd];
				}
				Client *c = new Client(scratch);
				c->sockfd = clientfd;
				clients[clientfd] = c;
				epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
			} else {
				clients[events[i].data.fd]->parse_message();
			}
		}
	}

	close(listener);
}
