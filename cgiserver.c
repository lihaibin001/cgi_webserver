#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cgiserver.h"
#include "List.h"
#include "CommonDefine.h"

#define CGI_SRV_DEBUG_ENABLE 1

#if CGI_SRV_DEBUG_ENABLE
#define WEB_SRV_DEBUG(...) do{ \
    printf("[WEB SRV] ");\
	printf(__VA_ARGS__); \
	}while(0)
#else
#define WEB_SRV_DEBUG(...)
#endif

#define MSG_TYPE_USER_CERTIFY 1
#define MSG_TYPE_GET_STATUS 2
#define MSG_TYPE_GET_CERTIFIED_STATUS 3
#define MSG_TYPE_LOGTOU 4

#define MSG_DATA_LEN 2048

typedef struct
{
    long type;
    char data[MSG_DATA_LEN];
}msg_type_t;

typedef struct
{
    char name[32];
    char password[32];
    char ip[32];
}user_info_t;

static user_info_t g_default_user_info = 
{
    "root",
    "root",
    "",
};

static List *g_pUserList;

static user_info_t g_user_info;
static cgi_status_t g_status;
static bool g_certify_status;
static bool cgi_srv_user_certify(user_info_t *pUserInfo);

static bool cgi_add_new_user(user_info_t *pUserInfo)
{
    if(g_pUserList == NULL)
    {
        if((g_pUserList = List_Create(sizeof(user_info_t))) == NULL)
        {
            return false;
        }
        if(RET_FUNCTION_OK != List_Add(g_pUserList, pUserInfo))
        {
            return false;
        }
    }
    return true;
}

static bool cgi_check_current_user(user_info_t *pUserInfo)
{
    int user_cnt, i;
    user_info_t *pUser_tmp;
    user_cnt = List_Count(g_pUserList);
    if(user_cnt == 0)
    {
        return false;
    }
    for(i = 0; i < user_cnt; i++)
    {
        pUser_tmp = List_Find(g_pUserList, i);
        if(pUser_tmp != NULL)
        {
            if(!memcmp(pUser_tmp->name, pUserInfo->name, 32) && !memcmp(pUser_tmp->password, pUserInfo->password, 32))
            {
                if(!memcmp(pUser_tmp->ip, pUserInfo->ip, 32))
                {
                    return true;
                }
                else
                {
                    return cgi_add_new_user(pUserInfo);
                }
            }
        }
    }
    return true;
}


static bool cgi_srv_user_certify(user_info_t *pUserInfo)
{
    if(pUserInfo == NULL)
    {
        return false;
    }
    user_info_t user_info_tmp;
    size_t ret;
    FILE *stream = fopen("cgi_profile","a+");
    if(stream != NULL)
    {
        WEB_SRV_DEBUG("Open profile successfully\n");
        ret = fread(&user_info_tmp, sizeof(user_info_t), 1, stream);
        if(ret > 0)
        {   
            memcpy(&g_user_info, &user_info_tmp, sizeof(user_info_t));
        }
        else if(ret == 0)
        {
            memcpy(&g_user_info, &g_default_user_info, sizeof(user_info_t));
        }
        else
        { 
            memcpy(&g_user_info, &g_default_user_info, sizeof(user_info_t));
        }
        fclose(stream);
    }
    else
    {
        WEB_SRV_DEBUG("Open profile failed!\n");
        perror("[WEB SRV]");
    }
    if(!memcmp(pUserInfo->name, g_user_info.name, 32) && !memcmp(pUserInfo->password, g_user_info.password, 32))
    {
       return true; 
    }
    return false;
}

