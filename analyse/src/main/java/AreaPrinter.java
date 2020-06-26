import de.erichseifert.vectorgraphics2d.VectorGraphics2D;

import java.awt.*;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.util.*;
import java.util.function.Consumer;

public class AreaPrinter {

    boolean transparent;
    boolean finish;
    HashMap<Integer, LinkedList<Polygon2D>> prints;
    HashMap<Integer, LinkedList<Point2D>> current;
    HashSet<Integer> step;
    float single_offset;

    public AreaPrinter(boolean trans) {
        transparent = trans;
        prints = new HashMap<>();
        current = new HashMap<>();
        step = new HashSet<>();
        finish = false;
        single_offset = 10.f;
    }

    public void addPoint(VectorGraphics2D vg, int id, Point2D p) throws IllegalAccessException {
        if (finish) {
            throw new IllegalAccessException("AreaPrinter is Finished, can't add any Points after called finish");
        }
        if (!current.containsKey(id)) {
            current.put(id, new LinkedList<>());
        }
        current.get(id).push(p);
        step.add(id);
    }

    public void setSingleOffset(float offset) {
        single_offset = offset;
    }

    public void step(Rectangle2D dim) {
        double y = dim.getY() + dim.getHeight();
        HashSet<Integer> remove = new HashSet<>();
        for(Map.Entry<Integer, LinkedList<Point2D>> entry : current.entrySet()) {
            Integer id = entry.getKey();
            if (!step.contains(entry.getKey())) {
                remove.add(entry.getKey());
                LinkedList<Point2D> points = entry.getValue();
                boolean single = points.size() == 1;

                int size = points.size() + 2;
                float xs[] = new float[size];
                float ys[] = new float[size];

                Point2D p = points.getFirst();
                xs[0] = (float) p.getX() - single_offset;
                ys[0] = (float) y;
                Iterator<Point2D> it = points.iterator();
                int i = 0;
                while(it.hasNext()) {
                    i++; //start at 1;
                    p = it.next();
                    xs[i] = (float) p.getX();
                    ys[i] = (float) p.getY();
                }
                p = points.getLast();
                xs[size - 1] = (float) p.getX() + single_offset;
                ys[size - 1] = (float) y;

                if (!prints.containsKey(id)) {
                    prints.put(id, new LinkedList<>());
                }
                prints.get(id).push(new Polygon2D(xs, ys, size));
            }
        }
        for (Integer id : remove) {
            current.remove(id);
        }
        step.clear();
    }

    public void finish(VectorGraphics2D vg, Rectangle2D dim) {
        step(dim);
        step(dim);
        finish = true;
        print(vg);
    }

    private void print(VectorGraphics2D vg) {
        Color c = vg.getColor();
        LinkedList<Integer> ids = new LinkedList<>(prints.keySet());
        Collections.sort(ids, new Comparator<Integer>() {
            @Override
            public int compare(Integer o1, Integer o2) {
                return o2.compareTo(o1);
            }
        });
        for(Integer id : ids) {
            vg.setColor(transparent ? ColorUtil.getTransparentColorOfId(id) : ColorUtil.getColorOfId(id));
            for(Polygon2D poly : prints.get(id)) {
                vg.fill(poly);
            }
        }
        vg.setColor(c);
    }
}
