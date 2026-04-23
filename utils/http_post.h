#ifndef HTTP_POST_H
#define HTTP_POST_H

int HttpPostInit(int enabled, const char *url, int authEnabled, const char *user, const char *password, int queueMax, int queueTtlSeconds);
void HttpPostShutdown();
int HttpPostQueueMessage(int bMatch, int bMonitorOnly,
    const char *szCapcode, const char *szTime, const char *szDate, const char *szMode,
    const char *szType, const char *szBitrate, const char *szMessage, const char *szLabel);

#endif /* ! HTTP_POST_H */
