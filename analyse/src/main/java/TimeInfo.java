public enum TimeInfo {
    PM_TIME(8),
    ANALYSIS_TIME(7),
    FUNCTION_TIME(3),
    ALIAS_TIME(2),
    MODREF_TIME(1);

    public int id;
    TimeInfo(int id) {
        this.id = id;
    }

}
