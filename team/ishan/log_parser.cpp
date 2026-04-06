#include "../include/log_parser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <sstream>
#include <cstring>
#include <vector>

std::string escapeJson(const std::string& input) {
    std::string output;
    output.reserve(input.size() + 16);
    for (char c : input) {
        switch (c) {
            case '"':  output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default:   output += c; break;
        }
    }
    return output;
}

// Attempt to extract syslog-style timestamp from beginning of line
// Format: "Mon DD HH:MM:SS" e.g. "Apr  5 14:22:01"
std::string extractTimestamp(const std::string& line) {
    // Match: 3-letter month, spaces, day, time
    static std::regex tsRegex("^([A-Za-z]{3})\\s+(\\d{1,2})\\s+(\\d{2}:\\d{2}:\\d{2})");
    std::smatch m;
    if (std::regex_search(line, m, tsRegex)) {
        return m[1].str() + " " + m[2].str() + " " + m[3].str();
    }
    return "";
}

// Extract hostname/process from syslog
std::string extractProcess(const std::string& line) {
    // After timestamp + hostname, process is like "sshd[1234]:" or "sudo:"
    static std::regex procRegex("[A-Za-z]{3}\\s+\\d{1,2}\\s+\\d{2}:\\d{2}:\\d{2}\\s+\\S+\\s+(\\S+?)(?:\\[\\d+\\])?:");
    std::smatch m;
    if (std::regex_search(line, m, procRegex)) {
        return m[1].str();
    }
    return "";
}

extern "C" {

const char* parse_logs(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::string err = "{\"error\": \"Could not open file\"}";
        char* cstr = new char[err.length() + 1];
        strcpy(cstr, err.c_str());
        return cstr;
    }

    std::string line;
    
    // Auth patterns
    std::regex failedPassRegex("Failed password for (invalid user )?([^ ]+) from ([^ ]+) port (\\d+)");
    std::regex acceptedPassRegex("Accepted (?:password|publickey) for ([^ ]+) from ([^ ]+) port (\\d+)");
    std::regex invalidUserRegex("Invalid user ([^ ]+) from ([^ ]+)");
    std::regex rootSessionRegex("session opened for user (root|admin)");
    std::regex sudoRegex("sudo:\\s+(\\S+)\\s+:.*COMMAND=(.*)");
    std::regex suRegex("su\\[.*\\]:.*(session opened|Successful su) for user (\\S+)");
    
    // Network patterns
    std::regex iptablesRegex("(DROP|REJECT|ACCEPT).*SRC=([^ ]+).*DST=([^ ]+).*DPT=(\\d+)");
    
    // System patterns
    std::regex segfaultRegex("segfault at");
    std::regex oomRegex("Out of memory|oom-killer");
    std::regex sqlInjRegex("union select|select\\s+\\*\\s+from|drop\\s+table|1=1|or 1=1", std::regex_constants::icase);
    
    std::smatch match;
    std::ostringstream json;
    json << "[";
    bool first = true;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;
        std::string severity, action, user = "", ip_address = "", process_name = "";
        std::string timestamp = extractTimestamp(line);
        std::string detected_process = extractProcess(line);

        bool matched = false;

        // SSH Failed Login
        if (std::regex_search(line, match, failedPassRegex)) {
            severity = "high";
            action = "ssh_failed_login";
            user = match[2].str();
            ip_address = match[3].str();
            process_name = "sshd";
            matched = true;
        }
        // SSH Accepted Login
        else if (std::regex_search(line, match, acceptedPassRegex)) {
            severity = "medium";
            action = "ssh_accepted_login";
            user = match[1].str();
            ip_address = match[2].str();
            process_name = "sshd";
            matched = true;
        }
        // Invalid User
        else if (std::regex_search(line, match, invalidUserRegex)) {
            severity = "high";
            action = "ssh_invalid_user";
            user = match[1].str();
            ip_address = match[2].str();
            process_name = "sshd";
            matched = true;
        }
        // Root/Admin Session
        else if (std::regex_search(line, match, rootSessionRegex)) {
            severity = "critical";
            action = "root_session";
            user = match[1].str();
            ip_address = "localhost";
            process_name = detected_process;
            matched = true;
        }
        // Sudo Command
        else if (std::regex_search(line, match, sudoRegex)) {
            severity = "high";
            action = "sudo_command";
            user = match[1].str();
            process_name = match[2].str();
            matched = true;
        }
        // SU attempt
        else if (std::regex_search(line, match, suRegex)) {
            severity = "high";
            action = "su_attempt";
            user = match[2].str();
            process_name = "su";
            matched = true;
        }
        // Firewall / iptables
        else if (std::regex_search(line, match, iptablesRegex)) {
            std::string fw_action = match[1].str();
            ip_address = match[2].str();
            std::string dst = match[3].str();
            process_name = "port_" + match[4].str();
            
            if (fw_action == "DROP" || fw_action == "REJECT") {
                severity = "medium";
                action = "network_drop";
            } else {
                severity = "low";
                action = "network_accept";
            }
            matched = true;
        }
        // SQL Injection in logs
        else if (std::regex_search(line, sqlInjRegex)) {
            severity = "critical";
            action = "sql_injection";
            process_name = "webapp";
            matched = true;
        }
        // Segfault
        else if (std::regex_search(line, segfaultRegex)) {
            severity = "medium";
            action = "segfault";
            process_name = detected_process;
            matched = true;
        }
        // OOM Killer
        else if (std::regex_search(line, oomRegex)) {
            severity = "high";
            action = "oom_kill";
            process_name = detected_process;
            matched = true;
        }
        // Generic error catch
        else if (line.find("error") != std::string::npos || line.find("ERROR") != std::string::npos) {
            severity = "low";
            action = "system_error";
            process_name = detected_process;
            matched = true;
        }

        if (matched) {
            if (!first) json << ",";
            first = false;
            
            json << "{"
                 << "\"line\":" << lineNumber << ","
                 << "\"timestamp\":\"" << escapeJson(timestamp) << "\","
                 << "\"severity\":\"" << severity << "\","
                 << "\"action\":\"" << action << "\","
                 << "\"user\":\"" << escapeJson(user) << "\","
                 << "\"ip_address\":\"" << escapeJson(ip_address) << "\","
                 << "\"process\":\"" << escapeJson(process_name) << "\","
                 << "\"source_type\":\"syslog\","
                 << "\"raw_log\":\"" << escapeJson(line) << "\""
                 << "}";
        }
    }

    json << "]";
    file.close();

    std::string result = json.str();
    char* cstr = new char[result.length() + 1];
    strcpy(cstr, result.c_str());
    return cstr;
}

void free_memory(const char* ptr) {
    if (ptr != nullptr) {
        delete[] ptr;
    }
}

} // extern "C"
