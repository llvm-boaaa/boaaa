#ifndef BOAAA_RAW_TYPE_H
#define BOAAA_RAW_TYPE_H

#include <type_traits>
namespace boaaa {

	template<typename K>
	struct rem
	{
	public:
		typedef std::remove_cv_t<std::remove_reference_t<std::remove_pointer_t<K>>> type;
	};

	template<typename K>
	using rem_t = typename rem<K>::type;

	template<bool B, class T>
	struct _raw_or_remove;

	template<bool B, typename T>
	using _raw_or_remove_t = typename _raw_or_remove<B, T>::type;

	template<bool B, class T>
	struct _raw_or_remove { typedef T type; };

	template<class T>
	struct _raw_or_remove<false, T> 
	{
		typedef _raw_or_remove_t<std::is_same<T, rem_t<T>>::value, rem_t<T>> type;
	};


	template<typename T>
	struct raw_type;

	template<typename T>
	using raw_type_t = typename raw_type<T>::type;

	template<typename T>
	struct raw_type
	{
		typedef _raw_or_remove_t<false, T> type;
	};
}

#define _raw_type(arg) raw_type_t<arg>
#define _raw_type_inst(arg) raw_type_t<decltype(arg)>

#endif //!BOAAA_RAW_TYPE_H