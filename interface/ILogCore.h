#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "Utility.h"

namespace LELog{
    class ILogCore {
    public:
        virtual ~ILogCore() = default;
        //void SetApplication(LEApplication::IApplication* pApplication) {game = pApplication;}

        //Print to screen
        virtual void Print(const std::string& message) = 0;
        template<typename... Args> // 可变参数模板 - 处理所有类型
        void Print(const std::string& format, Args... args) {
            std::string formatted = formatString(format, args...);
            Print(formatted);
        }

        //Log to File(Array, glm::vec3)
        virtual void LogArray(const std::string& message, float *n, int size) = 0;
        virtual void LogVec3(const std::string& message, glm::vec3 v) = 0;

        //Log to File
        virtual bool SetLogFile(const std::string& filename) = 0;
        virtual void CloseLogFile() = 0;
        virtual bool IsFileLogging() const = 0;
        
        virtual std::string GetLogFileName(std::string exampleName) = 0;
        virtual std::string CreateDateFolder(const std::string& basePath = "") = 0;

        virtual void Log(const std::string& message) = 0;
        template<typename... Args> // 可变参数模板 - 处理所有类型
        void Log(const std::string& format, Args... args) {
            std::string formatted = formatString(format, args...);
            Log(formatted);
        }

    private:
        // 递归终止条件
        void formatImpl(std::ostringstream& oss, const std::string& format, size_t index) {
            oss << format.substr(index);
        }

        // 递归格式化
        template<typename T, typename... Args>
        void formatImpl(std::ostringstream& oss, const std::string& format, size_t index, T value, Args... args) {
            size_t pos = format.find("{}", index);
            if (pos == std::string::npos) {
                oss << format.substr(index);
                return;
            }
            
            oss << format.substr(index, pos - index);
            oss << value;
            formatImpl(oss, format, pos + 2, args...);
        }
        
        // 基础情况
        inline std::string formatString(const std::string& format) {
            return format;
        }

        // 处理单个格式说明符
        template<typename T>
        void formatArg(std::stringstream& ss, const std::string& specifier, T&& arg) {
            if (specifier.empty()) {
                ss << arg;
                return;
            }
            
            // 保存原始状态
            std::ios_base::fmtflags original_flags = ss.flags();
            char original_fill = ss.fill();
            
            // 十六进制格式处理
            if (specifier.find('x') != std::string::npos || specifier.find('X') != std::string::npos) {
                bool uppercase = (specifier.find('X') != std::string::npos);
                bool alternate_form = (specifier.find('#') != std::string::npos);
                bool zero_pad = (specifier.find('0') != std::string::npos);
                
                // 提取总宽度
                int total_width = 0;
                for (char c : specifier) {
                    if (c >= '0' && c <= '9') {
                        total_width = total_width * 10 + (c - '0');
                    }
                }
                
                ss << std::hex;
                if (uppercase) ss << std::uppercase;
                
                // 先转换数字为十六进制字符串
                std::stringstream num_ss;
                num_ss << std::hex;
                if (uppercase) num_ss << std::uppercase;
                num_ss << arg;
                std::string hex_num = num_ss.str();
                
                // 计算数字部分需要的宽度
                int number_width = total_width;
                if (alternate_form) {
                    number_width -= 2; // 减去 "0x" 前缀
                }
                
                // 输出前缀
                if (alternate_form) {
                    ss << "0x";
                }
                
                // 手动零填充
                if (zero_pad && number_width > 0) {
                    if (hex_num.length() < number_width) {
                        int zeros_needed = number_width - hex_num.length();
                        for (int i = 0; i < zeros_needed; ++i) {
                            ss << '0';
                        }
                    }
                }
                
                // 输出数字
                ss << hex_num;
                
            } 
            // 十进制格式处理
            else if (specifier.find('d') != std::string::npos) {
                bool zero_pad = (specifier.find('0') != std::string::npos);
                
                // 提取宽度
                int width = 0;
                for (char c : specifier) {
                    if (c >= '0' && c <= '9') {
                        width = width * 10 + (c - '0');
                    }
                }
                
                // 十进制：使用标准库的宽度设置
                if (width > 0) {
                    if (zero_pad) {
                        ss << std::setfill('0') << std::setw(width);
                    } else {
                        ss << std::setfill(' ') << std::setw(width);
                    }
                }
                
                ss << arg;
                
            } else {
                // 其他格式
                ss << arg;
            }
            
            // 恢复状态
            ss.flags(original_flags);
            ss.fill(original_fill);
        }

        // 递归格式化函数
        template<typename T, typename... Args>
        std::string formatString(const std::string& format, T&& first, Args&&... rest) {
            std::stringstream result;
            size_t start_pos = 0;
            size_t open_brace = format.find('{', start_pos);
            
            if (open_brace == std::string::npos) {
                // 没有找到 {，直接返回原字符串
                result << format;
                if (sizeof...(Args) > 0) {
                    // 如果还有剩余参数但没地方放，可以抛出异常或忽略
                }
                return result.str();
            }
            
            size_t close_brace = format.find('}', open_brace);
            if (close_brace == std::string::npos) {
                // 没有对应的 }，直接返回
                result << format;
                return result.str();
            }
            
            // 输出 { 之前的内容
            result << format.substr(start_pos, open_brace - start_pos);
            
            // 提取格式说明符
            std::string specifier = format.substr(open_brace + 1, close_brace - open_brace - 1);
            
            // 格式化当前参数
            formatArg(result, specifier, std::forward<T>(first));
            
            // 处理剩余部分
            std::string remaining = format.substr(close_brace + 1);
            result << formatString(remaining, std::forward<Args>(rest)...);
            
            return result.str();
        }
    }; //class

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
} //namespace
