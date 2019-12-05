#ifndef BOAAA_VERSION_PARSER_H
#define BOAAA_VERSION_PARSER_H

#include "boaaa/support/data_store.h"
#include <system_error>
#include <memory>

namespace boaaa
{
	template<typename... Tail>
	class VersionParseManager;

	template<typename Type, typename...Tail>
	class VersionParserImpl;

	template<typename Type, typename...Tail>
	using container 
		= VersionParseManager<Tail...>::container;
	template<typename Type, typename...Tail>
	using parseFunc 
		= VersionParseManager<Tail...>::parseFuncType<Type>;
	template<typename Type, typename...Tail>
	using generateFunc 
		= VersionParseManager<Tail...>::generateFuncType<Type>;

	template<typename Type, typename...Tail>
	class VersionParser
	{
	public:

		VersionParser(std::unique_ptr<VersionParserImpl<Type, Tail...>> imp) : imp(imp) {};

		container parse(const Type& data)
		{
			return imp->parse(data);
		}

		Type generate(const container& data) = 0;
	private:
		std::unique_ptr<VersionParserImpl<Type, Tail...>> imp;
	};

	template<typename Type, typename...Tail>
	class VersionParserImpl
	{
	public:

		virtual container parse(const Type& data) = 0;
		virtual Type generate(const container& data) = 0;

	private:
	};

	template<typename... Tail>
	class VersionParseManager
	{
	public:
		using container = data_store<Tail...>;
		template<typename Type>
		typedef container (*parseFunc_t(Type));
		template<typename Type>
		typedef Type (*generateFunc_t(container));

		template<typename Type>
		using parseFuncType = parseFunc_t<Type>;
		template<typename Type>
		using generateFuncType = generateFunc_t<Type>;

		void registerVP(VersionParser p)
	};

}

#endif // !BOAAA_VERSION_PARSER_H
