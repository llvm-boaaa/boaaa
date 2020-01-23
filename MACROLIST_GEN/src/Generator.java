import java.lang.*;

public class Generator {

    //macro
    static final String MACRO_PREFIX                    = ""; //""BOAAA_MACROLIST_";
    static final String MACROLIST_HEADER                = MACRO_PREFIX + "MACROLIST_H";
    static final String LIST_VER                        = MACRO_PREFIX + "LIST_VER";
    static final String LIST_VER_MAX                    = MACRO_PREFIX + "LIST_VER_MAX";
    static final String MACROLIST_ERROR_LOG             = MACRO_PREFIX + "ERROR_LOG";

    static final String EVAL                            = MACRO_PREFIX + "EVAL";

    static final String LIST_INCLUDE_0                  = MACRO_PREFIX + "LIST_INCLUDE_0";
    static final String LIST_INCLUDE_1                  = MACRO_PREFIX + "LIST_INCLUDE_1";
    static final String LIST_INCLUDE_2                  = MACRO_PREFIX + "LIST_INCLUDE_2";

    static final String LIST_INCLUDE_0_STORE_XXX        = MACRO_PREFIX + "LIST_INCLUDE_0_STORE_";
    static final String LIST_INCLUDE_1_STORE_XXX        = MACRO_PREFIX + "LIST_INCLUDE_1_STORE_";
    static final String LIST_INCLUDE_2_STORE_XXX        = MACRO_PREFIX + "LIST_INCLUDE_2_STORE_";

    static final String LIST_MACRO_STORE_0              = MACRO_PREFIX + "LIST_MACRO_STORE_0";
    static final String LIST_MACRO_STORE_1              = MACRO_PREFIX + "LIST_MACRO_STORE_1";
    static final String LIST_MACRO_STORE_2              = MACRO_PREFIX + "LIST_MACRO_STORE_2";

    static final String LIST_MACRO_STORE_0_XXX          = MACRO_PREFIX + "LIST_MACRO_STORE_0_";
    static final String LIST_MACRO_STORE_1_XXX          = MACRO_PREFIX + "LIST_MACRO_STORE_1_";
    static final String LIST_MACRO_STORE_2_XXX          = MACRO_PREFIX + "LIST_MACRO_STORE_2_";

    //macro static
    static final String IF          = "#if     ";
    static final String IFDEF       = "#ifdef  ";
    static final String IFNDEF      = "#ifndef ";
    static final String DEFINE      = "#define ";
    static final String ENDIF       = "#endif  ";
    static final String UNDEF       = "#undef  ";
    static final String INCLUDE     = "#include ";

    static final String LF          = "\n";

    //ints
    static final int LIST_SIZE    = 1000;
    static final int PADDING_SIZE = (int) Math.log10(LIST_SIZE + 1) + 1;


    //filenames
    static final String PROJECT_FOULDER                 = "boaaa/lv/macrolist/";

    static final String MACROLIST_DEF                   = "MACROLIST_DEF.h";

    static final String MACROLIST_STORE_inc             = "MACROLIST_DEF.inc";

    static final String MACROLIST_EVAL_inc              = "MACROLIST_EVAL.inc";
    static final String MACROLIST_EVAL2_inc             = "MACROLIST_EVAL2.inc";

    static final String MACROLIST_UNDEF_inc             = "MACROLIST_UNDEF.inc";

    //filecontent
    static final String MACROLIST_EVAL2_CONTENT         = INCLUDE + "\"" + PROJECT_FOULDER +"MACROLIST_EVAL.inc\"";



    public static void main(String[] args) {
        System.out.println(generateMACROLIST_STORE());
    }

    public static String generateMACROLIST_STORE() {
        String out = "";
        for(int i = 0; i < LIST_SIZE; i++) {
            out += ifDoEndif( LIST_VER + " == " + generatePaddedInteger(i, PADDING_SIZE), true,
                    DEFINE + LIST_VER + " "   + generatePaddedInteger(i + 1, PADDING_SIZE)     + LF,
                    ifdefDoEndif( LIST_INCLUDE_0,
                            DEFINE + LIST_INCLUDE_0_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE)
                                    + " " + eval(LIST_INCLUDE_0)                                            + LF),
                    ifdefDoEndif( LIST_INCLUDE_1,
                            DEFINE + LIST_INCLUDE_1_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE)
                                    + " " + eval(LIST_INCLUDE_1)                                            + LF),
                    ifdefDoEndif( LIST_INCLUDE_2,
                            DEFINE + LIST_INCLUDE_2_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE)
                                    + " " + eval(LIST_INCLUDE_1)                                            + LF),

                    LF,

