/**
 * ICFG.cpp
 * @author kisslune 
 */

#include "CFGA.h"

using namespace SVF;
using namespace llvm;
using namespace std;

int main(int argc, char **argv)
{
    auto moduleNameVec =
            OptionBase::parseOptions(argc, argv, "Whole Program Points-to Analysis",
                                     "[options] <input-bitcode...>");

    LLVMModuleSet::buildSVFModule(moduleNameVec);

    SVFIRBuilder builder;
    auto pag = builder.build();
    auto icfg = pag->getICFG();

    CFGAnalysis analyzer = CFGAnalysis(icfg);

    // TODO: complete the following method: 'CFGAnalysis::analyze'
    analyzer.analyze(icfg);

    analyzer.dumpPaths();
    LLVMModuleSet::releaseLLVMModuleSet();
    return 0;
}


void CFGAnalysis::analyze(SVF::ICFG *icfg)
{
    // Sources and sinks are specified when an analyzer is instantiated.
    for (auto src : sources)
        for (auto snk : sinks)
        {
            // TODO: DFS the graph, starting from src and detecting the paths ending at snk.
            // Use the class method 'recordPath' (already defined) to record the path you detected.
            //@{
            vector<unsigned> path;
            // Only prune nodes already on this path in the same calling context.
            set<pair<unsigned, stack<unsigned>>> visited;

            auto dfs = [&](auto&& self, const ICFGNode* node) -> void
            {
                auto state = make_pair(node->getId(), callStack);
                if (!visited.insert(state).second)
                    return;

                path.push_back(node->getId());
                if (node->getId() == snk)
                {
                    recordPath(path);
                }
                else
                {
                    for (const auto* edge : node->getOutEdges())
                    {
                        if (const auto* call = dyn_cast<CallCFGEdge>(edge))
                        {
                            unsigned callSite = call->getCallSite()->getId();
                            // Do not expand the same recursive call site again.
                            auto activeCalls = callStack;
                            while (!activeCalls.empty() && activeCalls.top() != callSite)
                                activeCalls.pop();
                            if (!activeCalls.empty())
                                continue;
                            callStack.push(callSite);
                            self(self, edge->getDstNode());
                            callStack.pop();
                        }
                        else if (const auto* ret = dyn_cast<RetCFGEdge>(edge))
                        {
                            unsigned callSite = ret->getCallSite()->getId();
                            if (callStack.empty() || callStack.top() != callSite)
                                continue;
                            callStack.pop();
                            self(self, edge->getDstNode());
                            callStack.push(callSite);
                        }
                        else if (edge->isIntraCFGEdge())
                        {
                            self(self, edge->getDstNode());
                        }
                    }
                }
                path.pop_back();
                visited.erase(state);
            };

            dfs(dfs, icfg->getICFGNode(src));
            //@}
        }
}
