#include <CUnit/CUError.h>
#include <CUnit/TestDB.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Console.h>
#include <CUnit/CUnit.h>
#include "aggregationSuite.h"

int main(){
	CU_pSuite pSuiteAggregation = NULL;

	if(CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	pSuiteAggregation = CU_add_suite("Suite", NULL, NULL);

	if(pSuiteAggregation == NULL)
		return CU_get_error();
	
	CU_ADD_TEST(pSuiteAggregation, testWordFinding);
	CU_ADD_TEST(pSuiteAggregation, testWordFindingIfTabulator);
	CU_ADD_TEST(pSuiteAggregation, testWordFindingDoubleSpaceCase);

	CU_console_run_tests();

	CU_cleanup_registry();
}
