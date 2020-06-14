import de.erichseifert.vectorgraphics2d.VectorGraphics2D;

import javax.swing.*;
import java.awt.*;
import java.awt.geom.*;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

public class PrintUtil {

    public enum Align {
        left,
        right,
    }

    public enum Orientation {
        up,
        right,
        down,
        left
    }

    static final int ICON_SIZE = 25;
    static final int DOUBLE_ICON_SIZE = ICON_SIZE * 2;
    static final int HEXAGON_ICON_SIZE = ICON_SIZE / 5 * 6;
    static final int SCALA_LINE_LENGTH = 15;
    static final int SCALA_SCALE_OFFSET = FontUtil.Standart.getSize() / 4;

    public static void printDiagrammLines(VectorGraphics2D vg, Rectangle2D dimension, StepContext conX, StepContext conY, Align alignAxis, String yAxis, String xAxis, Rectangle2D values) {
        printDiagrammLines(vg, dimension, conX, conY, alignAxis, yAxis, xAxis, values, true);
    }

    public static void printDiagrammLines(VectorGraphics2D vg, Rectangle2D dimension, StepContext conX, StepContext conY, Align alignAxis, String yAxis, String xAxis, Rectangle2D values, boolean xArrow) {

        Point2D startX;
        Point2D endX;

        Point2D startY;
        Point2D endY;

        if (values.getX() < 0 && values.getWidth() > 0) {

            double xpos = Util.map(0, values.getX(), values.getWidth(), dimension.getX(), dimension.getX() + dimension.getWidth());

            startY = new Point2D.Double(xpos, dimension.getY());
            endY   = new Point2D.Double(xpos, dimension.getY() + dimension.getHeight());


        } else {
            if (alignAxis == Align.left) {
                startY = new Point2D.Double(dimension.getX(), dimension.getY());
                endY = new Point2D.Double(dimension.getX(), dimension.getY() + dimension.getHeight());
            } else {
                startY = new Point2D.Double(dimension.getX() + dimension.getWidth(), dimension.getY());
                endY = new Point2D.Double(dimension.getX() + dimension.getWidth(), dimension.getY() + dimension.getHeight());
            }
        }

        startX = new Point2D.Double(dimension.getX(), dimension.getY() + dimension.getHeight());
        endX   = new Point2D.Double(dimension.getX() + dimension.getWidth(), dimension.getY() + dimension.getHeight());
        vg.draw(new Line2D.Double(startX, endX));
        vg.draw(new Line2D.Double(startY, endY));

        if (xArrow)
            printArrow(vg, endX, Orientation.right);
        printArrow(vg, startY, Orientation.up);

        if (conX != null && !scalarString(conX.digits).isEmpty()) {
            printTextOverPoint(vg, new Point2D.Double(dimension.getX() + dimension.getWidth(),
                    dimension.getY() + dimension.getHeight()), "in " + scalarString(conX.digits), SCALA_SCALE_OFFSET);
        }
        if (conY != null && !scalarString(conY.digits).isEmpty()) {
            printTextOverPoint(vg, new Point2D.Double(dimension.getX() + (alignAxis == Align.left ? 0 : dimension.getWidth()),
                    dimension.getY()), "in " + scalarString(conY.digits), SCALA_SCALE_OFFSET);
        }

        if (!yAxis.isEmpty()) {
            Point2D centerY = new Point2D.Double(dimension.getX() / 4,
                    dimension.getY() + dimension.getHeight() / 2);

            AffineTransform at = vg.getTransform();
            vg.translate(centerY.getX(), centerY.getY());
            vg.rotate(-Math.PI/2);
            printTextCenter(vg, new Point2D.Double(0.0,0.0), yAxis);
            vg.setTransform(at);
        }

        if (!xAxis.isEmpty()) {
            Point2D centerX = new Point2D.Double(dimension.getX() + dimension.getWidth() / 2,
                    dimension.getY() * 1.5 + dimension.getHeight());

            printTextCenter(vg, centerX, xAxis);
        }
    }

