import de.erichseifert.vectorgraphics2d.VectorGraphics2D;

import java.awt.*;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.util.HashMap;

public class LinePrinter {

    private HashMap<Integer, Point2D> m_last_points;
    private HashMap<Integer, Boolean> m_print_past_point;
    private HashMap<Integer, Boolean> m_print_curr_point;
    private int m_line_width;

    public LinePrinter() {
        m_last_points = new HashMap<>();
        m_print_past_point = new HashMap<>();
        m_print_curr_point = new HashMap<>();
    }

    public void addPoint(VectorGraphics2D vg, int id, Point2D p) {
        Stroke s = vg.getStroke();
        Color c = vg.getColor();
        m_print_curr_point.put(id, true);
        if (id < ColorUtil.COLORS.length) {
            vg.setColor(ColorUtil.COLORS[id]);
        }
        if (m_print_past_point.containsKey(id)) {
            vg.setStroke(new BasicStroke(10, BasicStroke.CAP_BUTT, BasicStroke.JOIN_MITER));
        } else {
            vg.setStroke(new BasicStroke(10, BasicStroke.CAP_BUTT,
                    BasicStroke.JOIN_MITER, 10,
                    new float[]{40, 20}, 0));
        }
        if (m_last_points.containsKey(id)) {
            vg.draw(new Line2D.Double(m_last_points.get(id), p));
        }
        m_last_points.put(id, p);
        vg.setColor(c);
        vg.setStroke(s);
    }

    public void step() {
        m_print_past_point = m_print_curr_point;
        m_print_curr_point = new HashMap<>();
    }
}
