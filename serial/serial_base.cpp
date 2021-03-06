#include "serial_base.h"


namespace NGround {

    TSerialBase::TSerialBase(ESerialMode mode)
        : Mode(mode)
    {
    }

    bool TSerialBase::IsInput() const {
        return Mode == ESerialMode::IN;
    }

    bool TSerialBase::IsOutput() const {
        return Mode == ESerialMode::OUT;
    }


} // namespace NGround