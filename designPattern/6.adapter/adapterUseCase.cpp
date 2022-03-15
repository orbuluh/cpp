#include "adapterUseCase.h"
#include <iostream>

namespace adapter {
void demo() {
    BackwardCompatibleAdapter<LegacyCriticalApp> legacy;
    legacy.doX();
    BackwardCompatibleAdapter<RefactoredCriticalApp> refactored;
    refactored.doX();
}
} // namespace adapter