    public static void printBox(VectorGraphics2D vg, Rectangle2D dimension, double percent,  double start, double end, double min, double max, int id, double width) {
        Color c = vg.getColor();
        if (id < ColorUtil.COLORS.length )
            vg.setColor(ColorUtil.COLORS[id]);

        double height = Util.map(Math.abs(end - start), min, max, 0, dimension.getHeight());

        vg.fill(new Rectangle2D.Double(dimension.getX() + Util.map(percent, 0, 1, 0, dimension.getWidth()) - width / 2,
                                       dimension.getY() + Util.map(start, min, max, dimension.getHeight(), 0) - height,
                                       width,
                                       height));

        vg.setColor(c);
    }

    public static void printXAxisScala(VectorGraphics2D vg, Rectangle2D dimension, StepContext con, double min, double max, boolean printLine) {

    }

    public static void printYAxisScala(VectorGraphics2D vg, Rectangle2D dimension, StepContext con, double min, double max) {
        printYAxisScala(vg, dimension, con, min, max, true, Align.left);
    }

    public static void printYAxisScala(VectorGraphics2D vg, Rectangle2D dimension, StepContext con, double min, double max, boolean printLine, Align align) {
        double x    = dimension.getX() + (align == Align.left ? 0 : dimension.getWidth());
        double notX = dimension.getX() + (align != Align.left ? 0 : dimension.getWidth());
        double topY = dimension.getY();
        double botY = dimension.getY() + dimension.getHeight();
        //bottom start y, represents the linear scaling y start after min, first step may be not the same as the following
        double bsY  = Util.map(con.minStep, min, max, botY, topY);

        long factor = scalarFactor(con.digits);
        int lastIndex = con.steps + 1;
        double smin = con.minStep;

        double []scala   = new         double[con.steps + 2];
        Point2D []lefts  = new Point2D.Double[con.steps + 2];
        Point2D []rights = new Point2D.Double[con.steps + 2];

        scala[0]  = (min / factor);
        lefts[0]  = new Point2D.Double(x - SCALA_LINE_LENGTH, botY);
        rights[0] = new Point2D.Double(x + SCALA_LINE_LENGTH, botY);
        for (int i = 1; i < lastIndex; i++ ) {
            scala[i]  = (Util.map(i, 1, lastIndex, smin, max) / factor);
            //System.out.println("i: " + i + " lastIndex: " + lastIndex + " smin = " + smin + " max: " + max + " factor: " + factor);
            double curY = Util.map(i,1, lastIndex, bsY, topY);
            lefts[i]  = new Point2D.Double(x - SCALA_LINE_LENGTH, curY);
            rights[i] = new Point2D.Double(x + SCALA_LINE_LENGTH, curY);
        }
        scala[lastIndex]  = (max / factor);
        lefts[lastIndex]  = new Point2D.Double(x - SCALA_LINE_LENGTH, topY);
        rights[lastIndex] = new Point2D.Double(x + SCALA_LINE_LENGTH, topY);

        for(int i = 0; i <= lastIndex; i++) {
            vg.draw(new Line2D.Double(lefts[i], rights[i]));
            String value;
            //cast value to int, if no float
            if (scala[i] == Math.round(scala[i])) {
                value = "" + (int) scala[i];
            } else {
                value = "" + scala[i];
            }
            if (align == Align.left) {
                printTextLeftOfPoint(vg, lefts[i], value, dimension.getX() / 4);
            } else {
                printTextRightOfPoint(vg, rights[i], value, dimension.getX() / 4);
            }

            if (printLine) {
                Stroke s = vg.getStroke();
                vg.setStroke(new BasicStroke( Diagramm.STROKE_WIDTH / 3.0f));
                vg.draw(new Line2D.Double(new Point2D.Double(x, lefts[i].getY()), new Point2D.Double(notX, lefts[i].getY())));
                vg.setStroke(s);
            }
        }
    }

    public static void printSideboard(VectorGraphics2D vg, Rectangle2D sb_dim, HashMap<Integer, String> id_name) {
        printSideboard(vg, sb_dim, id_name, true);
    }

