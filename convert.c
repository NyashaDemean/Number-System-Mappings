#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


// Convert number to string in base (2-16) using division algorithm
void div_convert(uint32_t n, int base, char *out) {
    char digits[] = "0123456789ABCDEF";
    char buffer[65];
    int pos = 0;

    // Handle the zero case
    if (n == 0) {
        strcpy(out, "0");
        return;
    }

    // Ectract digits from right to left
    while (n > 0){
        buffer[pos] = digits[n % base];
        pos++;
        n /= base;
    }

    // Reverse into out
    int j;
    for (j = 0; j < pos; j++){
        out[j] = buffer[pos - j - 1];
    }
    out[pos] = '\0';
}


// Convert number to string in base (2-16) using subtraction of powers
void sub_convert(uint32_t n, int base, char *out) {
    char digits[] = "0123456789ABCDEF";
    char buffer[65];
    int pos = 0;

    // Handle the zero case
    if (n == 0) {
        strcpy(out, "0");
        return;
    }

    //Find the largest power of the base <= n
    uint32_t power = 1;
    while (power <= n / base){
        power *= base;
    }

    while (power > 0){
        int digit = n / power;
        buffer[pos] = digits[digit];
        pos++;
        n -= digit * power;
        power /= base;
    }

    buffer[pos] = '\0';
    strcpy(out, buffer);
}


void print_tables(uint32_t n){
    char bin[65], oct[65], dec[65], hex[65];

    // Original
    div_convert(n, 2, bin);
    div_convert(n, 8, oct);
    sprintf(dec, "%u", n);
    div_convert(n, 16, hex);
    printf("Original: Binary=%s Octal=%s Decimal=%s Hex=%s\n", bin, oct, dec, hex);

    //Left shift by 3
    uint32_t shifted = n << 3;
    div_convert(shifted, 2, bin);
    div_convert(shifted, 8, oct);
    sprintf(dec, "%u", shifted);
    div_convert(shifted, 16, hex);
    printf("Left Shift by 3: Binary=%s Octal=%s Decimal=%s Hex=%s\n", bin, oct, dec, hex);

    // With 0xFF
    uint32_t masked = n & 0xFF;
    div_convert(masked, 2, bin);
    div_convert(masked, 8, oct);
    sprintf(dec, "%u", masked);
    div_convert(masked, 16, hex);
    printf("AND with 0xFF: Binary=%s Octal=%s Decimal=%s Hex=%s\n", bin, oct, dec, hex);

}

// Build a 32-bit binary string from an unsigned value (exactly 32 chars + '\0').
static void u32_to_bits(uint32_t x, char *out32) {
    for (int i = 31; i >= 0; --i) {
        out32[31 - i] = ((x >> i) & 1u) ? '1' : '0';
    }
    out32[32] = '\0';
}

// 3-bit map per octal digit; ALWAYS 3 bits per digit, no stripping.
void oct_to_bin(const char *oct, char *out) {
    size_t pos = 0;
    for (size_t i = 0; oct[i] != '\0'; ++i) {
        char c = oct[i];
        if (c < '0' || c > '7') { out[0] = '\0'; return; } // simple validation
        int d = c - '0';
        out[pos++] = (d & 0b100) ? '1' : '0';
        out[pos++] = (d & 0b010) ? '1' : '0';
        out[pos++] = (d & 0b001) ? '1' : '0';
    }
    out[pos] = '\0';
}

// oct -> bin (3 bits each) -> left-pad to nibble -> hex (uppercase); all-zero -> "0".
void oct_to_hex(const char *oct, char *out) {
    char bin[512]; // generous
    oct_to_bin(oct, bin);
    size_t len = strlen(bin);

    // If input was "0" (-> "000") or any sequence of zeros
    int all_zero = 1;
    for (size_t i = 0; i < len; ++i) if (bin[i] == '1') { all_zero = 0; break; }
    if (len == 0) { strcpy(out, "0"); return; }
    if (all_zero) { strcpy(out, "0"); return; }

    int pad = (int)((4 - (len % 4)) % 4);
    char padded[520];
    int p = 0;
    for (int i = 0; i < pad; ++i) padded[p++] = '0';
    memcpy(padded + p, bin, len);
    p += (int)len;
    padded[p] = '\0';

    static const char hex_digits[] = "0123456789ABCDEF";
    int outp = 0;
    for (int i = 0; i < p; i += 4) {
        int v = (padded[i]   - '0') * 8 +
                (padded[i+1] - '0') * 4 +
                (padded[i+2] - '0') * 2 +
                (padded[i+3] - '0');
        out[outp++] = hex_digits[v];
    }
    out[outp] = '\0';

    // trim leading zeros to match tests like "0" not "0000"
    // (We already handled all-zero case; here we keep normal hex, e.g., "1FF", "40".)
    // Nothing else to do.
}

// 4-bit map per hex digit; accept lower/upper case; ALWAYS 4 bits per digit.
void hex_to_bin(const char *hex, char *out) {
    size_t pos = 0;
    for (size_t i = 0; hex[i] != '\0'; ++i) {
        char c = hex[i];
        int v;
        if (c >= '0' && c <= '9') v = c - '0';
        else if (c >= 'A' && c <= 'F') v = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f') v = c - 'a' + 10;
        else { out[0] = '\0'; return; } // simple validation

        out[pos++] = (v & 0b1000) ? '1' : '0';
        out[pos++] = (v & 0b0100) ? '1' : '0';
        out[pos++] = (v & 0b0010) ? '1' : '0';
        out[pos++] = (v & 0b0001) ? '1' : '0';
    }
    out[pos] = '\0';
}

// Sign–magnitude: bit31 is sign; bits 30..0 carry magnitude of |n| (truncated to 31 bits).
void to_sign_magnitude(int32_t n, char *out32) {
    uint32_t mag;
    int negative = (n < 0);
    if (!negative) {
        // positive or zero: just normal bits, then force sign bit to 0
        u32_to_bits((uint32_t)n, out32);
        out32[0] = '0';
        return;
    }
    // For negatives: magnitude = |n|. INT_MIN is 2^31 which doesn't fit 31 bits; truncate.
    if (n == INT32_MIN) mag = (1u << 31); // this will be truncated to 0 in lower 31 bits below
    else mag = (uint32_t)(-n);

    // Build 32 zeros, set sign, then fill lower 31 bits from mag.
    for (int i = 0; i < 32; ++i) out32[i] = '0';
    out32[32] = '\0';
    out32[0] = '1'; // sign bit

    // copy mag into bits 1..31 (which correspond to 30..0)
    for (int bit = 0; bit < 31; ++bit) {
        int src_bit = bit;                  // from mag bit position
        int dst_idx = 31 - bit;             // out index (LSB at end)
        out32[dst_idx] = ((mag >> src_bit) & 1u) ? '1' : '0';
    }
}

// One’s complement: positives as-is; negatives = bitwise-NOT of positive magnitude across 32 bits.
void to_ones_complement(int32_t n, char *out32) {
    if (n >= 0) {
        u32_to_bits((uint32_t)n, out32);
        return;
    }
    // Build the positive magnitude pattern, then flip every bit.
    uint32_t mag = (n == INT32_MIN) ? (1u << 31) : (uint32_t)(-n);
    char tmp[33];
    u32_to_bits(mag, tmp);
    for (int i = 0; i < 32; ++i) out32[i] = (tmp[i] == '1') ? '0' : '1';
    out32[32] = '\0';
}

// Two’s complement: the actual machine representation (cast + print).
void to_twos_complement(int32_t n, char *out32) {
    u32_to_bits((uint32_t)n, out32);
}