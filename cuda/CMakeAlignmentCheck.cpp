/*
 * This is component of StoRM - Cuda Plugin to check whether type alignment matches the assumptions done while optimizing the code.
 */
 #include <cstdint>
 #include <utility>
 #include <vector>
 
 #define CONTAINER_SIZE 100ul
 
 template <typename IndexType, typename ValueType>
 int checkForAlignmentOfPairTypes(size_t containerSize, IndexType const firstValue, ValueType const secondValue) {
	std::vector<std::pair<IndexType, ValueType>>* myVector = new std::vector<std::pair<IndexType, ValueType>>();
	for (size_t i = 0; i < containerSize; ++i) {
		myVector->push_back(std::make_pair(firstValue, secondValue));
	}
	size_t myVectorSize = myVector->size();
	IndexType* firstStart = &(myVector->at(0).first);
	IndexType* firstEnd = &(myVector->at(myVectorSize - 1).first);
	ValueType* secondStart = &(myVector->at(0).second);
	ValueType* secondEnd = &(myVector->at(myVectorSize - 1).second);
	size_t startOffset = reinterpret_cast<size_t>(secondStart) - reinterpret_cast<size_t>(firstStart);
	size_t endOffset = reinterpret_cast<size_t>(secondEnd) - reinterpret_cast<size_t>(firstEnd);
	size_t firstOffset = reinterpret_cast<size_t>(firstEnd) - reinterpret_cast<size_t>(firstStart);
	size_t secondOffset = reinterpret_cast<size_t>(secondEnd) - reinterpret_cast<size_t>(secondStart);
	
	delete myVector;
	myVector = nullptr;
	
	if (myVectorSize != containerSize) {
		return -2;
	}
	
	// Check for alignment:
	// Requirement is that the pairs are aligned like: first, second, first, second, first, second, ...
	if (sizeof(IndexType) != sizeof(ValueType)) {
		return -3;
	}
	if (startOffset != sizeof(IndexType)) {
		return -4;
	}
	if (endOffset != sizeof(IndexType)) {
		return -5;
	}
	if (firstOffset != ((sizeof(IndexType) + sizeof(ValueType)) * (myVectorSize - 1))) {
		return -6;
	}
	if (secondOffset != ((sizeof(IndexType) + sizeof(ValueType)) * (myVectorSize - 1))) {
		return -7;
	}
	
	return 0;
 }
 
 
 int main(int argc, char* argv[]) {
	int result = 0;
	
	result = checkForAlignmentOfPairTypes<uint_fast64_t, double>(CONTAINER_SIZE, 42, 3.14);
	if (result != 0) {
		return result;
	}
	
	return 0;
 }
