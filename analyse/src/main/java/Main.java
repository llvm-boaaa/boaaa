import de.erichseifert.vectorgraphics2d.*;
import org.json.*;

import java.io.IOException;
import java.lang.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.sql.Time;
import java.util.*;
import java.util.function.BiConsumer;
import java.util.function.Consumer;

public class Main {
    //add/remove "/" below to switch mode
    /*
    public static boolean TEST = true;
    public static boolean DATA = false;
    /*/
    public static boolean TEST = false;
    public static boolean DATA = true;
    //*/

    //format
    //*
    public static final DiagrammPrinter.EXPORT FORMAT = DiagrammPrinter.EXPORT.EPS;
    /*/
    public static final DiagrammPrinter.EXPORT FORMAT = DiagrammPrinter.EXPORT.SVG;
    //*/


    //add/remove "/" below to switch mode
    //*
    public static boolean metadata = true;
    /*/
    public static boolean metadata = false;
    //*/

    public static final String pathToJson = "./results_llvm_final.json";
    public static final String pathToMetaJson = "./results_slib_final.json";

    public static HashMap<Integer, Integer> colorid = new HashMap<>();
    public static HashMap<Integer, String> version_label = new HashMap<>();

    public static HashMap<String, String>   aa_key_map = new HashMap<>();
    public static HashMap<String, String>   aa_key_map_50 = new HashMap<>();
    public static HashMap<Integer, String>  aa_label = new HashMap<>();
    public static HashMap<String, Integer>  aa_id_map = new HashMap<>();
    public static HashMap<Integer, Integer> aa_color_map = new HashMap<>();

    public static HashMap<Integer, String>  ar_label = new HashMap<>();
    public static HashMap<Integer, Integer> ar_color_map = new HashMap<>();

    public static HashMap<Integer, String> mr_label = new HashMap<>();
    public static HashMap<Integer, Integer> mr_color_map = new HashMap<>();

    public static HashMap<Integer, String> time_label = new HashMap<>();
    public static HashMap<Integer, Integer> time_color_map = new HashMap<>();

    public static LinkedList<String> NoAliasList = new LinkedList<>();
    public static LinkedList<String> PartialAliasList = new LinkedList<>();
    public static LinkedList<String> MustAliasList = new LinkedList<>();
    public static LinkedList<String> MPNAliasList = new LinkedList<>();

