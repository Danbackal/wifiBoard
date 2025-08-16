#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"

// IP: 192.168.68.105

#define TCP_PORT 4242

#define RESPONSE "HTTP/1.1 200 OK\r\n\r\n"

static struct tcp_pcb *client_pcb;

static err_t tcp_server_send_data(void *arg, struct tcp_pcb *tpcb) {
    err_t err = tcp_write(tpcb, RESPONSE, strlen(RESPONSE), TCP_WRITE_FLAG_COPY);
    return ERR_OK;
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
    if (err != ERR_OK || client_pcb == NULL) {
        printf("Failure in accept\n");
        return ERR_VAL;
    }
    printf("Hello World");
    return tcp_server_send_data(arg, client_pcb);

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

    printf("Connecting to WiFi ... /n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Failed to connect... /n");
        return -1;
    } else {
        printf("Connected");
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
        printf("Failure binding to port \n");
        return -1;
    }

    pcb = tcp_listen_with_backlog(pcb, 1);
    tcp_arg(pcb, NULL);
    tcp_accept(pcb, tcp_server_accept);


    while (true) {
        cyw43_arch_poll();
    }
    cyw43_arch_deinit();
}
