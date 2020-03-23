#ifndef BOAAA_SUPPORT_UNIONFIND_H
#define BOAAA_SUPPORT_UNIONFIND_H

#include <type_traits>
#include <inttypes.h>

namespace boaaa {
namespace support {

	template<class Type, class TypeComperator = std::less<Type>>
	class UnionFind {
	private:
		using UF = typename UnionFind<Type, TypeComperator>;

		const Type& m_value;
		UF* m_union;
		size_t m_num;
		TypeComperator m_comp;
	public:
		UnionFind(const Type& value, TypeComperator comp = TypeComperator()) : m_value(value), m_num(1), m_comp(comp) { m_union = this; }
		~UnionFind() {}

		UF* concat(UF& other) {
			UF*  p = parent();
			UF* op = other.parent();
			//this and other are the same UnionFind, so no concat
			if (p == op) return p;

			if (p->size() < op->size()) 
			{
				p->parent(op);
				op->m_num  += p->m_num;
				p->m_num   = 0;
			}
			else 
			{
				op->parent(p);
				p->m_num    += op->m_num;
				op->m_num   = 0;
			}
			//if p is parent of UnionFind, it contains itself as parent, when not it contains the new head as parent.
			return p->m_union;
		}

		void parent(UF* value) {
			m_union = value;
		}

		UF* parent() {
			if (m_union != this) m_union = m_union->parent();
			return m_union;
		}

		size_t size() {
			if (m_union == this) return m_num;
			return parent()->size();
		}

		const Type& value() {
			return m_value;
		}

		operator const Type&() {
			return value();
		}
	};

	template<class Type, class TypeComperator = std::less<Type>>
	class UnionFindComparator {
	private:
		using UF = UnionFind<Type, TypeComperator>;
		TypeComperator m_comp;
	public:
		UnionFindComparator(TypeComperator comp = TypeComperator()) : m_comp(comp) {}

		bool operator() (UF& lhs, UF& rhs) const {
			return m_comp(lhs.parent(), rhs.parent());
		}
	};

}
}

#endif //!BOAAA_SUPPORT_UNIONFIND_H