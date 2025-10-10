#pragma once


namespace LEApplication{
    class IApplication;
}

namespace LESDL{
    struct ISDLCore {
        virtual ~ISDLCore() = default;
        virtual void greet() = 0; 
        virtual void SetApplication(LEApplication::IApplication* pApplication) = 0;
        // virtual void playWith(animal::IPet* pet) = 0;

        // virtual void setPet(animal::IPet *pet) = 0;
        // virtual void playWithPet() = 0;
    };
}
