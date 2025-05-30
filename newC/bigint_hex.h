#pragma once
#include "bigint_base.h"

//{hex_b}{hexm_b}
#define BIGINTHEX_DIV_DOUBLE 0

namespace BigIntHexNS {
#if BIGINTHEX_DIV_DOUBLE
    const uint32_t COMPRESS_BIT = 30;
#else
    const uint32_t COMPRESS_BIT = 32;
#endif
    const uint32_t COMPRESS_HALF_BIT = COMPRESS_BIT / 2;
    const uint32_t COMPRESS_HALF_MOD = 1 << COMPRESS_HALF_BIT;
    const uint32_t COMPRESS_HALF_MASK = COMPRESS_HALF_MOD - 1;
    const uint64_t COMPRESS_MOD = (uint64_t)1 << COMPRESS_BIT;
    const uint32_t COMPRESS_MASK = COMPRESS_MOD - 1;

    const uint32_t BIGINT_NTT_THRESHOLD = BIGINT_X64 ? 2000 : 1900;
    const uint32_t BIGINT_MUL_THRESHOLD = BIGINT_X64 ? 110 : 100;
    const uint32_t BIGINT_DIV_THRESHOLD = 1500;
    const uint32_t BIGINT_DIVIDEDIV_THRESHOLD = BIGINT_MUL_THRESHOLD * 3;
#if BIGINT_X64
    const uint32_t NTT_MAX_SIZE = 1 << (23 + BIGINTHEX_DIV_DOUBLE);
#else
    const uint32_t NTT_MAX_SIZE = 1 << 21;
#endif

    template <typename T> inline T high_digit(T digit) { return digit >> COMPRESS_BIT; }
#if BIGINTHEX_DIV_DOUBLE
    template <typename T> inline uint32_t low_digit(T digit) { return (uint32_t)(digit & COMPRESS_MASK); }
#else
    template <typename T> inline uint32_t low_digit(T digit) { return (uint32_t)digit; }
#endif

    class BigIntHex {
    protected:
        typedef uint32_t base_t;
        typedef int64_t carry_t;
        typedef uint64_t ucarry_t;
        int sign;
        std::vector<base_t> v;
        typedef BigIntHex BigInt_t;
        template <typename _Tx, typename _Ty> static inline void carry(_Tx& add, _Ty& baseval, _Tx newval) {
            add += newval;
            baseval = low_digit(add);
            add = high_digit(add);
        }
        template <typename _Tx, typename _Ty> static inline void borrow(_Tx& add, _Ty& baseval, _Tx newval) {
            add += newval;
            baseval = low_digit(add);
            add = high_digit(add);
        }

