/**
 * Randy Thiemann 2013
 **/

#ifndef COLOR_H_
#define COLOR_H_

class Color {
public:
    Color();
    Color(unsigned long r, unsigned long g, unsigned long b);

    unsigned long r() const { return mR; }
    void r(unsigned long r) { mR = r; }
    unsigned long g() const { return mG; }
    void g(unsigned long g) { mR = g; }
    unsigned long b() const { return mB; }
    void b(unsigned long b) { mR = b; }

    bool operator==(const Color& c) const;

private:
    // The C++ standard says that an unsigned long should be at least 32b wide.
    unsigned long mR;
    unsigned long mG;
    unsigned long mB;
};

#endif /* COLOR_H_ */
