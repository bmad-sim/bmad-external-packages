/****h* H5Gf/H5Gf
 * PURPOSE
 *  This file contains C stubs for H5G Fortran APIs
 *
 * COPYRIGHT
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://www.hdfgroup.org/licenses.               *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 ******
 */

#include "H5f90.h"
#include "H5Eprivate.h"

/****if* H5Gf/h5gget_obj_info_idx_c
 * NAME
 *  h5gget_obj_info_idx_c
 * PURPOSE
 *  Call H5Gget_obj_info to return name and the type of group
 *  member
 * INPUTS
 *  loc_id - file or group identifier
 *  name - name of the group
 *  namelen - name length
 *  idx - index of the group member
 * OUTPUTS
 *  obj_name - buffer to store member's name
 *  obj_namelen - length of the buffer
 *  obj_type - type of the object
 * RETURNS
 *  0 on success, -1 on failure
 * SOURCE
 */
int_f
h5gget_obj_info_idx_c(hid_t_f *loc_id, _fcd name, int_f *namelen, int_f *idx, _fcd obj_name,
                      int_f *obj_namelen, int_f *obj_type)
/******/
{
    H5O_info2_t oinfo;
    hid_t       c_loc_id = (hid_t)*loc_id;
    char       *c_name   = NULL;
    size_t      c_obj_namelen;
    char       *c_obj_name = NULL;
    hsize_t     c_idx      = (hsize_t)*idx;
    hid_t       gid        = (-1); /* Temporary group ID */
    int         ret_value  = -1;

    /*
     * Convert FORTRAN name to C name
     */
    if (NULL == (c_name = (char *)HD5f2cstring(name, (size_t)*namelen)))
        goto DONE;

    /*
     * Allocate buffer to hold name of the object
     */
    c_obj_namelen = (size_t)*obj_namelen;
    if (c_obj_namelen)
        if (NULL == (c_obj_name = (char *)malloc(c_obj_namelen + 1)))
            goto DONE;

    /* Get a temporary group ID for the group to query */
    if ((gid = H5Gopen2(c_loc_id, c_name, H5P_DEFAULT)) < 0)
        goto DONE;

    /* Query the object's information */
    if (H5Lget_name_by_idx(gid, ".", H5_INDEX_NAME, H5_ITER_INC, c_idx, c_obj_name, c_obj_namelen,
                           H5P_DEFAULT) < 0)
        goto DONE;
    if (H5Oget_info_by_idx3(gid, ".", H5_INDEX_NAME, H5_ITER_INC, c_idx, &oinfo, H5O_INFO_BASIC,
                            H5P_DEFAULT) < 0)
        goto DONE;

    /* XXX: Switch from using H5Gget_objtype_by_idx() means that this routine won't
     *  work on non-hard links - QAK
     */
    *obj_type = oinfo.type;

    /*
     * Convert C name to FORTRAN and place it in the given buffer
     */
    HD5packFstring(c_obj_name, _fcdtocp(obj_name), c_obj_namelen);
    ret_value = 0;

DONE:
    /* Close the temporary group, if it was opened */
    if (gid > 0)
        H5Gclose(gid);

    if (c_obj_name)
        free(c_obj_name);
    if (c_name)
        free(c_name);
    return ret_value;
}

/****if* H5Gf/h5gn_members_c
 * NAME
 *  h5gn_members_c
 * PURPOSE
 *  Call H5Gget_info_by_name to find number of objects in the group
 * INPUTS
 *  loc_id - file or group identifier
 *  name - name of the group
 *  namelen - name length
 * OUTPUTS
 *  nmemebers - number of members
 * RETURNS
 *  0 on success, -1 on failure
 * SOURCE
 */
