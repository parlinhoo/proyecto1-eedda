#include "../inc/bitvector.hpp"

#include <climits>
#include <cmath>
#include <cstdlib>
#include <iostream>

size_t bitvector::get_size() const {
    return size_bits;
}

std::string bitvector::get_bit_string() const {
    std::string str("");

    for (int p = (size_bits-1) % (word_size); p >= 0; p--) {
        char c = ( vector[0] & (1UL << p) ) == 0 ? '0' : '1';
        str.push_back(c);
    }

    for (uint i = 1; i < size_longs; i++) {
        str.push_back(' ');
        for (int p = word_size-1; p >= 0; p--) {
            char c = ( vector[i] & (1UL << p) ) == 0 ? '0' : '1';
            str.push_back(c);
        }
    }

    return str;
}

void bitvector::set_bit(size_t bit, bool value) {
    if (bit > size_bits-1) {
        std::cout << "Se intentó establecer un valor en una posición fuera de rango de un bitvector" << std::endl;
        return;
    }

    size_t target_long = size_longs-1 - (bit / word_size);
    unsigned long mask = 1UL << (bit % word_size);

    vector[target_long] = value ? (vector[target_long] | mask) : (vector[target_long] & ~mask);
}

bool bitvector::get_bit(size_t bit) const {
    if (bit > size_bits-1) {
        std::cout << "Se intentó establecer un valor en una posición fuera de rango de un bitvector" << std::endl;
        return 0;
    }

    size_t target_long = size_longs-1 - (bit / word_size);
    unsigned long mask = 1UL << (bit % word_size);

    return (vector[target_long] & mask);
}

bool bitvector::is_zero() const {
    size_t n_bits_last = size_bits % word_size;

    if (n_bits_last > 0) {
        unsigned long preproc_mask = 0;

        for (int i = 0; i < n_bits_last; i++) {
            preproc_mask = (preproc_mask << 1) | 1;
        }

        vector[0] = vector[0] & preproc_mask;
    }

    for (int i = 0; i < size_longs; i++) {
        if (vector[i] != 0) return false;
    }
    
    return true;
}

bitvector bitvector::operator|(bitvector& vec2) const {
    bitvector bigger = this->size_bits > vec2.size_bits ? *this : vec2;
    bitvector smaller = this->size_bits > vec2.size_bits ? vec2 : *this;

    size_t diff = bigger.size_longs - smaller.size_longs;

    bitvector vec(bigger.size_bits);

    for (int i = 0; i < diff; i++) {
        vec.vector[i] = bigger.vector[i];
    }

    for (int i = 0; i < smaller.size_longs; i++) {
        vec.vector[i+diff] = bigger.vector[i+diff] | smaller.vector[i];
    }

    return vec;
}

bitvector bitvector::operator&(bitvector& vec2) const {
    bitvector bigger = this->size_bits > vec2.size_bits ? *this : vec2;
    bitvector smaller = this->size_bits > vec2.size_bits ? vec2 : *this;

    size_t diff = bigger.size_longs - smaller.size_longs;

    bitvector vec(bigger.size_bits);

    for (int i = 0; i < smaller.size_longs; i++) {
        vec.vector[i+diff] = bigger.vector[i+diff] & smaller.vector[i];
    }

    return vec;
}

bitvector& bitvector::operator&=(bitvector& vec2) {
    bitvector bigger = this->size_bits > vec2.size_bits ? *this : vec2;
    bitvector smaller = this->size_bits > vec2.size_bits ? vec2 : *this;

    size_t diff = bigger.size_longs - smaller.size_longs;

    for (int i = 0; i < size_longs; i++) {
        vector[i] = bigger.vector[i+diff] & smaller.vector[i];
    }

    return *this;
}

bitvector bitvector::operator^(bitvector& vec2) const {
    bitvector bigger = this->size_bits > vec2.size_bits ? *this : vec2;
    bitvector smaller = this->size_bits > vec2.size_bits ? vec2 : *this;

    size_t diff = bigger.size_longs - smaller.size_longs;

    bitvector vec(bigger.size_bits);

    for (int i = 0; i < diff; i++) {
        vec.vector[i] = bigger.vector[i];
    }

    for (int i = 0; i < smaller.size_longs; i++) {
        vec.vector[i+diff] = bigger.vector[i+diff] ^ smaller.vector[i];
    }

    return vec;
}

bitvector bitvector::operator~() const {
    bitvector vec(size_bits);
    for (int i = 0; i < size_longs; i++) {
        vec.vector[i] = ~vector[i];
    }
    return vec;
}

bitvector bitvector::operator<<(size_t bits) const {
    bitvector vec(*this);

    unsigned long prev_last_bits = 0;
    unsigned long last_bits_mask;

    for (int i = size_longs-1; i >= 0; i--) {
        last_bits_mask = vec.vector[i] >> (word_size-bits);
        vec.vector[i] = (vec.vector[i] << bits) | prev_last_bits;
        prev_last_bits = last_bits_mask;
    }

    return vec;
}

bitvector& bitvector::operator<<=(size_t bits) {
    unsigned long prev_last_bits = 0;
    unsigned long last_bits_mask;

    for (int i = size_longs-1; i >= 0; i--) {
        last_bits_mask = vector[i] >> (word_size-bits);
        vector[i] = (vector[i] << bits) | prev_last_bits;
        prev_last_bits = last_bits_mask;
    }

    return *this;
}

bitvector bitvector::operator>>(size_t bits) const {
    bitvector vec(*this);

    unsigned long prev_first_bits = 0;
    unsigned long first_bits_mask;

    size_t n_bits_last = vec.size_bits % word_size;

    if (n_bits_last > 0) {
        unsigned long preproc_mask = 0;

        for (int i = 0; i < n_bits_last; i++) {
            preproc_mask = (preproc_mask << 1) | 1;
        }

        vec.vector[0] = vec.vector[0] & preproc_mask;
    }

    for (int i = 0; i < size_longs; i++) {
        first_bits_mask = vec.vector[i] << (word_size-bits);
        vec.vector[i] = (vec.vector[i] >> bits) | prev_first_bits;
        prev_first_bits = first_bits_mask;
    }

    return vec;
}

bitvector::bitvector(size_t size_bits) : size_bits(size_bits) {
    if (!size_bits) {
        std::cout << "No se puede crear un bitvector de tamaño 0." << std::endl;
    }
    size_longs = std::ceil( (float) size_bits / (float) (word_size) );
    vector = (unsigned long*) calloc(size_longs, sizeof(unsigned long));
}

bitvector::bitvector(size_t size_bits, bool zeros) : size_bits(size_bits) {
    if (!size_bits) {
        std::cout << "No se puede crear un bitvector de tamaño 0." << std::endl;
    }
    size_longs = std::ceil( (float) size_bits / (float) (word_size) );
    vector = (unsigned long*) calloc(size_longs, sizeof(unsigned long));

    if (!zeros) {
        for (int i = 0; i < size_longs; i++) {
            vector[i] = ULONG_MAX;
        }
    }
}

bitvector::bitvector(const bitvector& vec2) {
    size_longs = vec2.size_longs;
    size_bits = vec2.size_bits;
    vector = (unsigned long*) calloc(size_longs, sizeof(unsigned long));
    for (int i = 0; i < size_longs; i++) {
        vector[i] = vec2.vector[i];
    }
}

bitvector::~bitvector() {
    free(vector);
}
