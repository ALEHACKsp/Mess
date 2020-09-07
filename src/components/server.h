#pragma once

#include "../util/io.h"
#include "../util/events.h"
#include "client.h"

struct server_t {
    uint16_t port;
    fd_set fdset;
    int sock;
    bool active;
    bool allow_multiple_connections;
    std::vector<client_t> clients;

    int select_timeout = 5;

    event_t<int, std::string_view> on_accept;
    event_t<int> on_disconnect;
    event_t<int, std::string_view> on_recv;

    bool start();
    int peek();
    void accept_new();
    void read();
    void drop();
};