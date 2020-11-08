int32 main() {
    int32 x = 4;
    int32[4][5] y = {1, 2, 3};
    for (int32 x = 0; x < 10; x = x + 1) {
        print(x);
    }
    if (x == y) {
        print(y);
    }
}
