import de.erichseifert.vectorgraphics2d.*;
import org.json.*;

import java.io.IOException;
import java.lang.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;

public class Main {
    public static final String pathToJson = "./results_full_4.json";

    public static HashMap<Integer, Integer> colorid = new HashMap<>();

    public static void main(String[] args) {
        colorid.put(40, 0);
        colorid.put(50, 1);
        colorid.put(60, 2);
        colorid.put(71, 3);
        colorid.put(80, 4);
        colorid.put(90, 5);

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

        //select export by comment in

        Diagramm instructions = DiagrammGenerator.generateInstructionCount(obj);

        Diagramm chart = DiagrammGenerator.generateTest();
        //write diagrams to disc

        DiagrammPrinter.printDiagram(chart.generateVG(), "test");
        DiagrammPrinter.printDiagram(instructions.generateVG(), "instructions");
        //DiagrammPrinter.printDiagram(chart.generateVG(), "test", "eps");
    }
}
