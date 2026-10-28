int leaf(int x) { return x; }
int wrap(int x) { return leaf(x); }
int main(void) { return wrap(1) + wrap(2); }
