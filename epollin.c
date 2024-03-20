/*  epollin.c - epoll read events demo
    Copyright (C) 2024  hoff.industries

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/epoll.h>

#define MAX_EPOLL_EVENTS 16
#define MAX_INPUT_PIPES 4

int main() {
	int epollFd = -1;
	int eventsReady = 0;
	int inf[MAX_INPUT_PIPES][2];

	char message[] = "Hello there!";
	char receiveBuffer[4096];

	struct epoll_event einf[MAX_INPUT_PIPES];
	struct epoll_event eventQueue[MAX_EPOLL_EVENTS];

	epollFd = epoll_create1(0);
	if (epollFd == -1) {
		printf("Couldn't create epoll instance!\n");
	}

	printf("Watching for type %d events.\n", EPOLLIN);

	for (int i = 0; i < MAX_INPUT_PIPES; i++) {

		pipe(inf[i]);

		printf("fd for %d = %d, %d\n", i, inf[i][0], inf[i][1]);

		einf[i].events = EPOLLIN;
		einf[i].data.fd = inf[i][0];

		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, inf[i][0], &einf[i]) == -1) {
			printf("Couldn't add to epoll!\n");
		}

		if (i % 2 == 0) {

			write(inf[i][1], message, sizeof(message));
		}
	}

	while (true) {
		epoll_ctl(epollFd, EPOLL_CTL_DEL, inf[2][0], &einf[2]);

		eventsReady = epoll_wait(epollFd, eventQueue, MAX_EPOLL_EVENTS, -1);

		for (int i = 0; i < eventsReady; i++) {
			printf("Got an event %d for fd %d!\n",
				eventQueue[i].events, eventQueue[i].data.fd);
			read(eventQueue[i].data.fd, receiveBuffer, sizeof(receiveBuffer));

			printf("Received the message: %s\n", receiveBuffer);
		}
	}
}
