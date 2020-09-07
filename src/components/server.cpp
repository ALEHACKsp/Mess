#include "../include.h"
#include "server.h"

bool server_t::start()
{
    if (!port)
    {
        io::log_err("port isnt set.");
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
    {
        io::log_err("failed to create socket.");
        return false;
    }

    int enable = 1;
    int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    if (ret < 0)
    {
        io::log_err("failed to set socket option.");
        return false;
    }

    struct addrinfo hints, *addrinfo = nullptr;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    ret = getaddrinfo(nullptr, std::to_string(port).data(), &hints, &addrinfo);
    if (ret != 0)
    {
        io::log_err("failed to get address info.");
        close(sock);
        return false;
    }

    ret = bind(sock, addrinfo->ai_addr, addrinfo->ai_addrlen);
    if (ret < 0)
    {
        io::log_err("failed to bind port {}.", port);
        close(sock);
        return false;
    }
    io::log("port bound.");
    freeaddrinfo(addrinfo);

    ret = listen(sock, SOMAXCONN);
    if (ret < 0)
    {
        io::log_err("failed to listen on port {}.", port);
        close(sock);
        return false;
    }

    io::log("listening on {}.", port);

    active = true;

    return true;
}

void server_t::accept_new()
{
    if (!FD_ISSET(sock, &fdset))
        return;

    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    const int new_sock =
        accept(sock, reinterpret_cast<sockaddr *>(&addr), &len);

    const auto ip = inet_ntoa(addr.sin_addr);
    if (new_sock < 0)
    {
        io::log("{} failed to accept.", ip);
        close(new_sock);
        return;
    }

    on_accept.call(new_sock, ip);
}

int server_t::peek()
{
    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);

    int maxfd = sock;
    for (auto &c : clients)
    {
        FD_SET(c.socket, &fdset);

        maxfd = std::max(maxfd, c.socket);
    }

    timeval tv;
    tv.tv_sec = select_timeout;
    tv.tv_usec = 0;

    return select(maxfd + 1, &fdset, nullptr, nullptr, &tv);
}

void server_t::read()
{
    for (auto &c : clients)
    {
        std::array<char, 256> buf;
        if (!FD_ISSET(c.socket, &fdset))
            continue;

        int size = c.read(buf.data(), buf.size());
        if (size > 0)
        {
            on_recv.call(c.socket, std::string_view(buf.data(), size));
            continue;
        }

        c.valid = false;
    }
}

void server_t::drop()
{
    for (auto &c : clients)
    {
        if (!c.valid)
        {
            on_disconnect.call(c.socket);
        }
    }
}