#include <iostream>
#include <z3.h>

int main() {
    unsigned major, minor, build_number, revision_number;
    Z3_get_version(&major, &minor, &build_number, &revision_number);
    std::cout << major << "." << minor << "." << build_number << std::endl;
    return 0;
}
