/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://www.hdfgroup.org/licenses.               *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Purpose:	"All" selection dataspace I/O functions.
 */

/****************/
/* Module Setup */
/****************/

#include "H5Smodule.h" /* This source code file is part of the H5S module */

/***********/
/* Headers */
/***********/
#include "H5private.h"   /* Generic Functions                        */
#include "H5Eprivate.h"  /* Error handling                           */
#include "H5Iprivate.h"  /* ID Functions                             */
#include "H5Spkg.h"      /* Dataspace functions                      */
#include "H5VMprivate.h" /* Vector functions                         */

/****************/
/* Local Macros */
/****************/

/******************/
/* Local Typedefs */
/******************/

/********************/
/* Local Prototypes */
/********************/

/* Selection callbacks */
static herr_t   H5S__all_copy(H5S_t *dst, const H5S_t *src, bool share_selection);
static herr_t   H5S__all_release(H5S_t *space);
static htri_t   H5S__all_is_valid(const H5S_t *space);
static hssize_t H5S__all_serial_size(H5S_t *space);
static herr_t   H5S__all_serialize(H5S_t *space, uint8_t **p);
static herr_t   H5S__all_deserialize(H5S_t **space, const uint8_t **p, const size_t p_size, bool skip);
static herr_t   H5S__all_bounds(const H5S_t *space, hsize_t *start, hsize_t *end);
static herr_t   H5S__all_offset(const H5S_t *space, hsize_t *off);
static int      H5S__all_unlim_dim(const H5S_t *space);
static htri_t   H5S__all_is_contiguous(const H5S_t *space);
static htri_t   H5S__all_is_single(const H5S_t *space);
static htri_t   H5S__all_is_regular(H5S_t *space);
static htri_t   H5S__all_shape_same(H5S_t *space1, H5S_t *space2);
static htri_t   H5S__all_intersect_block(H5S_t *space, const hsize_t *start, const hsize_t *end);
static herr_t   H5S__all_adjust_u(H5S_t *space, const hsize_t *offset);
static herr_t   H5S__all_adjust_s(H5S_t *space, const hssize_t *offset);
static herr_t   H5S__all_project_scalar(const H5S_t *space, hsize_t *offset);
static herr_t   H5S__all_project_simple(const H5S_t *space, H5S_t *new_space, hsize_t *offset);
static herr_t   H5S__all_iter_init(H5S_t *space, H5S_sel_iter_t *iter);

/* Selection iteration callbacks */
static herr_t  H5S__all_iter_coords(const H5S_sel_iter_t *iter, hsize_t *coords);
static herr_t  H5S__all_iter_block(const H5S_sel_iter_t *iter, hsize_t *start, hsize_t *end);
static hsize_t H5S__all_iter_nelmts(const H5S_sel_iter_t *iter);
static htri_t  H5S__all_iter_has_next_block(const H5S_sel_iter_t *iter);
static herr_t  H5S__all_iter_next(H5S_sel_iter_t *sel_iter, size_t nelem);
static herr_t  H5S__all_iter_next_block(H5S_sel_iter_t *sel_iter);
static herr_t  H5S__all_iter_get_seq_list(H5S_sel_iter_t *iter, size_t maxseq, size_t maxbytes, size_t *nseq,
                                          size_t *nbytes, hsize_t *off, size_t *len);
static herr_t  H5S__all_iter_release(H5S_sel_iter_t *sel_iter);

/*****************************/
/* Library Private Variables */
/*****************************/

/*********************/
/* Package Variables */
/*********************/

/* Selection properties for "all" selections */
const H5S_select_class_t H5S_sel_all[1] = {{
    H5S_SEL_ALL,

    /* Methods on selection */
    H5S__all_copy,
    H5S__all_release,
    H5S__all_is_valid,
    H5S__all_serial_size,
    H5S__all_serialize,
    H5S__all_deserialize,
    H5S__all_bounds,
    H5S__all_offset,
    H5S__all_unlim_dim,
    NULL,
    H5S__all_is_contiguous,
    H5S__all_is_single,
    H5S__all_is_regular,
    H5S__all_shape_same,
    H5S__all_intersect_block,
    H5S__all_adjust_u,
    H5S__all_adjust_s,
    H5S__all_project_scalar,
    H5S__all_project_simple,
    H5S__all_iter_init,
}};

