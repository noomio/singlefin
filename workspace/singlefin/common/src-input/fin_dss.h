#if !defined(FIN_API_DSS_H_INCLUDED)
#define FIN_API_DSS_H_INCLUDED

#define DSS_ADDR_SIZE 48

/* Number of simultaneously dss data calls supported */
#define DSS_CONCURRENT_NUM	4


#define __FIN_DSS_NOTIFY__ fin_dss_notify_t notify


extern fin_dss_ctx_t *dss_ctx_store[DSS_CONCURRENT_NUM];

#endif  /* FIN_API_DSS_H_INCLUDED */