        bool raw_less(const BigInt_t& b) const {
            if (v.size() != b.size()) return v.size() < b.size();
            for (size_t i = v.size() - 1; i < v.size(); i--)
                if (v[i] != b.v[i]) return v[i] < b.v[i];
            return false; // eq
        }
        bool raw_eq(const BigInt_t& b) const {
            if (this == &b) return true;
            if (v.size() != b.size()) return false;
            for (size_t i = 0; i < v.size(); ++i)
                if (v[i] != b.v[i]) return false;
            return true;
        }
        BigInt_t& raw_add(const BigInt_t& b) {
            if (v.size() < b.size()) v.resize(b.size());
            ucarry_t add = 0;
            for (size_t i = 0; i < b.v.size(); i++)
                carry(add, v[i], (ucarry_t)v[i] + b.v[i]);
            for (size_t i = b.v.size(); add && i < v.size(); i++)
                carry(add, v[i], (ucarry_t)v[i]);
            add ? v.push_back((base_t)add) : trim();
            return *this;
        }
        BigInt_t& raw_offset_add(const BigInt_t& b, size_t offset) {
            ucarry_t add = 0;
            for (size_t i = 0; i < b.size(); ++i)
                carry(add, v[i + offset], (ucarry_t)v[i + offset] + b.v[i]);
            for (size_t i = b.size() + offset; add; ++i)
                carry(add, v[i], (ucarry_t)v[i]);
            return *this;
        }
        BigInt_t& raw_sub(const BigInt_t& b) {
            if (v.size() < b.v.size()) v.resize(b.v.size());
            carry_t add = 0;
            for (size_t i = 0; i < b.v.size(); i++)
                borrow(add, v[i], (carry_t)v[i] - (carry_t)b.v[i]);
            for (size_t i = b.v.size(); add && i < v.size(); i++)
                borrow(add, v[i], (carry_t)v[i]);
            if (add) {
                sign = -sign;
                add = 1;
                for (size_t i = 0; i < v.size(); i++)
                    carry(add, v[i], (carry_t)(COMPRESS_MOD - v[i] - 1));
            }
            trim();
            return *this;
        }
        BigInt_t& raw_offset_sub(const BigInt_t& b, size_t offset) {
            carry_t add = 0;
            for (size_t i = 0; i < b.v.size(); i++)
                borrow(add, v[i + offset], (carry_t)v[i + offset] - (carry_t)b.v[i]);
            for (size_t i = offset + b.v.size(); add && i < v.size(); i++)
                borrow(add, v[i], (carry_t)v[i]);
            return *this;
        }
        BigInt_t& raw_offset_mulsub(const BigInt_t& b, base_t mul, size_t offset) {
            if (mul == 0) return *this;
            carry_t add = 0;
            for (size_t i = 0; i < b.v.size(); i++)
                borrow(add, v[i + offset], (carry_t)v[i + offset] - (carry_t)b.v[i] * (carry_t)mul);
            for (size_t i = offset + b.v.size(); add && i < v.size(); i++)
                borrow(add, v[i], (carry_t)v[i]);
            return *this;
        }
        BigInt_t& raw_mul_int(base_t m) {
            if (m == 0) {
                set(0);
                return *this;
            }
            else if (m == 1)
                return *this;
            ucarry_t add = 0;
            size_t i = 0;
            for (; i + 4 <= v.size(); i += 4) {
                carry(add, v[i], v[i] * (ucarry_t)m);
                carry(add, v[i + 1], v[i + 1] * (ucarry_t)m);
                carry(add, v[i + 2], v[i + 2] * (ucarry_t)m);
                carry(add, v[i + 3], v[i + 3] * (ucarry_t)m);
                // carry(add, v[i + 4], v[i + 4] * (ucarry_t)m);
                // carry(add, v[i + 5], v[i + 5] * (ucarry_t)m);
                // carry(add, v[i + 6], v[i + 6] * (ucarry_t)m);
                // carry(add, v[i + 7], v[i + 7] * (ucarry_t)m);
            }
            for (; i < v.size(); i++)
                carry(add, v[i], v[i] * (ucarry_t)m);
            if (add) v.push_back((base_t)add);
            return *this;
        }
        BigInt_t& raw_mul(const BigInt_t& a, const BigInt_t& b) {
            if (a.is_zero() || b.is_zero()) {
                return set(0);
            }
            if (a.size() == 1 && a.v[0] == 1) {
                *this = b;
                sign *= a.sign;
                return *this;
            }
            if (b.size() == 1 && b.v[0] == 1) {
                *this = a;
                sign *= b.sign;
                return *this;
            }
            if (a.size() == 2 && a.v[1] == 1 && a.v[0] == 0) {
                *this = b;
                raw_shl(1);
                sign *= a.sign;
                return *this;
            }
            if (b.size() == 2 && b.v[1] == 1 && b.v[0] == 0) {
                *this = a;
                raw_shl(1);
                sign *= b.sign;
                return *this;
            }
            v.clear();
            v.resize(a.size() + b.size());
            for (size_t i = 0; i < a.size(); i++) {
                ucarry_t add = 0, av = a.v[i];
#if !BIGINTHEX_DIV_DOUBLE
                if (av == 0) continue;
#endif
                size_t j = 0;
                for (; j + 4 <= b.size(); j += 4) {
                    carry(add, v[i + j], v[i + j] + av * b.v[j]);
                    carry(add, v[i + j + 1], v[i + j + 1] + av * b.v[j + 1]);
                    carry(add, v[i + j + 2], v[i + j + 2] + av * b.v[j + 2]);
                    carry(add, v[i + j + 3], v[i + j + 3] + av * b.v[j + 3]);
                    // carry(add, v[i + j + 4], v[i + j + 4] + av * b.v[j + 4]);
                    // carry(add, v[i + j + 5], v[i + j + 5] + av * b.v[j + 5]);
                    // carry(add, v[i + j + 6], v[i + j + 6] + av * b.v[j + 6]);
                    // carry(add, v[i + j + 7], v[i + j + 7] + av * b.v[j + 7]);
                }
                for (; j < b.size(); ++j) {
                    carry(add, v[i + j], v[i + j] + av * b.v[j]);
                }
#if BIGINTHEX_DIV_DOUBLE
                v[i + b.size()] += (base_t)add;
#else
                for (size_t j = i + b.size(); add; ++j)
                    carry(add, v[j], (ucarry_t)v[j]);
#endif
            }
            trim();
            return *this;
        }
        // Karatsuba algorithm
        BigInt_t& raw_mul_karatsuba(const BigInt_t& a, const BigInt_t& b) {
            if (std::min(a.size(), b.size()) <= BIGINT_MUL_THRESHOLD) {
                return raw_mul(a, b);
            }
            if (a.size() * 2 < b.size() || b.size() * 2 < a.size()) { // split
                BigInt_t t;
                if (a.size() < b.size()) {
                    size_t split = b.size() / 2;
                    t.raw_mul_karatsuba(a, b.raw_shr_to(split));
                    t.raw_shl(split);
                    raw_mul_karatsuba(a, b.raw_lowdigits_to(split));
                    raw_add(t);
                }
                else {
                    size_t split = a.size() / 2;
                    t.raw_mul_karatsuba(b, a.raw_shr_to(split));
                    t.raw_shl(split);
                    raw_mul_karatsuba(b, a.raw_lowdigits_to(split));
                    raw_add(t);
                }
                return *this;
            }
            if (std::min(a.size(), b.size()) <= BIGINT_NTT_THRESHOLD)
                ;
            else if ((a.size() + b.size()) <= NTT_MAX_SIZE)
                return raw_nttmul(a, b);
            BigInt_t ah, al, bh, bl, h, m;
            size_t split = std::max(std::min((a.size() + 1) / 2, b.size() - 1), std::min(a.size() - 1, (b.size() + 1) / 2));
            al.v.assign(a.v.begin(), a.v.begin() + split);
            ah.v.assign(a.v.begin() + split, a.v.end());
            bl.v.assign(b.v.begin(), b.v.begin() + split);
            bh.v.assign(b.v.begin() + split, b.v.end());

            raw_mul_karatsuba(al, bl);
            h.raw_mul_karatsuba(ah, bh);
            m.raw_mul_karatsuba(al + ah, bl + bh);
            m.raw_sub(*this);
            m.raw_sub(h);
            v.resize(a.size() + b.size());

            raw_offset_add(m, split);
            raw_offset_add(h, split * 2);
            trim();
            return *this;
        }
        BigInt_t& raw_nttmul(const BigInt_t& a, const BigInt_t& b) {
            if (std::min(a.size(), b.size()) <= BIGINT_MUL_THRESHOLD) {
                return raw_mul(a, b);
            }
            if (std::min(a.size(), b.size()) <= BIGINT_NTT_THRESHOLD || (a.size() + b.size()) > NTT_MAX_SIZE) {
                return raw_mul_karatsuba(a, b);
            }
            if (a.size() * 3 < b.size() || b.size() * 3 < a.size()) { // split
                BigInt_t t;
                if (a.size() < b.size()) {
                    size_t split = b.size() / 2;
                    t.raw_nttmul(a, b.raw_shr_to(split));
                    t.raw_shl(split);
                    raw_nttmul(a, b.raw_lowdigits_to(split));
                    raw_add(t);
                }
                else {
                    size_t split = a.size() / 2;
                    t.raw_nttmul(b, a.raw_shr_to(split));
                    t.raw_shl(split);
                    raw_nttmul(b, a.raw_lowdigits_to(split));
                    raw_add(t);
                }
                return *this;
            }
            size_t len, lenmul = 2;
            std::vector<NTT_NS::ntt_base_t>& ntt_a = NTT_NS::ntt1.ntt_a, & ntt_b = NTT_NS::ntt1.ntt_b;
            std::vector<int64_t>& ntt_c = NTT_NS::ntt1.ntt_c;
            ntt_a.resize(a.size() * 2);
            ntt_b.resize(b.size() * 2);
            for (size_t i = 0, j = 0; i < a.size(); ++i, ++j) {
                ntt_a[j] = a.v[i] & COMPRESS_HALF_MASK;
                ntt_a[++j] = a.v[i] >> COMPRESS_HALF_BIT;
            }
            if (a == b) {
                NTT_NS::ntt_prepare(a.size() * 2, a.size() * 2, len, 7);
                NTT_NS::sqr_conv();
                len = a.size() * 2 * lenmul;
            }
            else {
                for (size_t i = 0, j = 0; i < b.size(); ++i, ++j) {
                    ntt_b[j] = b.v[i] & COMPRESS_HALF_MASK;
                    ntt_b[++j] = b.v[i] >> COMPRESS_HALF_BIT;
                }
                NTT_NS::ntt_prepare(a.size() * 2, b.size() * 2, len, 7);
                NTT_NS::mul_conv();
                len = (a.size() + b.size()) * lenmul;
            }
            while (len > 0 && ntt_c[--len] == 0)
                ;
            v.clear();
            uint64_t add = 0;
            v.reserve(++len / 2 + 2);
            for (size_t i = 0; i < len; i += 2) {
                add += ntt_c[i] + ((ntt_c[i + 1] & COMPRESS_HALF_MASK) << COMPRESS_HALF_BIT);
                v.push_back(low_digit(add));
                add = high_digit(add) + (ntt_c[i + 1] >> COMPRESS_HALF_BIT);
            }
            for (; add; add = high_digit(add))
                v.push_back(low_digit(add));
            trim();
            return *this;
        }
        BigInt_t& raw_div(const BigInt_t& a, const BigInt_t& b, BigInt_t& r) {
            r = a;
            if (a.raw_less(b)) {
                return set(0);
            }
            else if (b.size() == 2 && b.v[1] == 1 && b.v[0] == 0) {
                *this = a;
                r.set(a.v[0]);
                return raw_shr(1);
            }
            v.clear();
            v.resize(a.size() - b.size() + 1);
            r.v.resize(a.size() + 1);
            size_t offset = b.size();
#if BIGINTHEX_DIV_DOUBLE
            double db = b.v.back();
            if (b.size() > 2) {
                double t = b.v[b.size() - 2] + b.v[b.size() - 3] / (double)COMPRESS_MOD;
                db += (b.v.back() / (double)COMPRESS_HALF_MOD + t) / COMPRESS_MOD;
            }
            else if (b.size() > 1) {
                db += (b.v.back() / (double)COMPRESS_HALF_MOD + b.v[b.size() - 2]) / COMPRESS_MOD;
            }
            db = 1 / db;
#else
            ucarry_t db = (ucarry_t)b.v.back() << (COMPRESS_BIT - 1);
            if (b.size() > 1) {
                db += (b.v[b.size() - 2] >> 1) + 1;
            }
#endif
            for (size_t i = a.size() - offset; i <= a.size();) {
#if BIGINTHEX_DIV_DOUBLE
                carry_t rm = ((carry_t)r.v[i + offset] << (COMPRESS_BIT)) + r.v[i + offset - 1], m;
                m = std::max((carry_t)(rm * db), (carry_t)r.v[i + offset]);
#else
                ucarry_t rm = ((ucarry_t)r.v[i + offset] << (COMPRESS_BIT)) + r.v[i + offset - 1], m = 0;
                if (rm) {
                    if ((rm >> 1) > db) {
                        m = (rm >> 1) / db;
                        ++i;
                    }
                    else {
                        if (rm > 0xffffffff)
                            if (rm > (((ucarry_t)0xffff << 32) | 0xffffffff))
                                m = rm / ((db >> (COMPRESS_BIT - 1)) + 1);
                            else
                                m = (rm << COMPRESS_HALF_BIT) / ((db >> (COMPRESS_HALF_BIT - 1)) + 1);
                        else
                            m = (rm << (COMPRESS_BIT - 1)) / db;
                        if (m >= COMPRESS_MOD) {
                            m >>= COMPRESS_BIT;
                            ++i;
                        }
                    }
                }
#endif
#if BIGINTHEX_DIV_DOUBLE
                v[i] += (base_t)m;
                r.raw_offset_mulsub(b, (base_t)m, i);
#else
                if (m) {
                    v[i] += (base_t)m;
                    BigInt_t bm = b;
                    bm.raw_mul_int((base_t)m);
                    r.raw_offset_sub(bm, i);
                }
#endif
                i -= !r.v[i + offset];
            }
            {
#if BIGINTHEX_DIV_DOUBLE
                ucarry_t db = (ucarry_t)b.v.back() << (COMPRESS_BIT - 1);
                if (b.size() > 1) {
                    db += (b.v[b.size() - 2] >> 1) + 1;
                }
#endif
                size_t i = 0;
                ucarry_t rm = r.v[i + offset - 1], m = 0;
                m = (rm << (COMPRESS_BIT - 1)) / db;
                if (m) {
                    v[i] += (base_t)m;
                    BigInt_t bm = b;
                    bm.raw_mul_int((base_t)m);
                    r.raw_offset_sub(bm, i);
                }
            }
            r.trim();
            carry_t add = 0;
            while (!r.raw_less(b)) {
                r.raw_sub(b);
                ++add;
            }

            for (size_t i = 0; i < v.size(); i++)
                carry(add, v[i], (carry_t)v[i]);
            trim();
            return *this;
        }
        BigInt_t& raw_shr(size_t n) {
            if (n == 0) return *this;
            if (n >= size()) {
                set(0);
                return *this;
            }
            v.erase(v.begin(), v.begin() + n);
            return *this;
        }
        BigInt_t raw_shr_to(size_t n) const {
            BigInt_t r;
            if (n >= size()) return r;
            r.v.assign(v.begin() + n, v.end());
            return BIGINT_STD_MOVE(r);
        }
        BigInt_t raw_lowdigits_to(size_t n) const {
            if (n >= size()) return *this;
            BigInt_t r;
            r.v.assign(v.begin(), v.begin() + n);
            r.trim();
            return BIGINT_STD_MOVE(r);
        }
        BigInt_t& raw_shl(size_t n) {
            if (n == 0 || is_zero()) return *this;
            v.insert(v.begin(), n, 0);
            return *this;
        }
        BigInt_t& keep(size_t n) {
            size_t s = n < v.size() ? v.size() - n : (size_t)0;
            if (s && v[s - 1] >= COMPRESS_MOD >> 1) ++v[s];
            return raw_shr(s);
        }
        BigInt_t& raw_fastdiv(const BigInt_t& a, const BigInt_t& b) {
            if (a.raw_less(b)) {
                set(0);
                return *this;
            }
            if (b.size() * 2 - 2 > a.size()) {
                BigInt_t ta = a, tb = b;
                size_t ans_len = a.size() - b.size() + 2;
                size_t r = b.size() - ans_len;
                ta.raw_shr(r);
                tb.raw_shr(r);
                return raw_fastdiv(ta, tb);
            }
            if (b.size() < BIGINT_DIV_THRESHOLD) {
                BigInt_t r;
                return raw_div(a, b, r);
            }
            size_t extand_digs = 2;
            size_t ans_len = a.size() - b.size() + extand_digs + 1, s_len = ans_len + 32;
            std::vector<size_t> len_seq;
            BigInt_t b2, x0, x1, t;
            x1.v.resize(1);
            x1.v.back() = (base_t)(COMPRESS_MOD / (b.v.back() + (double)(b.v[b.size() - 2]) / COMPRESS_MOD));
            x0.v.push_back(x1.v.back() + 1);
            size_t keep_size = 1;
            while (s_len > 32) {
                len_seq.push_back(std::min(s_len, ans_len));
                s_len = s_len / 2 + 1;
            }
            while (x1 != x0) {
                // x1 = x0(2 - x0 * b)
                x0 = x1;
                b2 = b;
                size_t tsize = std::min(keep_size * 2, ans_len);
                b2.keep(tsize);
                t = x0 * b2;
                t.keep(tsize);
                for (size_t i = t.size() - 2; i < t.size(); --i)
                    t.v[i] ^= COMPRESS_MASK;
                if (t.v.back() != 1) {
                    t.v.back() ^= COMPRESS_MASK;
                    t.v.push_back(1);
                }
                else {
                    t.v.pop_back();
                    t.trim();
                }
                x1 *= t;
                keep_size = std::min(keep_size * 2, s_len);
                x1.keep(keep_size);
            }
            for (; !len_seq.empty(); len_seq.pop_back()) {
                // x1 = x0(2 - x0 * b)
                x0 = x1;
                b2 = b;
                size_t tsize = std::min(keep_size * 2, ans_len);
                b2.keep(tsize);
                t = x0 * b2;
                t.keep(tsize);
                for (size_t i = t.size() - 2; i < t.size(); --i)
                    t.v[i] ^= COMPRESS_MASK;
                if (t.v.back() != 1) {
                    t.v.back() ^= COMPRESS_MASK;
                    t.v.push_back(1);
                }
                else {
                    t.v.pop_back();
                    t.trim();
                }
                ucarry_t add = 1;
                for (size_t i = 0; add && i < t.v.size(); i++)
                    carry(add, t.v[i], (ucarry_t)t.v[i]);
                if (add) t.v.push_back((base_t)add);
                x1 *= t;
                keep_size = len_seq.back();
                x1.keep(keep_size);
            }
            x1 *= a;
            if (x1.v[a.size() + extand_digs - 1] >= COMPRESS_MOD - 1)
                *this = x1.raw_shr(a.size() + extand_digs).raw_add(BigInt_t(1));
            else
                *this = x1.raw_shr(a.size() + extand_digs);
            return *this;
        }
        BigInt_t& raw_dividediv_recursion(const BigInt_t& a, const BigInt_t& b, BigInt_t& r) {
            if (a < b) {
                r = a;
                return set(0);
            }
            else if (b.size() <= BIGINT_DIVIDEDIV_THRESHOLD) {
                return raw_div(a, b, r);
            }
            size_t base = (b.size() + 1) / 2;
            if (a.size() <= base * 3) {
                base = b.size() / 2;
                BigInt_t ma = a, mb = b, e;
                BigInt_t ha = ma.raw_shr_to(base);
                BigInt_t hb = mb.raw_shr_to(base);
                raw_dividediv_recursion(ha, hb, r);
                ha = *this * b;
                while (a < ha) {
                    ha.raw_sub(b);
                    raw_sub(BigInt_t(1));
                }
                r = a - ha;
                return *this;
            }
            if (a.size() > base * 4) base = a.size() / 2;
            BigInt_t ha = a.raw_shr_to(base);
            BigInt_t c, d, m;
            raw_dividediv_recursion(ha, b, d);
            raw_shl(base);
            m.v.resize(base + d.size());
            for (size_t i = 0; i < base; ++i)
                m.v[i] = a.v[i];
            for (size_t i = 0; i < d.size(); ++i)
                m.v[base + i] = d.v[i];
            c.raw_dividediv_recursion(m, b, r);
            raw_add(c);
            return *this;
        }
        BigInt_t& raw_dividediv(const BigInt_t& a, const BigInt_t& b, BigInt_t& r) {
            if (b.size() <= BIGINT_DIVIDEDIV_THRESHOLD) {
                raw_div(a, b, r);
                return *this;
            }
            if (b.size() * 2 - 2 > a.size()) {
                BigInt_t ta = a, tb = b;
                size_t ans_len = a.size() - b.size() + 2;
                size_t shr = b.size() - ans_len;
                ta.raw_shr(shr);
                tb.raw_shr(shr);
                return raw_dividediv(ta, tb, r);
            }
            carry_t mul = (carry_t)(((uint64_t)(COMPRESS_MOD + 1) * (COMPRESS_MOD - 1)) /       //
                (*(b.v.begin() + b.v.size() - 1) * (uint64_t)COMPRESS_MOD + //
                    *(b.v.begin() + b.v.size() - 2) + 1));
            BigInt_t ma = a * BigInt_t((intmax_t)mul), mb = b * BigInt_t((intmax_t)mul);
            while (mb.v.back() < COMPRESS_MOD >> 1) {
                int32_t m = 2;
                ma.raw_mul_int(m);
                mb.raw_mul_int(m);
                mul *= m;
            }
            BigInt_t d;
            ma.sign = mb.sign = 1;
            raw_dividediv_recursion(ma, mb, d);
            r.raw_div(d, BigInt_t((int)mul), ma);
            return *this;
        }
        void trim() {
            while (v.back() == 0 && v.size() > 1)
                v.pop_back();
        }
        size_t size() const { return v.size(); }
        //{hexm_e}
        BigIntBase transbase(int32_t out_base) const {
            if (size() <= 8) {
                BigIntBase sum(out_base);
                BigIntBase base(out_base);
                BigIntBase ownbase(out_base);
                ownbase.set(COMPRESS_MOD);
                BigIntBase mul(out_base), t(out_base);
                base.set(1);
                t.set(v[0]);
                mul.raw_mul(base, t);
                sum.raw_add(mul);
                for (size_t i = 1; i < v.size(); i++) {
                    base.raw_mul(ownbase, BigIntBase(base));
                    t.set(v[i]);
                    mul.raw_mul(base, t);
                    sum.raw_add(mul);
                }
                return BIGINT_STD_MOVE(sum);
            }
            else {
                static std::vector<BigIntBase> pow_list;
                static int32_t last_base = 0, pow_list_cnt;
                BigIntBase base(out_base);
                if (out_base != last_base) {
                    pow_list.clear();
                    pow_list.push_back(base.set(COMPRESS_MOD));
                    pow_list_cnt = 0;
                    last_base = out_base;
                }
                size_t s = 1, id = 0;
                for (; s < size() / 3; s *= 2, ++id) {
                    if (s >= (size_t)1 << pow_list_cnt) {
                        pow_list.push_back(BigIntBase(out_base));
                        pow_list.back().raw_nttsqr(pow_list[pow_list_cnt]);
                        ++pow_list_cnt;
                    }
                }
                base = pow_list[id];
                BigInt_t h = raw_shr_to(s), l = *this;
                l.v.resize(s);
                BigIntBase r = h.transbase(out_base);
                BigIntBase sum(out_base);
                sum.raw_nttmul(r, base);
                r = l.transbase(out_base);
                sum.raw_add(r);
                return BIGINT_STD_MOVE(sum);
            }
        }
        std::string out_mul(int32_t out_base = 10, int32_t pack = 0) const {
            BigIntBase sum = transbase(out_base);
            std::string out;
            int32_t d = 0;
            for (size_t i = 0, j = 0;;) {
                if (j < 1) {
                    if (i < sum.size())
                        d += sum.v[i];
                    else if (d == 0)
                        break;
                    j += sum.digits;
                    ++i;
                }
                if (out_base <= 10 || d % out_base < 10) {
                    out.push_back(char((d % out_base) + '0'));
                }
                else {
                    out.push_back(char((d % out_base) + 'A' - 10));
                }
                d /= out_base;
                j -= 1;
            }
            if (pack == 0)
                while (out.size() > 1 && *out.rbegin() == '0')
                    out.erase(out.begin() + out.size() - 1);
            else
                while ((int32_t)out.size() > pack && *out.rbegin() == '0')
                    out.erase(out.begin() + out.size() - 1);
            while ((int32_t)out.size() < pack)
                out.push_back('0');
            if (out.empty()) out.push_back('0');
            if (sign < 0 && !this->is_zero()) out.push_back('-');
            std::reverse(out.begin(), out.end());
            return out;
        }
        //{hexm_b}
        std::string out_base2(size_t bits) const {
            if (is_zero()) return "0";
            const char* digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            std::string out;
            carry_t d = 0;
            for (size_t i = 0, j = 0;;) {
                if (j < bits) {
                    if (i < v.size())
                        d += v[i] << j;
                    else if (d == 0)
                        break;
                    j += COMPRESS_BIT;
                    ++i;
                }
                out.push_back(digits[d & ((1 << bits) - 1)]);
                d >>= bits;
                j -= bits;
            }
            while (out.size() > 1 && *out.rbegin() == '0')
                out.erase(out.begin() + out.size() - 1);
            if (sign < 0 && !this->is_zero()) out.push_back('-');
            std::reverse(out.begin(), out.end());
            return out;
        }
        std::string out_hex() const { return out_base2(4); }
        BigInt_t& from_str_base2(const char* s, int bits) {
            v.clear();
            int32_t sign = 1;
            const char* p = s + strlen(s) - 1;
            while (*s == '-')
                sign *= -1, ++s;
            while (*s == '0')
                ++s;

            int64_t d = 0, hdigit = 0;
            for (; p >= s; p--) {
                int64_t digit = -1;
                if (*p >= '0' && *p <= '9')
                    digit = *p - '0';
                else if (*p >= 'A' && *p <= 'Z')
                    digit = *p - 'A' + 10;
                else if (*p >= 'a' && *p <= 'z')
                    digit = *p - 'a' + 10;
                hdigit += digit << d;
                d += bits;
                if (d >= COMPRESS_BIT) {
                    v.push_back(hdigit & COMPRESS_MASK);
                    d -= COMPRESS_BIT;
                    hdigit >>= COMPRESS_BIT;
                }
            }
            if (hdigit || v.empty()) {
                v.push_back((base_t)hdigit);
            }
            this->sign = sign;
            return *this;
        }
        //{hexm_e}
        BigInt_t& _from_str(const std::string& s, int base) {
            if (s.size() <= 12) {
                int64_t v = 0;
                for (size_t i = 0; i < s.size(); ++i) {
                    int digit = -1;
                    if (s[i] >= '0' && s[i] <= '9')
                        digit = s[i] - '0';
                    else if (s[i] >= 'A' && s[i] <= 'Z')
                        digit = s[i] - 'A' + 10;
                    else if (s[i] >= 'a' && s[i] <= 'z')
                        digit = s[i] - 'a' + 10;
                    v = v * base + digit;
                }
                return set(v);
            }
            BigInt_t m(base), h;
            size_t len = 1;
            for (; len * 3 < s.size(); len *= 2) {
                m *= m;
            }
            h._from_str(s.substr(0, s.size() - len), base);
            _from_str(s.substr(s.size() - len), base);
            *this += m * h;
            return *this;
        }
        //{hexm_b}
    public:
        BigIntHex() { set(0); }
        explicit BigIntHex(int n) { set(n); }
        explicit BigIntHex(intmax_t n) { set(n); }
        explicit BigIntHex(const char* s, int base = 10) { from_str(s, base); }
        explicit BigIntHex(const std::string& s, int base = 10) { from_str(s, base); }
        BigInt_t& set(intmax_t n) {
            v.resize(1);
            v[0] = 0;
            uintmax_t s;
            if (n < 0) {
                sign = -1;
                s = -n;
            }
            else {
                sign = 1;
                s = n;
            }
            for (size_t i = 0; s; i++) {
                v.resize(i + 1);
                v[i] = low_digit(s);
                s = high_digit(s);
            }
            return *this;
        }
        BigInt_t& from_str(const char* s, int base = 10) {
            //{hexm_e}
            if ((base & (base - 1)) == 0) {
                //{hexm_b}
                if (base == 16) {
                    return from_str_base2(s, 4);
                }
                else if (base == 8) {
                    return from_str_base2(s, 3);
                }
                else if (base == 4) {
                    return from_str_base2(s, 2);
                }
                else if (base == 2) {
                    return from_str_base2(s, 1);
                }
                else {
                    return from_str_base2(s, 5);
                }
                //{hexm_e}
            }
            int vsign = 1, i = 0;
            while (s[i] == '-') {
                ++i;
                vsign = -vsign;
            }
            _from_str(std::string(s + i), base);
            sign = vsign;
            return *this;
            //{hexm_b}
        }
        BigInt_t& from_str(const std::string& s, int base = 10) { return this->from_str(s.c_str(), base); }
        bool is_zero() const {
            if (v.size() == 1 && v[0] == 0) return true;
            return false;
        }
        bool operator<(const BigInt_t& b) const {
            if (sign * b.sign > 0) {
                if (sign > 0)
                    return raw_less(b);
                else
                    return b.raw_less(*this);
            }
            else {
                if (sign > 0)
                    return false;
                else
                    return true;
            }
        }
        bool operator==(const BigInt_t& b) const {
            if (is_zero() && b.is_zero()) return true;
            if (sign != b.sign) return false;
            return raw_eq(b);
        }
        LESS_THAN_AND_EQUAL_COMPARABLE(BigInt_t)

