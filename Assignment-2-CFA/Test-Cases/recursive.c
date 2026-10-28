int f(int x) { if (x <= 0) return x; return f(x - 1); }
int main(void) { return f(2); }