void cgi_server(void)
{
    for(;;)
    {
        //Get receive message queue
        int rx_msg_id = msgget((key_t)1000, 0666 | IPC_CREAT);
        if(rx_msg_id == -1)
        {
            perror("[WEB SRV]");
            continue;
        }
        //Get send message queue
        int tx_msg_id = msgget((key_t)1001, 0666 | IPC_CREAT);
        if(tx_msg_id == -1 )
        {
            perror("[WEB SRV]");
            continue;
        }
        msg_type_t rx_msg;
        msg_type_t tx_msg;
        if(msgrcv(rx_msg_id, &rx_msg, MSG_DATA_LEN, 0, 0) == -1)
        {
            continue;
        }
        WEB_SRV_DEBUG("Get message type:%ld\n", rx_msg.type);
        switch(rx_msg.type)
        {
            case MSG_TYPE_USER_CERTIFY:
                tx_msg.type = MSG_TYPE_USER_CERTIFY; 
                WEB_SRV_DEBUG("name:%s\n",((user_info_t*)rx_msg.data)->name);
                WEB_SRV_DEBUG("password:%s\n",((user_info_t*)rx_msg.data)->password);
                WEB_SRV_DEBUG("ip:%s\n",((user_info_t*)rx_msg.data)->ip);
                tx_msg.data[0] = (char)cgi_srv_user_certify((user_info_t *)rx_msg.data);
                g_certify_status = tx_msg.data[0];
                WEB_SRV_DEBUG("verify result:%d\n",g_certify_status);
                break;
            case MSG_TYPE_GET_STATUS: 
                tx_msg.type = MSG_TYPE_GET_STATUS;
                tx_msg.data[0] = (char)g_status;
                break;
            case MSG_TYPE_GET_CERTIFIED_STATUS:
                tx_msg.type = MSG_TYPE_USER_CERTIFY;
                tx_msg.data[0] = g_certify_status; 
                break;
            case MSG_TYPE_LOGTOU:
                tx_msg.type = MSG_TYPE_LOGTOU;
                g_certify_status = false;
                tx_msg.data[0] = 1;
                WEB_SRV_DEBUG("logout\n");
                break;
            default:
                break;
        }
        if(msgsnd(tx_msg_id, &tx_msg, MSG_DATA_LEN, 0) == -1)
        {
            perror("[WEB SRV]");
        }
        WEB_SRV_DEBUG("Send event %ld\n",tx_msg.type);
        //msgctl(rx_msg_id, IPC_RMID, 0); 
    }
}


bool cgi_req_user_certified(char name[], char password[], char ip[])
{
    int tx_msg_id, rx_msg_id;
    msg_type_t tx_msg;
    msg_type_t rx_msg;
    uint8_t try_cnt = 3;
    tx_msg_id = msgget((key_t)1000, 0666 | IPC_CREAT);
    if(tx_msg_id == -1)
    {
        return false;
    }
    rx_msg_id = msgget((key_t)1001, 0666 | IPC_CREAT);
    if(rx_msg_id == -1)
    {
        msgctl(tx_msg_id, IPC_RMID, 0); 
        return false;
    }
    memset(&tx_msg, 0, sizeof(msg_type_t));
    tx_msg.type = MSG_TYPE_USER_CERTIFY;
    memcpy(((user_info_t *)tx_msg.data)->name, name, 32);
    memcpy(((user_info_t *)tx_msg.data)->password, password, 32);
    memcpy(((user_info_t *)tx_msg.data)->ip, ip, 32);
    while(msgsnd(tx_msg_id, (const void *)&tx_msg, MSG_DATA_LEN, 0) == -1)
    {
        //msgctl(tx_msg_id, IPC_RMID, 0);
        if(try_cnt-- == 0)
        {
            return false;
        }
    }
    memset(&rx_msg, 0, sizeof(msg_type_t));
    try_cnt = 3; 
    //msgrcv(rx_msg_id, (void *)&rx_msg, MSG_DATA_LEN, 0, 0);
    while(msgrcv(rx_msg_id, (void *)&rx_msg, MSG_DATA_LEN, 0, 0) == -1)
    {
        if(try_cnt-- == 0)
        {
            return false;
        }
    }
    //msgctl(rx_msg_id, IPC_RMID, 0);
    //msgctl(tx_msg_id, IPC_RMID, 0); 
    if(rx_msg.data[0] == 1)
    {
        return true;
    }
    return false;
}

