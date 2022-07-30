#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "aggregationSuite.h"
#include "../core/aggregationModule.h"

int characterLimit = 80;

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


