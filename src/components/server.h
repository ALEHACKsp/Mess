#pragma once

#include <openssl/ssl.h>

#include "../util/io.h"
#include "../util/events.h"
#include "client.h"

struct server_context_t
{
    SSL_CTX *ssl_ctx;
    std::string cert;
    std::string key;

    void init()
    {
        SSL_library_init();

        ssl_ctx = SSL_CTX_new(TLS_server_method());
        if (!ssl_ctx)
        {
            io::log_err("failed to create ssl context.");
            return;
        }

        //SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, 0);
        int res = SSL_CTX_use_certificate_chain_file(ssl_ctx, cert.data());
        if (res != 1)
        {
            io::log_err("failed to load certificate.");
            return;
        }

        res = SSL_CTX_use_PrivateKey_file(ssl_ctx, key.data(), SSL_FILETYPE_PEM);
        if (res != 1)
        {
            io::log_err("failed to load private key.");
            return;
        }
    }
};

struct server_t
{
    uint16_t port;
    fd_set fdset;
    int sock;
    bool active;
    bool allow_multiple_connections;
    std::vector<client_t> clients;

    server_context_t ctx;

    int select_timeout = 5;

    event_t<int, std::string_view> on_accept;
    event_t<int> on_disconnect;
    event_t<int, std::string_view> on_recv;

    bool start();
    int peek();
    void accept_new();
    void read();
    void drop();

    void shutdown()
    {
        close(sock);
    }
};