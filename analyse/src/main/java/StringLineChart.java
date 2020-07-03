import com.sun.org.apache.bcel.internal.classfile.StackMapEntry;
import de.erichseifert.vectorgraphics2d.VectorGraphics2D;

import java.awt.*;
import java.awt.geom.Dimension2D;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.util.*;
import java.util.function.Consumer;

public class StringLineChart extends Diagramm {

    private double minY = Double.POSITIVE_INFINITY;
    private double maxY = Double.NEGATIVE_INFINITY;

    private HashMap<Integer, String> m_order = new HashMap<>();
    private HashMap<String, HashMap<Integer, Double>> m_values = new HashMap<>();
    private HashMap<String, HashMap<Integer, Double>> m_alt_values = new HashMap<>();
    private HashMap<Integer, Integer> colorIdMap = new HashMap<>();
    private HashMap<Integer, String> sideboard;

    private String headline;

    public StringLineChart(int width, int height) {
        super(width, height);
        headline = "";
        sideboard = null;
    }

    public void startAtZero() {
        if (0 < minY) minY = 0;
    }

    public void addHeadline(String head) {headline = head; }

    public void addAltData(Integer order, String id, HashMap<Integer, Double> values) {

    }

    public void addSideboard(HashMap<Integer, String> sb) {
        sideboard = sb;
    }

    public void addData(Integer order, String id, HashMap<Integer, Double> values) {
        m_order.put(order, id);
        m_values.put(id, values);

        Iterator it = values.entrySet().iterator();
        for (Map.Entry<Integer, Double> entry : values.entrySet()) {
            if (entry.getValue() < minY) {
                minY = entry.getValue();
            }
            if (entry.getValue() > maxY) {
                maxY = entry.getValue();
            }
        }
    }

    public void addColorIdMap(HashMap<Integer, Integer> idmap) {
        colorIdMap = idmap;
    }

