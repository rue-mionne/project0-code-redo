#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "aggregationSuite.h"
#include "../core/aggregationModule.h"


void testWordFinding(){
	wchar_t *test_line={L"test "};
	wchar_t *test_word={L"test"};
	CU_ASSERT_FALSE(wcscmp(test_word,findWord(&test_line)));
}

void testWordFindingIfTabulator(){
	wchar_t *test_line={L"test\t"};
	wchar_t *test_word={L"test"};
	CU_ASSERT_FALSE(wcscmp(test_word, findWord(&test_line)));
}

void testWordFindingDoubleSpaceCase(){
	wchar_t *test_line={L"\t\ttest"};
	wchar_t *test_word={L"test"};
	CU_ASSERT_FALSE(wcscmp(test_word, findWord(&test_line)));
}

void testWordFindingUnicode(){
	wchar_t *test_line={L"\tтест"};
	wchar_t *test_word={L"тест"};
	CU_ASSERT_FALSE(wcscmp(test_word, findWord(&test_line)));
}

void testLoadingToAggregationSingleWord(){
	wchar_t *test_line={L"testing"};
	wchar_t *test_aggregation[1]={L"testing"};
	CU_ASSERT_FALSE(wcscmp(*test_aggregation, *formAggregation(test_line)));
}

void testLoadingToAggregationMultipleWords(){
	wchar_t *test_line={L"testing line"};
	wchar_t *test_aggregation[2]={L"testing", L"line"};
	wchar_t **result_aggregation=formAggregation(test_line);
	for(int i=0; i<2;i++){
		CU_ASSERT_FALSE(wcscmp(test_aggregation[i], result_aggregation[i]));
	}
}
