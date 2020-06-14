import de.erichseifert.vectorgraphics2d.VectorGraphics2D;

import java.awt.*;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.util.*;
import java.util.function.Consumer;

public class RelationChart extends Diagramm {

    private static final double minY = 0.0;
    private static final double maxY = 100.0;

    private HashMap<Integer, String> m_order = new HashMap<>();
    private HashMap<String, HashMap<Integer, Double>> m_values = new HashMap<>();
    private HashMap<String, Double> m_sum = new HashMap<>();
    private HashMap<Integer, Integer> colorIdMap = new HashMap<>();
    private HashMap<Integer, String> sideboard;
    private String headline;

    double maxSum = 0.0;

    public RelationChart(int width, int height) {
        super(width, height);
        sideboard = null;
        headline = "";
    }

    public void addHeadline(String head) {headline = head; }

    public void addSideboard(HashMap<Integer, String> sb) {
        sideboard = sb;
    }

    public void addColorIdMap(HashMap<Integer, Integer> idmap) {
        colorIdMap = idmap;
    }

    public void addData(Integer order, String id, HashMap<Integer, Double> values) {
        m_order.put(order, id);
        m_values.put(id, values);

        Iterator it = values.entrySet().iterator();
        double sum = 0;
        for (Map.Entry<Integer, Double> entry : values.entrySet()) {
            assert(entry.getKey() >= 0.0);
            sum += entry.getValue();
        }
        m_sum.put(id, sum);
    }



    @Override
    void printToVG(VectorGraphics2D vg) {
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
        double top_y       = (PERCENT_5 + (headline.isEmpty() ? 0.0 : PERCENT_7))   * canvas.getHeight();
        double down_y      = PERCENT_5                                              * canvas.getHeight() + maxWidth;
        double hight_y     = canvas.getHeight() - top_y - down_y;

        double left_x      = PERCENT_7                                             * canvas.getWidth();
        double right_x     = PERCENT_3                                             * canvas.getWidth();
                            //(m_alt_values.isEmpty() ? PERCENT_2 : PERCENT_7)     * canvas.getWidth();
        double right_axis_x= PERCENT_2                                             * canvas.getWidth();
        double sideboard_x = (sideboard == null      ? 0.0       : PERCENT_10)     * canvas.getWidth();
        double width_x     = canvas.getWidth() - left_x - right_x - right_axis_x - sideboard_x;

        Rectangle2D dimension = new Rectangle2D.Double(left_x,  top_y, width_x, hight_y);
        Rectangle2D sb_dim = new Rectangle2D.Double(left_x + width_x + right_axis_x, top_y, sideboard_x, hight_y);

        if (!headline.isEmpty()) {
            Font f = vg.getFont();
            vg.setFont(FontUtil.HeadLine);
            PrintUtil.printTextCenter(vg, new Point2D.Double(canvas.getCenterX(), dimension.getY() / 2), headline);
            vg.setFont(f);
        }

        PrintUtil.StepContext con = Diagramm.PERCENT_STEP_CONTEXT;
        //PrintUtil.printDiagrammLines(vg, dimension, null, con, PrintUtil.Align.right, m_yAxisText, m_xAxisText, new Rectangle2D.Double(0, minY, 0, maxY), false);

        PrintUtil.printDiagrammLines(vg, dimension, null, con, PrintUtil.Align.left, "percent", "", new Rectangle2D.Double(0.0, minY, 0.0, maxY), false);
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
        double box_width = printDimension.getWidth() / ((ids.size() - 1) * 2);

        for(String s : ids) {
            //print text
            //first 0% last 100%
            double percent = 1.0 - ((double) i /(double) (ids.size() - 1));
            PrintUtil.printTextUnderXAxis(vg, printDimension, s, percent);

            //print data

            HashMap<Integer, Double> map = m_values.get(s);

            double sum = m_sum.get(s);

            double last = 0.0;
            assert(map != null);

            for(Map.Entry<Integer, Double> entry : map.entrySet()) {
                double current = last + entry.getValue();
                if (!idmap.containsKey(entry.getKey())) {
                    if (i >= ColorUtil.COLORS.length || i >= IconUtil.IconType.values().length) {
                        throw new IndexOutOfBoundsException("Maximal number of ids: " + Math.min(ColorUtil.COLORS.length, IconUtil.IconType.values().length));
                    }
                    idmap.put(entry.getKey(), idnum++);
                }
                int id = idmap.get(entry.getKey());

                Point2D p = PrintUtil.printPosition(printDimension, percent, entry.getValue(), minY, maxY);
                //System.out.println("last: " + last + " current: " + current + " sum: " + sum);
                PrintUtil.printBox(vg, printDimension, percent, last, current, 0, sum, id, box_width);

                last = current;
            }

            //increment
            i++;
        }

        if (sideboard != null && !sideboard.isEmpty()) {
            HashMap<Integer, String> id_name = new HashMap<>();
            for (Map.Entry<Integer, String> entry : sideboard.entrySet()) {
                assert(idmap.containsKey(entry.getKey()));
                id_name.put(idmap.get(entry.getKey()), entry.getValue());
            }
            PrintUtil.printSideboard(vg, sb_dim, id_name, false);
        }

    }
}
