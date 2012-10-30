#include <iostream>


int main(void)
{
	std::cout << "Hello World!" << std::endl;

#ifdef DEBUG
	std::cout << "Hello World! (with debug)" << std::endl;
#endif
	return 0;
}
