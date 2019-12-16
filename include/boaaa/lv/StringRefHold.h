#ifndef BOAAA_STRING_REF_HOLDER_REF_H
#define BOAAA_STRING_REF_HOLDER_REF_H

#ifdef LLVM_VERSION_40
#include "llvm/ADT/StringRef.h"
#endif //!LLVM_VERSION_40
#ifdef LLVM_VERSION_50
#include "llvm/ADT/StringRef.h"
#endif //!LLVM_VERSION_50
#ifdef LLVM_VERSION_90
#include "llvm/ADT/StringRef.h"
#endif //!LLVM_VERSION_90

namespace boaaa
{
	namespace {
		// Constexpr version of std::strlen.
		static constexpr size_t strLen(const char* Str) {
#if __cplusplus > 201402L
			return std::char_traits<char>::length(Str);
#elif __has_builtin(__builtin_strlen) || defined(__GNUC__)
			return __builtin_strlen(Str);
#else
			const char* Begin = Str;
			while (*Str != '\0')
				++Str;
			return Str - Begin;
#endif
		}
	}

	char* copyMemoryifnewPointer(const char* Str, size_t length) {
		static const char* tmp;
		static char* mem;
		if (!Str) {
			tmp = nullptr;
			mem = nullptr;
			return nullptr;
		}
		if (tmp == Str)
			return mem;
		tmp = Str;
		mem = new char[length];
		memcpy(mem, Str, length);
		return mem;
	}

	char* copyMemoryifnewPointer(const char* Str)
	{
		return copyMemoryifnewPointer(Str, Str ? strlen(Str) : 0);
	}

	class StringRefHold : public llvm::StringRef
	{
	public:
		/*implicit*/ StringRefHold() = default;

		/// Disable conversion from nullptr.  This prevents things like
		/// if (S == nullptr)
		StringRefHold(std::nullptr_t) = delete;

		/// Construct a string ref from a cstring.
		/*implicit*/ StringRefHold(const char* Str) : llvm::StringRef(copyMemoryifnewPointer(Str), Str ? strLen(Str) : 0),
			Data(copyMemoryifnewPointer(Str)), Length(Str ? strLen(Str) : 0) 
			{
			copyMemoryifnewPointer(nullptr); //resets tmp so can create multiple 
			}

		/// Construct a string ref from a pointer and length.
		/*implicit*/ StringRefHold(const char* data, size_t length) : StringRef(copyMemoryifnewPointer(data, length), length),
			Data(copyMemoryifnewPointer(data, length)), Length(length)
			{
			copyMemoryifnewPointer(nullptr); //resets tmp so can create multiple 
			}

		/// Construct a string ref from an std::string.
		/*implicit*/ StringRefHold(const std::string & Str) : llvm::StringRef(copyMemoryifnewPointer(Str.data(), Str.length()), Str.length()), 
			Data(copyMemoryifnewPointer(Str.data(), Str.length())), Length(Str.length()) 
		{
			
		}

		~StringRefHold() 
		{
			delete Data;
		}

	private:
		char* Data = nullptr;
		size_t Length;
	};

}

#endif // !BOAAA_STRING_REF_HOLDER_REF_H