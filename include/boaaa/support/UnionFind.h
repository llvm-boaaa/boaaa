#ifndef BOAAA_SUPPORT_UNIONFIND_H
#define BOAAA_SUPPORT_UNIONFIND_H

#include <type_traits>
#include <inttypes.h>

namespace boaaa {
namespace support {

	template<class Type, class Comperator = std::less<Type>>
	class UnionFind {
	private:
		using UF = typename UnionFind<Type, Comperator>;
		Type& m_value;
		UnionFind<Type>& m_union;
		size_t m_num;

	public:
		UnionFind(Type& value) : m_value(value),  m_union(this), m_num(0) {}

		UF& concat(UF& other) {
			UF  p = parent();
			UF op = other.parent();
			//this and other are in same UnionFind, so no concat
			if (Comperator(p, op) && Comperator(op, p)) return p;

			if (p.size() < op.size()) 
			{
				p.m_union = op;
				op.m_num  += p.m_num;
				p.m_num   = 0;
			}
			else 
			{
				op.m_union = p;
				p.m_num    += op.m_num;
				op.m_num   = 0;
			}
			//if p is parent of UnionFind, it contains itself as parent, when not it contains the new head as parent.
			return p.m_union;
		}

		UF& parent() {
			if (m_union != this) m_union = m_union.parent();
			return m_union;
		}

		size_t size() {
			return parent().size();
		}
	};

	template<class Type, class Comperator = std::less<Type>>
	class UnionFindComparator {
	private:
		using UF = UnionFind<Type, Comperator>;
	public:
		bool operator() (const UF& lhs, const UF& rhs) const {
			return Comperator(lhs.parent(), rhs.parent());
		}
	};

}
}

#endif //!BOAAA_SUPPORT_UNIONFIND_H