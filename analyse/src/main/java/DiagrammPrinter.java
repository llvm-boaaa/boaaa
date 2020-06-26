import de.erichseifert.vectorgraphics2d.Document;
import de.erichseifert.vectorgraphics2d.Processor;
import de.erichseifert.vectorgraphics2d.Processors;
import de.erichseifert.vectorgraphics2d.VectorGraphics2D;
import de.erichseifert.vectorgraphics2d.util.PageSize;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class DiagrammPrinter {

    public enum EXPORT {
        SVG("svg"),
        EPS("eps"),
        PDF("pdf");

        public String ext;
        EXPORT(String extension) {
            ext = extension;
        }
    }

    public static void printDiagramms(HashMap<String, Diagramm> charts, String prefix, EXPORT ef) {
        int i = 1;
        for (Map.Entry<String, Diagramm> chart : charts.entrySet()) {
            System.out.print("(" + i + "/" + charts.size() + ") ");
            DiagrammPrinter.printDiagram(chart.getValue().generateVG(), prefix, chart.getKey(), ef);
            i++;
        }
    }

    public static void printDiagramms(HashMap<String, Diagramm> charts, String prefix) {
        int i = 1;
        for (Map.Entry<String, Diagramm> chart : charts.entrySet()){
            System.out.print("(" + i + "/" + charts.size() + ") ");
            DiagrammPrinter.printDiagram(chart.getValue().generateVG(), prefix, chart.getKey());
            i++;
        }
    }

    public static void printDiagram(VectorGraphics2D vg, String prefix, String filename) {
        printDiagram(vg, prefix, filename, EXPORT.SVG);
    }

    public static void printDiagram(VectorGraphics2D vg, String prefix, String filename, EXPORT ef) {
        String fileExtension = ef.ext;
        if (!prefix.isEmpty())
            filename = prefix + "/" + filename;

        Processor processor = Processors.get(fileExtension);
        Document doc = processor.getDocument(vg.getCommands(),
                new PageSize(vg.getClipBounds().width, vg.getClipBounds().height));
        File dir = new File(fileExtension + (prefix.isEmpty() ? "" : "/" + prefix));
        if (!dir.exists())
            dir.mkdirs();
        try {
            doc.writeTo(new FileOutputStream(fileExtension + "/" + filename + "." + fileExtension));
            System.out.println("written file: " + filename + "." + fileExtension);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
