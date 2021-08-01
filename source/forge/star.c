
/*

    box.c

    Draw a star shaped polygon

*/

#include "inc/forge.h"


float m_inner = 1;
float m_outer = 2;
float m_points = 5;

static void usage(void)
{
    printf("\nUSAGE:\n");
    printf("star <inner> <outer> [points]\n");
    printf("<inner>\t\tInner radius\n");
    printf("<outer>\t\tOuter radius\n");
    printf("<points>\t\tNumber of points (min=3)\n");
    exit(0);
}


static void sort_out_args(int argc, char** argv)
{
    if (argc < 2) usage();
    if (argc > 4) usage();
    m_inner = atof(argv[1]);
    if (argc > 2)
    {
        m_outer = atof(argv[2]);
        if (argc > 3)
        {
            m_points = atoi(argv[3]);
            if (m_points < 3) usage();
        }
    }
    if (m_inner > m_outer)
    {
        float temp = m_inner;
        m_inner = m_outer;
        m_outer = temp;
    }
}


static void draw_star()
{
    float step = 2 * PI / m_points;
    float half_step = step / 2;
    float theta = 0;
    for (int i=0; i<m_points; i++)
    {
        float xo =  m_outer * sin(theta);
        float yo = -m_outer * cos(theta);
        printf("%f %f\n", xo, yo);
        float xi =  m_inner * sin(theta + half_step);
        float yi = -m_inner * cos(theta + half_step);
        printf("%f %f\n", xi, yi);
        theta += step;
    }
}


int main(int argc, char** argv, char** envp)
{
    sort_out_args(argc, argv);
    draw_star();
    exit(0);
}
