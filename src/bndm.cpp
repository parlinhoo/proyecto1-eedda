#include "../inc/bitvector.hpp"
#include "../inc/bndm.hpp"

#include <iostream>
#include <map>

const char WILDCARD = '$';

std::vector<std::vector<int>> bndm(std::string text, std::vector<std::string> patterns) {
    std::string main_pattern("");

    std::map<char, size_t> index;
    std::vector<bitvector> vectors;
    bool wildcard = false;

    size_t max_size = 0;

    for (uint i = 0; i < patterns.size(); i++) {
        if (patterns[i].length() > max_size) {
            max_size = patterns[i].length();
        }
    }

    for (uint i = 0; i < max_size; i++) {
        for (std::string pat : patterns) {
            if (i < (max_size - pat.length())) {
                main_pattern.push_back(WILDCARD);
                wildcard = true;
                continue;
            }
            main_pattern.push_back(pat.at(i - max_size + pat.length()));
        }
    }

    std::cout << main_pattern << std::endl;

    for (uint i = 0; i < main_pattern.length(); i++) {
        char char_i = main_pattern.at(i);

        auto pair = index.try_emplace(char_i, vectors.size());
        if (pair.second) {
            vectors.push_back(bitvector(main_pattern.length()));
        }

        vectors.at(index[char_i]).set_bit(main_pattern.length()-1-i, 1);
    }

    if (wildcard) {
        for (int i = 0; i < vectors[index[WILDCARD]].get_size(); i++) {
            if (vectors[index[WILDCARD]].get_bit(i)) {
                for (int j = 0; j < vectors.size(); j++) {
                    vectors[j].set_bit(i, 1);
                }
            }
        }
    }


    for (const auto& pair : index) {
        std::cout << pair.first << ": " << vectors.at(pair.second).get_bit_string() << std::endl;
    }


    // algoritmo (por fin)
    
    std::vector<std::vector<int>> occurences;

    occurences.resize(patterns.size());

    size_t pos = 0;

    while (pos <= (text.length() - max_size)) {
        int j = max_size;
        int last = max_size;
        bitvector D(main_pattern.length(), false);
        while (!D.is_zero()) {
            D &= vectors[index[text.at(pos + j - 1)]];
            j--;
            for (size_t i = 0; i < patterns.size(); i++) {
                if (D.get_bit(main_pattern.length()-1-i)) {
                    if (j > 0) last = j;
                    else occurences[i].push_back(pos + max_size - patterns[i].length());
                }
            }
            D <<= patterns.size();
        }
        pos += last;
    }

    return occurences;
}