int_f
h5gn_members_c(hid_t_f *loc_id, _fcd name, int_f *namelen, int_f *nmembers)
/******/
{
    char      *c_name = NULL;
    H5G_info_t ginfo;
    int        ret_value = -1;

    /*
     * Convert FORTRAN name to C name
     */
    if (NULL == (c_name = (char *)HD5f2cstring(name, (size_t)*namelen)))
        goto DONE;

    /* Call H5Gget_info_by_name() for the number of objects in the group */
    if (H5Gget_info_by_name((hid_t)*loc_id, c_name, &ginfo, H5P_DEFAULT) < 0)
        goto DONE;

    *nmembers = (int_f)ginfo.nlinks;
    ret_value = 0;

DONE:
    if (c_name)
        free(c_name);
    return ret_value;
}

/****if* H5Gf/h5glink_c
 * NAME
 *  h5glink_c
 * PURPOSE
 *  Call H5Glink to link the specified type
 * INPUTS
 *  loc_id - identifier of file or group
 *  link_type - link type
 *  current_name - name of the existing object for hard link,
 *  anything for the soft link
 *  current_namelen - current name length
 *  new_name - new name for the object
 *  new_namelen - new_name length
 * RETURNS
 *  0 on success, -1 on failure
 * SOURCE
 */

int_f
h5glink_c(hid_t_f *loc_id, int_f *link_type, _fcd current_name, int_f *current_namelen, _fcd new_name,
          int_f *new_namelen)
/******/
{
    char *c_current_name = NULL, *c_new_name = NULL;
    int   ret_value = -1;

    /*
     *  Convert Fortran name to C name
     */
    if (NULL == (c_current_name = (char *)HD5f2cstring(current_name, (size_t)*current_namelen)))
        goto DONE;
    if (NULL == (c_new_name = (char *)HD5f2cstring(new_name, (size_t)*new_namelen)))
        goto DONE;

    /*
     *  Call appropriate link creation function
     */
    switch ((H5L_type_t)*link_type) {
        case H5L_TYPE_HARD:
            if (H5Lcreate_hard((hid_t)*loc_id, c_current_name, H5L_SAME_LOC, c_new_name, H5P_DEFAULT,
                               H5P_DEFAULT) < 0)
                goto DONE;
            break;

        case H5L_TYPE_SOFT:
            if (H5Lcreate_soft(c_current_name, (hid_t)*loc_id, c_new_name, H5P_DEFAULT, H5P_DEFAULT) < 0)
                goto DONE;
            break;

            /* Cases below were added to remove the warnings in gcc 4.9.2 and probably other */
        case H5L_TYPE_EXTERNAL:
            ret_value = -1;
            goto DONE;
            break;

        case H5L_TYPE_MAX:
            ret_value = -1;
            goto DONE;
            break;

        case H5L_TYPE_ERROR:
            ret_value = -1;
            goto DONE;
            break;
            /* End of the warnings fix */

        default: /* Unknown/unhandled link type */
            goto DONE;
    } /* end switch */
    ret_value = 0;

DONE:
    if (c_current_name)
        free(c_current_name);
    if (c_new_name)
        free(c_new_name);

    return ret_value;
}

/****if* H5Gf/h5glink2_c
 * NAME
 *  h5glink2_c
 * PURPOSE
 *  Call H5Glink2 to link the specified type
 * INPUTS
 *  cur_loc_id - identifier of file or group
 *  cur_name - name of the existing object for hard link relative
 *  to cur_loc_id location,
 *  anything for the soft link
 *  current_namelen - current name length
 *  link_type - link type
 *  new_loc_id - location identifier
 *  new_name - new name for the object relative to the new_loc_id
 *  location
 *  new_namelen - new_name length
 * RETURNS
 *  0 on success, -1 on failure
 * HISTORY
 *
 * SOURCE
 */

int_f
h5glink2_c(hid_t_f *cur_loc_id, _fcd cur_name, int_f *cur_namelen, int_f *link_type, hid_t_f *new_loc_id,
           _fcd new_name, int_f *new_namelen)
