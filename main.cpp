#include "inc/bitvector.hpp"
#include "inc/bndm.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    std::vector<std::string> patterns = {"aabbaab", "baab"};

    std::vector<std::vector<int>> result = bndm("abbabaabbaab", patterns);

    for (uint i = 0; i < patterns.size(); i++) {
        std::cout << patterns[i] << ": ";
        for (int index : result[i]) {
            std::cout << index << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
