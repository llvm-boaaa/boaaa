#ifndef BOAAA_VERSION_ERROR_H
#define BOAAA_VERSION_ERROR_H

#include <system_error>

enum class version_error_code
{
	IndexOutOfBounds = 1,
	TypeError
};

namespace std
{
	template<>
	struct is_error_condition_enum<version_error_code> : true_type {};
}

std::error_code make_error_code(version_error_code);

namespace {

	struct version_error_category : std::error_category
	{
		const char* name() const noexcept override;
		std::string message(int ev) const override;
	};

	const version_error_category the_version_error_category{};
}

std::error_code make_error_code(version_error_code e)
{
	return { static_cast<int>(e), the_version_error_category };
}

#endif //!BOAAA_VERSION_ERROR_H