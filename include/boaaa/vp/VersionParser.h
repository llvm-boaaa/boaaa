#ifndef BOAAA_VP_VERSION_PARSER_H
#define BOAAA_VP_VERSION_PARSER_H

#include "boaaa/support/data_store.h"

#include <cstdlib>
#include <system_error>
#include <memory>
#include <unordered_map>

namespace boaaa
{

	template<typename... Tail>
	class VersionParseManager
	{
	public:
		typedef data_store<Tail...> container;

		VersionParseManager() {
			m_store = new std::unordered_map<uint64_t, container>();
		}

		uint64_t registerContainer(container&& data)
		{
			uint64_t hash = data.hash();
			(*m_store).insert({ hash, data });
			return hash;
		}

		container getContainer(const uint64_t hash)
		{
			return (*m_store)[hash];
		}

	private:
		std::unordered_map<uint64_t, container>* m_store;
	};

	template<typename Type, typename...Tail>
	class VersionParser
	{
	public:
		using VPM = VersionParseManager<Tail...>;
		using container = typename VPM::container;

		VersionParser() { m_manager = nullptr; }
		~VersionParser() = default;

		virtual container parse(Type& data) = 0;
		virtual Type generate(container&& data) = 0;

		Type parseRegistered(uint64_t hash) 
		{
			return generate(m_manager->getContainer(hash));
		}

		uint64_t registerData(Type data)
		{
			if (!m_manager) return 0;
			return m_manager->registerContainer(parse(data));
		}

		uint64_t registerContainer(container& data)
		{
			if (!m_manager) return 0;
			return m_manager->registerContainer(data);
		}

		void registerVPM(VPM* manager) {
			if (!manager) return;
			m_manager = manager;
		}

	private:
		VPM* m_manager;
	};
	
	template<typename Type, typename Store, typename ...Tail>
	class StoreVersionParser
	{
	public:
		using VPM = VersionParseManager<Tail...>;
		using container = typename VPM::container;
		typedef std::unique_ptr<Store> store_t;

		StoreVersionParser() { m_manager = nullptr; }
		~StoreVersionParser() = default;

		virtual container parse(Type& data) = 0;
		virtual Type generate(container&& data, store_t& store) = 0;

		store_t generateStorage() { return std::make_unique<Store>(); };
		Type parseRegistered(uint64_t hash, store_t& store)
		{
			return generate(m_manager->getContainer(hash), store);
		}

		uint64_t registerData(Type data)
		{
			if (!m_manager) return 0;
			return m_manager->registerContainer(parse(data));
		}

		uint64_t registerContainer(container& data)
		{
			if (!m_manager) return 0;
			return m_manager->registerContainer(data);
		}

		void registerVPM(VPM* manager) {
			if (!manager) return;
			m_manager = manager;
		}

	private:
		VPM* m_manager;
	};
}

#endif // !BOAAA_VP_VERSION_PARSER_H
