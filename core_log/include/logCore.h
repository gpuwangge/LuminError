#pragma once
#include "ILogCore.h"
#include <fstream>
#include "Utility.h"

namespace LELog{
    class LogCore final : public ILogCore{
    public:
        LogCore(){}
        ~LogCore(){}
        void Print(const std::string& message) override;
        void LogArray(const std::string& message, float *n, int size) override;
        void LogVec3(const std::string& message, glm::vec3 v) override;
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