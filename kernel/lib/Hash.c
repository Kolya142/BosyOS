unsigned int hash_step1(unsigned int a, unsigned int b, unsigned int c, unsigned int d) {
    return a^b|(c*d)-d%0xa2f;
}
unsigned int hash_step2(unsigned int a, unsigned int b, unsigned int c, unsigned int d) {
    return b*d*(b/a%a)^d+0xd+c*0x23db;
}
unsigned int hash_step3(unsigned int a, unsigned int b, unsigned int c, unsigned int d) {
    return (b-c%d)*d+0x2ba-a^b;
}
unsigned int hash_step4(unsigned int a, unsigned int b, unsigned int c, unsigned int d) {
    unsigned int e = a*b%c+d;
    unsigned int f = c^b-d&a;
    return e*0x2df^f/(a+b)/c*hash_step1(a, b, c, d)%0xd0f;
}

unsigned int calculate_shash(unsigned int n) {
    if (n < 0) {
        return -1;
    }
    n++;
    n = hash_step2(n, 0xdb, 0x2ba, 0x2fcb)^n-0x2fcb*n-n;
    unsigned int a = hash_step1(n, n, n, n);
    unsigned int b = hash_step2(n, a, n, a);
    unsigned int c = hash_step3(b, a, n, b);
    unsigned int d = hash_step4(a, n, c, n);
    a = hash_step1(a, b, c, d);
    b = hash_step2(a, b, c, d);
    c = hash_step3(a, b, c, d);
    d = hash_step4(a, b, c, d);
    a += d * b;
    c += b * a;
    return hash_step4(a, b, c, d);
}