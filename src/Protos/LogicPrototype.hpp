#pragma once
using namespace N;

using Bit = bool;

inline Bit NOT(const Bit bit)
{
    return !bit;
}
inline Bit OR(const Bit bit, const Bit bit2)
{
    return bit || bit2;
}
inline Bit AND(const Bit bit, const Bit bit2)
{
    return bit && bit2;
}
inline Bit XOR(const Bit bit, const Bit bit2)
{
    return bit != bit2;
}

template <int Amount> struct Bits
{
    Bits() {}
    template <typename... Args>
    Bits(Args... args) requires(sizeof...(Args) == Amount && (std::convertible_to<Args, Bit> && ...))
        : m_Bits{static_cast<Bit>(args)...}
    {
    }

    Bit& operator()(unsigned int index)
    {
        return m_Bits[index];
    }

    const Bit& operator()(unsigned int index) const
    {
        return m_Bits[index];
    }

    friend std::ostream& operator<<(std::ostream& os, const Bits& bits)
    {
        for (auto it = bits.m_Bits.rbegin(); it != bits.m_Bits.rend(); ++it)
        {
            os << *it;
        }

        return os;
    }

  private:
    std::array<Bit, Amount> m_Bits{0};
};
using Byte = Bits<8>;

inline Bits<2> HalfAdder(const Bit bit, const Bit bit2)
{
    Bit sum = XOR(bit, bit2);
    Bit carry = AND(bit, bit2);
    return {sum, carry};
}

inline Bits<2> FullAdder(const Bit bit1, const Bit bit2, const Bit carry)
{
    Bits<2> halfAdded = HalfAdder(bit1, bit2);
    Bits<2> halfAdded2 = HalfAdder(halfAdded(0), carry);

    Bit sum = halfAdded2(0);
    Bit carry2 = OR(halfAdded(1), halfAdded2(1));
    return {sum, carry2};
}

struct ALU
{
    using Flag = Bit;
    Flag Zero = true;
    Flag Overflow = false;

    template <int Amount> Bits<Amount> Add(const Bits<Amount>& bits1, const Bits<Amount>& bits2)
    {
        Bit carry = false;
        Bits<Amount> result;
        Zero = true;
        Overflow = false;

        for (int i = 0; i < Amount; ++i)
        {
            Bits<2> fullAdded = FULLADDER(bits1(i), bits2(i), carry);
            Bit sum = fullAdded(0);
            result(i) = sum;
            carry = fullAdded(1);

            if (sum == 1)
            {
                Zero = false;
            }
        }

        if (carry == 1)
        {
            Overflow = true;
        }

        return result;
    }

    template <int Amount> Bits<Amount> And(const Bits<Amount>& bits1, const Bits<Amount>& bits2)
    {
        Bits<Amount> result;

        for (int i = 0; i < Amount; ++i)
        {
            result(i) = AND(bits1(i), bits2(i));
        }
        return result;
    }
    template <int Amount> Bits<Amount> Or(const Bits<Amount>& bits1, const Bits<Amount>& bits2)
    {
        Bits<Amount> result;

        for (int i = 0; i < Amount; ++i)
        {
            result(i) = OR(bits1(i), bits2(i));
        }
        return result;
    }
    template <int Amount> Bits<Amount> Xor(const Bits<Amount>& bits1, const Bits<Amount>& bits2)
    {
        Bits<Amount> result;

        for (int i = 0; i < Amount; ++i)
        {
            result(i) = XOR(bits1(i), bits2(i));
        }
        return result;
    }
    template <int Amount> Bits<Amount> Not(const Bits<Amount>& bits)
    {
        Bits<Amount> result;

        for (int i = 0; i < Amount; ++i)
        {
            result(i) = NOT(bits(i));
        }
        return result;
    }
};

struct uInt8 : Bits<8>
{
    using Bits::Bits;
    constexpr uInt8(uint8_t value)
    {
        for (int i = 0; i < 8; ++i)
        {
            (*this)(i) = value % 2;
            value /= 2;
        }
    }
};