/******/
{
    char *c_cur_name = NULL, *c_new_name = NULL;
    int   ret_value = -1;

    /*
     *  Convert Fortran name to C name
     */
    if (NULL == (c_cur_name = (char *)HD5f2cstring(cur_name, (size_t)*cur_namelen)))
        goto DONE;
    if (NULL == (c_new_name = (char *)HD5f2cstring(new_name, (size_t)*new_namelen)))
        goto DONE;

    /*
     *  Call appropriate link creation function
     */
    switch ((H5L_type_t)*link_type) {
        case H5L_TYPE_HARD:
            if (H5Lcreate_hard((hid_t)*cur_loc_id, c_cur_name, (hid_t)*new_loc_id, c_new_name, H5P_DEFAULT,
                               H5P_DEFAULT) < 0)
                goto DONE;
            break;

        case H5L_TYPE_SOFT:
            if (H5Lcreate_soft(c_cur_name, (hid_t)*new_loc_id, c_new_name, H5P_DEFAULT, H5P_DEFAULT) < 0)
                goto DONE;
            break;
            /* Cases below were added to remove the warnings in gcc 4.9.2 and probably other */
        case H5L_TYPE_EXTERNAL:
            ret_value = -1;
            goto DONE;
            break;

        case H5L_TYPE_MAX:
            ret_value = -1;
            goto DONE;
            break;

        case H5L_TYPE_ERROR:
            ret_value = -1;
            goto DONE;
            break;
            /* End of the warnings fix */

        default: /* Unknown/unhandled link type */
            goto DONE;
    } /* end switch */
    ret_value = 0;

DONE:
    if (c_cur_name)
        free(c_cur_name);
    if (c_new_name)
        free(c_new_name);
    return ret_value;
}

/****if* H5Gf/h5gunlink_c
 * NAME
 *  h5gunlink_c
 * PURPOSE
 *  Call H5Gunlink to remove  the specified name
 * INPUTS
 *  loc_id - identifier of file or group
 *  name - name of the object to unlink
 * RETURNS
 *  0 on success, -1 on failure
 * SOURCE
 */

int_f
h5gunlink_c(hid_t_f *loc_id, _fcd name, int_f *namelen)
/******/
{
    char *c_name    = NULL;
    int   ret_value = -1;

    /*
     *  Convert Fortran name to C name
     */
    if (NULL == (c_name = (char *)HD5f2cstring(name, (size_t)*namelen)))
        goto DONE;

    /*
     *  Call H5Gunlink function
     */
    if (H5Ldelete((hid_t)*loc_id, c_name, H5P_DEFAULT) < 0)
        goto DONE;
    ret_value = 0;

DONE:
    if (c_name)
        free(c_name);
    return ret_value;
}

/****if* H5Gf/h5gmove_c
 * NAME
 *  h5gmove_c
 * PURPOSE
 *  Call H5Gmove to rename an object within an HDF5 file
 * INPUTS
 *  loc_id - identifier of file or group
 *  src_name - name of the original object
 *  src_namelen - original name length
 *  dst_name - new name for the object
 *  dst_namelen - new name length
 * RETURNS
 *  0 on success, -1 on failure
 * SOURCE
 */

int_f
h5gmove_c(hid_t_f *loc_id, _fcd src_name, int_f *src_namelen, _fcd dst_name, int_f *dst_namelen)
/******/
{
    char *c_src_name = NULL, *c_dst_name = NULL;
    int   ret_value = -1;

    /*
     *  Convert Fortran name to C name
     */
    if (NULL == (c_src_name = (char *)HD5f2cstring(src_name, (size_t)*src_namelen)))
        goto DONE;
    if (NULL == (c_dst_name = (char *)HD5f2cstring(dst_name, (size_t)*dst_namelen)))
        goto DONE;

    /*
     *  Call H5Gmove function
     */
    if (H5Lmove((hid_t)*loc_id, c_src_name, H5L_SAME_LOC, c_dst_name, H5P_DEFAULT, H5P_DEFAULT) < 0)
        goto DONE;

    ret_value = 0;

DONE:
    if (c_src_name)
        free(c_src_name);
    if (c_dst_name)
        free(c_dst_name);
    return ret_value;
}

