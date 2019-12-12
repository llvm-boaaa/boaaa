#ifndef BOAAA_VERSION_ERROR_H
#define BOAAA_VERSION_ERROR_H

#include <system_error>

namespace boaaa
{
	enum class version_error_code
	{
		success = 0,
		IndexOutOfBounds,
		TypeError
	};

	const std::error_category& version_error_category();
	std::error_condition make_error_condition(version_error_code e);

	inline std::error_code make_error_code(version_error_code E) {
		return std::error_code(static_cast<int>(E), version_error_category());
	}
}
namespace std
{
	template<>
	struct std::is_error_condition_enum<boaaa::version_error_code> : true_type {};
}


#endif //!BOAAA_VERSION_ERROR_H