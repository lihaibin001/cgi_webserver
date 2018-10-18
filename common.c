#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>

void cgi_cookies(void)
{
    char **array, **arrayStep;
    char cname[1024], cvalue[1024];
    if (cgiCookies(&array) != cgiFormSuccess) {
        return;
    }
    arrayStep = array;
    fprintf(cgiOut, "<table border=1>\n");
    fprintf(cgiOut, "<tr><th>Cookie<th>Value</tr>\n");
    while (*arrayStep) {
        char value[1024];
        fprintf(cgiOut, "<tr>");
        fprintf(cgiOut, "<td>");
        cgiHtmlEscape(*arrayStep);
        fprintf(cgiOut, "<td>");
        cgiCookieString(*arrayStep, value, sizeof(value));
        cgiHtmlEscape(value);
        fprintf(cgiOut, "\n");
        arrayStep++;
    }
    fprintf(cgiOut, "</table>\n");
    cgiFormString("cname", cname, sizeof(cname));
    cgiFormString("cvalue", cvalue, sizeof(cvalue));
    if (strlen(cname)) {
        fprintf(cgiOut, "New Cookie Set On This Call:<p>\n");
        fprintf(cgiOut, "Name: ");
        cgiHtmlEscape(cname);
        fprintf(cgiOut, "Value: ");
        cgiHtmlEscape(cvalue);
        fprintf(cgiOut, "<p>\n");
        fprintf(cgiOut, "If your browser accepts cookies (many do not), this new cookie should appear in the above list the next time the form is            submitted.<p>\n");
    }
    cgiStringArrayFree(array);
}

