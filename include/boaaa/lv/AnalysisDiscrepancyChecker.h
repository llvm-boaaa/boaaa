#ifndef BOAAA_ANALYSIS_DISCREPANCY_CHECKER_H
#define BOAAA_ANALYSIS_DISCREPANCY_CHECKER_H

#include "boaaa/lv/version_context.h"
#include "boaaa/support/raw_type.h"

#include <set>

namespace boaaa {

	struct AnalysisDiscrepancyChecker
	{
		void checkAnalysis(evaluation_aa_sets& aa_sets, evaluation_no_aa_sets& no_aa_sets)
		{
			for (_raw_type_inst(aa_sets)::iterator aa_it = aa_sets.begin(), aa_end = aa_sets.end(); aa_it != aa_end; aa_it++) 
			{
				evaluation_sets* es = aa_it->second.get();

				for (_raw_type_inst(aa_sets)::iterator aa_it2 = aa_sets.begin(), aa_end2 = aa_sets.end(); aa_it2 != aa_it; aa_it2++) 
				{
					evaluation_sets* es2 = aa_it2->second.get();

					for (evaluation_sets::iterator f_it = es2->begin(), f_end = es2->end(); f_it != f_end; f_it++)
					{
						size_t GUID = f_it->first;
						unionfind_map* ufms = f_it->second.get();
						unionfind_map* ufml = (*es2)[GUID].get();
						bool swap = false;
						if (ufms->size() > ufml->size())
						{
							//swop so in ufms is the smaller set
							unionfind_map* tmp = ufms;
							ufms = ufml;
							ufml = tmp;
							bool swap = true;
						}

						std::set<size_t> headset;
						for (unionfind_map::const_iterator p_it_l = ufml->begin(), p_end_l = ufml->end(); p_it_l != p_end_l; p_it_l++)
						{
							size_t id = p_it_l->second->parent()->value();
							//if inserted need to check
							if (headset.insert(id).second)
							{
								unionfind_map::const_iterator checkS = ufms->find(id);
								if (checkS != ufms->end()) {
									unionfind_map::const_iterator checkL = ufml->find(checkS->second->parent()->value()); //O(n^6)
									if (checkL != ufml->end()) {
										if (checkL->second->parent()->value() == p_it_l->second->parent()->value()) 
										{
											//all ok
											continue;
										}
									}
								}
								// maybe inconsistent need to check
								auto getMap = [&](_raw_type_inst(aa_sets)::iterator& it) -> unionfind_map* {
									evaluation_explicite_sets* ees = no_aa_sets[it->first].get();
									evaluation_sets*			es = (*ees)[GUID].get();
									return							 (*es)[id].get();
								};

								unionfind_map* no_aa_map_s = getMap((!swap ? aa_it : aa_it2));							

								for (unionfind_map::const_iterator p_it_l2 = ufml->begin(), p_end_l2 = ufml->end(); p_it_l2 != p_end_l2; p_it_l2++)
								{
									//when alias from id to pointer check if other alias says no alias
									if (p_it_l2->second->parent()->value() == id) 
									{
										if (no_aa_map_s->contains(p_it_l2->second->value()))
										{
											//the aas are not consistent.
											llvm::errs() << aa_it->first << " and " << aa_it2->first << " have different results in function: " << GUID <<  " for: " << p_it_l2->first << ", " << id << "\n";
										}
									}
								}
							}
						}
					}
				}
			}
		}
	};
}


#endif