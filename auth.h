//
// Created by ChileungL on 24/05/2018.
//

#pragma once

typedef struct {

    char user_agent[128];
    char algo_id[64];

    char host_name[16];
    char local_time[32];
    char client_id[64];
    char mac_addr[32];
    char ostag[32];

    char cdc_domain[16];
    char cdc_area[16];
    char cdc_schoolid[16];


    char ipv4_addr[16];

    char ticket_url[256];
    char auth_url[256];

    char ticket[64];

    char keep_url[256];
    char term_url[256];
} AUTH_CONTEXT;

int auth_init(AUTH_CONTEXT *ctx,
              const char *cdc_domain,
              const char *cdc_area,
              const char *cdc_schoolid,
              const char *ostag,
              const char *host_name,
              const char *user_agent,
              const char *algo_id,
              const char *client_id
);

int auth_login(AUTH_CONTEXT *ctx, const char *userid, const char *passwd);

long auth_keep(AUTH_CONTEXT *ctx);

int auth_logout(AUTH_CONTEXT *ctx);