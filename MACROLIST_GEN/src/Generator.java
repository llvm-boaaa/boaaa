import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.lang.*;

public class Generator {

    //macro
    static final String MACRO_PREFIX                    = "BOAAA_MACROLIST_";
    static final String MACROLIST_HEADER                = MACRO_PREFIX + "MACROLIST_H";
    static final String LIST_VER                        = MACRO_PREFIX + "LIST_VER";
    static final String LIST_VER_MAX                    = MACRO_PREFIX + "LIST_VER_MAX";
    static final String MACROLIST_ERROR_LOCK            = MACRO_PREFIX + "ERROR_LOCK";
    static final String MACROLIST_EVAL_LOCK             = MACRO_PREFIX + "EVAL_LOCK";

    static final String PUSP_MACRO_INCLUDE              = MACRO_PREFIX + "PUSP_MACRO_INCLUDE";
    static final String POP_MACRO_INCLUDE               = MACRO_PREFIX + "POP_MACRO_INCLUDE";

    static final String EVAL                            = MACRO_PREFIX + "EVAL";
    static final String EVAL_                           = MACRO_PREFIX + "EVAL_";
    static final String UNDEF_MACRO                     = MACRO_PREFIX + "UNDEF";

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

    static final String MACRO_FUNC_0                    = MACRO_PREFIX + "MACRO_FUNC_0";
    static final String MACRO_FUNC_1                    = MACRO_PREFIX + "MACRO_FUNC_1";
    static final String MACRO_FUNC_2                    = MACRO_PREFIX + "MACRO_FUNC_2";

    //macro static
    static final String IF          = "#if     ";
    static final String IFDEF       = "#ifdef  ";
    static final String NOT         = "not     ";
    static final String IFNDEF      = "#ifndef ";
    static final String DEFINE      = "#define ";
    static final String ENDIF       = "#endif  ";
    static final String UNDEF       = "#undef  ";
    static final String INCLUDE     = "#include ";
    static final String ELSE        = "#else   ";
    static final String PRAGMA_PUSP = "#pragma push_macro";
    static final String PRAGMA_POP  = "#pragma pop_macro";

    static final String LF          = "\n";

    static final int inlineLogBase8(int val) {
        int i = 0;
        while(val > 0) {
            val /= 8;
            i++;
        }
        return i;
    }

    //ints
    static final int LIST_SIZE    = 0776; //510
    static final int PADDING_SIZE = (int) Math.log10(LIST_SIZE + 1) + 2;


    //filenames
    static final String PROJECT_FOULDER                 = "boaaa/lv/macrolist/";

    static final String MACROLIST_DEF                   = "MACROLIST_DEF.h";

    static final String MACROLIST_STORE_inc             = "MACROLIST_STORE.inc";

    static final String MACROLIST_EVAL_inc              = "MACROLIST_EVAL.inc";
    static final String MACROLIST_EVAL2_inc             = "MACROLIST_EVAL2.inc";

    static final String MACROLIST_UNDEF_inc             = "MACROLIST_UNDEF.inc";

    static final String MACROLIST_PUSH_MACROS           = "MACROLIST_pushMacros.inc";
    static final String MACROLIST_POP_MACROS            = "MACROLIST_popMacros.inc";

    //filecontent
    static final String MACROLIST_EVAL2_CONTENT         = include(PROJECT_FOULDER + MACROLIST_EVAL_inc);



    public static void main(String[] args) {
        File dir = new File(PROJECT_FOULDER);
        if(!dir.exists())
            dir.mkdirs();
        //System.out.println(generateMACROLIST_EVAL());
        writeStringToFile(PROJECT_FOULDER + MACROLIST_DEF, generateMACROLIST_DEF());
        writeStringToFile(PROJECT_FOULDER + MACROLIST_STORE_inc, generateMACROLIST_STORE());
        writeStringToFile(PROJECT_FOULDER + MACROLIST_EVAL_inc, generateMACROLIST_EVAL());
        writeStringToFile(PROJECT_FOULDER + MACROLIST_EVAL2_inc, MACROLIST_EVAL2_CONTENT);
        writeStringToFile(PROJECT_FOULDER + MACROLIST_UNDEF_inc, generateMACROLIST_UNDEF());
        writeStringToFile(PROJECT_FOULDER + MACROLIST_PUSH_MACROS, generateMACROLIST_PUSH());
        writeStringToFile(PROJECT_FOULDER + MACROLIST_POP_MACROS, generateMACROLIST_POP());
    }

