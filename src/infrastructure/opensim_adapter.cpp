#include "cycleopt/infrastructure/opensim_adapter.hpp"
namespace cycleopt {
bool isOpenSimAvailable() {
#ifdef OPENSIM_FOUND
    return true;
#else
    return false;
#endif
}
}
