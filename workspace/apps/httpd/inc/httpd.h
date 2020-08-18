
#ifndef __HTTPD_H__
#define __HTTPD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <dss.h>


#ifndef htons
#define htons(x) \
        ((unsigned short)((((unsigned short)(x) & 0x00ff) << 8) | \
                  (((unsigned short)(x) & 0xff00) >> 8)))
#endif

#define LOCALHOST "127.0.0.1"
#define DSS_NET_IP	NULL

typedef struct httpd{
	uint32_t port;
	int socket;
	__DSS_NOTIFY__;
} httpd_t;


httpd_t *httpd_new(const char *addr, uint32_t port);
int httpd_stop(httpd_t *ctx);
void httpd_start(httpd_t *ctx);

#ifdef __cplusplus
}
#endif

#endif 