/****if* H5Gf/h5gmove2_c
 * NAME
 *  h5gmove2_c
 * PURPOSE
 *  Call H5Gmove2 to rename an object within an HDF5 file
 * INPUTS
 *  src_loc_id - identifier of file or group
 *  src_name - name of the original object relative to src_loc_id
 *  src_namelen - original name length
 *  dst_loc_id - new location identifier
 *  dst_name - new name for the object relative to dst_loc_id
 *  dst_namelen - new name length
 * RETURNS
 *  0 on success, -1 on failure
 * SOURCE
 */

int_f
h5gmove2_c(hid_t_f *src_loc_id, _fcd src_name, int_f *src_namelen, hid_t_f *dst_loc_id, _fcd dst_name,
           int_f *dst_namelen)
/******/
{
    char *c_src_name = NULL, *c_dst_name = NULL;
    int   ret_value = -1;

    /*
     *  Convert Fortran name to C name
     */
    if (NULL == (c_src_name = (char *)HD5f2cstring(src_name, (size_t)*src_namelen)))
        goto DONE;
    if (NULL == (c_dst_name = (char *)HD5f2cstring(dst_name, (size_t)*dst_namelen)))
        goto DONE;

    /*
     *  Call H5Gmove2 function
     */
    if (H5Lmove((hid_t)*src_loc_id, c_src_name, (hid_t)*dst_loc_id, c_dst_name, H5P_DEFAULT, H5P_DEFAULT) < 0)
        goto DONE;

    ret_value = 0;

DONE:
    if (c_src_name)
        free(c_src_name);
    if (c_dst_name)
        free(c_dst_name);
    return ret_value;
}

/****if* H5Gf/h5gget_linkval_c
 * NAME
 *  h5gget_linkval_c
 * PURPOSE
 *  Call H5Gget_linkval to return the name of object
 * INPUTS
 *  loc_id - identifier of file or group
 *  name - name of the object that symbolic link points to
 *  namelen - the name length
 *  size - length of retrurned value
 * OUTPUTS
 *  value - name to be returned
 * RETURNS
 *  0 on success, -1 on failure
 * SOURCE
 */

int_f
h5gget_linkval_c(hid_t_f *loc_id, _fcd name, int_f *namelen, size_t_f *size, _fcd value)
/******/
{
    char *c_name    = NULL;
    char *c_value   = NULL;
    int   ret_value = -1;

    /*
     *  Convert Fortran name to C name
     */
    if (NULL == (c_name = (char *)HD5f2cstring(name, (size_t)*namelen)))
        goto DONE;

    /*
     *  Allocate buffer to hold name of the value
     */
    if (*size)
        c_value = (char *)malloc((size_t)*size);
    if (c_value == NULL) {
        free(c_name);
        return ret_value;
    }

    /*
     *  Call H5Lget_val function
     */
    if (H5Lget_val((hid_t)*loc_id, c_name, c_value, (size_t)*size, H5P_DEFAULT) < 0)
        goto DONE;

    /*
     *  Convert C name to FORTRAN and place it in the given buffer
     */
    HD5packFstring(c_value, _fcdtocp(value), (size_t)*size);
    ret_value = 0;

DONE:
    if (c_value)
        free(c_value);
    if (c_name)
        free(c_name);
    return ret_value;
}

/****if* H5Gf/h5gset_comment_c
 * NAME
 *  h5gset_comment_c
 * PURPOSE
 *  Call H5Oset_comment_by_name to set comments for the specified object
 * INPUTS
 *  loc_id - identifier of file or group
 *  name - name of object whose comment is to be set or reset
 *  namelen - the name length
 *  comment - the new comment
 *  commentlen - new comment length
 * RETURNS
 *  0 on success, -1 on failure
 * SOURCE
 */
