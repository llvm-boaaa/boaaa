#include "boaaa/lv/50/SeaDsaWrapper.h"

#include "boaaa/support/raw_type.h"

#include <iostream>
#include <set>

#ifdef SEA_DSA
using namespace llvm;

namespace {

    struct NodeAndOffset {

        const sea_dsa::Node* Node;
        unsigned int Offset;

        NodeAndOffset(const sea_dsa::Node* node, unsigned int offset) : Node(node), Offset(offset) {}

        bool operator< (const NodeAndOffset& other) const {
            if (this->Node == other.Node) return this->Offset < other.Offset;
            return this->Node < other.Node;
        }

        static AliasResult alias(const NodeAndOffset& lhs, const MemoryLocation& lhsMem, const NodeAndOffset& rhs, const MemoryLocation& rhsMem) {
            unsigned int lhs_size = (lhsMem.Size == MemoryLocation::UnknownSize ? 0 : lhsMem.Size);
            unsigned int rhs_size = (rhsMem.Size == MemoryLocation::UnknownSize ? 0 : rhsMem.Size);
            
            auto check = [](unsigned int lowerStart, unsigned int higherStart, unsigned int sizeLower) -> AliasResult
            {
                if (lowerStart + sizeLower > higherStart) return PartialAlias;
                return MayAlias;
            };

            if (lhs.Node != rhs.Node) return NoAlias;
            if (lhs.Offset == rhs.Offset) return MustAlias;
            if (lhs.Offset < rhs.Offset)
                return check(lhs.Offset, rhs.Offset, lhs_size);
            else
                return check(rhs.Offset, lhs.Offset, rhs_size);
        }
    };

    struct NodeAndOffsetComparetor {

        NodeAndOffsetComparetor() = default;

        bool operator() (const NodeAndOffset& lhs, const NodeAndOffset& rhs) const {
            return lhs.Node < rhs.Node;
        }
    };

    //When Node has to Heads, the dominant AliasResult is MustAlias > PartialAlias > MayAlias > NoAlias
    // Example:
    //
    //          ------------    ------------    | N1 -> M1 && N2 -> M2 -> NoAlias
    //          |    N1    |    |    N2    |    | N1 -> M2 && N2 -> M2 -> MustAlias
    //          ------------    ------------    | ================================
    //           |        |      |              | Result should be Alias, same with May and Partial
    //           v        v      v
    //  ------------    ------------
    //  |    M1    |    |    M2    |
    //  ------------    ------------
    struct AliasMerger {
        bool accessed = false;
        AliasResult res = NoAlias;

        void operator+= (AliasResult _res) {
            accessed = true;
            
            if (res < _res) res = _res;
        }

        AliasResult get() {
            if (accessed) return res;
            return MayAlias; //if no alias is set we don't know anything about the current Location so MayAlias
        }
    };
}

SeaDsaResult::SeaDsaResult(sea_dsa::DsaAnalysis& Analysis) : analysis(Analysis), global(Analysis.getDsaAnalysis()) { }
SeaDsaResult::~SeaDsaResult() { }

//copyed from BasicAA
static const Function* getParent(const Value* V) {
    if (const Instruction* inst = dyn_cast<Instruction>(V)) {
        if (!inst->getParent())
            return nullptr;
        return inst->getParent()->getParent();
    }

    if (const Argument* arg = dyn_cast<Argument>(V))
        return arg->getParent();

    return nullptr;
}
//copied from BasicAA
static bool notDifferentParent(const Value* O1, const Value* O2) {

    const Function* F1 = getParent(O1);
    const Function* F2 = getParent(O2);

    return !F1 || !F2 || F1 == F2;
}

