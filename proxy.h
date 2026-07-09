#ifndef PROXY_H
#define PROXY_H

#include "types.h"

void load_proxies(const char *filename);
void apply_proxy_pool_filter(void);

#endif
