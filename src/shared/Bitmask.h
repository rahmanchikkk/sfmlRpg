#pragma once
#include <stdint.h>

using Bitset = uint32_t;

class Bitmask {
public:
    Bitmask() : m_bits(0) {};
    Bitmask(const Bitset& l_bits) : m_bits(l_bits) {};

    Bitset GetMask() const { return m_bits; };
    void SetMask(const Bitset& l_bits) { m_bits = l_bits; };

    bool Matches(const Bitmask& l_mask, const Bitset& l_relevant = 0) {
        return (l_relevant ? ((l_mask.GetMask() & l_relevant) == (m_bits & l_relevant)) :
            (m_bits == l_mask.GetMask()));
    };

    bool GetBit(const unsigned int& l_pos) const {
        return (m_bits & (1 << l_pos));
    };

    void ClearBit(const unsigned int& l_pos) {
        m_bits &= ~(1 << l_pos);
    };

    void ToggleBit(const unsigned int& l_pos) {
        m_bits ^= (1 << l_pos);
    };

    void TurnOnBit(const unsigned int& l_pos) {
        m_bits |= (1 << l_pos);
    };

    void TurnOnBits(const Bitset& l_bits) {
        m_bits |= l_bits;
    };

    void Clear() {
        m_bits = 0;
    };
private:
    Bitset m_bits;
};