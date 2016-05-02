#pragma once    

#include <ground/base/base.h>


namespace NGround {

    class TSerialBase {
    public:
        enum class ESerialMode {
            IN = 0,
            OUT = 1
        };

        TSerialBase(ESerialMode mode);

        bool IsInput() const;

        bool IsOutput() const;

    protected:
        ESerialMode Mode;
    };

} // namespace NGround