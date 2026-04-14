#ifndef HTTP_POST_H
#define HTTP_POST_H

int HttpPostInit(int enabled, const char *url, int authEnabled, const char *user, const char *password, int queueMax, int queueTtlSeconds);
void HttpPostShutdown();
int HttpPostQueueMessage(int bMatch, int bMonitorOnly,
    const char *sz1, const char *sz2, const char *sz3, const char *sz4,
    const char *sz5, const char *sz6, const char *sz7, const char *szLabel);

#endif /* ! HTTP_POST_H */
