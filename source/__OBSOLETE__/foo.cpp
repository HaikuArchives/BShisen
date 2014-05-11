#include <stdio.h>
#include <stdlib.h>
#include "stack.cpp"



int main (int argc, char **argv)
{
stack<int> s;
int i;

	for(i=1; i < 34; i++)
		s.push(i);


	while (!s.empty())
		fprintf(stdout, " %d\n", s.pop());



}