            BigInt_t& operator=(intmax_t n) { return set(n); }
        BigInt_t& operator=(const char* s) { return from_str(s); }
        BigInt_t& operator=(const std::string s) { return from_str(s); }
        BigInt_t operator+(const BigInt_t& b) const {
            if (sign * b.sign > 0)
                return BIGINT_STD_MOVE(BigInt_t(*this).raw_add(b));
            else if (size() < b.size())
                return BIGINT_STD_MOVE(BigInt_t(b).raw_sub(*this).inv());
            else
                return BIGINT_STD_MOVE(BigInt_t(*this).raw_sub(b));
        }
        BigInt_t& operator+=(const BigInt_t& b) {
            if (this == &b)
                return *this += BigInt_t(b);
            if (sign * b.sign > 0)
                raw_add(b);
            else if (size() < b.size())
                *this = BIGINT_STD_MOVE(BigInt_t(b).raw_sub(*this).inv());
            else
                raw_sub(b);
            return *this;
        }

        BigInt_t operator-(const BigInt_t& b) const {
            if (sign * b.sign < 0)
                return BIGINT_STD_MOVE(BigInt_t(*this).raw_add(b));
            else if (size() < b.size())
                return BIGINT_STD_MOVE(BigInt_t(b).raw_sub(*this).inv());
            else
                return BIGINT_STD_MOVE(BigInt_t(*this).raw_sub(b));
        }
        BigInt_t& operator-=(const BigInt_t& b) {
            if (this == &b) {
                set(0);
                return *this;
            }
            if (sign * b.sign < 0)
                raw_add(b);
            else if (size() < b.size())
                *this = BIGINT_STD_MOVE(BigInt_t(b).raw_sub(*this).inv());
            else
                raw_sub(b);
            return *this;
        }
        BigInt_t& inv() {
            sign = -sign;
            return *this;
        }
        BigInt_t operator-() const {
            return BIGINT_STD_MOVE(BigInt_t(*this).inv());
        }

