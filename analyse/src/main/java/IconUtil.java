import de.erichseifert.vectorgraphics2d.VectorGraphics2D;

import java.awt.*;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Point2D;

public class IconUtil {

    public enum IconType {
        Triangle,
        Square,
        Circle,
        Hexagon,

        YShape,
        Cross,
        Ring,
        Star;

        static int ICON_TYPE_NUMBER = values().length;
    }

    public enum IconColor {

    }

    public static void printIconColor(VectorGraphics2D vg, Point2D p, int id) {
        Color tempc = vg.getColor();
        vg.setColor(ColorUtil.getColorOfId(id));
        printIcon(vg, p, id);
        vg.setColor(tempc);
    }

    public static void printIcon(VectorGraphics2D vg, Point2D p, int id) {
        if (id < IconType.ICON_TYPE_NUMBER) {
            printIcon(vg, p, IconType.values()[id]);
        } else {
            throw new IndexOutOfBoundsException("id: " + id + "is unknown");
        }
    }

    public static void printIcon(VectorGraphics2D vg, Point2D p, IconType it) {
        Stroke temps = vg.getStroke();
        vg.setStroke(new BasicStroke(10));
        switch (it) {
            case Triangle: {
                PrintUtil.printArrow(vg, p);
            } break;
            case YShape: {
                PrintUtil.printYShape(vg, p);
            } break;
            case Square: {
                PrintUtil.printSquare(vg, p);
            } break;
            case Cross: {
                PrintUtil.printCross(vg, p);
            } break;
            case Circle: {
                PrintUtil.printCircle(vg, p);
            } break;
            case Ring: {
                PrintUtil.printRing(vg, p);
            } break;
            case Hexagon: {
                PrintUtil.printHexagon(vg, p);
            } break;
            case Star: {
                PrintUtil.printStar(vg, p);
            }
        }
        vg.setStroke(temps);
    }
}