/*******************/
/* Local Variables */
/*******************/

/* Iteration properties for "all" selections */
static const H5S_sel_iter_class_t H5S_sel_iter_all[1] = {{
    H5S_SEL_ALL,

    /* Methods on selection iterator */
    H5S__all_iter_coords,
    H5S__all_iter_block,
    H5S__all_iter_nelmts,
    H5S__all_iter_has_next_block,
    H5S__all_iter_next,
    H5S__all_iter_next_block,
    H5S__all_iter_get_seq_list,
    H5S__all_iter_release,
}};

/*-------------------------------------------------------------------------
 * Function:	H5S__all_iter_init
 *
 * Purpose:	Initializes iteration information for "all" selection.
 *
 * Return:	Non-negative on success, negative on failure.
 *
 *-------------------------------------------------------------------------
 */
static herr_t
H5S__all_iter_init(H5S_t H5_ATTR_UNUSED *space, H5S_sel_iter_t *iter)
{
    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(space && H5S_SEL_ALL == H5S_GET_SELECT_TYPE(space));
    assert(iter);

    /* Start at the upper left location */
    iter->u.all.elmt_offset = 0;
    iter->u.all.byte_offset = 0;

    /* Initialize type of selection iterator */
    iter->type = H5S_sel_iter_all;

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_iter_init() */

/*-------------------------------------------------------------------------
 * Function:	H5S__all_iter_coords
 *
 * Purpose:	Retrieve the current coordinates of iterator for current
 *              selection
 *
 * Return:	Non-negative on success, negative on failure
 *
 *-------------------------------------------------------------------------
 */
static herr_t
H5S__all_iter_coords(const H5S_sel_iter_t *iter, hsize_t *coords)
{
    herr_t ret_value = SUCCEED; /* Return value */

    FUNC_ENTER_PACKAGE

    /* Check args */
    assert(iter);
    assert(coords);

    /* Calculate the coordinates for the current iterator offset */
    if (H5VM_array_calc(iter->u.all.elmt_offset, iter->rank, iter->dims, coords) < 0)
        HGOTO_ERROR(H5E_DATASPACE, H5E_CANTGET, FAIL, "can't retrieve coordinates");

done:
    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5S__all_iter_coords() */

/*-------------------------------------------------------------------------
 * Function:	H5S__all_iter_block
 *
 * Purpose:	Retrieve the current block of iterator for current
 *              selection
 *
 * Return:	Non-negative on success, negative on failure
 *
 *-------------------------------------------------------------------------
 */
static herr_t
H5S__all_iter_block(const H5S_sel_iter_t *iter, hsize_t *start, hsize_t *end)
{
    unsigned u; /* Local index variable */

    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(iter);
    assert(start);
    assert(end);

    for (u = 0; u < iter->rank; u++) {
        /* Set the start of the 'all' block */
        /* (Always '0' coordinates for now) */
        start[u] = 0;

        /* Compute the end of the 'all' block */
        /* (Always size of the extent for now) */
        end[u] = iter->dims[u] - 1;
    } /* end for */

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_iter_block() */

/*-------------------------------------------------------------------------
 * Function:	H5S__all_iter_nelmts
 *
 * Purpose:	Return number of elements left to process in iterator
 *
 * Return:	Non-negative number of elements on success, zero on failure
 *
 *-------------------------------------------------------------------------
 */
static hsize_t
H5S__all_iter_nelmts(const H5S_sel_iter_t *iter)
{
    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(iter);

    FUNC_LEAVE_NOAPI(iter->elmt_left)
} /* end H5S__all_iter_nelmts() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_iter_has_next_block
 PURPOSE
    Check if there is another block left in the current iterator
 USAGE
    htri_t H5S__all_iter_has_next_block(iter)
        const H5S_sel_iter_t *iter;       IN: Pointer to selection iterator
 RETURNS
    Non-negative (true/false) on success/Negative on failure
 DESCRIPTION
    Check if there is another block available in the selection iterator.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static htri_t
H5S__all_iter_has_next_block(const H5S_sel_iter_t H5_ATTR_UNUSED *iter)
{
    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(iter);

    FUNC_LEAVE_NOAPI(false)
} /* end H5S__all_iter_has_next_block() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_iter_next
 PURPOSE
    Increment selection iterator
 USAGE
    herr_t H5S__all_iter_next(iter, nelem)
        H5S_sel_iter_t *iter;       IN: Pointer to selection iterator
        size_t nelem;               IN: Number of elements to advance by
 RETURNS
    Non-negative on success/Negative on failure
 DESCRIPTION
    Advance selection iterator to the NELEM'th next element in the selection.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static herr_t
H5S__all_iter_next(H5S_sel_iter_t *iter, size_t nelem)
{
    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(iter);
    assert(nelem > 0);

    /* Increment the iterator */
    iter->u.all.elmt_offset += nelem;
    iter->u.all.byte_offset += (nelem * iter->elmt_size);

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_iter_next() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_iter_next_block
 PURPOSE
    Increment selection iterator to next block
 USAGE
    herr_t H5S__all_iter_next_block(iter)
        H5S_sel_iter_t *iter;       IN: Pointer to selection iterator
 RETURNS
    Non-negative on success/Negative on failure
 DESCRIPTION
    Advance selection iterator to the next block in the selection.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static herr_t
H5S__all_iter_next_block(H5S_sel_iter_t H5_ATTR_UNUSED *iter)
{
    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(iter);

    FUNC_LEAVE_NOAPI(FAIL)
} /* end H5S__all_iter_next_block() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_iter_get_seq_list
 PURPOSE
    Create a list of offsets & lengths for a selection
 USAGE
    herr_t H5S__all_iter_get_seq_list(iter,maxseq,maxelem,nseq,nelem,off,len)
        H5S_sel_iter_t *iter;   IN/OUT: Selection iterator describing last
                                    position of interest in selection.
        size_t maxseq;          IN: Maximum number of sequences to generate
        size_t maxelem;         IN: Maximum number of elements to include in the
                                    generated sequences
        size_t *nseq;           OUT: Actual number of sequences generated
        size_t *nelem;          OUT: Actual number of elements in sequences generated
        hsize_t *off;           OUT: Array of offsets
        size_t *len;            OUT: Array of lengths
 RETURNS
    Non-negative on success/Negative on failure.
 DESCRIPTION
    Use the selection in the dataspace to generate a list of byte offsets and
    lengths for the region(s) selected.  Start/Restart from the position in the
    ITER parameter.  The number of sequences generated is limited by the MAXSEQ
    parameter and the number of sequences actually generated is stored in the
    NSEQ parameter.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static herr_t
H5S__all_iter_get_seq_list(H5S_sel_iter_t *iter, size_t H5_ATTR_UNUSED maxseq, size_t maxelem, size_t *nseq,
                           size_t *nelem, hsize_t *off, size_t *len)
{
    size_t elem_used; /* The number of elements used */

    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(iter);
    assert(maxseq > 0);
    assert(maxelem > 0);
    assert(nseq);
    assert(nelem);
    assert(off);
    assert(len);

    /* Determine the actual number of elements to use */
    H5_CHECK_OVERFLOW(iter->elmt_left, hsize_t, size_t);
    elem_used = MIN(maxelem, (size_t)iter->elmt_left);
    assert(elem_used > 0);

    /* Compute the offset in the dataset */
    off[0] = iter->u.all.byte_offset;
    len[0] = elem_used * iter->elmt_size;

    /* Should only need one sequence for 'all' selections */
    *nseq = 1;

    /* Set the number of elements used */
    *nelem = elem_used;

    /* Update the iterator */
    iter->elmt_left -= elem_used;
    iter->u.all.elmt_offset += elem_used;
    iter->u.all.byte_offset += len[0];

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_iter_get_seq_list() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_iter_release
 PURPOSE
    Release "all" selection iterator information for a dataspace
 USAGE
    herr_t H5S__all_iter_release(iter)
        H5S_sel_iter_t *iter;       IN: Pointer to selection iterator
 RETURNS
    Non-negative on success/Negative on failure
 DESCRIPTION
    Releases all information for a dataspace "all" selection iterator
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static herr_t
H5S__all_iter_release(H5S_sel_iter_t H5_ATTR_UNUSED *iter)
{
    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(iter);

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_iter_release() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_release
 PURPOSE
    Release all selection information for a dataspace
 USAGE
    herr_t H5S__all_release(space)
        H5S_t *space;       IN: Pointer to dataspace
 RETURNS
    Non-negative on success/Negative on failure
 DESCRIPTION
    Releases "all" selection information for a dataspace
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static herr_t
H5S__all_release(H5S_t *space)
{
    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(space);

    /* Reset the number of elements in the selection */
    space->select.num_elem = 0;

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_release() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_copy
 PURPOSE
    Copy a selection from one dataspace to another
 USAGE
    herr_t H5S__all_copy(dst, src, share_selection)
        H5S_t *dst;  OUT: Pointer to the destination dataspace
        H5S_t *src;  IN: Pointer to the source dataspace
        bool;     IN: Whether to share the selection between the dataspaces
 RETURNS
    Non-negative on success/Negative on failure
 DESCRIPTION
    Copies the 'all' selection information from the source
    dataspace to the destination dataspace.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static herr_t
H5S__all_copy(H5S_t *dst, const H5S_t H5_ATTR_UNUSED *src, bool H5_ATTR_UNUSED share_selection)
{
    FUNC_ENTER_PACKAGE_NOERR

    assert(src);
    assert(dst);

    /* Set number of elements in selection */
    dst->select.num_elem = (hsize_t)H5S_GET_EXTENT_NPOINTS(dst);

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_copy() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_is_valid
 PURPOSE
    Check whether the selection fits within the extent, with the current
    offset defined.
 USAGE
    htri_t H5S__all_is_valid(space);
        H5S_t *space;             IN: Dataspace pointer to query
 RETURNS
    true if the selection fits within the extent, false if it does not and
        Negative on an error.
 DESCRIPTION
    Determines if the current selection at the current offset fits within the
    extent for the dataspace.  Offset is irrelevant for this type of selection.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static htri_t
H5S__all_is_valid(const H5S_t H5_ATTR_UNUSED *space)
{
    FUNC_ENTER_PACKAGE_NOERR

    assert(space);

    FUNC_LEAVE_NOAPI(true)
} /* end H5S__all_is_valid() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_serial_size
 PURPOSE
    Determine the number of bytes needed to store the serialized "all"
        selection information.
 USAGE
    hssize_t H5S__all_serial_size(space)
        H5S_t *space;             IN: Dataspace pointer to query
 RETURNS
    The number of bytes required on success, negative on an error.
 DESCRIPTION
    Determines the number of bytes required to serialize an "all"
    selection for storage on disk.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static hssize_t
H5S__all_serial_size(H5S_t H5_ATTR_UNUSED *space)
{
    FUNC_ENTER_PACKAGE_NOERR

    assert(space);

    /* Basic number of bytes required to serialize point selection:
     *  <type (4 bytes)> + <version (4 bytes)> + <padding (4 bytes)> +
     *      <length (4 bytes)> = 16 bytes
     */
    FUNC_LEAVE_NOAPI(16)
} /* end H5S__all_serial_size() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_serialize
 PURPOSE
    Serialize the current selection into a user-provided buffer.
 USAGE
    herr_t H5S__all_serialize(space, p)
        H5S_t *space;           IN: Dataspace with selection to serialize
        uint8_t **p;            OUT: Pointer to buffer to put serialized
                                selection.  Will be advanced to end of
                                serialized selection.
 RETURNS
    Non-negative on success/Negative on failure
 DESCRIPTION
    Serializes the current element selection into a buffer.  (Primarily for
    storing on disk).
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static herr_t
H5S__all_serialize(H5S_t *space, uint8_t **p)
{
    uint8_t *pp = (*p); /* Local pointer for decoding */

    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(space);
    assert(p);
    assert(pp);

    /* Store the preamble information */
    UINT32ENCODE(pp, (uint32_t)H5S_GET_SELECT_TYPE(space)); /* Store the type of selection */
    UINT32ENCODE(pp, (uint32_t)H5S_ALL_VERSION_1);          /* Store the version number */
    UINT32ENCODE(pp, (uint32_t)0);                          /* Store the un-used padding */
    UINT32ENCODE(pp, (uint32_t)0);                          /* Store the additional information length */

    /* Update encoding pointer */
    *p = pp;

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_serialize() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_deserialize
 PURPOSE
    Deserialize the current selection from a user-provided buffer.
 USAGE
    herr_t H5S_all_deserialize(space, p)
        H5S_t **space;          IN/OUT: Dataspace pointer to place
                                selection into
        uint8 **p;              OUT: Pointer to buffer holding serialized
                                selection.  Will be advanced to end of
                                serialized selection.
 RETURNS
    Non-negative on success/Negative on failure
 DESCRIPTION
    Deserializes the current selection into a buffer.  (Primarily for retrieving
    from disk).
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static herr_t
H5S__all_deserialize(H5S_t **space, const uint8_t **p, const size_t p_size, bool skip)
{
    uint32_t version;                           /* Version number */
    H5S_t   *tmp_space = NULL;                  /* Pointer to actual dataspace to use,
                                                   either *space or a newly allocated one */
    herr_t         ret_value = SUCCEED;         /* return value */
    const uint8_t *p_end     = *p + p_size - 1; /* Pointer to last valid byte in buffer */
    FUNC_ENTER_PACKAGE

    assert(p);
    assert(*p);

    /* As part of the efforts to push all selection-type specific coding
       to the callbacks, the coding for the allocation of a null dataspace
       is moved from H5S_select_deserialize() in H5Sselect.c.
       This is needed for decoding virtual layout in H5O__layout_decode() */

    /* Allocate space if not provided */
    if (!*space) {
        if (NULL == (tmp_space = H5S_create(H5S_SIMPLE)))
            HGOTO_ERROR(H5E_DATASPACE, H5E_CANTCREATE, FAIL, "can't create dataspace");
    } /* end if */
    else
        tmp_space = *space;

    /* Decode version */
    if (H5_IS_KNOWN_BUFFER_OVERFLOW(skip, *p, sizeof(uint32_t), p_end))
        HGOTO_ERROR(H5E_DATASPACE, H5E_OVERFLOW, FAIL, "buffer overflow while decoding selection version");
    UINT32DECODE(*p, version);

    if (version < H5S_ALL_VERSION_1 || version > H5S_ALL_VERSION_LATEST)
        HGOTO_ERROR(H5E_DATASPACE, H5E_BADVALUE, FAIL, "bad version number for all selection");

    /* Skip over the remainder of the header */
    if (H5_IS_KNOWN_BUFFER_OVERFLOW(skip, *p, 8, p_end))
        HGOTO_ERROR(H5E_DATASPACE, H5E_OVERFLOW, FAIL, "buffer overflow while decoding header");
    *p += 8;

    /* Change to "all" selection */
    if (H5S_select_all(tmp_space, true) < 0)
        HGOTO_ERROR(H5E_DATASPACE, H5E_CANTDELETE, FAIL, "can't change selection");

    /* Return space to the caller if allocated */
    if (!*space)
        *space = tmp_space;

done:
    /* Free temporary space if not passed to caller (only happens on error) */
    if (!*space && tmp_space)
        if (H5S_close(tmp_space) < 0)
            HDONE_ERROR(H5E_DATASPACE, H5E_CANTFREE, FAIL, "can't close dataspace");

    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5S__all_deserialize() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_bounds
 PURPOSE
    Gets the bounding box containing the selection.
 USAGE
    herr_t H5S__all_bounds(space, start, end)
        H5S_t *space;           IN: Dataspace pointer of selection to query
        hsize_t *start;         OUT: Starting coordinate of bounding box
        hsize_t *end;           OUT: Opposite coordinate of bounding box
 RETURNS
    Non-negative on success, negative on failure
 DESCRIPTION
    Retrieves the bounding box containing the current selection and places
    it into the user's buffers.  The start and end buffers must be large
    enough to hold the dataspace rank number of coordinates.  The bounding box
    exactly contains the selection, ie. if a 2-D element selection is currently
    defined with the following points: (4,5), (6,8) (10,7), the bounding box
    with be (4, 5), (10, 8).  Calling this function on a "none" selection
    returns fail.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static herr_t
H5S__all_bounds(const H5S_t *space, hsize_t *start, hsize_t *end)
{
    unsigned rank; /* Dataspace rank */
    unsigned i;    /* index variable */

    FUNC_ENTER_PACKAGE_NOERR

    assert(space);
    assert(start);
    assert(end);

    /* Get the dataspace extent rank */
    rank = space->extent.rank;

    /* Just copy over the complete extent */
    for (i = 0; i < rank; i++) {
        start[i] = 0;
        end[i]   = space->extent.size[i] - 1;
    } /* end for */

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_bounds() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_offset
 PURPOSE
    Gets the linear offset of the first element for the selection.
 USAGE
    herr_t H5S__all_offset(space, offset)
        const H5S_t *space;     IN: Dataspace pointer of selection to query
        hsize_t *offset;        OUT: Linear offset of first element in selection
 RETURNS
    Non-negative on success, negative on failure
 DESCRIPTION
    Retrieves the linear offset (in "units" of elements) of the first element
    selected within the dataspace.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    Calling this function on a "none" selection returns fail.
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static herr_t
H5S__all_offset(const H5S_t H5_ATTR_UNUSED *space, hsize_t *offset)
{
    FUNC_ENTER_PACKAGE_NOERR

    assert(space);
    assert(offset);

    /* 'All' selections always start at offset 0 */
    *offset = 0;

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_offset() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_unlim_dim
 PURPOSE
    Return unlimited dimension of selection, or -1 if none
 USAGE
    int H5S__all_unlim_dim(space)
        H5S_t *space;           IN: Dataspace pointer to check
 RETURNS
    Unlimited dimension of selection, or -1 if none (never fails).
 DESCRIPTION
    Returns the index of the unlimited dimension in this selection, or -1
    if the selection has no unlimited dimension.  "All" selections are
    always unlimited in every dimension, though this is not reflected in
    other calls, where the selection is "clipped" against the current
    extent, so for consistency this function always returns -1.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static int
H5S__all_unlim_dim(const H5S_t H5_ATTR_UNUSED *space)
{
    FUNC_ENTER_PACKAGE_NOERR

    FUNC_LEAVE_NOAPI(-1)
} /* end H5S__all_unlim_dim() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_is_contiguous
 PURPOSE
    Check if a "all" selection is contiguous within the dataspace extent.
 USAGE
    htri_t H5S__all_is_contiguous(space)
        H5S_t *space;           IN: Dataspace pointer to check
 RETURNS
    true/false/FAIL
 DESCRIPTION
    Checks to see if the current selection in the dataspace is contiguous.
    This is primarily used for reading the entire selection in one swoop.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static htri_t
H5S__all_is_contiguous(const H5S_t H5_ATTR_UNUSED *space)
{
    FUNC_ENTER_PACKAGE_NOERR

    assert(space);

    FUNC_LEAVE_NOAPI(true)
} /* end H5S__all_is_contiguous() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_is_single
 PURPOSE
    Check if a "all" selection is a single block within the dataspace extent.
 USAGE
    htri_t H5S__all_is_single(space)
        H5S_t *space;           IN: Dataspace pointer to check
 RETURNS
    true/false/FAIL
 DESCRIPTION
    Checks to see if the current selection in the dataspace is a single block.
    This is primarily used for reading the entire selection in one swoop.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static htri_t
H5S__all_is_single(const H5S_t H5_ATTR_UNUSED *space)
{
    FUNC_ENTER_PACKAGE_NOERR

    assert(space);

    FUNC_LEAVE_NOAPI(true)
} /* end H5S__all_is_single() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_is_regular
 PURPOSE
    Check if a "all" selection is "regular"
 USAGE
    htri_t H5S__all_is_regular(space)
        H5S_t *space;     IN: Dataspace pointer to check
 RETURNS
    true/false/FAIL
 DESCRIPTION
    Checks to see if the current selection in a dataspace is the a regular
    pattern.
    This is primarily used for reading the entire selection in one swoop.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static htri_t
H5S__all_is_regular(H5S_t H5_ATTR_UNUSED *space)
{
    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(space);

    FUNC_LEAVE_NOAPI(true)
} /* end H5S__all_is_regular() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_shape_same
 PURPOSE
    Check if a two "all" selections are the same shape
 USAGE
    htri_t H5S__all_shape_same(space1, space2)
        H5S_t *space1;           IN: First dataspace to check
        H5S_t *space2;           IN: Second dataspace to check
 RETURNS
    true / false / FAIL
 DESCRIPTION
    Checks to see if the current selection in each dataspace are the same
    shape.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static htri_t
H5S__all_shape_same(H5S_t *space1, H5S_t *space2)
{
    int    space1_dim;       /* Current dimension in first dataspace */
    int    space2_dim;       /* Current dimension in second dataspace */
    htri_t ret_value = true; /* Return value */

    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(space1);
    assert(space2);

    /* Initialize dataspace dims */
    space1_dim = (int)space1->extent.rank - 1;
    space2_dim = (int)space2->extent.rank - 1;

    /* Recall that space1_rank >= space2_rank.
     *
     * In the following while loop, we test to see if space1 and space2
     * have identical size in all dimensions they have in common.
     */
    while (space2_dim >= 0) {
        if (space1->extent.size[space1_dim] != space2->extent.size[space2_dim])
            HGOTO_DONE(false);

        space1_dim--;
        space2_dim--;
    } /* end while */

    /* Since we are selecting the entire space, we must also verify that space1
     * has size 1 in all dimensions that it does not share with space2.
     */
    while (space1_dim >= 0) {
        if (space1->extent.size[space1_dim] != 1)
            HGOTO_DONE(false);

        space1_dim--;
    } /* end while */

done:
    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5S__all_shape_same() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_intersect_block
 PURPOSE
    Detect intersections of selection with block
 USAGE
    htri_t H5S__all_intersect_block(space, start, end)
        H5S_t *space;           IN: Dataspace with selection to use
        const hsize_t *start;   IN: Starting coordinate for block
        const hsize_t *end;     IN: Ending coordinate for block
 RETURNS
    Non-negative true / false on success, negative on failure
 DESCRIPTION
    Quickly detect intersections with a block
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
htri_t
H5S__all_intersect_block(H5S_t H5_ATTR_UNUSED *space, const hsize_t H5_ATTR_UNUSED *start,
                         const hsize_t H5_ATTR_UNUSED *end)
{
    FUNC_ENTER_PACKAGE_NOERR

    /* Sanity check */
    assert(space);
    assert(H5S_SEL_ALL == H5S_GET_SELECT_TYPE(space));
    assert(start);
    assert(end);

    FUNC_LEAVE_NOAPI(true)
} /* end H5S__all_intersect_block() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_adjust_u
 PURPOSE
    Adjust an "all" selection by subtracting an offset
 USAGE
    herr_t H5S__all_adjust_u(space, offset)
        H5S_t *space;           IN/OUT: Pointer to dataspace to adjust
        const hsize_t *offset; IN: Offset to subtract
 RETURNS
    Non-negative on success, negative on failure
 DESCRIPTION
    Moves selection by subtracting an offset from it.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static herr_t
H5S__all_adjust_u(H5S_t H5_ATTR_UNUSED *space, const hsize_t H5_ATTR_UNUSED *offset)
{
    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(space);
    assert(offset);

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_adjust_u() */

/*--------------------------------------------------------------------------
 NAME
    H5S__all_adjust_s
 PURPOSE
    Adjust an "all" selection by subtracting an offset
 USAGE
    herr_t H5S__all_adjust_u(space, offset)
        H5S_t *space;           IN/OUT: Pointer to dataspace to adjust
        const hssize_t *offset; IN: Offset to subtract
 RETURNS
    Non-negative on success, negative on failure
 DESCRIPTION
    Moves selection by subtracting an offset from it.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static herr_t
H5S__all_adjust_s(H5S_t H5_ATTR_UNUSED *space, const hssize_t H5_ATTR_UNUSED *offset)
{
    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(space);
    assert(offset);

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_adjust_s() */

/*-------------------------------------------------------------------------
 * Function:	H5S__all_project_scalar
 *
 * Purpose:	Projects a single element 'all' selection into a scalar
 *              dataspace
 *
 * Return:	Non-negative on success, negative on failure.
 *
 *-------------------------------------------------------------------------
 */
static herr_t
H5S__all_project_scalar(const H5S_t H5_ATTR_UNUSED *space, hsize_t *offset)
{
    FUNC_ENTER_PACKAGE_NOERR

    /* Check args */
    assert(space && H5S_SEL_ALL == H5S_GET_SELECT_TYPE(space));
    assert(offset);

    /* Set offset of selection in projected buffer */
    *offset = 0;

    FUNC_LEAVE_NOAPI(SUCCEED)
} /* end H5S__all_project_scalar() */

/*-------------------------------------------------------------------------
 * Function:	H5S__all_project_simple
 *
 * Purpose:	Projects an 'all' selection onto/into a simple dataspace
 *              of a different rank
 *
 * Return:	Non-negative on success, negative on failure.
 *
 *-------------------------------------------------------------------------
 */
static herr_t
H5S__all_project_simple(const H5S_t H5_ATTR_UNUSED *base_space, H5S_t *new_space,
                        hsize_t H5_ATTR_UNUSED *offset)
{
    herr_t ret_value = SUCCEED; /* Return value */

    FUNC_ENTER_PACKAGE

    /* Check args */
    assert(base_space && H5S_SEL_ALL == H5S_GET_SELECT_TYPE(base_space));
    assert(new_space);
    assert(offset);

    /* Select the entire new space */
    if (H5S_select_all(new_space, true) < 0)
        HGOTO_ERROR(H5E_DATASPACE, H5E_CANTSET, FAIL, "unable to set all selection");

done:
    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5S__all_project_simple() */

/*--------------------------------------------------------------------------
 NAME
    H5S_select_all
 PURPOSE
    Specify the entire extent is selected
 USAGE
    herr_t H5S_select_all(dsid)
        hid_t dsid;             IN: Dataspace ID of selection to modify
        bool rel_prev;      IN: Flag whether to release previous selection or not
 RETURNS
    Non-negative on success/Negative on failure
 DESCRIPTION
    This function selects the entire extent for a dataspace.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
herr_t
H5S_select_all(H5S_t *space, bool rel_prev)
{
    herr_t ret_value = SUCCEED; /* return value */

    FUNC_ENTER_NOAPI(FAIL)

    /* Check args */
    assert(space);

    /* Remove current selection first */
    if (rel_prev)
        if (H5S_SELECT_RELEASE(space) < 0)
            HGOTO_ERROR(H5E_DATASPACE, H5E_CANTDELETE, FAIL, "can't release selection");

    /* Set number of elements in selection */
    space->select.num_elem = (hsize_t)H5S_GET_EXTENT_NPOINTS(space);

    /* Set selection type */
    space->select.type = H5S_sel_all;

done:
    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5S_select_all() */

/*--------------------------------------------------------------------------
 NAME
    H5Sselect_all
 PURPOSE
    Specify the entire extent is selected
 USAGE
    herr_t H5Sselect_all(dsid)
        hid_t dsid;             IN: Dataspace ID of selection to modify
 RETURNS
    Non-negative on success/Negative on failure
 DESCRIPTION
    This function selects the entire extent for a dataspace.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
herr_t
H5Sselect_all(hid_t spaceid)
{
    H5S_t *space;               /* Dataspace to modify selection of */
    herr_t ret_value = SUCCEED; /* return value */

    FUNC_ENTER_API(FAIL)

    /* Check args */
    if (NULL == (space = (H5S_t *)H5I_object_verify(spaceid, H5I_DATASPACE)))
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "not a dataspace");

    /* Call internal routine to do the work */
    if (H5S_select_all(space, true) < 0)
        HGOTO_ERROR(H5E_DATASPACE, H5E_CANTDELETE, FAIL, "can't change selection");

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Sselect_all() */
