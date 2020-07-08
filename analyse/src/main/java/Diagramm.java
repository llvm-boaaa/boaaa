import de.erichseifert.vectorgraphics2d.*;

import java.awt.*;

public abstract class Diagramm {

    public static final float STROKE_WIDTH = 15.0f;

    public static final double PERCENT_2 = 0.02;
    public static final double PERCENT_2_5 = 0.025;
    public static final double PERCENT_3 = 0.03;
    public static final double PERCENT_5 = 0.05;
    public static final double PERCENT_7 = 0.07;
    public static final double PERCENT_10 = 0.1;
    public static final double PERCENT_15 = 0.15;
    public static final double PERCENT_80 = 0.8;
    public static final double PERCENT_85 = 0.85;
    public static final double PERCENT_90 = 0.9;
    public static final double PERCENT_100 = 1.0;

    public static final PrintUtil.StepContext PERCENT_STEP_CONTEXT = new PrintUtil.StepContext(4,20, 3);

    protected int m_width;
    protected int m_height;
    protected String m_xAxisText;
    protected String m_yAxisText;


    public Diagramm(int width, int height) {
        m_width = width;
        m_height = height;
        m_xAxisText = "";
        m_yAxisText = "";
    }

    Diagramm addXAxisText(String s) {
        m_xAxisText = s == null ? "" : s; return this;
    }

    Diagramm addYAxisText(String s) {
        m_yAxisText = s == null ? "" : s; return this;
    }

    abstract void printToVG(VectorGraphics2D vg);

    VectorGraphics2D generateVG() {
        VectorGraphics2D  vg = new VectorGraphics2D();
        vg.setClip(0, 0, m_width, m_height);
        vg.setBackground(Color.WHITE);
        vg.setFont(FontUtil.Standart.deriveFont(Font.PLAIN, (float) (Math.min(m_width, m_height) / 30.0)));
        vg.setStroke(new BasicStroke(STROKE_WIDTH));
        printToVG(vg);
        return vg;
    }

    public static Diagramm getDiagramm(DiagrammType type, int width, int height) {
        switch(type) {
            case StringLineChart:
                return new StringLineChart(width, height);
            default:
                System.out.println("Diagram type not implemented");
                return null;
        }
    }
}
