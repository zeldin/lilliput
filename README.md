lilliput
========

lilliput is a set of C++14 templates to help you keep the big-endians
and the little-endians apart.  They can convert any integral datatype
(such as uint32_t) into a variant which is guaranteed to be stored in
a certain endianness, regardless of the native endianness of the host.

be<T> will give you a big-endian T, and le<T> will give you a
little-endian T.  An optional second template parameter lets you
change the type of the pieces that are arranged according to the
specificed endianness (default uint8_t).  This way you can create for
example a 32-bit PDP-11 endian type like so:
```
typedef be<int32_t, le<uint16_t>> pdpint32_t;
```

Features:
 * Fully symmetric implementation with no built-in preference for
   either big-endian or little-endian.

 * Completely standard C++.  No compiler intrinsics or similar are used.

 * Everything is constexpr, meaning that the generated types can be
   used for static data with the correct layout being applied at compile
   time.

 * Loads and stores which happen to match the host native endianness can
   be optimized into regular load and store instructions (use of clang++
   is recommended).

 * Extensible - other access patterns than big_endian and little_endian
   can be defined if desired.