    public static String generateMACROLIST_STORE() {
        String out = "";
        for(int i = 0; i < LIST_SIZE; i++) {
            out += ifDoEndif( LIST_VER + " == " + generatePaddedInteger(i, PADDING_SIZE), true,
                    UNDEF + LIST_VER                                                                    +LF,
                    DEFINE + LIST_VER + " "   + generatePaddedInteger(i + 1, PADDING_SIZE)          + LF,
                    ifdefDoEndif( LIST_INCLUDE_0,
                            DEFINE + LIST_INCLUDE_0_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE)
                                    + " " + LIST_INCLUDE_0                                            + LF),
                    ifdefDoEndif( LIST_INCLUDE_1,
                            DEFINE + LIST_INCLUDE_1_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE)
                                    + " " + LIST_INCLUDE_1                                            + LF),
                    ifdefDoEndif( LIST_INCLUDE_2,
                            DEFINE + LIST_INCLUDE_2_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE)
                                    + " " + LIST_INCLUDE_1                                            + LF),

                    LF,

                    ifdefDoEndif(LIST_MACRO_STORE_0,
                            DEFINE + LIST_MACRO_STORE_0_XXX + generatePaddedInteger(i, PADDING_SIZE)
                                    + " " + LIST_MACRO_STORE_0                                         + LF),
                    ifdefDoEndif(LIST_MACRO_STORE_1,
                            DEFINE + LIST_MACRO_STORE_1_XXX + generatePaddedInteger(i, PADDING_SIZE)
                                    + " " + LIST_MACRO_STORE_1                                         + LF),
                    ifdefDoEndif(LIST_MACRO_STORE_2,
                            DEFINE + LIST_MACRO_STORE_2_XXX + generatePaddedInteger(i, PADDING_SIZE)
                                    + " " + LIST_MACRO_STORE_2                                         + LF),
                    ""
            );
            out += LF;
        }

        out += ifDoEndif(LIST_VER + " == " + generatePaddedInteger(LIST_SIZE, PADDING_SIZE), true,
                IFNDEF + MACROLIST_ERROR_LOCK                                                        + LF,
                DEFINE + MACROLIST_ERROR_LOCK                                                            + LF,
                "namespace {"                                                                           + LF,
                "constexpr int macrolist_error() {"                                                     + LF,
                "    static_assert(\"MACROLIST-ERROR: Number of List-elements to huge, "                +
                        "generate new list with more capacity!\");"                                     + LF,
                "    return 0;"                                                                         + LF,
                "}"                                                                                     + LF,
                "const int val_macrolist_error = macrolist_error();"                                    + LF,
                "}"                                                                                     + LF,
                ENDIF                                                                                   + LF
                );
        out +=                                                                                            LF;
        out += "// store"                                                                               + LF;
        out += INCLUDE + PUSP_MACRO_INCLUDE                                                             + LF;
        out += include(MACROLIST_PUSH_MACROS)                                                           + LF;
        out +=                                                                                            LF;
        out += "// undef"                                                                               + LF;
        out += UNDEF + LIST_INCLUDE_0                                                                   + LF;
        out += UNDEF + LIST_INCLUDE_1                                                                   + LF;
        out += UNDEF + LIST_INCLUDE_2                                                                   + LF;
        out += UNDEF + LIST_MACRO_STORE_0                                                               + LF;
        out += UNDEF + LIST_MACRO_STORE_1                                                               + LF;
        out += UNDEF + LIST_MACRO_STORE_2                                                               + LF;
        return out;
    }

    public static String generateMACROLIST_EVAL() {
        String out = "";
        out += IFNDEF + MACROLIST_EVAL_LOCK                                                              + LF;
        out += DEFINE + MACROLIST_EVAL_LOCK                                                              + LF;
        out += ifndefDoEndif(LIST_VER_MAX, true,
                DEFINE + LIST_VER_MAX + " " + generatePaddedInteger(LIST_SIZE - 1, PADDING_SIZE) + LF
        );
        out += include(PROJECT_FOULDER + MACROLIST_EVAL2_inc)                                       + LF;
        out += UNDEF + MACROLIST_EVAL_LOCK                                                               + LF;
        out += include(PROJECT_FOULDER + MACROLIST_UNDEF_inc)                                       + LF;
        out += UNDEF + MACRO_FUNC_0                                                                      + LF;
        out += UNDEF + MACRO_FUNC_1                                                                      + LF;
        out += UNDEF + MACRO_FUNC_2                                                                      + LF;
        out += UNDEF + LIST_INCLUDE_0                                                                   + LF;
        out += UNDEF + LIST_INCLUDE_1                                                                   + LF;
        out += UNDEF + LIST_INCLUDE_2                                                                   + LF;
        out += UNDEF + LIST_MACRO_STORE_0                                                               + LF;
        out += UNDEF + LIST_MACRO_STORE_1                                                               + LF;
        out += UNDEF + LIST_MACRO_STORE_2                                                               + LF;
        out +=                                                                                            LF;
        out += ELSE                                                                                      + LF;
        out +=                                                                                            LF;
        out += INCLUDE + POP_MACRO_INCLUDE                                                              + LF;
        out += include(MACROLIST_POP_MACROS)                                                            + LF;
        out +=                                                                                            LF;
        for (int i = 0; i < LIST_SIZE; i++) {
            out += ifDoEndif(LIST_VER + " == " + generatePaddedInteger(i, PADDING_SIZE), true,
                    UNDEF + LIST_VER                                                                   + LF,
                    DEFINE + LIST_VER + " " + generatePaddedInteger(i + 1, PADDING_SIZE)            + LF,
                    ifdefDoEndif(LIST_INCLUDE_0_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE),
                            INCLUDE + LIST_INCLUDE_0_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE) + LF
                            ),
                        ifdefDoEndif(LIST_INCLUDE_1_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE),
                        INCLUDE + LIST_INCLUDE_1_STORE_XXX + generatePaddedInteger(i,PADDING_SIZE)      + LF
                            ),
                        ifdefDoEndif(LIST_INCLUDE_2_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE),
                             INCLUDE + LIST_INCLUDE_2_STORE_XXX + generatePaddedInteger(i, PADDING_SIZE) + LF
                                ),
                        LF,
                        ifdefDoEndif(MACRO_FUNC_0, true,
                                applyMacro(MACRO_FUNC_0, LIST_MACRO_STORE_0_XXX + generatePaddedInteger(i, PADDING_SIZE)) + LF,
                                    ELSE                                                                            + LF,
                                    LIST_MACRO_STORE_0_XXX + generatePaddedInteger(i, PADDING_SIZE)                 + LF
                                ),
                    ifdefDoEndif(MACRO_FUNC_1, true,
                                    applyMacro(MACRO_FUNC_1, LIST_MACRO_STORE_1_XXX + generatePaddedInteger(i, PADDING_SIZE)) + LF,
                                    ELSE                                                                            + LF,
                                    LIST_MACRO_STORE_1_XXX + generatePaddedInteger(i, PADDING_SIZE)                 + LF
                                ),
                    ifdefDoEndif(MACRO_FUNC_2, true,
                                applyMacro(MACRO_FUNC_2, LIST_MACRO_STORE_2_XXX + generatePaddedInteger(i, PADDING_SIZE)) + LF,
                                    ELSE                                                                            + LF,
                                    LIST_MACRO_STORE_2_XXX + generatePaddedInteger(i, PADDING_SIZE)                 + LF
                                )
                    )                                                                                               + LF;
        }

        out += ifDoEndif(LIST_VER + " < " + LIST_VER_MAX, true,
                include( PROJECT_FOULDER + MACROLIST_EVAL2_inc )                                        + LF
                );
        out += ENDIF + " //!" + MACROLIST_EVAL_LOCK                                                               +LF;
        return out;
    }

    public static String generateMACROLIST_UNDEF() {
        String out = "";
        //generell
        out += UNDEF + LIST_VER                                                                         + LF;
        out += DEFINE + LIST_VER + " " + generatePaddedInteger(0, PADDING_SIZE)                 + LF;
        out += UNDEF + LIST_VER_MAX                                                                     + LF;

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
        out += DEFINE + EVAL_ + "(X) X"                                                                  + LF;
        out += DEFINE + EVAL + "(X) " + EVAL_ + "(X)"                                                   + LF;
        out +=                                                                                            LF;
        out += DEFINE + UNDEF_MACRO + " 0"                                                              + LF;

        out += ENDIF                                                                                    + LF;
        return out;
    }

    public static String generateMACROLIST_PUSH() {
        String out = "";
        /*out += ifndefDoEndif(LIST_INCLUDE_0,
                DEFINE + LIST_INCLUDE_0 + " " + UNDEF_MACRO                                         + LF
        );*/
        out += pusp_macro(LIST_INCLUDE_0)                                                               +LF;

        /*out += ifndefDoEndif(LIST_INCLUDE_1,
                DEFINE + LIST_INCLUDE_1 + " " + UNDEF_MACRO                                         + LF
        );*/
        out += pusp_macro(LIST_INCLUDE_1)                                                               +LF;

        /*out += ifndefDoEndif(LIST_INCLUDE_2,
                DEFINE + LIST_INCLUDE_2 + " " + UNDEF_MACRO                                         + LF
        );*/
        out += pusp_macro(LIST_INCLUDE_2)                                                               +LF;

        /*out += ifndefDoEndif(LIST_MACRO_STORE_0,
                DEFINE + LIST_MACRO_STORE_0 + " " + UNDEF_MACRO                                    + LF
        );*/
        out += pusp_macro(LIST_MACRO_STORE_0)                                                           +LF;

        /*out += ifndefDoEndif(LIST_MACRO_STORE_1,
                DEFINE + LIST_MACRO_STORE_1 + " " + UNDEF_MACRO                                    + LF
        );*/
        out += pusp_macro(LIST_MACRO_STORE_1)                                                           +LF;

        /*out += ifndefDoEndif(LIST_MACRO_STORE_2,
                DEFINE + LIST_MACRO_STORE_2 + " " + UNDEF_MACRO                                    + LF
        );*/
        out += pusp_macro(LIST_MACRO_STORE_2)                                                           +LF;

        return out;
    }

    public static String generateMACROLIST_POP() {
        String out = "";
        out += pop_macro(LIST_INCLUDE_0)                                                                +LF;
        /*out += ifDoEndif(NOT + LIST_INCLUDE_0,
                UNDEF + LIST_INCLUDE_0                                                              + LF
        );
        */
        out += pop_macro(LIST_INCLUDE_1)                                                                +LF;
        /*out += ifDoEndif(NOT + LIST_INCLUDE_1,
                UNDEF + LIST_INCLUDE_1                                                              + LF
        );
        */
        out += pop_macro(LIST_INCLUDE_2)                                                                +LF;
        /*out += ifDoEndif(NOT + LIST_INCLUDE_2,
                UNDEF + LIST_INCLUDE_2                                                              + LF
        );
        */
        out += pop_macro(LIST_MACRO_STORE_0)                                                             +LF;
        /*out += ifDoEndif(NOT + LIST_MACRO_STORE_0,
                UNDEF + LIST_MACRO_STORE_0                                                          + LF
        );
        */
        out += pop_macro(LIST_MACRO_STORE_1)                                                             +LF;
        /*out += ifDoEndif(NOT + LIST_MACRO_STORE_1,
                UNDEF + LIST_MACRO_STORE_1                                                          + LF
        );
        */
        out += pop_macro(LIST_MACRO_STORE_2)                                                             +LF;
        /*out += ifDoEndif(NOT + LIST_MACRO_STORE_2,
                UNDEF + LIST_MACRO_STORE_2                                                          + LF
        );
        */
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
            c = (char) ("0".codePointAt(0) + n % 8);
            n /= 8;
            s = c + s;
        }
        if (padding <= i)
            System.out.println("PADDING ERROR, check file!");
        for(; i < padding; i++)
            s = '0' + s;
        return s;
    }

    //generators

    public static void writeStringToFile(String filename, String content) {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(new File(filename).getAbsoluteFile()));

            writer.write(content);
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
            System.out.println("Error while write to file: " + filename);
            return;
        }
        System.out.println("Writen File: " + filename);
    }

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

    public static String include(String file) {return INCLUDE + "\"" + file + "\"";}

    public static String applyMacro(String func, String param) {
        return func + "( " + param + " )";
    }

    public static String pusp_macro(String macro) {
        return PRAGMA_PUSP + "(\"" + macro + "\")";
    }

    public static String pop_macro(String macro) {
        return PRAGMA_POP + "(\"" + macro + "\")";
    }
}