AliasResult SeaDsaResult::alias(const MemoryLocation& LocA, const MemoryLocation& LocB)
{
    //MemoryLocation.Size in Bytes

    using namespace sea_dsa;

    const llvm::Value* ValueA = LocA.Ptr;
    const llvm::Value* ValueB = LocB.Ptr;

    const Function* F = getParent(ValueA);

    {//only needed F2 for check
        if (!F) {
            F = getParent(ValueB);
            
            if (!F) return MayAlias; //no alias Information Found
        }
        else
        {
            const Function* F2 = getParent(ValueB);
            if (F2) {
                assert(F == F2 && "SeaDsa doesn't support interprocedural queries.");
                if (F != F2) return MayAlias; //return in nodebug-mode MayAlias. 
            }
        }
    }
    
    //F must be a valide Function
    assert(F);

    if (!global.hasGraph(*F)) return MayAlias;
    Graph& G = global.getGraph(*F);

    //if (!global.hasSummaryGraph(*F)) return MayAlias;
    //Graph& G = global.getSummaryGraph(*F);

    //when one Value is a global and not in the Graph of the Function it is not used in the Function, we could return NoAlias her, 
    //but we don't have any Information about why the Alias-check is called on this two Pointers so we say MayAlias and a other Analysis can return a better Result.
    if (!G.hasCell(*ValueA) || !G.hasCell(*ValueB)) return MayAlias;

    const Cell CA = G.getCell(*ValueA);
    const Cell CB = G.getCell(*ValueB);

    const Node* NA = CA.getNode();
    const Node* NB = CB.getNode();

    typedef std::set<NodeAndOffset, NodeAndOffsetComparetor> NodeAndOffsetSet;

    NodeAndOffsetSet SetA;
    NodeAndOffsetSet SetB;

    NodeAndOffsetSet recursion_break;

    std::function<bool(unsigned int, const Node*, NodeAndOffsetSet&)> dfs;
    dfs = [&dfs, &recursion_break](unsigned int offset, const Node* node, NodeAndOffsetSet& set) -> bool {
        bool recursion = false;
        if (recursion_break.find({ node, offset }) != recursion_break.end()) {
            return false;
        }
        if (node->isForwarding()) {
            Cell c = node->getForwardDest();
            recursion |= dfs(offset + c.getOffset(), c.getNode(), set);
        } else if (node->getNumLinks() > 0) {
            recursion_break.insert({ node, offset });
            for (auto& FieldCell : node->getLinks()) {
                Cell c = *FieldCell.second.get();
                Node *n = c.getNode();
                if (n && n != node) {
                    //recursion |= dfs(offset + FieldCell.first.getOffset() + c.getOffset(), n, set);
                    recursion |= dfs(offset + c.getOffset(), n, set);
                }
            }
        }
        else {
            set.insert({ node, offset });
            return true;
        }
        return recursion;
    };

    if (!dfs(CA.getOffset(), NA, SetA)) {
        SetA.insert({ NA, CA.getOffset() });
    }
    recursion_break.clear();
    if (!dfs(CB.getOffset(), NB, SetB)) {
        SetB.insert({ NB, CB.getOffset() });
    }
    recursion_break.clear();

    assert(SetA.size() > 0);
    assert(SetB.size() > 0);

    AliasMerger merger;

    for (const NodeAndOffset& n1 : SetA)
    {
        for (const NodeAndOffset& n2 : SetB)
        {
            merger += NodeAndOffset::alias(n1, LocA, n2, LocB);
        }
    }

    return merger.get();
}


bool SeaDsaWrapperPass::runOnModule(Module& M)
{
    Result.reset(new SeaDsaResult(getAnalysis<sea_dsa::DsaAnalysis>()));
    return false;
}

char SeaDsaWrapperPass::ID = 0;

INITIALIZE_PASS(SeaDsaWrapperPass, "sea-dsa-wrapper", "SeaDsa Wrapper that implements llvm standart analysis interface", false, true)                                                        

SeaDsaWrapperPass::SeaDsaWrapperPass() : ModulePass(ID) {
    initializeSeaDsaWrapperPassPass(*PassRegistry::getPassRegistry());
}

SeaDsaWrapperPass::~SeaDsaWrapperPass() { }

ModulePass* createSeaDsaWrapperPass() {
    return new SeaDsaWrapperPass();
}

void SeaDsaWrapperPass::getAnalysisUsage(AnalysisUsage& AU) const
{
    AU.addRequired<sea_dsa::DsaAnalysis>();
    AU.setPreservesAll();
}

#endif //!SEA_DSA