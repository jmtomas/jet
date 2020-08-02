#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10

#define PORT 6969

struct Socket {
	int descriptor;

	Socket() {
		descriptor = socket(AF_INET, SOCK_STREAM, 0);
	}

	Socket(int descriptor) : descriptor(descriptor) {}

	typedef void (*EventAction) (int);

	void listen(EventAction on_new_connection, EventAction on_existing_connection) {
		sockaddr_in addr = { AF_INET, htons(PORT), htonl(INADDR_LOOPBACK)};
		int opt = 1;
		setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
		bind(descriptor, (sockaddr *) &addr, sizeof(sockaddr_in));
		::listen(descriptor, MAX_EVENTS);

		int epollfd = epoll_create1(0);

		epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = descriptor;
		epoll_ctl(epollfd, EPOLL_CTL_ADD, descriptor, &ev);

		epoll_event events[MAX_EVENTS];

		while (true) {
			int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

			for (int i = 0; i < nfds; i++) {
				if (events[i].data.fd == descriptor) {
					int clientfd = accept(descriptor, 0, 0);
					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = clientfd;
					epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
					on_new_connection(clientfd);
				} else {
					on_existing_connection(events[i].data.fd);
				}
			}
		}

	}

	void connect() {
		sockaddr_in addr = { AF_INET, htons(PORT), htonl(INADDR_LOOPBACK) };
		::connect(descriptor, (sockaddr *) &addr, sizeof(sockaddr_in));
	}

	size_t send(const void *msg, size_t length) {
		return write(descriptor, msg, length);
	}

	size_t send(Message &msg) {
		size_t result = write(descriptor, msg.data, msg.offset);
		msg.offset = 0;
		return result;
	}

	size_t recv(void *msg, size_t length) {
		return read(descriptor, msg, length);
	}

	size_t recv(Message &msg) {
		msg.offset = 0;
		return read(descriptor, msg.data, MESSAGE_SIZE);
	}

	~Socket() {
		close(descriptor);
	}
};
