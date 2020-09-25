/*
 * Filename: bcamp_io_app_testrunner.c
 *
 * Purpose: This file contains a unit test runner for the Bootcamp IO App
 *
 */

/*
 * Includes
 */
#include "ut_support.h"

#include "bcamp_io_app_testcase.h"

/*
 * Function Definitions
 */

void UtTest_Setup(void)
{
    UT_Text("Bootcamp IO App Unit Test Output File\n\n");

    UtTest_Add(TestBCAMP_IO_ProcessGroundCommand, TestBCAMP_IO_Setup, TestBCAMP_IO_Teardown, "TestBCAMP_IO_ProcessGroundCommand");
}

