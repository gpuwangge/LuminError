#pragma once
#include "ILogCore.h"
//#include <vulkan/vulkan.h>
#include <fstream>
//#include <mutex>
//#include <memory>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace LELog{
    class LogCore final : public ILogCore{
    public:
        LogCore(){}
        ~LogCore(){}
        void Print(const std::string& message) override;
        void Log(const std::string& message) override;

        bool SetLogFile(const std::string& filename) override;
        void CloseLogFile() override;
        bool IsFileLogging() const override;
        std::string GetLogFileName(std::string exampleName) override;

    private:
        std::ofstream file_;
        std::string filename_;
        bool fileEnabled_;
        //std::mutex mutex_;
    };

    EXPORT_FACTORY_FOR(LogCore);
}//namespace