//#include "binary_search.h"
//#include "vector_ops.h"
//#include "comma_op_magic.h"
//#include "heterogenous_lookup.h"
//#include "find_example.h"
//#include "fold_expression.h"
//#include "structural_binding.h"
//#include "iterator_behavior.h"
//#include "list_ops.h"
//#include "bitset_ops.h"
//#include "unordered_map_ops.h"
//#include "chrono_ops.h"
//#include "iostream_ops.h"
//#include "regex_ops.h"
//#include "object_lifetime.h"
//#include "utility_ops.h"
//#include "smart_pointer_ops.h"
//#include "address_ops.h"
#include "copy_elision.h"

#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char* argv[]) {
    //std_vector_ops::demo();
    //comma_op_magic::demo();
    //heterogenous_lookup::demo();
    //find_example::demo();
    //fold_expression::demo();
    //structural_binding::demo();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    return 0;
}
