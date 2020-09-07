#pragma once

struct client_context_t
{
    int state;
};

struct ssl_context_t
{
};

struct client_t
{
    int socket;
    client_context_t ctx;
    std::string ip;
    bool valid;

    int write(void *data, size_t size)
    {
        return send(socket, data, size, 0);
    }

    int read(void *data, size_t size)
    {
        return recv(socket, data, size, 0);
    }

    bool operator==(const int sock)
    {
        return socket == sock;
    }

    bool operator==(const std::string_view ip_)
    {
        return ip == ip_;
    }
};