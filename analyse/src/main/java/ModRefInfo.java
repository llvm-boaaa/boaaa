public enum ModRefInfo {
    NoModRef(0),
    Mod(1),
    Ref(2),
    ModRef(3),
    Must(4),
    MustRef(5),
    MustMod(6),
    MustModRef(7);

    public int id;
    ModRefInfo(int id) {
        this.id = id;
    }

}
