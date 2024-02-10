//
//  echo.c
//  sh
//
//  Created by Dietmar Planitzer on 1/11/24.
//  Copyright © 2024 Dietmar Planitzer. All rights reserved.
//

#include "Interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int cmd_echo(InterpreterRef _Nonnull self, int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        puts(argv[i]);
    }

    return EXIT_SUCCESS;
}
