#ifndef HTML_PAGE_H
#define HTML_PAGE_H

static const char HTML_PAGE[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"
    "\r\n"
    "<!DOCTYPE html>"
    "<html>"
    "<body>"
    "<form action=\"/submit\" method=\"POST\">"
    "<input type=\"text\" name=\"message\">"
    "<input type=\"submit\" value=\"Send\">"
    "</form>"
    "</body>"
    "</html>";

#endif // HTML_PAGE_H