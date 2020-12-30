extern int32 readInt32();
extern int32 printd(int32 X);

void printArr(int32[32] X, int32 L) {
   for (int32 i = 0; i < L; i = i + 1) {
    printd(X[i]);
    }
}


int32 main() {
    int32 L = readInt32();
    int32[L] X;
    for (int32 i = 0; i < L; i = i + 1) {
        X[i] = readInt32();
    }
    printArr(X, L);
    return 0;
}

