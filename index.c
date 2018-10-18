#include <cgic.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "cgiserver.h"

int cgiMain(void) {
    /* Send the content type, letting the browser know this is HTML */
    //CookieSet();
    cgiHeaderContentType("text/html");
    /* Top of the page */
    fprintf(cgiOut, "<HTML><meta http-equiv=\"content-type\" content=\"text/html\"; charset=\"UTF-8\"><HEAD>\n");
    fprintf(cgiOut, "\
            <script>\
            function jump_to_price_modify()\
            {\
                window.location.realod(\"/cgi-bin/price_modify.cgi\");\
            }\
            function jump_to_map_modify()\
            {\
                window.location.realod(\"/cgi-bin/map_modify.cgi\");\
            }\
            function jump_to_param_modify()\
            {\
                window.location.realod(\"/cgi-bin/param_modify.cgi\");\
            }\
            funciotn logout()\
            {\
                window.location.realod(\"/cgi-bin/logout.cgi\")\
            }\
            </script>"
           );
    fprintf(cgiOut, "<BODY onunload=\"logout()\">\n");

    if(cgi_is_certified())
    {
        fprintf(cgiOut, "\
                <button type=\"button\"  onclick=\"jump_to_price_modify()\" name=\"modify_price\">修改价签</button>\
                <button type=\"button\"  onclick=\"jump_to_map_modify()\"  name=\"modify_partial_map\">局部图下发</button>\
                <button type=\"button\"  onclick=\"jump_to_param_modify()\" name=\"modify_param\">参数修改</button>\
                ");
    }
    else
    {
        fprintf(cgiOut, "Authentication failed\n");
    }
    fprintf(cgiOut, "</BODY></HTML>\n");
    return 0;
} 

