#include <MoxieCoroutine.h>

using namespace moxie;

void CoroutineTestFunc();

MoxieCoroutine *coroutine = new MoxieCoroutine(CoroutineTestFunc);

void CoroutineTestFunc() {
	LOGGER_WARN("begin start CoroutineTestFunc.");
	coroutine->yeild();
	LOGGER_WARN("begin stop CoroutineTestFunc.");
}

int main() {
	coroutine->run();
	std::cout << "main start" << std::endl;
	coroutine->resume();
	std::cout << "main will out" << std::endl;
	return 0;
}
