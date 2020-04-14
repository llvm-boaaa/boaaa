#ifndef BOAAA_SUPPORT_UNIONFIND_H
#define BOAAA_SUPPORT_UNIONFIND_H

#include <type_traits>
#include <inttypes.h>

namespace boaaa {
namespace support {

	template<class Type, class TypeComperator = std::less<Type>>
	class UnionFind {
	private:
		using UF = UnionFind<Type, TypeComperator>;

		const Type m_value;
		UF* m_union;
		size_t m_num;
		TypeComperator m_comp;

		void parent(UF* value) { m_union = value; }

	public:
		UnionFind(const Type value, TypeComperator comp = TypeComperator()) : m_value(value), m_num(1), m_comp(comp) { m_union = this; }
		~UnionFind() {}

		UnionFind(const UnionFind<Type, TypeComperator>& obj) = delete;

		UF* concat(UF* other) {
			UF*  p = parent();
			UF* op = other->parent();
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
		
		UF* parent() {
			if (m_union != this) m_union = m_union->parent();
			return m_union;
		}

		const UF* parent() const {
			if (m_union != this) return m_union->parent();
			return m_union;
		}

		size_t size() {
			if (m_union == this) return m_num;
			return parent()->size();
		}

		const Type value() {
			return m_value;
		}

		const Type value() const {
			return m_value;
		}

		operator const Type() {
			return value();
		}

		operator const Type() const {
			return value();
		}
	};

	template<class Type, class TypeComperator = std::less<Type>>
	struct UnionFindComparator {
	private:
		using UF = UnionFind<Type, TypeComperator>;
	public:

		constexpr bool operator ()(const UF& lhs, const UF& rhs) const {
			static TypeComperator comp = TypeComperator();
			return comp(lhs.parent()->value(), rhs.parent()->value());
		}
	};

}
}

#endif //!BOAAA_SUPPORT_UNIONFIND_H