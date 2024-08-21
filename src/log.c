#include "m.h"
#include "i.h"


static m_LogLevel current_log_level = M_LOG_INFO;

void m_set_loglevel(m_LogLevel level) {
    current_log_level = level;
}

void m_log_raw(m_LogLevel level, const char* format, ...) {
    if (level < current_log_level) {
        return;
    }

    va_list args;
    va_start(args, format);

    switch (level) {
        case M_LOG_TRACE:
            fprintf(stdout, "[TRACE] ");
            break;
        case M_LOG_INFO:
            fprintf(stdout, "[INFO] ");
            break;
        case M_LOG_WARN:
            fprintf(stdout, "[WARN] ");
            break;
        case M_LOG_ERROR:
            fprintf(stdout, "[ERROR] ");
            break;
        case M_LOG_FATAL:
            fprintf(stdout, "[FATAL] ");
            break;
    }

    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    va_end(args);
}