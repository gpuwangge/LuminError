#include "logCore.h"
#include <chrono>
//#include <filesystem> //not work with mingw

//to use _mkdir()
#ifdef _WIN32
    #include <direct.h>
#endif

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

//namespace fs = std::filesystem;

std::string LogCore::CreateDateFolder(const std::string& basePath) {
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);

    // 格式化日期字符串
    char buffer[100];
    //std::strftime(buffer, sizeof(buffer), "%Y%m%d", &tm);
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M", &tm);
    std::string folderName = buffer;

    // 构建完整路径
    std::string fullPath = basePath.empty() ? "/" + folderName : basePath + folderName;

    // 尝试创建文件夹
    #ifdef _WIN32
        if (_mkdir(fullPath.c_str()) == 0) {
    #else
        if (mkdir(fullPath.c_str(), 0755) == 0) {
    #endif
            //std::cout << "Folder created successfully: " << fullPath << std::endl;
        } else {
            // 检查错误类型
            if (errno == EEXIST) {
                //std::cout << "Folder already exists: " << fullPath << std::endl;
            } else {
                std::cout << "Folder creation failed (error code: " << errno << "): " << fullPath << std::endl;
            }
        }

    return fullPath;

    // // 获取当前时间
    // auto now = std::chrono::system_clock::now();
    // auto time = std::chrono::system_clock::to_time_t(now);
    // std::tm tm = *std::localtime(&time);
    
    // // 格式化日期字符串
    // char buffer[100];
    // std::strftime(buffer, sizeof(buffer), "%Y%m%d", &tm);
    // std::string folderName = buffer;
    
    // // 构建完整路径
    // std::string fullPath;
    // if (basePath.empty()) {
    //     fullPath = folderName;
    // } else {
    //     //fullPath = basePath + "/" + folderName;
    //     fullPath = basePath + folderName;
    // }
    
    // // 检查文件夹是否存在，不存在则创建
    // if (!fs::exists(fullPath)) {
    //     if (fs::create_directory(fullPath)) {
    //         std::cout << "文件夹创建成功: " << fullPath << std::endl;
    //     } else {
    //         std::cout << "文件夹创建失败: " << fullPath << std::endl;
    //     }
    // } else {
    //     std::cout << "文件夹已存在: " << fullPath << std::endl;
    // }
    
    // return fullPath;
}

}//namespace