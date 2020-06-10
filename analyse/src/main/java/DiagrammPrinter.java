import de.erichseifert.vectorgraphics2d.Document;
import de.erichseifert.vectorgraphics2d.Processor;
import de.erichseifert.vectorgraphics2d.Processors;
import de.erichseifert.vectorgraphics2d.VectorGraphics2D;
import de.erichseifert.vectorgraphics2d.util.PageSize;

import java.io.FileOutputStream;
import java.io.IOException;

public class DiagrammPrinter {

    public static void printDiagram(VectorGraphics2D vg, String filename) {
        printDiagram(vg, filename, "svg");
    }

    public static void printDiagram(VectorGraphics2D vg, String filename, String fileExtension) {
        Processor processor = Processors.get(fileExtension);
        Document doc = processor.getDocument(vg.getCommands(),
                new PageSize(vg.getClipBounds().width, vg.getClipBounds().height));
        try {
            doc.writeTo(new FileOutputStream(filename + "." + fileExtension));
            System.out.println("written file: " + filename + "." + fileExtension);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
