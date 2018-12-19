/***********************************************************************
Copyright (c) 2018
Marcus Comstedt

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***********************************************************************/

#if !defined(LILLIPUT_HH_EE5774C4_DBC2_442F_845F_2B158BD648D2_INCLUDED)
#define LILLIPUT_HH_EE5774C4_DBC2_442F_845F_2B158BD648D2_INCLUDED

#include <cstdint>
#include <limits>
#include <type_traits>

namespace lilliput {

  struct big_endian {};

  struct little_endian {};

  template<typename T> struct value_type { typedef T type; };

  template<typename T, typename L, std::size_t B, typename E> class access { };

  template<typename T, typename L, std::size_t B> class access<T, L, B, big_endian> {

    typedef typename value_type<L>::type limb_type;

    static_assert(std::is_unsigned<T>::value,
		  "Basetype must be an unsigned arithmetic type");
    static_assert(std::is_unsigned<limb_type>::value,
		  "Limbtype must be an unsigned arithmetic type");
    
  public:

    static constexpr T combine_right(const T& t, const L& l, std::size_t n) {
      return static_cast<limb_type>(l) | (t << B);
    }
    
    static constexpr T combine_left(const T& t, const L& l, std::size_t n) {
      return t;
    }
    
    static constexpr T split_right(const T& t, L& l, std::size_t n) {
      return t;
    }

    static constexpr T split_left(const T& t, L& l, std::size_t n) {
      l = static_cast<limb_type>(t);
      return t >> B;
    }
    
  };

  template<typename T, typename L, std::size_t B> class access<T, L, B, little_endian> {

    typedef typename value_type<L>::type limb_type;

    static_assert(std::is_unsigned<T>::value,
		  "Basetype must be an unsigned arithmetic type");
    static_assert(std::is_unsigned<limb_type>::value,
		  "Limbtype must be an unsigned arithmetic type");
    
  public:

    static constexpr T combine_right(const T& t, const L& l, std::size_t n) {
      return t;
    }
    
    static constexpr T combine_left(const T& t, const L& l, std::size_t n) {
      return static_cast<limb_type>(l) | (t << B);
    }
    
    static constexpr T split_right(const T& t, L& l, std::size_t n) {
      l = static_cast<limb_type>(t);
      return t >> B;
    }

    static constexpr T split_left(const T& t, L& l, std::size_t n) {
      return t;
    }
    
  };

  template<typename T, typename L, typename A, typename U = T> class sliced_storage {

  public:
    typedef T value_type;
    typedef U base_type;
    typedef L limb_type;
    typedef A access_mode;

    static_assert(sizeof(value_type) == sizeof(base_type),
		  "Base type must have the same size as value type");

    static constexpr std::size_t const bits =
      sizeof(base_type) * std::numeric_limits<unsigned char>::digits;
    static constexpr std::size_t limb_bits =
      sizeof(limb_type) * std::numeric_limits<unsigned char>::digits;

    static_assert(bits % limb_bits == 0,
		  "Base type size must be a multiple of the limb size");

    static constexpr std::size_t num_limbs = bits / limb_bits;


  private:
    limb_type data[num_limbs];

    typedef access<base_type, limb_type, limb_bits, access_mode> accessor;

    template<std::size_t N> constexpr base_type split(typename std::enable_if<(N >= num_limbs), const base_type&>::type t)
    {
      return t;
    }

    template<std::size_t N> constexpr base_type split(typename std::enable_if<(N < num_limbs), const base_type&>::type t)
    {
      return accessor::split_left(split<N+1>(accessor::split_right(t, data[N], N)), data[N], N);
    }

    constexpr void set(const value_type& n)
    {
      split<0>(n);
    }

    template<std::size_t N> constexpr typename std::enable_if<(N >= num_limbs), base_type>::type combine(const base_type& t) const
    {
      return t;
    }
    
    template<std::size_t N> constexpr typename std::enable_if<(N < num_limbs), base_type>::type combine(const base_type& t) const
    {
      return accessor::combine_left(combine<N+1>(accessor::combine_right(t, data[N], N)), data[N], N);
    }

    constexpr value_type get() const
    {
      return combine<0>(base_type());
    }

  public:
    constexpr sliced_storage() : data{} { set(value_type()); }
    constexpr sliced_storage(const value_type& n) : data{} { set(n); }
    constexpr operator value_type() const { return get(); }
    constexpr const value_type& operator=(const value_type& n)
    {
      set(n);
      return n;
    }
  };

  template<typename T, typename L, typename A, typename U> struct value_type<sliced_storage<T, L, A, U>> { typedef T type; };

  template<typename T, typename L = uint8_t> using le =
    sliced_storage<T, L, little_endian, typename std::make_unsigned<T>::type>;
  template<typename T, typename L = uint8_t> using be =
    sliced_storage<T, L, big_endian, typename std::make_unsigned<T>::type>;

}
#endif // !defined(LILLIPUT_HH_EE5774C4_DBC2_442F_845F_2B158BD648D2_INCLUDED)
