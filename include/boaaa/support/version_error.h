#ifndef BOAAA_VERSION_ERROR_H
#define BOAAA_VERSION_ERROR_H

#include <system_error>

namespace boaaa {

	enum class version_error_code
	{
		IndexOutOfBounds = 1,
		TypeError
	};
}
namespace std
{
	template<>
	struct std::is_error_condition_enum<boaaa::version_error_code> : true_type {};
}

namespace boaaa
{
	class version_error_category_t : public std::error_category
	{
	public:
		const char* boaaa::version_error_category_t::name() const noexcept override
		{
			return "version_parser_error";
		}

		std::string boaaa::version_error_category_t::message(int ev) const override
		{
			switch (static_cast<version_error_code>(ev))
			{
			case version_error_code::IndexOutOfBounds:
				return "overhanded index is not recogniced as a parameter";
			case version_error_code::TypeError:
				return "overhanded type is not the same as stored";
			}
		}
	} version_error_category;

	inline std::error_condition make_error_condition(version_error_code e) {
		return std::error_condition(static_cast<int>(e), version_error_category);
	}
}

#endif //!BOAAA_VERSION_ERROR_H