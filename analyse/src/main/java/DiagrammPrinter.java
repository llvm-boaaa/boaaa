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

    public static void printDiagramms(HashMap<String, Diagramm> charts, EXPORT ef) {
        for (Map.Entry<String, Diagramm> chart : charts.entrySet())
            DiagrammPrinter.printDiagram(chart.getValue().generateVG(), chart.getKey(), ef);
    }

    public static void printDiagramms(HashMap<String, Diagramm> charts) {
        for (Map.Entry<String, Diagramm> chart : charts.entrySet())
            DiagrammPrinter.printDiagram(chart.getValue().generateVG(), chart.getKey());
    }

    public static void printDiagram(VectorGraphics2D vg, String filename) {
        printDiagram(vg, filename, EXPORT.SVG);
    }

    public static void printDiagram(VectorGraphics2D vg, String filename, EXPORT ef) {
        String fileExtension = ef.ext;

        Processor processor = Processors.get(fileExtension);
        Document doc = processor.getDocument(vg.getCommands(),
                new PageSize(vg.getClipBounds().width, vg.getClipBounds().height));
        File dir = new File(fileExtension);
        if (!dir.exists())
            dir.mkdir();
        try {
            doc.writeTo(new FileOutputStream(fileExtension + "/" + filename + "." + fileExtension));
            System.out.println("written file: " + filename + "." + fileExtension);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
