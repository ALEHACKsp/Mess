#include "include.h"

#include <mbedtls/include/mbedtls/net.h>
#include <mbedtls/include/mbedtls/ssl.h>
#include <mbedtls/include/mbedtls/entropy.h>
#include <mbedtls/include/mbedtls/ctr_drbg.h>
#include <mbedtls/include/mbedtls/debug.h>

int main()
{
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, 0, 0);

    int ret = mbedtls_net_connect(&server_fd, "127.0.0.1",
                                  "6666", MBEDTLS_NET_PROTO_TCP);
    if (ret != 0)
    {
        printf(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
        return 0;
    }

    if (mbedtls_ssl_config_defaults(&conf,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT) != 0)
    {
        return 0;
    }

    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);

    if (mbedtls_ssl_setup(&ssl, &conf) != 0)
    {
        return 0;
    }

    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    if (mbedtls_ssl_handshake(&ssl) != 0)
    {
        return 0;
    }

    while (1)
    {
        std::string s;
        std::cin >> s;

        int ret = mbedtls_ssl_write(&ssl, (unsigned char *)s.data(), s.size());
        printf("write returned %d.", ret);
    }
}