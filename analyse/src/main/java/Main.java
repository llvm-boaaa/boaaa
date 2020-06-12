import de.erichseifert.vectorgraphics2d.*;
import org.json.*;

import java.io.IOException;
import java.lang.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class Main {
    public static final String pathToJson = "./results_full_4.json";

    public static HashMap<Integer, Integer> colorid = new HashMap<>();
    public static HashMap<Integer, String> version_label = new HashMap<>();

    public static HashMap<String, String>   aa_key_map = new HashMap<>();
    public static HashMap<Integer, String>  aa_label = new HashMap<>();
    public static HashMap<String, Integer>  aa_id_map = new HashMap<>();
    public static HashMap<Integer, Integer> aa_color_map = new HashMap<>();

    public static void main(String[] args) {
        init();

        //read Json
        JSONObject obj = null;
        if (!pathToJson.isEmpty()) {
            String jsonContent;
            try {
                jsonContent = new String(Files.readAllBytes(Paths.get(pathToJson)));
            } catch (IOException e) {
                e.printStackTrace();
                return;
            }

            obj = new JSONObject(jsonContent);
        }

        HashMap<String, Diagramm> diagrams = new HashMap<>();
        diagrams.put("instructions", DiagrammGenerator.generateInstructionCount(obj));
        //all possible ids: "sec_pm_time", "mil_pm_time", "mic_pm_time", "nan_pm_time",
        //                  "sec_function_time", "mil_function_time", "mic_function_time", "nan_function_time",
        //                  "sec_alias_time", "mil_alias_time", "mic_alias_time", "nan_alias_time",
        //                  "sec_modref_time", "mil_modref_time", "mic_modref_time", "nan_modref_time",
        //
        //                  "alias_sum", "no_alias_count", "may_alias_count", "partial_alias_count", "must_alias_count",
        //                  "modref_sum", "no_modref_count", "mod_count", "ref_count", "modref_count", "must_count", "must_mod_count", "must_ref_count", "must_modref_count",
        //                  "alias_sets", "no_alias_sets", "mean_alias_sets", "var_alias_sets", "mean_no_alias_sets", "var_no_alias_sets"
        String[] aa_keys = new String[]{"alias_sum", "no_alias_count", "may_alias_count", "partial_alias_count", "must_alias_count",
                "alias_sets", "no_alias_sets", "mean_alias_sets", "var_alias_sets", "mean_no_alias_sets", "var_no_alias_sets"};


            //for (int ver : new int[]{40, 50, 60, 71, 80, 90}) {
        for (int ver : new int[]{40, 60, 71, 80, 90}) {
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

        //select export by comment in

        diagrams.put("instructions",DiagrammGenerator.generateInstructionCount(obj));

        //Diagramm chart = DiagrammGenerator.generateTest();
        //write diagrams to disc

        //DiagrammPrinter.printDiagram(chart.generateVG(), "test");
        for (Map.Entry<String, Diagramm> chart : diagrams.entrySet())
            DiagrammPrinter.printDiagram(chart.getValue().generateVG(), chart.getKey());

        //DiagrammPrinter.printDiagram(chart.generateVG(), "test", "eps");
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

        aa_key_map.put("llvm::TypeBasedAAWrapperPass->llvm::ScopedNoAliasAAWrapperPass->llvm::BasicAAWrapperPass", "clang");
        aa_label.put(id, "clang");
        aa_color_map.put(id, id);
        aa_id_map.put("llvm::TypeBasedAAWrapperPass->llvm::ScopedNoAliasAAWrapperPass->llvm::BasicAAWrapperPass", id++);
    }
}
