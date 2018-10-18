
#include <cgic.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "cgiserver.h"

int cgiMain(void) {
    char name[32] = "";
    char password[32] = "";
    cgiHeaderContentType("text/html");
    fprintf(cgiOut, "<HTML><meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"><HEAD>\n");
    fprintf(cgiOut, "<BODY>\n");
    cgiFormStringNoNewlines("username", name, 32);
    if(strlen(name) == 0)
    {
        fprintf(cgiOut, "<script> alert(\"请输入用户名\") </script>");
        fprintf(cgiOut, "<script>window.location.href='/'</script>");
        goto ext;
    }
    cgiFormStringNoNewlines("password", password, 32);
    if(strlen(password) == 0)
    {
        fprintf(cgiOut, "<script> alert(\"请输入密码\") </script>");
        fprintf(cgiOut, "<script>window.history.back(-1)</script>");
        goto ext;
    }
    if(cgi_req_user_certified(name, password))   
    {
        fprintf(cgiOut, "<script>window.location.reload(\"/cgi-bin/index.cgi\")</script>");
        //fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"url=/cgi-bin/index.cgi\">");
    }
    else
    {
        fprintf(cgiOut, "<script> alert(\"用户名或密码错误\") </script>");
        fprintf(cgiOut, "<script>window.history.back(-1)</script>");
    }
ext:
    fprintf(cgiOut, "</BODY></HTML>\n");

    return 0;
}
