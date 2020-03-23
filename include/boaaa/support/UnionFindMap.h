#ifndef BOAAA_SUPPORT_UNIONFINDMAP_H
#define BOAAA_SUPPORT_UNIONFINDMAP_H

#include "UnionFind.h"

#include <functional>
#include <list>
#include <map>

namespace boaaa {
namespace support {
	
#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif

	template<class KeyReference, class Key>
	struct KeyHelper {
		using  _Key          = typename std::conditional_t<std::is_pointer<Key>::value, Key, Key&>;
		typedef KeyReference key_component;
		typedef Key			 key_value;

		typedef std::function<KeyReference(const _Key)> generateKey;

		KeyHelper(generateKey gen = 
			//assuming Key == KeyReference it is well defined, otherwise assuming Key contains operator KeyReference operator (Key value)
			[](const _Key value) -> KeyReference { return value; }) : m_gen(gen) {}

		KeyReference gen(const _Key key) {
			return m_gen(key);
		}
	private:
		generateKey m_gen;
	};

	template<class Key, class KeyReference = Key, class KeyComperator = std::less<Key>, class KeyReferenceComperator = std::less<KeyReference>>
	class UnionFindMap {
	private:
		using _Key						= typename std::conditional_t<std::is_pointer<Key>::value, Key, Key&>;
		using UF						= typename UnionFind<Key, KeyComperator>;
		using UFM						= typename std::map<KeyReference, UF>;
		using UFL						= typename std::list<UF>;
		using iterator					= typename UFM::iterator;
		using const_iterator			= typename UFM::const_iterator;
		using const_reverse_iterator	= typename UFM::const_reverse_iterator;
		using head_iterator				= typename UFL::const_iterator;

		//don`t use names defined above because of autocomplete of vs

		UFM m_ufs;
		UFL m_heads;
		KeyHelper<KeyReference, Key> m_helper;

		constexpr auto hash(const _Key value) -> KeyReference {
			return m_helper.gen(value);
		}

		void _add(UF value, bool ishead = true) {
			if (contains(value)) return;
			m_ufs.insert({hash( value.value()), value }); //UF contains key inside it data and asigns it when it is needed as key-type
			if (ishead)
				m_heads.push_back(value);
		}
	public:

		UnionFindMap(KeyHelper<KeyReference, Key> _KeyHelper = KeyHelper<KeyReference, Key>()) : m_ufs(), m_heads(), m_helper(_KeyHelper) {}

		void add(_Key value) {
			_add(UF(value));
		}

		void concat(_Key lhs, _Key rhs) {

			auto init_or_asign = [=](_Key val, bool& hs) -> UF& {
				iterator f_hs = m_ufs.find(hash(val));
				if (f_hs != end()) { hs = true; return f_hs->second; }
				UF uf = UF(val);
				_add(uf, false);
				return uf;
			};

			bool lhs_in = false;
			bool rhs_in = false;
			UF _lhs = init_or_asign(lhs, lhs_in);
			UF _rhs = init_or_asign(rhs, rhs_in);
			if (&_lhs == &_rhs) return;
			UF* _res = _lhs.concat(_rhs);
			if (!contains(*_res)) _add(*_res); //assures _res is inside the map, !important use _add, because not construct _res again.
			if (_res != &_lhs) m_heads.remove_if([=](const UF& other) { return &_lhs == &other; }); //compare pointers
			if (_res != &_rhs) m_heads.remove_if([=](const UF& other) { return &_rhs == &other; });
			if (!lhs_in) _add(_lhs, false); //add lhs or rhs to list if not onside the map
			if (!rhs_in) _add(_rhs, false);
		}

		size_t size() {
			return m_ufs.size();
		}

		size_t headssize() {
			return m_heads.size();
		}

		//iterator forwarding
		_NODISCARD head_iterator heads() const {
			return m_heads.begin();
		}

		_NODISCARD head_iterator headsend() const {
			return m_heads.end();
		}

		_NODISCARD bool contains(const _Key value) {
			return find(value) != end();
		}

		_NODISCARD const_iterator find(const _Key value) {
			return this->m_ufs.find(hash(value));
		}

		_NODISCARD const_iterator begin() {
			return m_ufs.begin();
		}

		_NODISCARD const_iterator end() {
			return m_ufs.end();
		}

		_NODISCARD const_reverse_iterator rbegin() {
			return m_ufs.rbegin();
		}

		_NODISCARD const_reverse_iterator rend() {
			return m_ufs.rend();
		}

		_NODISCARD const_iterator cbegin() {
			return begin();
		}

		_NODISCARD const_iterator cend() {
			return end();
		}

		_NODISCARD const_reverse_iterator crbegin() {
			return rbegin();
		}

		_NODISCARD const_reverse_iterator crend() {
			return rend();
		}
	};

}
}

#endif //!BOAAA_SUPPORT_UNIONFINDMAP_H