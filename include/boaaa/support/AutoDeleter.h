#ifndef BOAAA_SUPPORT_AUTODELETER_H
#define BOAAA_SUPPORT_AUTODELETER_H

#include <functional>

namespace boaaa {
namespace support {
	
	template<typename Type>
	class AutoDeleter {
	private:
		using del = std::function<void(Type&)>;
	
		Type T;
		del f;

	public:
		AutoDeleter(Type& value, del func) : T(value), f(func) {}
		~AutoDeleter() { f(T); }

	};

}
}

#endif //!BOAAA_SUPPORT_AUTODELETER_H 