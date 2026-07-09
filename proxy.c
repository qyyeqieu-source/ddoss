#include "proxy.h"
#include "logger.h"
#include <strings.h>

static int is_us_country(const char *country) {
    if (!country || !*country) return 0;
    char tmp[32];
    size_t n = strlen(country);
    if (n >= sizeof(tmp)) n = sizeof(tmp) - 1;
    for (size_t i = 0; i < n; ++i) tmp[i] = tolower((unsigned char)country[i]);
    tmp[n] = '\0';
    return strstr(tmp, "us") != NULL || strstr(tmp, "usa") != NULL ||
           strstr(tmp, "america") != NULL || strstr(tmp, "united states") != NULL;
}

void load_proxies(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        LOG_WARN("Could not open proxies file: %s", filename);
        return;
    }
    proxies = malloc(sizeof(Proxy) * MAX_PROXIES);
    if (!proxies) {
        LOG_ERR("Failed to allocate memory for proxies!");
        fclose(fp);
        return;
    }
    
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        if (proxy_count >= MAX_PROXIES) break;
        char *h = strtok(line, ":\r\n");
        char *p = strtok(NULL, ":\r\n");
        char *u = strtok(NULL, ":\r\n");
        char *pw = strtok(NULL, ":\r\n");
        char *country = strtok(NULL, ":\r\n");
        if (!h || !p) continue;
        
        memset(&proxies[proxy_count], 0, sizeof(Proxy));
        strncpy(proxies[proxy_count].host, h, sizeof(proxies[proxy_count].host) - 1);
        proxies[proxy_count].host[sizeof(proxies[proxy_count].host) - 1] = '\0';
        
        proxies[proxy_count].port = atoi(p);
        
        if (u && pw) {
            strncpy(proxies[proxy_count].user, u, sizeof(proxies[proxy_count].user) - 1);
            proxies[proxy_count].user[sizeof(proxies[proxy_count].user) - 1] = '\0';
            
            strncpy(proxies[proxy_count].pass, pw, sizeof(proxies[proxy_count].pass) - 1);
            proxies[proxy_count].pass[sizeof(proxies[proxy_count].pass) - 1] = '\0';
            
            proxies[proxy_count].has_auth = 1;
        }
        if (country) {
            strncpy(proxies[proxy_count].country, country, sizeof(proxies[proxy_count].country) - 1);
            proxies[proxy_count].country[sizeof(proxies[proxy_count].country) - 1] = '\0';
            proxies[proxy_count].is_us = is_us_country(proxies[proxy_count].country);
        }
        proxy_count++;
    }
    fclose(fp);
    LOG_INFO("Tornado Engine: Loaded %d strong proxies", proxy_count);
}
