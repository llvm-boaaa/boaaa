#include "boaaa/lv/50/SeaDsaWrapper.h"

#include "boaaa/support/raw_type.h"

#include <iostream>
#include <vector>

#ifdef SEA_DSA
using namespace llvm;

namespace {

    struct NodeAndOffset {

        const sea_dsa::Node* Node;
        unsigned int Offset;

        NodeAndOffset(const sea_dsa::Node* node, unsigned int offset) : Node(node), Offset(offset) {}

        bool operator< (const NodeAndOffset& other) {
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

    assert(notDifferentParent(ValueA, ValueB) &&
        "SeaDsa doesn't support interprocedural queries.");

    const Function* F = getParent(ValueA);

    {//only needed F2 for check
        const Function* F2 = getParent(ValueB);
        //if couldn't find Parent then don't know how to get Alias Informations, so returning may alias
        if (!F || !F2 || F != F2) return MayAlias;
    }
    
    if (!global.hasGraph(*F)) return MayAlias;
    Graph& G = global.getGraph(*F);

    //if (!global.hasSummaryGraph(*F)) return MayAlias;
    //Graph& G = global.getSummaryGraph(*F);

    const Cell CA = G.getCell(*ValueA);
    const Cell CB = G.getCell(*ValueB);

    const Node* NA = CA.getNode();
    const Node* NB = CB.getNode();

    std::vector<NodeAndOffset> SetA;
    std::vector<NodeAndOffset> SetB;

    std::function<void(unsigned int, const Node*, std::vector<NodeAndOffset>&)> dfs;
    dfs = [&dfs](unsigned int offset, const Node* node, std::vector<NodeAndOffset>& set) {
        assert(node->getNumLinks() > 0);
        for (auto& link : node->getLinks())
        {
            const Node* _node = link.second.get()->getNode();
            unsigned int new_offset = offset + link.second.get()->getOffset() + link.first.getOffset();

            if (_node->getNumLinks() > 0)
                dfs(new_offset, _node, set);
            else
                set.push_back({ _node, new_offset });
        }
    };


    if (NA->getNumLinks() > 0) {
        dfs(CA.getOffset(), NA, SetA);
    } 
    else {
        SetA.push_back({ NA, CA.getOffset() });
    }

    if (NB->getNumLinks() > 0) {
        dfs(CB.getOffset(), NB, SetB);
    }
    else {
        SetB.push_back({ NB, CB.getOffset() });
    }

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