    @Override
    public void printToVG(VectorGraphics2D vg) {
        double maxWidth = 0;
        double maxHeight = 0;
        for (Map.Entry<Integer, String> entry : m_order.entrySet()) {
            Rectangle2D bounds = Util.getTextDimension(vg, entry.getValue());
            double width = bounds.getWidth();
            double height = bounds.getHeight();
            if (width > maxWidth)
                maxWidth = width;
            if (height > maxHeight) {
                maxHeight = height;
            }
        }

        //get ids in correct order
        LinkedList<String> ids = new LinkedList<>();
        {
            LinkedList<Map.Entry<Integer, String>> stringstemp = new LinkedList<>(m_order.entrySet());
            Collections.sort(stringstemp, new Comparator<Map.Entry<Integer, String>>() {
                @Override
                public int compare(Map.Entry<Integer, String> o1, Map.Entry<Integer, String> o2) {
                    return o1.getKey().compareTo(o2.getKey());
                }
            });
            stringstemp.forEach(new Consumer<Map.Entry<Integer, String>>() {
                @Override
                public void accept(Map.Entry<Integer, String> entry) {
                    ids.push(entry.getValue());
                }
            });
        }

        Rectangle2D canvas = vg.getClipBounds();
        double top_y       = (PERCENT_5 + (headline.isEmpty() ? PERCENT_3 : PERCENT_7))   * canvas.getHeight();
        double down_y      = PERCENT_5                                              * canvas.getHeight() + maxWidth;
        double hight_y     = canvas.getHeight() - top_y - down_y;

        double left_x      = PERCENT_7                                             * canvas.getWidth();
        double right_x     = PERCENT_3                                             * canvas.getWidth();
        double right_axis_x= (m_alt_values.isEmpty() ? PERCENT_2 : PERCENT_7)      * canvas.getWidth();
        double sideboard_x = (sideboard == null      ? 0.0       : PERCENT_10)     * canvas.getWidth();
        double width_x     = canvas.getWidth() - left_x - right_x - right_axis_x - sideboard_x;

        Rectangle2D dimension = new Rectangle2D.Double(left_x,  top_y, width_x, hight_y);
        Rectangle2D sb_dim = new Rectangle2D.Double(left_x + width_x + right_axis_x, top_y, sideboard_x, hight_y);

        minY = Util.roundToBeforeStep(minY);
        maxY = Util.roundToNextStep(maxY);

        if (!headline.isEmpty()) {
            Font f = vg.getFont();
            vg.setFont(FontUtil.HeadLine);
            PrintUtil.printTextCenter(vg, new Point2D.Double(canvas.getCenterX(), dimension.getY() / 2), headline);
            vg.setFont(f);
        }

        PrintUtil.StepContext con = PrintUtil.getIntervals((int) minY, (int) maxY);
        //PrintUtil.printDiagrammLines(vg, dimension, null, con, PrintUtil.Align.right, m_yAxisText, m_xAxisText, new Rectangle2D.Double(0, minY, 0, maxY), false);

        PrintUtil.printDiagrammLines(vg, dimension, null, con, PrintUtil.Align.left, m_yAxisText, m_xAxisText, new Rectangle2D.Double(0, minY, 0, maxY), false);
        double lower = headline.isEmpty() ? dimension.getY() : dimension.getY() / 3;
        dimension = new Rectangle2D.Double(dimension.getX(), dimension.getY() + lower, dimension.getWidth(), dimension.getHeight() - lower);
        PrintUtil.printYAxisScala(vg, dimension, con, minY, maxY);

        Rectangle2D printDimension = new Rectangle2D.Double(dimension.getX() * 1.5, dimension.getY(), dimension.getWidth() - dimension.getX(), dimension.getHeight());

        int i = 0;
        int idnum = 0;
        HashMap<Integer, Integer> idmap;
        if (colorIdMap != null) {
            idmap = new HashMap<>(colorIdMap);
        } else {
            idmap = new HashMap<>();
        }
        for(String s : ids) {
            double percent = 1.0 - ((double) i /(double) (ids.size() - 1));
            PrintUtil.printTextUnderXAxis(vg, printDimension, s, percent);
            HashMap<Integer, Double> map = m_values.get(s);
            for(Map.Entry<Integer, Double> entry : map.entrySet()) {
                if (!idmap.containsKey(entry.getKey())) {
                    if (i >= ColorUtil.COLORS.length || i >= IconUtil.IconType.values().length) {
                        throw new IndexOutOfBoundsException("Maximal number of ids: " + Math.min(ColorUtil.COLORS.length, IconUtil.IconType.values().length));
                    }
                    idmap.put(entry.getKey(), idnum++);
                }
            }
            i++;
        }

        LinePrinter lp = new LinePrinter();
        for(Map.Entry<Integer, Integer> ii : idmap.entrySet()) {
            i = 0;
            for (String s: ids) {
                int id = ii.getValue();
                double percent = 1.0 - ((double) i /(double) (ids.size() - 1));
                Double value = m_values.get(s).get(ii.getKey());
                if (value == null) {
                    lp.step();
                    i++;
                    continue;
                }

                Point2D p = PrintUtil.printPosition(printDimension, percent, value, minY, maxY);
                lp.addPoint(vg, id, p);
                IconUtil.printIconColor(vg, PrintUtil.printPosition(printDimension, percent, value, minY, maxY), id);

                //increment
                i++;
                lp.step();
            }
        }
        //*/

        if (sideboard != null && !sideboard.isEmpty()) {
            HashMap<Integer, String> id_name = new HashMap<>();
            for (Map.Entry<Integer, String> entry : sideboard.entrySet()) {
                assert(idmap.containsKey(entry.getKey()));
                id_name.put(idmap.get(entry.getKey()), entry.getValue());
            }
            PrintUtil.printSideboard(vg, sb_dim, id_name);
        }
    }
}
