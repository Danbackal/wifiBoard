#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "html_page.h"

// IP: I

#define TCP_PORT 4242

static const char http_response[] =
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

static struct tcp_pcb *client_pcb;

static err_t tcp_server_send_data(void *arg, struct tcp_pcb *tpcb) {
    err_t err = tcp_write(tpcb, HTML_PAGE, sizeof(HTML_PAGE)-1, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        printf("Failure to write data\n");
        return err;
    } else {
        printf("Data written\n");
    }
    return ERR_OK;
}

static err_t tcp_server_response(void *arg, struct tcp_pcb *client_pcb, pbuf *client_buf, err_t err) {

    if (err != ERR_OK || client_pcb == NULL || client_buf == NULL) {
        printf("Failure in response\n");
        return ERR_VAL;
    }
    printf("Response Received\n");
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    pbuf_copy_partial(client_buf, buffer, client_buf->tot_len, 0);
    printf("Received Data: %s\n", buffer);
    tcp_recved(client_pcb, client_buf->tot_len);
    pbuf_free(client_buf);
    printf("Accepted Response\n");
    err_t write = tcp_server_send_data(arg, client_pcb);
    if (write != ERR_OK) {
        printf("something still goes wrong \n");
    } else {
        printf("No errors with Data Write\n");
    }
    tcp_close(client_pcb);
    return write;

}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
    if (err != ERR_OK || client_pcb == NULL) {
        printf("Failure in accept\n");
        return ERR_VAL;
    }
    tcp_recv(client_pcb, tcp_server_response);
    printf("Accepted Connection\n");
    return ERR_OK;

}

int main()
{
    stdio_init_all();

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Connecting to WiFi ... \n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Failed to connect... \n");
        return -1;
    } else {
        printf("Connected\n");
    }

    // Example to turn on the Pico W LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
    if (!pcb) {
        printf("failed to create pcb\n");
        return false;
    }

    err_t err = tcp_bind(pcb, NULL, TCP_PORT);
    if (err) {
        printf("Failure binding to port\n");
        return -1;
    }

    pcb = tcp_listen_with_backlog(pcb, 1);
    tcp_arg(pcb, NULL);
    tcp_accept(pcb, tcp_server_accept);


    while (true) {
        cyw43_arch_poll();
        // printf(ip4addr_ntoa(netif_ip4_addr(netif_list)));
        // printf("\n");
    }
    cyw43_arch_deinit();
}
