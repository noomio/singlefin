#include "qapi_fs_types.h"
#include "qapi_status.h"
#include "qapi_socket.h"
#include "qapi_ns_utils.h"
#include "debug.h"
#include "httpd.h"

httpd_t * httpd_new(const char *addr, uint32_t port){
	httpd_t *ctx = malloc(sizeof(httpd_t));
	if(addr == NULL)
		memset(ctx->notify.addr,'\0',DSS_ADDR_SIZE);
	else
		strncpy(ctx->notify.addr,addr,DSS_ADDR_SIZE-1);

	ctx->port = port;
	ctx->socket = -1;
	txm_module_object_allocate(&ctx->notify.evt, sizeof(TX_EVENT_FLAGS_GROUP));
	tx_event_flags_create(ctx->notify.evt, "httpd_event_handle");
	return ctx;
}

int httpd_stop(httpd_t *ctx){
	return 0;
}


__attribute__ ((noreturn)) void httpd_start(httpd_t *ctx){

	const char *hello_str = "Hello from BG96!\n";
	const char *goodbye_str = "Goodbye from BG96!\n";
	char addr[16];

	struct sockaddr_in sockaddr_server,sockaddr_client;	
	memset(&sockaddr_client,0,sizeof(struct sockaddr_in));
	memset(&sockaddr_server,0,sizeof(struct sockaddr_in));

	TX_DEBUGF(HTTPD_DBG,("httpd: waiting for network...\n"));

	dss_wait_conn_notify(ctx);

	ctx->socket = qapi_socket(AF_INET,SOCK_STREAM,0);

	sockaddr_server.sin_family = AF_INET;
	sockaddr_server.sin_port = htons(ctx->port);
	sockaddr_server.sin_addr.s_addr = inet_addr(ctx->notify.addr);

	TX_DEBUGF(HTTPD_DBG,("httpd: socket=%d,address=%s,port=%u\n",ctx->socket,ctx->notify.addr,ctx->port));

	if(qapi_bind(ctx->socket, (struct sockaddr*)&sockaddr_server, sizeof(struct sockaddr_in)) == -1){
		TX_DEBUGF(HTTPD_DBG | TX_DBG_LEVEL_SERIOUS, ("httpd: address binding error!\n"));
	}

	//qapi_setsockopt(socket , SOL_SOCKET, SO_NBIO, NULL, 0);
	qapi_listen(ctx->socket, 4);

	for(;;){

		char buf[128];
		int recvd;
		int len = sizeof(sockaddr_client);

		TX_DEBUGF(HTTPD_DBG,("%s\n","httpd: accept waiting..."));
		int new_sock_fd = qapi_accept(ctx->socket, (struct sockaddr *)&sockaddr_client, &len);
		
		const char *ip_p = inet_ntop(AF_INET,&sockaddr_client.sin_addr.s_addr,addr,sizeof(addr));
		TX_DEBUGF(HTTPD_DBG,("httpd: accepted connection from: %s!\n",ip_p));
		qapi_send(new_sock_fd, hello_str, strlen(hello_str),MSG_DONTWAIT);

		qapi_setsockopt(new_sock_fd , SOL_SOCKET, SO_NBIO, NULL, 0);
		if((recvd = qapi_recv(new_sock_fd, buf, 128, 0)) > 0){
			buf[127] = '\0';
			TX_DEBUGF(HTTPD_DBG,("%s\n", buf));
		}

		qapi_send(new_sock_fd, goodbye_str, strlen(goodbye_str),MSG_DONTWAIT);
		qapi_socketclose(new_sock_fd);
	}


}