int_f
h5gset_comment_c(hid_t_f *loc_id, _fcd name, int_f *namelen, _fcd comment, int_f *commentlen)
/******/
{
    char *c_name = NULL, *c_comment = NULL;
    int   ret_value = -1;

    /*
     *  Convert Fortran name to C name
     */
    if (NULL == (c_name = (char *)HD5f2cstring(name, (size_t)*namelen)))
        goto DONE;
    if (NULL == (c_comment = (char *)HD5f2cstring(comment, (size_t)*commentlen)))
        goto DONE;

    /*
     *  Call H5Oset_comment_by_name function
     */
    if (H5Oset_comment_by_name((hid_t)*loc_id, c_name, c_comment, H5P_DEFAULT) < 0)
        goto DONE;
    ret_value = 0;

DONE:
    if (c_name)
        free(c_name);
    if (c_comment)
        free(c_comment);
    return ret_value;
}

/****if* H5Gf/h5gget_comment_c
 * NAME
 *  h5gget_comment_c
 * PURPOSE
 *  Call H5Oget_comment_by_name to retrieve comments for the specified object
 * INPUTS
 *  loc_id - identifier of file or group
 *  name - name of object whose comment is to be set or reset
 *  namelen - the name length
 *  bufsize - at most bufsize characters
 *  comment - the new comment
 * RETURNS
 *  0 on success, -1 on failure
 * SOURCE
 */
int_f
h5gget_comment_c(hid_t_f *loc_id, _fcd name, int_f *namelen, size_t_f *bufsize, _fcd comment)
/******/
{
    char  *c_name = NULL, *c_comment = NULL;
    size_t c_bufsize;
    int    ret_value = -1;

    /*
     *  Convert Fortran name to C name
     */
    if (NULL == (c_name = (char *)HD5f2cstring(name, (size_t)*namelen)))
        goto DONE;

    /*
     *  Allocate buffer to hold the comment
     */
    c_bufsize = (size_t)*bufsize;
    if (c_bufsize) {
        if (NULL == (c_comment = (char *)malloc(c_bufsize + 1)))
            goto DONE;
    } /* end if */

    /*
     *  Call H5Oget_comment_by_name function
     */
    if (H5Oget_comment_by_name((hid_t)*loc_id, c_name, c_comment, c_bufsize, H5P_DEFAULT) < 0)
        goto DONE;

    /*
     *  Convert C name to FORTRAN and place it in the given buffer
     */
    HD5packFstring(c_comment, _fcdtocp(comment), c_bufsize);
    ret_value = 0;

DONE:
    if (c_name)
        free(c_name);
    if (c_comment)
        free(c_comment);
    return ret_value;
}

/****if* H5Gf/h5gcreate_anon_c
 * NAME
 *  h5gcreate_anon_c
 * PURPOSE
 *  Call H5Gcreate_anon
 * INPUTS
 *
 *  loc_id  - Location identifier
 *  gcpl_id - Group creation property list identifier
 *  gapl_id - Group access property list identifier
 *
 * OUTPUTS
 *  grp_id - group identifier
 * RETURNS
 *  0 on success, -1 on failure
 * SOURCE
 */
int_f
h5gcreate_anon_c(hid_t_f *loc_id, hid_t_f *gcpl_id, hid_t_f *gapl_id, hid_t_f *grp_id)
/******/
{

    int_f ret_value = 0; /* Return value */

    if ((*grp_id = (hid_t_f)H5Gcreate_anon((hid_t)*loc_id, (hid_t)*gcpl_id, (hid_t)*gapl_id)) < 0)
        HGOTO_DONE(FAIL);

done:
    return ret_value;
}

/****if* H5Gf/h5gget_create_plist_c
 * NAME
 *  h5gget_create_plist_c
 * PURPOSE
 *  Call H5Gget_create_plist
 * INPUTS
 *
 *  grp_id - group identifier
 *
 * OUTPUTS
 *  gcpl_id - Group creation property list identifier
 * RETURNS
 *  0 on success, -1 on failure
 * SOURCE
 */
int_f
h5gget_create_plist_c(hid_t_f *grp_id, hid_t_f *gcpl_id)
/******/
{
    int_f ret_value = 0; /* Return value */

    if ((*gcpl_id = (hid_t_f)H5Gget_create_plist((hid_t)*grp_id)) < 0)
        HGOTO_DONE(FAIL);

done:
    return ret_value;
}
