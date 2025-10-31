#pragma once
#include <string>
#include <sstream>
#include <iostream>
// #include <vector>
// #include <memory>

// namespace LEApplication{
//     class IApplication;
// }

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
        
        //Log to File
        virtual bool SetLogFile(const std::string& filename) = 0;
        virtual void CloseLogFile() = 0;
        virtual bool IsFileLogging() const = 0;
        virtual std::string GetLogFileName(std::string exampleName) = 0;
        virtual void Log(const std::string& message) = 0;
        template<typename... Args> // 可变参数模板 - 处理所有类型
        void Log(const std::string& format, Args... args) {
            std::string formatted = formatString(format, args...);
            Log(formatted);
        }
        

    protected:
        //LEApplication::IApplication* game;

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
        
        // 主格式化函数
        template<typename... Args>
        std::string formatString(const std::string& format, Args... args) {
            std::ostringstream oss;
            formatImpl(oss, format, 0, args...);
            return oss.str();
        }
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