    public static void printSideboard(VectorGraphics2D vg, Rectangle2D sb_dim, HashMap<Integer, String> id_name, boolean icon) {
        assert(!id_name.isEmpty());
        LinkedList<Integer> order = new LinkedList<>();
        for (Map.Entry<Integer, String> entry : id_name.entrySet()) {
            if (!order.contains(entry.getKey()) && entry.getKey() != null)
                order.push(entry.getKey());
        }
        order.sort(new Comparator<Integer>() {
            @Override
            public int compare(Integer o1, Integer o2) {
                return o1.compareTo(o2);
            }
        });

        Point2D pIcon = new Point2D.Double(sb_dim.getX() + ((double) ICON_SIZE / 2), sb_dim.getY());
        int k = 0;
        for (Integer i : order ) {
            String s = id_name.get(i);
            Rectangle2D bounds = Util.getTextDimension(vg, s);
            Point2D p = new Point2D.Double(pIcon.getX(), pIcon.getY() + 2 * k * bounds.getHeight());
            Color c = vg.getColor();
            vg.setColor(ColorUtil.getColorOfId(i));
            if (icon) {
                IconUtil.printIcon(vg, p, i);
            } else {
                IconUtil.printIcon(vg, p, IconUtil.IconType.Square);
            }
            vg.setColor(c);
            printTextRightOfPoint(vg, p, s, 2 * ICON_SIZE);
            k++;
        }
    }

    private static void printTextLeftOfPoint(VectorGraphics2D vg, Point2D p, String text, double offset) {
        Rectangle2D bounds = Util.getTextDimension(vg, text);
        Point2D center = new Point2D.Double(p.getX() - offset - bounds.getWidth() / 2, p.getY());
        printTextCenter(vg, center, text);
    }

    private static void printTextRightOfPoint(VectorGraphics2D vg, Point2D p, String text, double offset) {
        Rectangle2D bounds = Util.getTextDimension(vg, text);
        Point2D center = new Point2D.Double(p.getX() + offset + bounds.getWidth() / 2, p.getY());
        printTextCenter(vg, center, text);
    }

    private static void printTextUnderPoint(VectorGraphics2D vg, Point2D p, String text, double offset) {
        Rectangle2D bounds = Util.getTextDimension(vg, text);
        Point2D center = new Point2D.Double(p.getX(), p.getY() + offset);
        printTextCenter(vg, center, text);
    }

    private static void printTextOverPoint(VectorGraphics2D vg, Point2D p, String text, double offset) {
        Rectangle2D bounds = Util.getTextDimension(vg, text);
        Point2D center = new Point2D.Double(p.getX(), p.getY() - offset - bounds.getHeight());
        printTextCenter(vg, center, text);
    }

    private static String scalarString(int digits) {
        if      (digits <= 3)
            return "";
        else if (digits <= 6)
            return "k";
        else if (digits <= 9)
            return "M";
        else if (digits <= 12)
            return "G";
        else if (digits <= 15)
            return "T";
        else if (digits <= 18)
            return "P";
        else
            return "E";
    }

    private static long scalarFactor(int digits) {
        if      (digits <= 3)
            return 1L;
        else if (digits <= 6)
            return 1_000L;
        else if (digits <= 9)
            return 1_000_000L;
        else if (digits <= 12)
            return 1_000_000_000L;
        else if (digits <= 15)
            return 1_000_000_000_000L;
        else if (digits <= 18)
            return 1_000_000_000_000_000L;
        else
            return 1_000_000_000_000_000_000L;
    }

    public static void printTextUnderXAxis(VectorGraphics2D vg, Rectangle2D dimension, String text, double percent) {
        Point2D pointOnAxis = new Point2D.Double(dimension.getX() + Util.map(Util.clip(percent, 0, 1), 0, 1, 0, dimension.getWidth()),
            dimension.getY() + dimension.getHeight());

        Rectangle2D dim = Util.getTextDimension(vg, text);
        //System.out.println(dim);

        Point2D textCenter = new Point2D.Double(pointOnAxis.getX() + dim.getHeight() / 2.0 - vg.getFontMetrics().getAscent(),
                // this /3 is incorrect, but looks at the moment at best...                     v
                pointOnAxis.getY() + vg.getClipBounds().getHeight() * Diagramm.PERCENT_2_5 + dim.getWidth() / 2);
        //System.out.println(textCenter.getY());
        AffineTransform af = vg.getTransform();
        vg.translate(textCenter.getX(), textCenter.getY());
        vg.rotate(-Math.PI/2);
        printTextCenter(vg, new Point2D.Double(0, 0), text);
        vg.setTransform(af);
    }

