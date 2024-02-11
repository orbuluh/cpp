#include "some_lib/some_lib.h"

#include "util/utility.h"

namespace some_lib {

int32_t call_util() { return util::get_value(); }

}  // namespace some_lib