        BigInt_t operator*(const BigInt_t& b) const {
            if (b.size() == 1) {
                BigInt_t r = *this;
                r.raw_mul_int((uint32_t)b.v[0]);
                r.sign *= b.sign;
                return BIGINT_STD_MOVE(r);
            }
            else if (v.size() == 1) {
                BigInt_t r = b;
                r.raw_mul_int((uint32_t)v[0]);
                r.sign *= sign;
                return BIGINT_STD_MOVE(r);
            }
            else {
                BigInt_t r;
                if (raw_less(b))
                    r.raw_nttmul(*this, b);
                else
                    r.raw_nttmul(b, *this);
                r.sign = sign * b.sign;
                return BIGINT_STD_MOVE(r);
            }
        }
        BigInt_t& operator*=(const BigInt_t& b) {
            if (b.size() == 1) {
                raw_mul_int((uint32_t)b.v[0]);
                sign *= b.sign;
                return *this;
            }
            else {
                return *this = *this * b;
            }
        }

        BigInt_t operator/(const BigInt_t& b) const {
            BigInt_t r, d;
            if (b.size() > BIGINT_DIV_THRESHOLD)
                d.raw_fastdiv(*this, b);
            else
                d.raw_dividediv(*this, b, r);
            d.sign = sign * b.sign;
            return BIGINT_STD_MOVE(d);
        }
        BigInt_t& operator/=(const BigInt_t& b) {
            if (this == &b) {
                return set(1);
            }
            return *this = *this / b;
        }
        BigInt_t operator%(const BigInt_t& b) const {
            if (b.size() == 1 && COMPRESS_MOD % b.v[0] == 0) {
                return BigInt_t((intmax_t)(v[0] % b.v[0]) * sign);
            }
            if (this == &b) {
                return BigInt_t((intmax_t)0);
            }
            return BIGINT_STD_MOVE(*this - *this / b * b);
        }
        BigInt_t& operator%=(const BigInt_t& b) {
            if (b.size() == 1 && COMPRESS_MOD % b.v[0] == 0) {
                return set(v[0] % b.v[0] * sign);
            }
            if (this == &b) {
                return set(0);
            }
            return *this = *this % b;
        }
        BigInt_t div(const BigInt_t& b, BigInt_t& r) {
            if (this == &b) {
                r.set(0);
                return set(1);
            }
            BigInt_t d;
            d.raw_fastdiv(*this, b);
            d.sign = sign * b.sign;
            r = *this - d * b;
            // d.raw_dividediv(*this, b, r);
            // d.sign = sign * b.sign;
            return BIGINT_STD_MOVE(d);
        }

        std::string to_str(int32_t out_base = 10, int32_t pack = 0) const {
            //{hexm_e}
            if ((out_base & (out_base - 1)) == 0) {
                //{hexm_b}
                if (out_base == 16) {
                    return out_hex();
                }
                else if (out_base == 8) {
                    return out_base2(3);
                }
                else if (out_base == 4) {
                    return out_base2(2);
                }
                else if (out_base == 2) {
                    return out_base2(1);
                }
                else {
                    return out_base2(5);
                }
                //{hexm_e}
            }
            return out_mul(out_base, pack);
            //{hexm_b}
        }
    };
} // namespace BigIntHexNS

using BigIntHexNS::BigIntHex;
