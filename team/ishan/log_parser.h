#ifndef LOG_PARSER_H
#define LOG_PARSER_H

extern "C" {
    // Parse the log file and return JSON string
    const char* parse_logs(const char* filepath);
    
    // Free memory automatically allocated for the JSON string
    void free_memory(const char* ptr);
}

#endif // LOG_PARSER_H
