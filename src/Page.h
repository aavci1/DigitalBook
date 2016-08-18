#ifndef PAGE_H
#define PAGE_H

enum Side {
    Left,
    Right
};

class PagePrivate;

class Page {
public:
    Page();
    ~Page();

    void setData(Side side, int width, int height, unsigned char *data);
    void render(Side side, int width, int height);

private:
    PagePrivate *d;
};

#endif // PAGE_H
