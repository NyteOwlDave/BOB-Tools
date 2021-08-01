/*

    circle.c

    Draw a simple circle (points only)

*/

#include "inc/forge.h"

static void usage(void);

int main(int argc, char** argv, char** envp) {
    if (argc != 3) {
        usage();
    }
    float radius = atof(argv[1]);
    if (isnan(radius) || (radius <= 0)) {
        usage();
    }
    int verts = atoi(argv[2]);
    if (verts < 3) {
        usage();
    }

    float slice = 2*PI / verts;

    float angle = 0;

    for (int n=0; n<verts; n++) {
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        printf("%f %f 0\n", x, y);
        angle += slice;
    }

	return 0;
}

// Show command line usage
static void usage(void)
{
    printf("CIRCLE radius vertices\n");
    exit(1);
}
