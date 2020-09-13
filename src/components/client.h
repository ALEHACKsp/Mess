#pragma once

struct client_context_t
{
    int state;
};

struct message_t
{
    message_t()
    {
        memset(msg, 0, sizeof(msg));
    }
    message_t(std::string_view buf)
    {
        memset(msg, 0, sizeof(msg));
        size = buf.size();
        memcpy(msg, buf.data(), size);
    }
    uint32_t size;
    char msg[256];
    char act;
};

struct client_t
{
    int socket;
    client_context_t ctx;
    std::string ip;
    bool valid;

    SSL *ssl;

    bool init_ssl(SSL_CTX *ctx)
    {
        ssl = SSL_new(ctx);
        if (!ssl)
        {
            io::log_err("failed to create ssl on {}.", ip);
            return false;
        }

        int ret = SSL_set_fd(ssl, socket);
        if (ret <= 0)
        {
            io::log_err("failed to set descriptor on {}.", ip);
            return false;
        }

        ret = SSL_accept(ssl);

        if (ret <= 0)
        {
            int err = SSL_get_error(ssl, ret);
            io::log_err("{} failed to accept ssl, return code {}.", ip, err);
            return false;
        }

        return true;
    }

    int write(const void *data, size_t size)
    {
        return SSL_write(ssl, data, size);
    }

    int read(void *data, size_t size)
    {
        return SSL_read(ssl, data, size);
    }

    void clean()
    {
        close(socket);
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }

    bool operator==(const client_t cli)
    {
        return ip == cli.ip;
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