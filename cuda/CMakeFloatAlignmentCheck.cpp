/*
 * This is component of StoRM - Cuda Plugin to check whether a pair of uint_fast64_t and float gets auto-aligned to match 64bit boundaries
 */
 #include <cstdint>
 #include <utility>
 #include <vector>
 
 #define CONTAINER_SIZE 100ul

int main(int argc, char* argv[]) {
	int result = 0;

	std::vector<std::pair<uint_fast64_t, float>> myVector;
	for (size_t i = 0; i < CONTAINER_SIZE; ++i) {
		myVector.push_back(std::make_pair(i, 42.12345f * i));
	}
	
	char* firstUintPointer = reinterpret_cast<char*>(&(myVector.at(0).first));
	char* secondUintPointer = reinterpret_cast<char*>(&(myVector.at(1).first));
	ptrdiff_t uintDiff = secondUintPointer - firstUintPointer;
	
	if (uintDiff == (2 * sizeof(uint_fast64_t))) {
		result = 2;
	} else if (uintDiff == (sizeof(uint_fast64_t) + sizeof(float))) {
		result = 3;
	} else {
		result = -5;
	}
	
	return result;
 }
