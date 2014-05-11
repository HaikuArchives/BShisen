/*
 * Copyright 1998, 1999 Kelvin W Sherlock
 * 
 *
 *
 * 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "Shisen.h"

/*
 * $Id: main.cpp,v 1.3 1999/03/05 00:23:41 baron Exp $
 */

int main(int argc, char **argv)
{
Shisen* S;

	S = new Shisen;
	S->Run();

	return 0;
}
