#include "binary_search.h"
#include "vector_ops.h"
#include "comma_op_magic.h"
#include "heterogenous_lookup.h"
#include "find_example.h"
#include "fold_expression.h"
#include "structural_binding.h"
#include "iterator_behavior.h"
#include "list_ops.h"
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
