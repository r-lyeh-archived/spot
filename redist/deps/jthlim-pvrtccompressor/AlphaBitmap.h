#ifndef JTHLIM_ALPHABITMAP_H
#define JTHLIM_ALPHABITMAP_H

#include "Bitmap.h"

namespace Javelin {

class AlphaBitmap : public Bitmap {
public:
    AlphaBitmap(int w, int h)
        : Bitmap(w, h, 1) {
    }

    const unsigned char *GetData() const { return data; }

    unsigned char *GetData() { return data; }
};

}

#endif
