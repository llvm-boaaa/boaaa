import de.erichseifert.vectorgraphics2d.VectorGraphics2D;
import org.javatuples.Pair;

import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedList;

public class LineChart extends Diagramm {

    private double minX = Double.POSITIVE_INFINITY;
    private double minY = Double.POSITIVE_INFINITY;
    private double maxX = Double.NEGATIVE_INFINITY;
    private double maxY = Double.NEGATIVE_INFINITY;
    private HashMap<String, LinkedList<Pair<Double, Double>>> m_values
            = new HashMap<String, LinkedList<Pair<Double, Double>>>();

    public LineChart(int width, int height) {
        super(width, height);
    }


    public void addData(String id, LinkedList<Pair<Double, Double>> values) {
        Collections.sort(values, new Comparator<Pair<Double, Double>>() {
            @Override
            public int compare(Pair<Double, Double> o1, Pair<Double, Double> o2) {
                return o1.getValue0().compareTo(o2.getValue0());
            }
        });

        m_values.put(id, values);

        for(Pair<Double, Double> entry : values) {
            if (entry.getValue0() < minX)
                minX = entry.getValue0();
            if (entry.getValue0() > maxX)
                maxX = entry.getValue0();

            if (entry.getValue1() < minY)
                minY = entry.getValue1();
            if (entry.getValue1() > maxY)
                maxY = entry.getValue1();
        }
    }

    @Override
    public void printToVG(VectorGraphics2D vg) {

    }
}
