#include "builderUseCase.h"
#include <iostream>

namespace builder {


void demo() {
    SelectBuilder s;
    s.select("id as user_id", "age", "name", "address")
     .distinct()
     .from("user")
     .where(column("age") >= 20 && column("address").is_not_null())
     .order_by("age desc");
     std::cout << s.str() << '\n';
}
} // namespace builder