    public static void printTextCenter(VectorGraphics2D vg, Point2D p, String text) {
        Rectangle2D dim = Util.getTextDimension(vg, text);
        //drawString prints to the string at the lower left corner.
        vg.drawString(text, (float) (p.getX() - dim.getWidth() / 2.0), (float) (p.getY() - dim.getHeight() / 2.0) + vg.getFontMetrics().getAscent());
    }

    public static Point2D printPosition(Rectangle2D dimension, double percent, double y, double minY, double maxY) {
        return printPosition(dimension, percent, y, 0, 1,  minY, maxY);
    }

    public static Point2D printPosition(Rectangle2D dimension, double x, double y, double minX, double maxX, double minY, double maxY) {
        double clipX = Util.clip(x, minX, maxX);
        double clipY = Util.clip(y, minY, maxY);

        return new Point2D.Double(dimension.getX() + Util.map(clipX, minX, maxX, 0, dimension.getWidth()),
                                  dimension.getY() + Util.map(clipY, minY, maxY, dimension.getHeight(), 0));
    }


    //icons

    public static void printArrow(VectorGraphics2D vg, Point2D p) {
        printArrow(vg, p, Orientation.up);
    }

    public static void printArrow(VectorGraphics2D vg, Point2D p, Orientation o) {
        float[] xpoints = new float[3];
        float[] ypoints = new float[3];
        double rotation = getRotationFromOrientation(o);

        for (int c = 0; c < 3; c++) {
            double angle = findAngle((double) c / 3, rotation);
            Point point = findPoint(p, angle, HEXAGON_ICON_SIZE);
            xpoints[c] =  point.x;
            ypoints[c] = point.y;
        }
        Polygon2D tri = new Polygon2D(xpoints, ypoints, 3);
        //vg.draw(tri);
        vg.fill(tri);
    }

    public static void printYShape(VectorGraphics2D vg, Point2D p) {
        printYShape(vg, p , Orientation.up);
    }

    public static void printYShape(VectorGraphics2D vg, Point2D p, Orientation o) {
        double[] xpoints = new double[3];
        double[] ypoints = new double[3];

        double rotation = getRotationFromOrientation(o);

        for (int c = 0; c < 3; c++) {
            double angle = findAngle((double) c / 3, rotation);
            Point point = findPoint(p, angle, HEXAGON_ICON_SIZE);
            xpoints[c] =  point.x;
            ypoints[c] = point.y;
        }

        vg.draw(new Line2D.Double(p, new Point2D.Double(xpoints[0], ypoints[0])));
        vg.draw(new Line2D.Double(p, new Point2D.Double(xpoints[1], ypoints[1])));
        vg.draw(new Line2D.Double(p, new Point2D.Double(xpoints[2], ypoints[2])));
    }
    
    public static void printSquare(VectorGraphics2D vg, Point2D p) {
        Rectangle2D rec = new Rectangle2D.Double(p.getX() - ICON_SIZE, p.getY() - ICON_SIZE,
                2 * ICON_SIZE, 2 * ICON_SIZE);
        //vg.draw(rec);
        vg.fill(rec);
    }

    public static void printCross(VectorGraphics2D vg, Point2D p) {
        int s = ICON_SIZE;
        Point2D p1 = new Point2D.Double(p.getX() - s, p.getY() - s);
        Point2D p2 = new Point2D.Double(p.getX() + s, p.getY() + s);

        Point2D p3 = new Point2D.Double(p.getX() - s, p.getY() + s);
        Point2D p4 = new Point2D.Double(p.getX() + s, p.getY() - s);

        vg.draw(new Line2D.Double(p1, p2));
        vg.draw(new Line2D.Double(p3, p4));
    }

    public static void printCircle(VectorGraphics2D vg, Point2D p) {
        Ellipse2D eli = new Ellipse2D.Double(p.getX() - ICON_SIZE, p.getY() - ICON_SIZE, DOUBLE_ICON_SIZE, DOUBLE_ICON_SIZE);
        //vg.draw(eli);
        vg.fill(eli);
    }

