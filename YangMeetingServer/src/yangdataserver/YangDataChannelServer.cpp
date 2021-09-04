/*
 * YangServerAdapter.cpp
 *
 *  Created on: 2020年09月30日
 *      Author: yang
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <st.h>
#include <yangdataserver/YangDataChannelServer.h>
#include "YangUserDataHandle.h"
#include <pthread.h>
YangDataChannelServer::YangDataChannelServer() {
	m_port=9999;
	m_fd=0;
	m_loop=0;
	m_isStart=0;
	m_conf=NULL;
}

YangDataChannelServer::~YangDataChannelServer() {
	m_conf=NULL;
}


void YangDataChannelServer::run() {
	m_isStart = 1;
	tcpServer();
	m_isStart = 0;
}
void YangDataChannelServer::stop() {
	m_loop=0;

}


 void* YangDataChannelServer::handle_request(void *arg) {

	// cli_nfd;
	//st_netfd_t cli_nfd = (st_netfd_t) arg;
	YangUserDataHandle userHandle((st_netfd_t) arg);
	userHandle.startLoop();
	printf("\n*******************connect disconnet............\n");

	return NULL;
}

 YangMapMeeting* YangDataChannelServer::userData=nullptr;
void YangDataChannelServer::tcpServer() {

	struct sockaddr_in cli_addr;
	int n = 0;
	st_netfd_t cli_nfd;
	m_loop=1;
	while(m_loop) {
		n = sizeof(cli_addr);
		cli_nfd = st_accept(m_fd, (struct sockaddr*) &cli_addr, &n,
		ST_UTIME_NO_TIMEOUT);
		if (cli_nfd == NULL) {
			printf("st_accept error");
			exit(1);
		}
		if (st_thread_create(handle_request, cli_nfd, 0, 0) == NULL) {
			printf("st_thread_create error");
			exit(1);
		}
	}

}

void YangDataChannelServer::init(int pport) {
	m_port=pport;
	if (st_init() < 0) {
		printf("st_init");
		exit(1);
	}
	int sock, n;

	//int serialize_accept = 0;
	struct sockaddr_in lcl_addr;

	lcl_addr.sin_family = AF_INET;
	lcl_addr.sin_port = htons(m_port);
	lcl_addr.sin_addr.s_addr = INADDR_ANY;


	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket error");
		exit(1);
	}

	n = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &n, sizeof(n)) < 0) {
		printf("setsockopt error");
		exit(1);
	}
	if (bind(sock, (struct sockaddr*) &lcl_addr, sizeof(lcl_addr)) < 0) {
		printf("bind error");
		exit(1);
	}
	//int flags = fcntl(sock, F_GETFL, 0);
//	fcntl(sock, F_SETFL, flags | O_NONBLOCK);
	listen(sock, 128);
	if ((m_fd = st_netfd_open_socket(sock)) == NULL) {
		printf("st_netfd_open error");
		exit(1);
	}

//	if ((m_fd = st_netfd_open_socket(sock)) == NULL)
	//	printf("ERROR: st_netfd_open_socket");

	st_timecache_set(1);

}

