//this file is unused, but if static_assert gets support for constexpr, this would make it easy to integrate easy custom error messages.

#ifndef COMPILE_ERROR_H
#define COMPILE_ERROR_H

namespace {

	//from https://stackoverflow.com/questions/23999573/convert-a-number-to-a-string-literal-with-constexpr
	namespace detail
	{
		template<unsigned... digits>
		struct to_chars { static const char value[]; };

		template<unsigned... digits>
		const char to_chars<digits...>::value[] = { ('0' + digits)..., 0 };

		template<unsigned rem, unsigned... digits>
		struct explode : explode<rem / 10, rem % 10, digits...> {};

		template<unsigned... digits>
		struct explode<0, digits...> : to_chars<digits...> {};
	}

	template<unsigned num>
	struct num_to_string : detail::explode<num / 10, num % 10> {};

	namespace sstring {

#include <type_traits>
#include <utility>

		template<int... Is>
		using sequence = std::integer_sequence<int, Is...>;

		template<int N>
		using make_sequence = std::make_integer_sequence<int, N>;


		// https://akrzemi1.wordpress.com/2017/06/28/compile-time-string-concatenation/
# define X_ASSERT(CHECK) \
    ( (CHECK) ? void(0) : []{assert(!#CHECK);}() )

		struct RefImpl {};   // tag class
		struct ArrayImpl {}; // tag class

		template <int N, typename Impl = RefImpl>
		class sstring // main template never used
		{
			static_assert(N != N, "***");
		};

		template <int N>
		class sstring<N, RefImpl>
		{
			const char(&_lit)[N + 1];
			template<int, typename> friend class string;

		public:
			constexpr sstring(const char(&lit)[N + 1]) : _lit((X_ASSERT(lit[N] == '\0'), lit)) {}

			constexpr char operator[](int i) const
			{
				return X_ASSERT(i >= 0 && i < N), _lit[i];
			}

			constexpr std::size_t size() const {
				return N;
			}

			constexpr const char* c_str() const {
				return _lit;
			}

			constexpr operator const char* () const { return c_str(); }
		};

		template <int N>
		using string_literal = sstring<N, RefImpl>;

		template <int N_PLUS_1>
		constexpr string_literal<N_PLUS_1 - 1> literal(const char(&lit)[N_PLUS_1])
		{
			return string_literal<N_PLUS_1 - 1>(lit);
		}

		template <int N>
		class sstring<N, ArrayImpl>
		{
			static_assert (N >= 0, "string with negative length would be created");

			char _array[N + 1];
			template<int, typename> friend class string;

			template <int N1, int... PACK1, int... PACK2>
			constexpr sstring(const string_literal<N1>& s1,
				const string_literal<N - N1>& s2,
				sequence<PACK1...>,
				sequence<PACK2...>)
				: _array{ s1[PACK1]..., s2[PACK2]..., '\0' }
			{
			}

			template <int... Il, typename T>
			constexpr explicit sstring(T const& l,
				sequence<Il...>,
				int offset
			)
				: _array{ l[Il + offset]..., 0 }
			{
			}

		public:
			template <int N1, typename std::enable_if<(N1 <= N), bool>::type = true>
			constexpr sstring(const string_literal<N1>& s1,
				const string_literal<N - N1>& s2)
				// delegate to the other constructor
				: sstring{ s1, s2, make_sequence<N1>{},
								   make_sequence<N - N1>{} }
			{
			}

			template <int N1, typename TL, typename TR, typename std::enable_if<(N1 <= N), bool>::type = true>
			constexpr sstring(const sstring<N1, TL>& s1,
				const sstring<N - N1, TR>& s2)
			{
				for (int i = 0; i < N1; ++i)
					_array[i] = s1[i];

				for (int i = 0; i < N - N1; ++i)
					_array[N1 + i] = s2[i];

				_array[N] = '\0';
			}

			template<int N>
			constexpr sstring(int num) : sstring<N, ArrayImpl>(num_to_string<num>::value)
			{
			}

			constexpr sstring(string_literal<N> l) // converting
				: sstring(l, make_sequence<N>{}, 0)
			{
			}

			constexpr char operator[](int i) const
			{
				return X_ASSERT(i >= 0 && i < N), _array[i];
			}

			constexpr std::size_t size() const
			{
				return N;
			}

			constexpr const char* c_str() const {
				return _array;
			}
			constexpr operator const char* () const {
				return c_str();
			}
		};

		template <int N>
		using array_string = sstring<N, ArrayImpl>;

		template <int N1, int N2, typename TL, typename TR>
		constexpr sstring<N1 + N2, ArrayImpl> operator+(const sstring<N1, TL>& s1,
			const sstring<N2, TR>& s2)
		{
			return sstring<N1 + N2, ArrayImpl>(s1, s2);
		}

#undef X_ASSERT
	}

	template<typename T>
	constexpr auto parse(T t) { return sstring::array_string(t) };

	template<typename T, typename ...ARGS>
	constexpr auto parse(T t, ARGS... args) { return sstring::array_string(t) + parse<ARGS...>(args...); };

	/*template<>
	constexpr auto parse(int t) {
		return sstring::array_string(num_to_string<t>::value);
	}

	template<typename char*>
	constexpr auto parse(char* t) {
		return sstring::literal(t);
	}

	template<typename int, typename... ARGS>
	constexpr auto parse(int t, ARGS... args) {
		return sstring::array_string(num_to_string<t>::value) + parse<ARGS...>(args));
	}

	template<typename char*, typename... ARGS>
	constexpr auto parse(char* t, ARGS... args) {
		return sstring::literal(t) + parse<ARGS...>(args);
	} */

}

namespace compile_error { 
	 template<typename ...ARGS> 
	 constexpr int compile_error(ARGS... message) 
	 { 
		 const char* error = ""; // parse<ARGS...>(message...).c_str();
		 static_assert(true, error); 
		 return 0;
	 }
}

#define COMPILE_ERROR_FUNC(...) namespace { const int __error = compile_error::compile_error<(__VA_ARGS__)>
#define COMPILE_ERROC_PARAM(...) ((__VA_ARGS__)); }

#endif