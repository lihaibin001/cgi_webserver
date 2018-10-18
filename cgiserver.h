#ifndef __CGISERVER_H__
#define __CGISERVER_H__

#include <stdbool.h>

typedef enum
{
    CGI_STATUS_LOGOUT,
    CGI_STATUS_LOGIN,
    CGI_STATUS_INDEX,
    CGI_STATUS_PRICE_MODIFY,
    CGI_STATUS_LOCAL_MAP_CFG,
    CGI_STATUS_PARAM_MODIFY,
    CGI_STATUS_ERR,
}cgi_status_t;

bool cgi_req_user_certified(char name[], char password[]);
void cgi_server(void);
cgi_status_t cgi_get_status(void);
bool cgi_is_certified(void);

#endif //__CGISERVER_H__
