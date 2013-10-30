#ifndef LOG_H_
#define LOG_H_

extern void wxLogMessage(const char* msg, ...);
extern void wxLogError(const char* msg, ...);

#define LogMessage wxLogMessage
#define LogError   wxLogError

#endif