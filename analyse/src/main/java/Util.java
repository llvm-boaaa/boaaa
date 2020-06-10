import de.erichseifert.vectorgraphics2d.VectorGraphics2D;

import java.awt.geom.Rectangle2D;

public class Util {

    public static double clip(double v, double min, double max) {
        return Math.min(Math.max(v, min), max);
    }

    public static double map(double v, double cmin, double cmax, double nmin, double nmax) {
        if (cmax - cmin == 0) {
            return clip(v, nmin, nmax);
        }
        return (v - cmin) / (cmax - cmin) * (nmax - nmin) + nmin;
    }


    public static double roundToNextStep(double d) {
        if (d == 0.0) return 0.0;

        double result = d < 0 ? -1.0 : 1.0;
        while (d > 1.0) {
            if (d > 10) {
                result *= 10.0;
                d /= 10.0;
            } else if (d > 2) {
                result *= 3;
                d /= 3.0;
            } else if (d > 1) {
                result *= 2.0;
                d /= 2.0;
            }
        }
        return result;
    }

    public static double roundToBeforeStep(double d) {
        if (d == 0.0) return 0.0;

        double result = d < 0 ? -1.0 : 1.0;
        while (d > 2.0) {
            if (d > 10) {
                result *= 10.0;
                d /= 10.0;
            } else if (d > 5.0) {
                result *= 5.0;
                d /= 5.0;
            } else if (d > 2.0) {
                result *= 2.0;
                d /= 2.0;
            }
        }
        return result;
    }

    public static Rectangle2D getTextDimension(VectorGraphics2D vg, String text) {
        return vg.getFontMetrics().getStringBounds(text, vg);
    }
}
