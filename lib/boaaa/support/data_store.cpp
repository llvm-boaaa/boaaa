#include "boaaa/support/data_store.h"

const char* version_error_category::name() const noexcept
{
	return "version_parser_error";
}

std::string version_error_category::message(int ev) const
{
	switch (static_cast<version_error_code>(ev))
	{
	case version_error_code::IndexOutOfBounds:
		return "overhanded index is not recogniced as a parameter";
	}
}