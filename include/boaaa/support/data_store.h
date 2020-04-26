#ifndef BOAAA_DATA_STORE_H
#define BOAAA_DATA_STORE_H

#include "boaaa/support/LLVMErrorOr.h"
#include "boaaa/support/version_error.h"
#include "boaaa/support/xxhash.h"

#include <cassert>
#include <cstring>

namespace boaaa 
{
	//https://riptutorial.com/cplusplus/example/19276/variadic-template-data-structures
	template<size_t idx, typename T>
	struct get_helper;

	enum class VersionError;
	struct version_error_category;

	template<typename ...Tail>
	struct _data_store
	{
	};

	template<typename T, typename... Tail>
	struct _data_store<T, Tail...>
	{
		_data_store() = default;

		_data_store(const T& head, const Tail&... tail) : head(head), tail(tail...) {}

		T head;
		_data_store<Tail...> tail;

	};

	template<typename T, typename... Tail>
	struct get_helper<0, _data_store<T, Tail...>>
	{
		using type = T;

		template<typename T2>
		using TOrError = typename std::conditional_t<std::is_same<T, T2>::value, T, void>;

		template<typename T2>
		static TOrError<T2> get(_data_store<TOrError<T2>, Tail...>& data) {
			return data.head;
		}

		template<typename T2>
		static void set(_data_store<TOrError<T2>, Tail...>& data, const TOrError<T2>& value)
		{
			data.head = value;
		}

		template<typename T2>
		static bool checkType(_data_store<T, Tail...>& data)
		{
			return std::is_same<T, T2>::value;
		}

		static size_t countBytes(_data_store<T, Tail...>& data)
		{
			return sizeof data.head;
		}

		static void writeBytes(_data_store<T, Tail...>& data, uint8_t* mem)
		{
			std::memcpy(mem, &data.head, sizeof data.head);
		}
	};

	template<size_t idx, typename T, typename... Tail>
	struct get_helper<idx, _data_store<T, Tail...>>
	{
		using type = typename get_helper<idx - 1, _data_store<Tail...>>::type;

		template<typename T2>
		using TOrT2 = typename std::conditional_t<std::is_same<T, T2>::value, T, T2>;

		template<typename T2>
		static TOrT2<T2> get(_data_store<T, Tail...>& data)
		{
			return get_helper<idx - 1, _data_store<Tail...>>::template get<TOrT2<T2>>(data.tail);
		}
		/*
		template<typename T>
		static T get(_data_store<T, Tail...>& data)
		{
			return get_helper<idx - 1, _data_store<Tail...>>::template get<T>(data.tail);
		}

		template<typename T2>
		static T2 get(_data_store<T, Tail...>& data)
		{
			return get_helper<idx - 1, _data_store<Tail...>>::template get<T2>(data.tail);
		}
		*/
		template<typename T2>
		static void set(_data_store<TOrT2<T2>, Tail...>& data, const TOrT2<T2>& value)
		{
			get_helper<idx - 1, _data_store<Tail...>>::template set<TOrT2<T2>>(data.tail, value);
		}
		/*
		template<typename T>
		static void set(_data_store<T, Tail...>& data, const T& value)
		{
			get_helper<idx - 1, _data_store<Tail...>>::template set<T>(data.tail, value);
		}

		template<typename T2>
		static void set(_data_store<T, Tail...>& data, const T2& value)
		{
			get_helper<idx - 1, _data_store<Tail...>>::template set<T2>(data.tail, value);
		}
		*/
		template<typename T2>
		static bool checkType(_data_store<T, Tail...>& data)
		{
			return get_helper<idx - 1, _data_store<Tail...>>::template checkType<T2>(data.tail);
		}

		static size_t countBytes(_data_store<T, Tail...>& data)
		{
			return (sizeof data.head) + get_helper<idx - 1, _data_store<Tail...>>::countBytes(data.tail);
		}

		static void writeBytes(_data_store<T, Tail...>& data, uint8_t* mem)
		{
			std::memcpy(mem, &data.head, sizeof data.head);
			get_helper<idx - 1, _data_store<Tail...>>::writeBytes(data.tail, mem + sizeof data.head);
		}
	};


#define __n sizeof...(Tail)

	template<typename ...Tail>
	struct data_store
	{
	public:
		using store = _data_store<Tail...>;
		template<size_t N>
		using type = typename get_helper<N, store>::type;

		data_store() = default;

		data_store(const Tail&... tail) : data(tail...)
		{
		};

		template<size_t idx>
		type<idx> get()
		{
			assert((idx < n));
			assert((get_helper<idx, store>::template checkType<type<idx>>(data)));

			return get_helper<idx, store>::template get<type<idx>>(data);
		}

		template<size_t idx>
		void set(const type<idx>& value)
		{
			assert((idx < n));
			assert((get_helper<idx, store>::template checkType<type<idx>>(data)));

			get_helper<idx, store>::template set<type<idx>>(data, value);
		}

		uint64_t hash(uint64_t seed = 0)
		{
			size_t const size = get_helper<__n - 1, _data_store<Tail...>>::countBytes(data);
			uint8_t* bytep = new uint8_t[size + 1];
			bytep[size] = 0;
			get_helper<__n - 1, store>::writeBytes(data, bytep);
			return xxhash(bytep, size, seed);
		}

		size_t size()
		{
			return n;
		}

	private:
		const size_t n = __n;
		store data;
	};

} //boaaa

#undef __n

#endif //!BOAAA_DATA_STORE_H
