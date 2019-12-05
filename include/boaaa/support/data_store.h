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
	struct _data_store {};

	template<typename ...Tail>
	struct data_store
	{
	public:
		data_store(Tail&... tail) : data(_data_store<Tail...>(tail...)) 
		{ 
			size = get_helper<Tail...>::count(tail...);
			bytes = get_helper<Tail...>::countBytes();
		};
		template<typename type, size_t idx>
		ErrorOr<type> get()
		{
			if (idx >= size)
				return ErrorOr<type>(version_error_category(version_error_code::IndexOutOfBounds));
			if (!get_helper<idx, _data_store<Tail...>>::checkType<type>())
				return ErrorOr<type>(version_error_category(version_error_code::TypeError));

			return ErrorOr<type>(static_cast<type>(data.get<idx>()));
		}

		uint64_t hash(uint64_t seed = 0)
		{
			uint8_t* bytep = (uint8_t*) malloc(bytes);
			get_helper<size, _data_store<Tail...>>::writeBytes(data, bytep);
			return xxhash(bytep, bytes, seed);
		}

	private:
		const size_t size;
		const size_t bytes;
		_data_store<Tail...> data;
	};

	template<typename T, typename... Tail>
	struct _data_store
	{
		_data_store(const T& first, const Tail&... tail) : first(first), tail(tail...) {}

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
			return data.first;
		}

		template<typename T2>
		static bool checkType(_data_store<T, Tail...>& data)
		{
			return std::is_same<T, T2>() && std::is_same<T2, T>();
		}

		static size_t count(_data_store<T, Tail...>& data)
		{
			return 1;
		}

		static size_t countBytes(_data_store<T, Tail...>& data)
		{
			return sizeof(T);
		}

		static void writeBytes(_data_store<T, Tail...>& data, uint8_t* mem)
		{
			std::memcpy(mem, &data.first, sizeof(T));
		}
	};

	template<size_t idx, typename T, typename... Tail>
	struct get_helper<idx, _data_store<T, Tail...>>
	{
		static auto get(_data_store<T, Tail...>& data)
		{
			return get_helper<idx - 1, _data_store<Tail...>>::get(data.rest);
		}

		static bool checkType(_data_store<T, Tail...>& data)
		{
			return get_helper<idx - 1, _data_store<Tail...>>::checkType(data.rest);
		}

		static size_t count(_data_store<T, Tail...>& data)
		{
			return 1 + get_helper<idx - 1, _data_store<Tail...>>::count(data.rest);
		}

		static size_t countBytes(_data_store<T, Tail...>& data)
		{
			return sizeof(T) + get_helper<idx - 1, _data_store<Tail...>>::countBytes(data.rest);
		}

		static void writeBytes(_data_store<T, Tail...>& data, uint8_t* mem)
		{
			std::memcpy(mem, &data.first, sizeof(T));
			get_helper<idx - 1, _data_store<Tail...>>::writeBytes(data.rest, mem + sizeof(T));
		}
	};
} //boaaa

#endif //!BOAAA_DATA_STORE_H