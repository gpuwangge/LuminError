#include "logCore.h"

namespace LELog{

void LogCore::Print(const std::string& message) {
    std::cout<<message<<std::endl;
}

void LogCore::Log(const std::string& message) {
    if(fileEnabled_) file_<<message<<std::endl;
}

bool LogCore::SetLogFile(const std::string& filename) {
    //std::lock_guard<std::mutex> lock(mutex_);
    
    CloseLogFile(); // 先关闭之前的文件
    
    file_.open(filename, std::ios::out | std::ios::app);
    if (!file_.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
        return false;
    }
    
    filename_ = filename;
    fileEnabled_ = true;
    
    // 写入文件头
    file_ << "=== Log started ===" << std::endl;
    file_.flush();
    
    return true;
}

void LogCore::CloseLogFile() {
    if (file_.is_open()) {
        file_ << "=== Log ended ===" << std::endl;
        file_.close();
    }
    fileEnabled_ = false;
}

bool LogCore::IsFileLogging() const {
    return fileEnabled_;
}

std::string LogCore::GetLogFileName(std::string exampleName) {
    std::string appName = exampleName;
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);
    
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &tm);
    
    return appName + "_" + std::string(buffer) + ".log";
}

}//namespace