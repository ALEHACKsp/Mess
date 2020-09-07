#include "include.h"
#include "util/io.h"
#include "image/pe.h"
#include "components/server.h"

int main()
{
    server_t s;
    s.port = 8888;
    s.select_timeout = 1;
    s.allow_multiple_connections = false;

    s.start();

    s.on_accept.add([&](int socket, std::string_view ip) {
        auto it = std::find(s.clients.begin(), s.clients.end(), ip);
        if (it != s.clients.end() && !s.allow_multiple_connections)
        {
            io::log("{} is already connected.", ip);
            close(socket);
            return;
        }

        client_t new_client;
        new_client.socket = socket;
        new_client.ip = ip;
        new_client.valid = true;

        s.clients.emplace_back(new_client);

        io::log("{} connected.", new_client.ip);
    });

    s.on_recv.add([&](int socket, std::string_view msg) {
        auto it = std::find(s.clients.begin(), s.clients.end(), socket);

        io::log("[{}] {}", it->ip, msg);
    });

    s.on_disconnect.add([&](int socket) {
        auto it = std::find(s.clients.begin(), s.clients.end(), socket);
        if (it != s.clients.end())
        {
            it->clean();

            s.clients.erase(it);
            io::log("{} disconnected.", it->ip);
        }
    });

    while (s.active)
    {
        int ret = s.peek();
        if (ret < 0)
        {
            io::log_err("select returned {}.", ret);
            break;
        }

        if (ret == 0)
        {
            continue;
        }

        s.accept_new();
        s.read();
        s.drop();
    }
}