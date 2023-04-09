#include "ObjectPool.h"
#include <time.h>

int main() {

	ObjectPool<int> pool;

	clock_t start;
	start = clock();
	for (int j = 0; j < 500; j++) {
		for (int i = 0; i < 40; i++) {
			auto obj = pool.acquire_object();
			pool.release_object(obj);
		}
	}
	std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n\n";


	start = clock();

	vector<int*>vec;
	for (int j = 0; j < 500; j++) {
		for (int i = 0; i < 40; i++) {
			int* p = new int(i);
			vec.push_back(p);
		}

		for (int i = 0; i < vec.size(); i++) {
			delete vec[i];
		}
		vec.clear();
	}
	std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n\n";
	

	

	return 0;

}