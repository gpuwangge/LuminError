#pragma once

#include "IYAMLCore.h"

//#include <memory>
//#include <vector>
#include <iostream>

//class CApplication;


namespace LEYAML{
    class YAMLCore final : public IYAMLCore{
    public:
        YAMLCore(){}
        ~YAMLCore(){}

        void Greet() override {
            std::cout << "Hello from YAMLCore!" << std::endl;
        }

    };
    EXPORT_FACTORY_FOR(YAMLCore);
}

