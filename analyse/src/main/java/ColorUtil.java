import java.awt.*;

public class ColorUtil {

    static final Color[] COLORS = new Color[]{
            new Color(24, 63, 99),
            new Color(70, 97, 32),
            new Color(71,12, 33),
            new Color(176, 149, 25),
            new Color(46, 39, 64),
            new Color(176, 82, 33),

            new Color(42, 111, 176),
            new Color(125, 173, 57),
            new Color(148, 25, 68),
            new Color(252, 213, 35),
            new Color(101, 86, 140),
            new Color(252, 120, 48)
    };

    public static Color getColorOfId(int id) {
        if (id < COLORS.length) {
            return COLORS[id];
        }
        throw new IndexOutOfBoundsException("color with id: " + id + "is unknown!");
    }

    public static Color getTransparentColorOfId(int id) {
        Color c = getColorOfId(id);

        return new Color(c.getRed(), c.getGreen(), c.getBlue(), 164);
    }
}
