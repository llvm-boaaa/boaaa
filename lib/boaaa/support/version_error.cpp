#include "boaaa/support/version_error.h"

namespace boaaa
{

	namespace {

		class version_error_category : public std::error_category
		{
		public:
			const char* name() const noexcept
			{
				return "version_parser_error";
			}

			std::string message(int ev) const
			{
				switch (static_cast<version_error_code>(ev))
				{
				case version_error_code::IndexOutOfBounds:
					return "overhanded index is not recogniced as a parameter";
				case version_error_code::TypeError:
					return "overhanded type is not the same as stored";
				default:
					return "unknown error";
				}
			}
		};

		static version_error_category the_version_error_category;
	}

	const std::error_category& version_error_category()
	{
		return the_version_error_category;
	}

	std::error_condition make_error_condition(version_error_code e) {
		return std::error_condition(static_cast<int>(e), the_version_error_category);
	}
} //std