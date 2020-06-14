public enum AliasResult {
    NoALias(0),
    MayAlias(1),
    PartialAlias(2),
    MustAlias(3);

    int id;
    AliasResult(int id) {
        this.id = id;
    }
}