    public static void main(String[] args) {
        init();

        //read Json
        JSONObject obj = null;
        if (!pathToJson.isEmpty()) {
            String jsonContent = "";
            try {
                if (metadata) {
                    jsonContent = new String(Files.readAllBytes(Paths.get(pathToMetaJson)));
                } else {
                    jsonContent = new String(Files.readAllBytes(Paths.get(pathToJson)));
                }
            } catch (IOException e) {
                e.printStackTrace();
                return;
            }

            obj = new JSONObject(jsonContent);
        }

        //sometimes "-" gets generated, but contains no date, so get removed
        if (obj.has("-")) {
            obj.remove("-");
        }

        HashMap<String, Diagramm> diagrams = new HashMap<>();

        if (DATA) {

            diagrams.put("instructions", DiagrammGenerator.generateInstructionCount(obj));

            for(String file : obj.keySet()) {
                String filename = "AR_" + file;
                diagrams.put(filename, DiagrammGenerator.generateAliasResultForFile(obj, "", file, 50));
            }

            //all possible ids: "sec_pm_time", "mil_pm_time", "mic_pm_time", "nan_pm_time",
            //                  "sec_function_time", "mil_function_time", "mic_function_time", "nan_function_time",
            //                  "sec_alias_time", "mil_alias_time", "mic_alias_time", "nan_alias_time",
            //                  "sec_modref_time", "mil_modref_time", "mic_modref_time", "nan_modref_time",
            //
            //                  "alias_sum", "no_alias_count", "may_alias_count", "partial_alias_count", "must_alias_count",
            //                  "modref_sum", "no_modref_count", "mod_count", "ref_count", "modref_count", "must_count", "must_mod_count", "must_ref_count", "must_modref_count",
            //                  "alias_sets", "no_alias_sets", "mean_alias_sets", "var_alias_sets", "mean_no_alias_sets", "var_no_alias_sets"
            String[] aa_keys = new String[]{"sec_pm_time", "alias_sum", "alias_sets", "ex_alias_sets", "no_alias_sets", "mean_alias_sets", "var_alias_sets", "mean_ex_alias_sets", "var_ex_alias_sets", "mean_no_alias_sets", "var_no_alias_sets"};

            LinkedList<String> empty = new LinkedList<>();
            LinkedList<String> NOSCEVAA = new LinkedList<>();
            NOSCEVAA.add("llvm::SCEVAAWrapperPass");

            for (int ver : new int[]{40, 50, 60, 71, 80, 90}) {

                diagrams.put("performance_no_" + ver, DiagrammGenerator.generatePerformanceChart(obj, "", ver, NoAliasList, scale10h6, NOSCEVAA));
                diagrams.put("performance_partial_" + ver, DiagrammGenerator.generatePerformanceChart(obj, "",ver, PartialAliasList, scale10h3, empty));
                diagrams.put("performance_must_" + ver, DiagrammGenerator.generatePerformanceChart(obj, "", ver, MustAliasList, scale10h3, empty));
                diagrams.put("performance_mpn_" + ver, DiagrammGenerator.generatePerformanceChart(obj, "", ver, MPNAliasList, scale10h6, NOSCEVAA));

                diagrams.put("alias_sum_relative_" + ver, DiagrammGenerator.generateRelativeAliasSet(obj, "", ver, "alias_sets"));
                diagrams.put("ex_alias_sum_relative_" + ver, DiagrammGenerator.generateRelativeAliasSet(obj, "", ver, "ex_alias_sets"));

                for (String key : aa_keys) {
                    String file = key.replace(" ", "-") + "_" + ver;
                    diagrams.put(file, DiagrammGenerator.generateVersionAASpecificStringLineChart(obj, ver, key, "LLVM_" + ver + " " + key + " for each Analysis", key, aa_label));
                }
            }

            for (Map.Entry<String, String> entry : aa_key_map.entrySet()) {
                String aa = entry.getKey();
                for (String key : aa_keys) {
                    String file = key.replace(" ", "-") + "_" + entry.getValue();
                    diagrams.put(file, DiagrammGenerator.generateVersionAASpecificStringLineChart(obj, aa, key, key + " for " + entry.getValue(), key, version_label));
                }
            }

            for (int ver : new int[]{40, 50, 60, 71, 80, 90}) {
                Set<Map.Entry<String, String>> set;
                if (ver == 50) {
                    set = aa_key_map_50.entrySet();
                } else {
                    set = aa_key_map.entrySet();
                }
                for(Map.Entry<String, String> entry : set) {
                    String headline_alias = "Alias-Result for " + entry.getValue() + " in LLVM_" + ver;
                    String file_alias = "AR_" + entry.getValue() + "_" + ver;
                    diagrams.put(file_alias, DiagrammGenerator.generateAliasResults(obj, headline_alias, ver, entry.getKey()));

                    String headline_modref = "ModRefInfo for " + entry.getValue() + " in LLVM_" + ver;
                    String file_modref = "MRI_" + entry.getValue() + "_" + ver;
                    diagrams.put(file_modref, DiagrammGenerator.generateModRefInfo(obj, headline_modref, ver, entry.getKey()));

                    String headline_time = "Time for " + entry.getValue() + " in LLVM_" + ver;
                    String file_time = "time_" + entry.getValue() + "_" + ver;
                    diagrams.put(file_time, DiagrammGenerator.generateTimeResult(obj, headline_time, ver, entry.getKey()));
                }
            }
        }
        //select export by comment in

        if (TEST) {
            diagrams.put("StringLineChartTest", DiagrammGenerator.generateTestStringLineChart());
            diagrams.put("RelationChartTest", DiagrammGenerator.generateTestRelationChart());
            diagrams.put("AreaChartTest", DiagrammGenerator.generateTestAreaChart());
        }

        //Diagramm chart = DiagrammGenerator.generateTest();
        //write diagrams to disc
        if (TEST) {
            DiagrammPrinter.printDiagramms(diagrams, "", DiagrammPrinter.EXPORT.SVG);
        } else {
            if (metadata) {
                DiagrammPrinter.printDiagramms(diagrams, "slib", FORMAT);
            } else {
                DiagrammPrinter.printDiagramms(diagrams, "llvm", FORMAT);
            }
        }
    }

