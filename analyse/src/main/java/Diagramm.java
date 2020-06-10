import de.erichseifert.vectorgraphics2d.*;

import java.awt.*;

public abstract class Diagramm {

    public static final float STROKE_WIDTH = 15.0f;

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

    void addXAxisText(String s) {
        m_xAxisText = s == null ? "" : s;
    }

    void addYAxisText(String s) {
        m_yAxisText = s == null ? "" : s;
    }

    abstract void printToVG(VectorGraphics2D vg);

    VectorGraphics2D generateVG() {
        VectorGraphics2D  vg = new VectorGraphics2D();
        vg.setClip(0, 0, m_width, m_height);
        vg.setBackground(Color.WHITE);
        vg.setFont(FontUtil.Standart.deriveFont(Font.PLAIN, (float) (Math.min(m_width, m_height) / 40.0)));
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