    public static void printRing(VectorGraphics2D vg, Point2D p) {
        Ellipse2D eli = new Ellipse2D.Double(p.getX() - ICON_SIZE, p.getY() - ICON_SIZE, DOUBLE_ICON_SIZE, DOUBLE_ICON_SIZE);
        vg.draw(eli);
    }

    public static  void printHexagon(VectorGraphics2D vg, Point2D p) {
        printHexagon(vg, p, 0);
    }

    public static void printHexagon(VectorGraphics2D vg, Point2D p, double rotation) {
        float[] xpoints = new float[6];
        float[] ypoints = new float[6];
        for (int c = 0; c < 6; c++) {
            double angle = findAngle((double) c / 6, rotation);
            Point point = findPoint(p, angle, HEXAGON_ICON_SIZE);
            xpoints[c] = point.x;
            ypoints[c] = point.y;
        }

        Polygon2D poly = new Polygon2D(xpoints, ypoints, 6);
        vg.fill(poly);
    }

    public static void printStar(VectorGraphics2D vg, Point2D p) {
        printStar(vg, p, 0);
    }

    public static void printStar(VectorGraphics2D vg, Point2D p, double rotation) {
        for (int c = 0; c < 6; c++) {
            double angle = findAngle((double) c / 6, rotation);
            Point point = findPoint(p, angle, HEXAGON_ICON_SIZE);
            vg.draw(new Line2D.Double(p, point));
        }
    }

    private static double findAngle(double fraction, double rotation) {
        return fraction * Math.PI * 2 + Math.toRadians((rotation + 180) % 360);
    }

    private static Point findPoint(Point2D p, double angle, double radius) {
        int x = (int) (p.getX() + Math.cos(angle) * radius);
        int y = (int) (p.getY() + Math.sin(angle) * radius);

        return new Point(x, y);
    }

    private static double getRotationFromOrientation(Orientation o) {
        double rotation = 0;
        switch (o) {
            case up: {
                rotation = 90;
            } break;
            case right: {
                rotation = 180;
            } break;
            case down: {
                rotation = 270;
            } break;
            case left:
            default: {
                rotation = 0;
            }
        }
        return rotation;
    }

    public static class StepContext {
        public int steps;
        public double minStep;
        public int digits;

        public StepContext() {
            this(0, 0);
        }

        public StepContext(int steps, double minStep) {
            this(steps, minStep, 0);
        }

        public StepContext(int steps, double minStep, int digits) {
            this.steps = steps;
            this.minStep = minStep;
            this.digits = digits;
        }
    }

    public static StepContext getIntervals(int min, int max) {
        // if min and max negative and min << max, then this function may return wrong values
        StepContext con = new StepContext();
        String minStr = new String("" + Math.abs(min));
        String maxStr = new String("" + Math.abs(max));
        int digitsMax = maxStr.length();
        int digitsMin = minStr.length();

        con.digits = digitsMax;

        int maxChar = Integer.parseInt(String.valueOf(maxStr.charAt(0)));
        int minChar = Integer.parseInt(String.valueOf(minStr.charAt(0)));

        if (digitsMax - digitsMin == 0) {
            if (maxChar == 1) {
                con.steps = 4;
            } else if (maxChar == 2) {
                con.steps = 3;
            } else {
                con.steps = maxChar - minChar - 1;
            }
            con.minStep = min + (max - min) / (double)(con.steps + 1);
        } else if (digitsMax - digitsMin == 1) {
            int dig = maxChar * 10 - minChar;
            con.steps = 0;
            while(con.steps == 0) {
                for (int i = 10; i > 1; i--) {
                    if (dig % i == 0) {
                        con.steps = i;
                        break;
                    }
                }
                dig--;
            }

            con.steps-=1;
            con.minStep = max - ((max - min) * con.steps) / (double)(con.steps + 1);
        } else { // ignore min
            if (maxChar == 1) {
                con.steps = 4;
            } else if (maxChar == 2) {
                con.steps = 3;
            } else {
                con.steps = maxChar - 1;
            }
            con.minStep = max / (double) (con.steps + 1);
        }
        return con;
    }
}