    public static void init() {
        colorid.put(40, 0);
        colorid.put(50, 1);
        colorid.put(60, 2);
        colorid.put(71, 3);
        colorid.put(80, 4);
        colorid.put(90, 5);

        for(int i = 40; i < 100; i+=10)
            if (i == 70) {
                version_label.put(71, "LLVM_71");
            } else {
                version_label.put(i, "LLVM_" + i);
            }

        int id = 0;
        aa_key_map.put("llvm::BasicAAWrapperPass", "BasicAA");
        aa_label.put(id, "BasicAA");
        aa_color_map.put(id, id);
        aa_id_map.put("llvm::BasicAAWrapperPass", id++);

        aa_key_map.put("llvm::CFLAndersAAWrapperPass", "CFLAnders");
        aa_label.put(id, "CFLAnders");
        aa_color_map.put(id, id);
        aa_id_map.put("llvm::CFLAndersAAWrapperPass", id++);

        aa_key_map.put("llvm::CFLSteensAAWrapperPass", "CFLSteens");
        aa_label.put(id, "CFLSteens");
        aa_color_map.put(id, id);
        aa_id_map.put("llvm::CFLSteensAAWrapperPass", id++);

        aa_key_map.put("llvm::SCEVAAWrapperPass", "SCEVAA");
        aa_label.put(id, "SCEVAA");
        aa_color_map.put(id, id);
        aa_id_map.put("llvm::SCEVAAWrapperPass", id++);

        aa_key_map.put("llvm::ScopedNoAliasAAWrapperPass", "SNAAA");
        aa_label.put(id, "SNAAA");
        aa_color_map.put(id, id);
        aa_id_map.put("llvm::ScopedNoAliasAAWrapperPass", id++);

        aa_key_map.put("llvm::TypeBasedAAWrapperPass", "TBAA");
        aa_label.put(id, "TBAA");
        aa_color_map.put(id, id);
        aa_id_map.put("llvm::TypeBasedAAWrapperPass", id++);

        aa_key_map.put("llvm::GlobalsAAWrapperPass->llvm::TypeBasedAAWrapperPass->llvm::ScopedNoAliasAAWrapperPass->llvm::BasicAAWrapperPass", "clang");
        aa_label.put(id, "clang");
        aa_color_map.put(id, id);
        aa_id_map.put("llvm::GlobalsAAWrapperPass->llvm::TypeBasedAAWrapperPass->llvm::ScopedNoAliasAAWrapperPass->llvm::BasicAAWrapperPass", id++);

        aa_key_map.forEach(new BiConsumer<String, String>() {
            @Override
            public void accept(String s, String s2) {
                aa_key_map_50.put(s, s2);
            }
        });

        aa_key_map_50.put("sea_dsa::DsaAnalysis->llvm::BottomUpSeaDsaWrapperPass", "SeaDsa-BU");
        aa_label.put(id, "SeaDsa-BU");
        aa_color_map.put(id, id);
        aa_id_map.put("sea_dsa::DsaAnalysis->llvm::BottomUpSeaDsaWrapperPass", id++);

        aa_key_map_50.put("sea_dsa::DsaAnalysis->llvm::ContextSensitiveBottomUpTopDownSeaDsaWrapperPass", "SeaDsa-BUTD");
        aa_label.put(id, "SeaDsa-BUTD");
        aa_color_map.put(id, id);
        aa_id_map.put("sea_dsa::DsaAnalysis->llvm::ContextSensitiveBottomUpTopDownSeaDsaWrapperPass", id++);

        aa_key_map_50.put("sea_dsa::DsaAnalysis->llvm::ContextInsensitiveSeaDsaWrapperPass", "SeaDsa-CIS");
        aa_label.put(id, "SeaDsa-CIS");
        aa_color_map.put(id, id);
        aa_id_map.put("sea_dsa::DsaAnalysis->llvm::ContextInsensitiveSeaDsaWrapperPass", id++);

        ar_label.put(AliasResult.NoALias.id, "NoAlias");
        ar_color_map.put(AliasResult.NoALias.id, 3);

        ar_label.put(AliasResult.MayAlias.id, "MayAlias");
        ar_color_map.put(AliasResult.MayAlias.id, 0);

        ar_label.put(AliasResult.PartialAlias.id, "PartialAlias");
        ar_color_map.put(AliasResult.PartialAlias.id, 7);

        ar_label.put(AliasResult.MustAlias.id, "MustAlias");
        ar_color_map.put(AliasResult.MustAlias.id, 1);

        id = 0;

        mr_label.put(ModRefInfo.NoModRef.id, "NoModRef");
        mr_color_map.put(ModRefInfo.NoModRef.id, id++);

        mr_label.put(ModRefInfo.Mod.id, "Mod");
        mr_color_map.put(ModRefInfo.Mod.id, id++);

        mr_label.put(ModRefInfo.Ref.id, "Ref");
        mr_color_map.put(ModRefInfo.Ref.id, id++);

        mr_label.put(ModRefInfo.ModRef.id, "ModRef");
        mr_color_map.put(ModRefInfo.ModRef.id, id++);

        mr_label.put(ModRefInfo.Must.id, "Must");
        mr_color_map.put(ModRefInfo.Must.id, id++);

        mr_label.put(ModRefInfo.MustMod.id, "MustMod");
        mr_color_map.put(ModRefInfo.MustMod.id, id++);

        mr_label.put(ModRefInfo.MustRef.id, "MustRef");
        mr_color_map.put(ModRefInfo.MustRef.id, id++);

        mr_label.put(ModRefInfo.MustModRef.id, "MustModRef");
        mr_color_map.put(ModRefInfo.MustModRef.id, id++);

        time_label.put(TimeInfo.PM_TIME.id, "PassManager");
        time_label.put(TimeInfo.ANALYSIS_TIME.id, "analyse");
        time_label.put(TimeInfo.FUNCTION_TIME.id, "funktion");
        time_label.put(TimeInfo.ALIAS_TIME.id, "alias");
        time_label.put(TimeInfo.MODREF_TIME.id, "ModRef");
        id = 0;
        time_color_map.put(TimeInfo.PM_TIME.id, TimeInfo.PM_TIME.id);
        time_color_map.put(TimeInfo.ANALYSIS_TIME.id, TimeInfo.ANALYSIS_TIME.id);
        time_color_map.put(TimeInfo.FUNCTION_TIME.id, TimeInfo.FUNCTION_TIME.id);
        time_color_map.put(TimeInfo.ALIAS_TIME.id, TimeInfo.ALIAS_TIME.id);
        time_color_map.put(TimeInfo.MODREF_TIME.id, TimeInfo.MODREF_TIME.id);

        NoAliasList.add(NO_ALIAS_COUNT);
        PartialAliasList.add(PARTIAL_ALIAS_COUNT);
        MustAliasList.add(MUST_ALIAS_COUNT);
        MPNAliasList.add(NO_ALIAS_COUNT);
        MPNAliasList.add(PARTIAL_ALIAS_COUNT);
        MPNAliasList.add(MUST_ALIAS_COUNT);


    }

