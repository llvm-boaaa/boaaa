#ifndef BOAAA_DATA_STORE_H
#define BOAAA_DATA_STORE_H

#include "LLVMErrorOr.h"
#include "boaaa/support/version_error.h"
#include "boaaa/support/xxhash.h"

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

		template<size_t idx>
		auto get()
		{
			return get_helper<idx, _data_store<T, Tail...>>::get(*this);
		}
	};

	template<typename T, typename... Tail>
	struct get_helper<0, _data_store<T, Tail...>>
	{
		static T get(_data_store<T, Tail...>& data) {
			return data.head;
		}

		static void set(_data_store<T, Tail...>& data, const T& value)
		{
			data.head = value;
		}

		template<typename T2>
		static bool checkType(_data_store<T, Tail...>& data)
		{
			return std::is_same<T, T2>() && std::is_same<T2, T>();
		}

		/*static size_t count(_data_store<T, Tail...>& data)
		{
			return 1;
		}*/

		static size_t countBytes(_data_store<T, Tail...>& data)
		{
			return sizeof(T);
		}

		static void writeBytes(_data_store<T, Tail...>& data, uint8_t* mem)
		{
			std::memcpy(mem, &data.head, sizeof(T));
		}
	};

	template<size_t idx, typename T, typename... Tail>
	struct get_helper<idx, _data_store<T, Tail...>>
	{
		static auto get(_data_store<T, Tail...>& data)
		{
			return get_helper<idx - 1, _data_store<Tail...>>::get(data.tail);
		}

		template<typename T2>
		static void set(_data_store<T, Tail...>& data, const T2& value)
		{
			get_helper<idx - 1, _data_store<Tail...>>::set<T2>(data.tail, value);
		}

		static void set(_data_store<T, Tail...>& data, const T& value)
		{
			get_helper<idx - 1, _data_store<Tail...>>::set<T>(data.tail, value);
		}

		template<typename T2>
		static bool checkType(_data_store<T, Tail...>& data)
		{
			return get_helper<idx - 1, _data_store<Tail...>>::checkType<T2>(data.tail);
		}

		/*static size_t count(_data_store<T, Tail...>& data)
		{
			return 1 + get_helper<idx - 1, _data_store<Tail...>>::count(data.tail);
		}*/

		static size_t countBytes(_data_store<T, Tail...>& data)
		{
			return sizeof(T) + get_helper<idx - 1, _data_store<Tail...>>::countBytes(data.tail);
		}

		static void writeBytes(_data_store<T, Tail...>& data, uint8_t* mem)
		{
			std::memcpy(mem, &data.head, sizeof(T));
			get_helper<idx - 1, _data_store<Tail...>>::writeBytes(data.tail, mem + sizeof(T));
		}
	};

	template<size_t n, typename ...Tail>
	struct data_store
	{
	public:
		using store = _data_store<Tail...>;

		data_store() = default;

		data_store(const Tail&... tail) : data(tail...)
		{
		};

		template<size_t idx, typename type>
		ErrorOr<type> get()
		{
			if (idx >= n - 1)
				return make_error_code(version_error_code::IndexOutOfBounds);
			if (!get_helper<idx, store>::template checkType<type>(data))
				return make_error_code(version_error_code::TypeError);

			return (type) data.get<idx>();
		}

		template<size_t idx, typename type>
		bool set(const type& value)
		{
			if (idx >= n - 1) return false;
			if (!get_helper<idx, store>::template checkType<type>(data))
				return false;

			get_helper<idx, store>::set<type>(data, value);
		}

		uint64_t hash(uint64_t seed = 0)
		{
			uint8_t* bytep = (uint8_t*)malloc(bytes);
			get_helper<n - 1, store>::writeBytes(data, bytep);
			return xxhash(bytep, bytes, seed);
		}

	private:
		const size_t bytes = sizeof...(Tail);
		store data;
	};

} //boaaa

#endif //!BOAAA_DATA_STORE_H