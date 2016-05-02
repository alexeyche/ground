#include "log.h"

namespace NGround {


    TLog& TLog::Instance() {
        static TLog _inst;
        return _inst;
    }


}