cgi_status_t cgi_get_status(void)
{
    cgi_status_t status;
    msg_type_t tx_msg, rx_msg;
    int tx_msg_id = -1, rx_msg_id = -1;
    tx_msg_id = msgget((key_t)1000, 0666 | IPC_CREAT);
    if(tx_msg_id == -1)
    {
        return CGI_STATUS_ERR;
    }
    memset(&rx_msg, 0, sizeof(msg_type_t));
    rx_msg.type = MSG_TYPE_GET_STATUS;
    if(msgsnd(tx_msg_id, &tx_msg, MSG_DATA_LEN, 0) == -1)
    {
        return CGI_STATUS_ERR;
    }
    rx_msg_id = msgget((key_t)1001, 0666 | IPC_CREAT);
    if(rx_msg_id == -1)
    {
        return CGI_STATUS_ERR;
    }
    if(msgrcv(rx_msg_id, &rx_msg, MSG_DATA_LEN, 0, 0) <= 0)
    {
        return CGI_STATUS_ERR;
    }
    status = (cgi_status_t)rx_msg.data[0];
    if (msgctl(rx_msg_id, IPC_RMID, 0) == -1) 
    {
    }
    return status;
}

bool cgi_is_certified(void)
{
    msg_type_t tx_msg, rx_msg;
    char try_cnt = 3;
    int tx_msg_id, rx_msg_id;
    tx_msg_id = msgget((key_t)1000, 0666 | IPC_CREAT);
    if(tx_msg_id == -1)
    {
        return false;
    }
    memset(&tx_msg, 0, sizeof(msg_type_t));
    tx_msg.type = MSG_TYPE_GET_CERTIFIED_STATUS; 
    while(msgsnd(tx_msg_id, &tx_msg, MSG_DATA_LEN, 0) == -1)
    {
        if(try_cnt-- == 0)
        {
            if (msgctl(tx_msg_id, IPC_RMID, 0) == -1) 
            {
                //Report error?
            } 
            return false;
        }
    }
    
    rx_msg_id = msgget((key_t)1001, 0666 | IPC_CREAT);
    if(rx_msg_id == -1)
    {
        return false;
    }

    try_cnt = 3;
    memset(&rx_msg, 0, sizeof(msg_type_t));
    while(msgrcv(rx_msg_id, &rx_msg, MSG_DATA_LEN, 0, 0) == -1)
    {
        if(try_cnt-- == 0)
        {
            break;
        }
    }
    msgctl(rx_msg_id, IPC_RMID, 0); 
    if(rx_msg.data[0] == 1)
    {
        return true;
    }
    return false;
}

bool cgi_logout(void)
{
    msg_type_t tx_msg, rx_msg;
    char try_cnt = 3;
    int tx_msg_id, rx_msg_id;
    tx_msg_id = msgget((key_t)1000, 0666 | IPC_CREAT);
    if(tx_msg_id == -1)
    {
        return false;
    }
    memset(&tx_msg, 0, sizeof(msg_type_t));
    tx_msg.type = MSG_TYPE_LOGTOU; 
    while(msgsnd(tx_msg_id, &tx_msg, MSG_DATA_LEN, 0) == -1)
    {
        if(try_cnt-- == 0)
        {
            if (msgctl(tx_msg_id, IPC_RMID, 0) == -1) 
            {
                //Report error?
            } 
            return false;
        }
    }
    
    rx_msg_id = msgget((key_t)1001, 0666 | IPC_CREAT);
    if(rx_msg_id == -1)
    {
        return false;
    }

    try_cnt = 3;
    memset(&rx_msg, 0, sizeof(msg_type_t));
    while(msgrcv(rx_msg_id, &rx_msg, MSG_DATA_LEN, 0, 0) == -1)
    {
        if(try_cnt-- == 0)
        {
            break;
        }
    }
    if (msgctl(rx_msg_id, IPC_RMID, 0) == -1) 
    {
        //perror("[WEB SRV]");
    } 
    if(rx_msg.data[0] == 1)
    {
        return true;
    }
    return false;
}
