
/*

    trans.c

    Transform functions

*/

#include "inc/forge.h"


extern PTRANSFORM TransformCreate(void);

static PTRANSFORM m_transform_head = NULL;


// Initialize
static void transform_init(void) {
    if (m_transform_head) return;
    m_transform_head = TransformCreate();
    MtxIdentity( &(m_transform_head->matrix) );
}

// Create new transform object
PTRANSFORM TransformCreate(void) {
    return (PTRANSFORM) vmalloc( sizeof (transform_t) );
}

extern void TransformPush(PMTX pm);
extern BOOL TransformPop(PMTX pm);
extern void TransformPopAll(void);

// Push transform to stack
void TransformPush(PMTX pm) {
    transform_init();
    PTRANSFORM pt = TransformCreate();
    MtxCat( pm, &(m_transform_head->matrix), &(pt->matrix) );
    pt->next = m_transform_head;
    m_transform_head = pt;
}

// Pop transform from stack
BOOL TransformPop(PMTX pm) {
    transform_init();
    PTRANSFORM pt = m_transform_head;
    BOOL result = FALSE;
    if (pm) {
        MtxCopy( &(pt->matrix), pm );
        result = TRUE;
    }
    if (pt->next) {
        m_transform_head = pt->next;
        free( pt );
    }
    return result;
}

// Pop all transforms from stack
void TransformPopAll(void) {
    transform_init();
    while (m_transform_head->next) {
        TransformPop( NULL );
    }
}

