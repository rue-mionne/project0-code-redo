#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aggregationSuite.h"
#include "../core/aggregationModule.h"

void testWordFinding(){
	char *test_line={"test "};
	char *test_word={"test"};
	CU_ASSERT_FALSE(strcmp(test_word, findWord(&test_line)));
}
