#ifndef BOAAA_SUPPORT_UNIONFINDMAP_H
#define BOAAA_SUPPORT_UNIONFINDMAP_H

#include "UnionFind.h"

#include <functional>
#include <list>
#include <map>
#include <utility>

namespace boaaa {
namespace support {
	
#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif

	template<class Key, class KeyComperator = std::less<Key>>
	class UnionFindMap {
	public:
		using _Key = typename std::conditional_t<std::is_pointer<Key>::value || std::is_integral<Key>::value, Key, Key&>;
		using UF = UnionFind<Key, KeyComperator>;
		using UFM = typename std::map<Key, UF*>;
		using UFL = typename std::list<UF*>;
		using iterator = typename UFM::iterator;
		using const_iterator = typename UFM::const_iterator;
		using const_reverse_iterator = typename UFM::const_reverse_iterator;
		using head_iterator = typename UFL::const_iterator;

		//don`t use names defined above because of autocomplete of vs
	private:
		UFM m_ufs;
		UFL m_heads;
		UnionFindComparator<Key, KeyComperator> m_uf_comp;

		UF* _add(UF* uf, bool ishead = true) {
			iterator it = m_ufs.find(uf->value());
			if (it != m_ufs.end())
			{
				delete uf;
				return it->second;
			}
			m_ufs.insert(std::make_pair(uf->value(), uf)); //UF contains key inside it data and asigns it when it is needed as key-type
			if (ishead)
				m_heads.push_back(uf);
			return uf;
		}
	public:

		UnionFindMap() : m_ufs(), m_heads(), m_uf_comp() {}

		~UnionFindMap()
		{
			m_heads.clear();
			for (auto it = m_ufs.begin(), end = m_ufs.end(); it != end; ++it)
				delete it->second;
			m_ufs.clear();
		}

		void add(_Key value) {
			_add(new UF(value));
		}

		void concat(_Key lhs, _Key rhs) {

			auto init_or_asign = [=](_Key val) -> UF* {
				iterator f_hs = m_ufs.find(val);
				if (f_hs != end()) { return f_hs->second->parent(); }
				return _add(new UF(val), false);
			};

			auto checkInside = [=](UF* uf) -> bool {
				for (auto it = m_heads.begin(), end = m_heads.end(); it != end; ++it)
				{
					if ((*it)->value() == uf->value())
						return true;
				}
				return false;
			};

			UF* _lhs = init_or_asign(lhs);
			UF* _rhs = init_or_asign(rhs);
			UF* _res = _lhs->concat(_rhs);
			if (!m_uf_comp(*_res, *_lhs) && !m_uf_comp(*_lhs, *_res))
			{
				m_heads.remove(_rhs);
				if (!checkInside(_lhs))
					m_heads.push_front(_lhs);
			} 
			else if (!m_uf_comp(*_res, *_rhs) && !m_uf_comp(*_rhs, *_res))
			{
				m_heads.remove(_lhs);
				if (!checkInside(_rhs))
					m_heads.push_front(_rhs);
			}
			else {
				if (!checkInside(_res))
					m_heads.push_front(_res);
			}
		}

		size_t size() {
			return m_ufs.size();
		}

		size_t headssize() {
			return m_heads.size();
		}

		//iterator forwarding
		_NODISCARD head_iterator heads() {
			return m_heads.begin();
		}

		_NODISCARD head_iterator headsend() {
			return m_heads.end();
		}

		_NODISCARD bool contains(const _Key value) {
			return find(value) != end();
		}

		_NODISCARD const_iterator find(const _Key value) {
			return this->m_ufs.find(value);
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