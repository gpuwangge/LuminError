#pragma once
#include "ILogCore.h"
#include <fstream>
//#include <vulkan/vulkan.h>
//#include <mutex>
//#include <memory>
//#include <ctime>
//#include <iomanip>

namespace LELog{
    class LogCore final : public ILogCore{
    public:
        LogCore(){}
        ~LogCore(){}
        void Print(const std::string& message) override;
        void LogArray(std::string s, float *n, int size) override;
        void Log(const std::string& message) override;

        bool SetLogFile(const std::string& filename) override;
        void CloseLogFile() override;
        bool IsFileLogging() const override;

        std::string GetLogFileName(std::string exampleName) override;
        std::string CreateDateFolder(const std::string& basePath = "") override;

    private:
        std::ofstream file_;
        std::string filename_;
        bool fileEnabled_;
        //std::mutex mutex_;
    };

    EXPORT_FACTORY_FOR(LogCore);
}//namespace