#ifndef BOAAA_SUPPORT_ENABLE_IF_ELSE_H
#define BOAAA_SUPPORT_ENABLE_IF_ELSE_H

#include <type_traits>

namespace boaaa
{
	template<bool B, class T, class F>
	struct enable_if_else { typedef F type; };

	template<class T, class F>
	struct enable_if_else<true, T, F> { typedef T type; };
}


#endif