    public static final String SEC_PM_TIME = "sec_pm_time";
    public static final String MIL_PM_TIME = "mil_pm_time";
    public static final String MIC_PM_TIME = "mic_pm_time";
    public static final String NAN_PM_TIME = "nan_pm_time";

    public static final String SEC_FC_TIME =  "sec_function_time";
    public static final String MIL_FC_TIME = "mil_function_time";
    public static final String MIC_FC_TIME = "mic_function_time";
    public static final String NAN_FC_TIME = "nan_function_time";

    public static final String SEC_ALIAS_TIME = "sec_alias_time";
    public static final String MIL_ALIAS_TIME = "mil_alias_time";
    public static final String MIC_ALIAS_TIME = "mic_alias_time";
    public static final String NAN_ALIAS_TIME = "nan_alias_time";

    public static final String SEC_MODREF_TIME = "sec_modref_time";
    public static final String MIL_MODREF_TIME = "mil_modref_time";
    public static final String MIC_MODREF_TIME = "mic_modref_time";
    public static final String NAN_MODREF_TIME = "nan_modref_time";

    public static final String ALIAS_SUM = "alias_sum";
    public static final String NO_ALIAS_COUNT = "no_alias_count";
    public static final String MAY_ALIAS_COUNT = "may_alias_count";
    public static final String PARTIAL_ALIAS_COUNT = "partial_alias_count";
    public static final String MUST_ALIAS_COUNT = "must_alias_count";

    public static final String MODREF_SUM = "modref_sum";
    public static final String NO_MODREF_COUNT = "no_modref_count";
    public static final String MOD_COUNT = "mod_count";
    public static final String REF_COUNT = "ref_count";
    public static final String MODREF_COUNT = "modref_count";
    public static final String MUST_COUNT = "must_count";
    public static final String MUST_MOD_COUNT = "must_mod_count";
    public static final String MUST_REF_COUNT = "must_ref_count";
    public static final String MUST_MODREF_COUNT = "must_modref_count";

    public static final String ALIAS_SETS = "alias_sets";
    public static final String NO_ALIAS_SETS = "no_alias_sets";
    public static final String MEAN_ALIAS_SETS = "mean_alias_sets";
    public static final String VAR_ALIAS_SETS = "var_alias_sets";
    public static final String MEAN_NO_ALIAS_SETS = "mean_no_alias_sets";
    public static final String VAR_NO_ALIAS_SETS = "var_no_alias_sets";

    public static final double scale10h3 = 1_000;
    public static final double scale10h6 = 1_000_000;
    public static final double scale10h9 = 1_000_000_000;
}
