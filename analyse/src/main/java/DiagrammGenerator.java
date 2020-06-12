import org.json.JSONObject;

import java.util.HashMap;

public class DiagrammGenerator {

    public static Diagramm generateInstructionCount(JSONObject obj) {
        StringLineChart chart = new StringLineChart(4000, 2000);
        chart.addHeadline("Instruction in each LLVM_Version");
        chart.addYAxisText("Instructions");
        chart.startAtZero();
        chart.addColorIdMap(Main.colorid);
        for (String key: obj.keySet()) {
            if (key.equals("libLLVMCore.a")) continue;
            Object cur = obj.get(key);
            if (cur instanceof JSONObject) {
                JSONObject jo = (JSONObject) cur;
                HashMap<Integer, Double> map = new HashMap<>();
                long sum = 0;
                int count = 0;
                for (String id : jo.keySet() ) {
                    boolean error = false;
                    Integer iid = 0;
                    try {
                        iid = Integer.parseInt(id);
                    } catch(NumberFormatException nfe) {
                        error = true;
                    }
                    if (!error) {
                        Object o_ver = jo.get(id);
                        if (o_ver instanceof JSONObject) {
                            JSONObject jo_ver = (JSONObject) o_ver;
                            if (jo_ver.has("instruction_count")) {
                                long val = jo_ver.getLong("instruction_count");
                                map.put(iid, (double) val);
                                sum += val;
                                count++;
                            }
                        }
                    }
                }
                if (count != 0) {
                    chart.addData((int) (sum / count), key, map);
                }
            }
        }
        chart.addSideboard(Main.version_label);

        return chart;
    }

    public static Diagramm generateVersionAASpecificStringLineChart(JSONObject obj, String aa, String key, String Headline, String yAxisText, HashMap<Integer, String> label) {
        return generateVersionAASpecificStringLineChart(obj, 0, aa, key, Headline, yAxisText, label);
    }

    public static Diagramm generateVersionAASpecificStringLineChart(JSONObject obj, int version, String key, String Headline, String yAxisText, HashMap<Integer, String> label) {
        return generateVersionAASpecificStringLineChart(obj, version, "", key, Headline, yAxisText, label);
    }

    private static Diagramm generateVersionAASpecificStringLineChart(JSONObject obj, int version, String aa, String key, String Headline, String yAxisText, HashMap<Integer, String> label) {
        StringLineChart chart = new StringLineChart(4000, 2000);
        chart.addHeadline(Headline);
        chart.addYAxisText(yAxisText);
        chart.startAtZero();
        //chart.addColorIdMap(Main.colorid);
        if ((version == 0) == (aa.isEmpty())) {
            System.out.println(version + " " + aa);
            throw new IllegalArgumentException("version == 0 or key. has to be empty");
        }
        HashMap<Integer, Integer> color_id = new HashMap<>();
        for (String file : obj.keySet()) {
            if (file.equals("libLLVMCore.a")) continue;
            Object cur = obj.get(file);
            if (cur instanceof JSONObject) {
                JSONObject jo_file = (JSONObject) cur;
                long sum = 0;
                int count = 0;
                for (String id : jo_file.keySet() ) {
                    boolean error = false;
                    Integer iid = 0;
                    try {
                        iid = Integer.parseInt(id);
                    } catch(NumberFormatException nfe) {
                        error = true;
                    }
                    if (!error) {
                        Object o_ver = jo_file.get(id);
                        if (o_ver instanceof JSONObject) {
                            JSONObject jo_ver = (JSONObject) o_ver;
                            if (jo_ver.has("instruction_count")) {
                                long val = jo_ver.getLong("instruction_count");
                                sum += val;
                                count++;
                            }
                        }
                    }
                }

                HashMap<Integer, Double> values = new HashMap<>();
                if (version == 0) {
                    for (String ver_key : jo_file.keySet()) {
                        boolean error = false;
                        Integer iid = 0;
                        try {
                            iid = Integer.parseInt(ver_key);
                        } catch(NumberFormatException nfe) {
                            error = true;
                        }
                        if (!error) {
                            if (!color_id.containsKey(iid)) {
                                color_id.put(iid, Main.colorid.get(iid));
                            }
                            Object ver = jo_file.get(ver_key);
                            if (ver instanceof JSONObject) {
                                JSONObject jo_ver = (JSONObject) ver;
                                //key not empty
                                if (jo_ver.has(aa)) {
                                    Object aao = jo_ver.get(aa);
                                    if (aao instanceof JSONObject) {
                                        JSONObject jo_aa = (JSONObject) aao;
                                        addKeyToHashMap(jo_aa, values, iid, key);
                                    }
                                }
                            }
                        }
                    }
                } else {
                    if (jo_file.has("" + version)) {
                        Object ver = jo_file.get("" + version);
                        if (ver instanceof JSONObject) {
                            JSONObject jo_ver = (JSONObject) ver;
                            for (String aa_id : jo_ver.keySet()) {
                                if (Main.aa_id_map.containsKey(aa_id)) {
                                    int iid = Main.aa_id_map.get(aa_id);
                                    if (Main.aa_color_map.containsKey(iid)) {
                                        int cid = Main.aa_color_map.get(iid);
                                        color_id.put(iid, cid);
                                    }
                                    Object aao = jo_ver.get(aa_id);
                                    if (aao instanceof JSONObject) {
                                        JSONObject jo_aa = (JSONObject) aao;
                                        addKeyToHashMap(jo_aa, values, iid, key);
                                    }
                                }
                            }
                        }
                    }
                }
                if (count != 0) {
                    chart.addData((int) (sum / count), file, values);
                }
            }
        }
        chart.addColorIdMap(color_id);
        chart.addSideboard(label);
        return chart;
    }

    private static void addKeyToHashMap(JSONObject aa, HashMap<Integer, Double> map, Integer id, String key) {
        if (aa.has(key)) {
            map.put(id, aa.getDouble(key));
        }
    }


    public static Diagramm generateTest() {
        StringLineChart chart = new StringLineChart(4000, 2000);
        chart.addColorIdMap(Main.colorid);
        HashMap<Integer, Double> m1 = new HashMap<>();
        m1.put(40, 100.0);
        m1.put(50, 150.0);
        m1.put(60, 200.0);

        HashMap<Integer, Double> m2 = new HashMap<>();
        m2.put(40, 150.0);
        m2.put(50, 100.0);
        m2.put(60, 300.0);

        HashMap<Integer, Double> m3 = new HashMap<>();
        m3.put(40, 20.0);
        m3.put(50, 150.0);
        m3.put(60, 200.0);

        HashMap<Integer, Double> m4 = new HashMap<>();
        //m4.put(40, 75.0);
        m4.put(50, 230.0);
        m4.put(60, 100.0);

        HashMap<Integer, Double> m5 = new HashMap<>();
        m5.put(40, 50.0);
        m5.put(50, 100.0);
        m5.put(60, 500.0);


        chart.addData(1, "file1", m1);
        chart.addData(2, "file2", m2);
        chart.addData(3, "file3", m3);
        chart.addData(4, "file4", m4);
        chart.addData(5, "file5", m5);
        chart.addData(6, "file1", m1);
        chart.addData(7, "file2", m2);
        chart.addData(8, "file3", m3);
        chart.addData(9, "file4", m4);
        chart.addData(10, "file5", m5);

        //chart.addXAxisText("x-Axis");
        chart.addYAxisText("y-Axis");
        chart.startAtZero();
        chart.addHeadline("Headline");
        return chart;
    }
}
