#include "include.h"
#include "util/io.h"
#include "image/pe.h"
#include "components/server.h"

int main()
{
    server_t s;
    s.port = 6666;
    s.select_timeout = 1;
    s.allow_multiple_connections = false;

    s.ctx.cert = "ssl/server.crt";
    s.ctx.key = "ssl/server.key";

    s.ctx.init();

    s.start();

    s.on_accept.add([&](int socket, std::string_view ip) {
        io::log("{} connected.", ip);

        client_t new_client;
        new_client.socket = socket;
        new_client.ip = ip;
        new_client.valid = new_client.init_ssl(s.ctx.ssl_ctx);

        auto it = std::find(s.clients.begin(), s.clients.end(), new_client);
        if (it != s.clients.end() && !s.allow_multiple_connections)
        {
            io::log_err("{} is already connected.", ip);
            new_client.clean();
            return;
        }

        s.clients.emplace_back(new_client);
    });

    s.on_recv.add([&](int socket, std::string_view msg) {
        auto it = std::find(s.clients.begin(), s.clients.end(), socket);

        message_t buf(msg);
        buf.act = 'd';

        it->write(&buf, sizeof(buf));

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

    std::thread([&]() {
        while (s.active)
        {
        }
    }).detach();

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