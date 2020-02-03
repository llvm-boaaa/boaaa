#ifndef BOAAA_LV_INITALIZE_PASS_END_BOAAA_H
#define BOAAA_LV_INITALIZE_PASS_END_BOAAA_H

#define INITIALIZE_PASS_END_BOAAA(passName, arg, name, cfg, analysis)          \
  PassInfo *PI = new PassInfo(                                                 \
      name, arg, &passName::ID,                                                \
      PassInfo::NormalCtor_t(callDefaultCtor<passName>), cfg, analysis);       \
  Registry.registerPass(*PI, true);                                            \
  return PI;                                                                   \
  }                                                                            \
  static llvm::once_flag Initialize##passName##PassFlag;                       \
  void initialize##passName##Pass(PassRegistry &Registry) {                    \
    llvm::call_once(Initialize##passName##PassFlag,                            \
                    initialize##passName##PassOnce, std::ref(Registry));       \
  }                                                                            \

/* LINE 12: removed llvm:: bevore initalize##passName##Pass */

#endif