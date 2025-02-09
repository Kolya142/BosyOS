int hash_step1(int a, int b, int c, int d) {
    return a^b|(c*d)-d%0xa2f;
}
int hash_step2(int a, int b, int c, int d) {
    return b*d*(b/a%a)^d+0xd+c*0x23db;
}
int hash_step3(int a, int b, int c, int d) {
    return (b-c%d)*d+0x2ba-a^b;
}
int hash_step4(int a, int b, int c, int d) {
    int e = a*b%c+d;
    int f = c^b-d&a;
    return e*0x2df^f/(a+b)/c*hash_step1(a, b, c, d)%0xd0f;
}

int calculate_shash(int n) {
    if (n < 0) {
        return -1;
    }
    n++;
    n = hash_step2(n, 0xdb, 0x2ba, 0x2fcb)^n-0x2fcb*n-n;
    int a = hash_step1(n, n, n, n);
    int b = hash_step2(n, a, n, a);
    int c = hash_step3(b, a, n, b);
    int d = hash_step4(a, n, c, n);
    a = hash_step1(a, b, c, d);
    b = hash_step2(a, b, c, d);
    c = hash_step3(a, b, c, d);
    d = hash_step4(a, b, c, d);
    a += d * b;
    c += b * a;
    return hash_step4(a, b, c, d);
}