                    ifdefDoEndif(LIST_MACRO_STORE_0,
                            DEFINE + LIST_MACRO_STORE_0_XXX + generatePaddedInteger(i, PADDING_SIZE)
                                    + " " + eval(LIST_MACRO_STORE_0)                                         + LF),
                    ifdefDoEndif(LIST_MACRO_STORE_1,
                            DEFINE + LIST_MACRO_STORE_1_XXX + generatePaddedInteger(i, PADDING_SIZE)
                                    + " " + eval(LIST_MACRO_STORE_1)                                         + LF),
                    ifdefDoEndif(LIST_MACRO_STORE_2,
                            DEFINE + LIST_MACRO_STORE_2_XXX + generatePaddedInteger(i, PADDING_SIZE)
                                    + " " + eval(LIST_MACRO_STORE_2)                                         + LF),
                    ""
            );
            out += LF;
        }

        out += ifDoEndif(LIST_VER + " == " + generatePaddedInteger(LIST_SIZE, PADDING_SIZE), true,
                IFNDEF + MACROLIST_ERROR_LOG                                                        + LF,
                DEFINE + MACROLIST_ERROR_LOG                                                            + LF,
                "namespace {"                                                                           + LF,
                "constexpr int macrolist_error() {"                                                     + LF,
                "    static_assert(\"MACROLIST-ERROR: Number of List-elements to huge, "                +
                        "generate new list with more capacity!\");"                                     + LF,
                "    return 0;"                                                                         + LF,
                "}"                                                                                     + LF,
                "const int macrolist_error = macrolist_error();"                                        + LF,
                "}"                                                                                     + LF,
                ENDIF                                                                                   + LF
                );


        return out;
    }

    public static String generateMACROLIST_EVAL() {
        String out = "";
        out += ifndefDoEndif(LIST_VER_MAX, true,
                    DEFINE + LIST_VER_MAX + " " + generatePaddedInteger(LIST_SIZE - 1, PADDING_SIZE)
                );
        for(int i = 0; i < LIST_SIZE; i++) {

        }

        out += IF + LIST_VER + " < " + LIST_VER_MAX                                                     + LF;
        out += INCLUDE
        out += ENDIF                                                                                    + LF;
        return out;
    }

    public static String generateMACROLIST_UNDEF() {
        String out = "";
        //generell

        //LIST
        for(int i = 0; i < LIST_SIZE; i++) {
            out += "// " + LIST_VER + " == " + generatePaddedInteger(i, PADDING_SIZE)                   + LF;
            out += UNDEF + LIST_INCLUDE_0_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE)           + LF;
            out += UNDEF + LIST_INCLUDE_1_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE)           + LF;
            out += UNDEF + LIST_INCLUDE_1_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE)           + LF;
            out +=                                                                                       LF;
            out += UNDEF + LIST_MACRO_STORE_0_XXX   + generatePaddedInteger(i, PADDING_SIZE)           + LF;
            out += UNDEF + LIST_MACRO_STORE_1_XXX   + generatePaddedInteger(i, PADDING_SIZE)           + LF;
            out += UNDEF + LIST_MACRO_STORE_2_XXX   + generatePaddedInteger(i, PADDING_SIZE)           + LF;
            out +=                                                                                       LF;
        }
        return out;
    }

    public static String generateMACROLIST_DEF() {
        String out = "";
        out += IFNDEF + MACROLIST_HEADER                                                                + LF;
        out += DEFINE + MACROLIST_HEADER                                                                + LF;
        out +=                                                                                            LF;
        out += DEFINE + EVAL + "(X) X"                                                                  + LF;


        out += ENDIF                                                                                    + LF;
        return out;
    }

    public static String generatePaddedInteger(int number, int padding) {
        String s = "";
        int i = 0;
        int n = number;
        char c;
        while (n != 0)
        {
            i++;
            c = (char) ("0".codePointAt(0) + n % 10);
            n /= 10;
            s = c + s;
        }
        if (padding <= i)
            System.out.println("PADDING ERROR, check file!");
        for(; i < padding; i++)
            s = '0' + s;
        return s;
    }

    //generators

    public static  String ifDoEndif(String var, String ...in) {
        return ifDoEndif(var, false, in);
    }

    public static String ifDoEndif(String var,  boolean comment, String ...in) {
        String out = "";
        out += IF + var                                                                              + LF;
        for(String s : in)
            out += s;
        out += ENDIF + (comment ? " // !" + var : "")                                                   + LF;
        return out;
    }

    public static  String ifdefDoEndif(String var, String ...in) {
        return ifdefDoEndif(var, false, in);
    }

    public static String ifdefDoEndif(String var,  boolean comment, String ...in) {
        String out = "";
        out += IFDEF + var                                                                              + LF;
        for(String s : in)
            out += s;
        out += ENDIF + (comment ? " // !" + var : "")                                                   + LF;
        return out;
    }

    public static  String ifndefDoEndif(String var, String ...in) {
        return ifndefDoEndif(var, false, in);
    }

    public static String ifndefDoEndif(String var,  boolean comment, String ...in) {
        String out = "";
        out += IFNDEF + var                                                                              + LF;
        for(String s : in)
            out += s;
        out += ENDIF + (comment ? " // !" + var : "")                                                   + LF;
        return out;
    }

    public static String eval(String in) {
        return EVAL + "( " + in + " )";
    }
}
