#pragma once

#include <string>

class bitvector {
private:
    static const size_t word_size = 8*sizeof(unsigned long);
    size_t size_longs;
    size_t size_bits;
    unsigned long* vector;

public:
    size_t get_size() const;
    std::string get_bit_string() const;
    void set_bit(size_t bit, bool value);
    bool get_bit(size_t bit) const;
    bool is_zero() const;
    
    bitvector operator|(bitvector& vec2) const;
    bitvector operator&(bitvector& vec2) const;
    bitvector operator^(bitvector& vec2) const;
    bitvector operator~() const;
    bitvector operator<<(size_t bits) const;
    bitvector operator>>(size_t bits) const;

    bitvector& operator&=(bitvector& vec2);
    bitvector& operator<<=(size_t vec2);

    bitvector(size_t size_bits);
    bitvector(size_t size_bits, bool zeros);
    bitvector(const bitvector& vec2);
    ~bitvector();
};
