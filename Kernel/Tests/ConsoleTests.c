//
//  ConsoleTests.c
//  Kernel Tests
//
//  Created by Dietmar Planitzer on 7/9/23.
//  Copyright © 2023 Dietmar Planitzer. All rights reserved.
//

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <apollo/apollo.h>

////////////////////////////////////////////////////////////////////////////////
// Interactive Console
////////////////////////////////////////////////////////////////////////////////

void interactive_console_test(int argc, char *argv[])
{
    while (true) {
        const int ch = getchar();
        if (ch == EOF) {
            printf("Read error\n");
            continue;
        }

        putchar(ch);
        //printf("0x%hhx\n", ch);
    }
}