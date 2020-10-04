#include <signal.h>

#include "interpreter.h"

int main(void)
{
	signal(SIGINT, SIG_IGN);
	routine();
	return 0;
}
