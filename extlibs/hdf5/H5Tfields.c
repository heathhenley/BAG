/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the files COPYING and Copyright.html.  COPYING can be found at the root   *
 * of the source code distribution tree; Copyright.html can be found at the  *
 * root level of an installed copy of the electronic HDF5 document set and   *
 * is linked from the top-level documents page.  It can also be found at     *
 * http://hdf.ncsa.uiuc.edu/HDF5/doc/Copyright.html.  If you do not have     *
 * access to either file, you may request a copy from hdfhelp@ncsa.uiuc.edu. *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Module Info: This module contains commond functionality for fields in
 *      enumerated & compound datatypes in the H5T interface.
 */

#define H5T_PACKAGE		/*suppress error about including H5Tpkg	  */

#include "H5private.h"		/*generic functions			  */
#include "H5Eprivate.h"		/*error handling			  */
#include "H5Iprivate.h"		/*ID functions		   		  */
#include "H5MMprivate.h"	/*memory management			  */
#include "H5Tpkg.h"		/*data-type functions			  */

#define PABLO_MASK	H5Tfields_mask

/* Interface initialization */
static int interface_initialize_g = 0;
#define INTERFACE_INIT H5T_init_fields_interface
static herr_t H5T_init_fields_interface(void);


/*--------------------------------------------------------------------------
NAME
   H5T_init_fields_interface -- Initialize interface-specific information
USAGE
    herr_t H5T_init_fields_interface()
   
RETURNS
    Non-negative on success/Negative on failure
DESCRIPTION
    Initializes any interface-specific data or routines.  (Just calls
    H5T_init_iterface currently).

--------------------------------------------------------------------------*/
static herr_t
H5T_init_fields_interface(void)
{
    FUNC_ENTER_NOAPI_NOINIT_NOFUNC(H5T_init_fields_interface);

    FUNC_LEAVE_NOAPI(H5T_init());
} /* H5T_init_fields_interface() */


/*-------------------------------------------------------------------------
 * Function:	H5Tget_nmembers
 *
 * Purpose:	Determines how many members TYPE_ID has.  The type must be
 *		either a compound data type or an enumeration data type.
 *
 * Return:	Success:	Number of members defined in the data type.
 *
 *		Failure:	Negative
 *
 * Errors:
 *
 * Programmer:	Robb Matzke
 *		Monday, December  8, 1997
 *
 * Modifications:
 *	Robb Matzke, 22 Dec 1998
 *	Also works with enumeration data types.
 *-------------------------------------------------------------------------
 */
int
H5Tget_nmembers(hid_t type_id)
{
    H5T_t	*dt = NULL;
    int	ret_value;

    FUNC_ENTER_API(H5Tget_nmembers, FAIL);
    H5TRACE1("Is","i",type_id);

    /* Check args */
    if (NULL == (dt = H5I_object_verify(type_id,H5I_DATATYPE)))
	HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "not a data type");

    if((ret_value = H5T_get_nmembers(dt))<0)
	HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "cannot return member number");
    
done:
    FUNC_LEAVE_API(ret_value);
}


/*-------------------------------------------------------------------------
 * Function:	H5T_get_nmembers
 *
 * Purpose:	Private function for H5Tget_nmembers.  Determines how many 
 *              members DTYPE has.  The type must be either a compound data
 *              type or an enumeration data type.
 *
 * Return:	Success:	Number of members defined in the data type.
 *
 *		Failure:	Negative
 *
 * Errors:
 *
 * Programmer:  Raymond Lu	
 *	        October 8, 2002	
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
int
H5T_get_nmembers(const H5T_t *dt)
{
    int	ret_value;

    FUNC_ENTER_NOAPI(H5T_get_nmembers, FAIL);

    assert(dt);

    if (H5T_COMPOUND==dt->type)
	ret_value = dt->u.compnd.nmembs;
    else if (H5T_ENUM==dt->type)
	ret_value = dt->u.enumer.nmembs;
    else
	HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "operation not supported for type class");
    
done:
    FUNC_LEAVE_NOAPI(ret_value);
}


/*-------------------------------------------------------------------------
 * Function:	H5Tget_member_name
 *
 * Purpose:	Returns the name of a member of a compound or enumeration
 *		data type. Members are stored in no particular order with
 *		numbers 0 through N-1 where N is the value returned by
 *		H5Tget_nmembers().
 *
 * Return:	Success:	Ptr to a string allocated with malloc().  The
 *				caller is responsible for freeing the string.
 *
 *		Failure:	NULL
 *
 * Programmer:	Robb Matzke
 *		Wednesday, January  7, 1998
 *
 * Modifications:
 *	Robb Matzke, 22 Dec 1998
 *	Also works with enumeration data types.
 *-------------------------------------------------------------------------
 */
char *
H5Tget_member_name(hid_t type_id, int membno)
{
    H5T_t	*dt = NULL;
    char	*ret_value;

    FUNC_ENTER_API(H5Tget_member_name, NULL);

    /* Check args */
    if (NULL == (dt = H5I_object_verify(type_id,H5I_DATATYPE)))
	HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, NULL, "not a data type");

    if((ret_value = H5T_get_member_name(dt, membno))==NULL)
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, NULL, "unable to get member name");

done:
    FUNC_LEAVE_API(ret_value);
}


/*-------------------------------------------------------------------------
 * Function:	H5T_get_member_name
 *
 * Purpose:	Private function for H5Tget_member_name.  Returns the name 
 *              of a member of a compound or enumeration data type. Members
 *              are stored in no particular order with numbers 0 through 
 *              N-1 where N is the value returned by H5Tget_nmembers().
 *
 * Return:	Success:	Ptr to a string allocated with malloc().  The
 *				caller is responsible for freeing the string.
 *
 *		Failure:	NULL
 *
 * Programmer:	Raymond Lu
 *              October 9, 2002	
 *
 * Modifications:
 *-------------------------------------------------------------------------
 */
char *
H5T_get_member_name(H5T_t *dt, int membno)
{
    char	*ret_value;

    FUNC_ENTER_NOAPI(H5T_get_member_name, NULL);

    assert(dt);

    switch (dt->type) {
        case H5T_COMPOUND:
            if (membno<0 || membno>=dt->u.compnd.nmembs)
                HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "invalid member number");
            ret_value = H5MM_xstrdup(dt->u.compnd.memb[membno].name);
            break;

        case H5T_ENUM:
            if (membno<0 || membno>=dt->u.enumer.nmembs)
                HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "invalid member number");
            ret_value = H5MM_xstrdup(dt->u.enumer.name[membno]);
            break;
            
        default:
            HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, NULL, "operation not supported for type class");
    }

done:
    FUNC_LEAVE_NOAPI(ret_value);
}


/*-------------------------------------------------------------------------
 * Function:    H5Tget_member_index
 *
 * Purpose:     Returns the index of a member in a compound or enumeration
 *              data type by given name.Members are stored in no particular 
 *              order with numbers 0 through N-1 where N is the value 
 *              returned by H5Tget_nmembers().
 *      
 * Return:      Success:        index of the member if exists.
 *              Failure:        -1.
 *
 * Programmer:  Raymond Lu
 *              Thursday, April 4, 2002
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
int
H5Tget_member_index(hid_t type_id, const char *name)
{
    H5T_t       *dt = NULL;
    int         ret_value=FAIL;
    int         nmembs, i;

    FUNC_ENTER_API(H5Tget_member_index, FAIL);
    H5TRACE2("Is","is",type_id,name);

    /* Check arguments */
    assert(name);
    if(NULL==(dt=H5I_object_verify(type_id,H5I_DATATYPE)))
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "not a data type");

    /* Locate member by name */
    switch (dt->type) {
        case H5T_COMPOUND:
            nmembs = dt->u.compnd.nmembs;
            for(i=0; i<nmembs; i++) {
                if(!HDstrcmp(dt->u.compnd.memb[i].name, name))
                    HGOTO_DONE(i);
            }
            break;  
        case H5T_ENUM:
            nmembs = dt->u.enumer.nmembs;
            for(i=0; i<nmembs; i++) {
                if(!HDstrcmp(dt->u.enumer.name[i], name))
                    HGOTO_DONE(i);
            }
            break;
        default:
            HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "operation not supported for this type");
    }

done:
    FUNC_LEAVE_API(ret_value);
}


/*-------------------------------------------------------------------------
 * Function:	H5T_sort_value
 *
 * Purpose:	Sorts the members of a compound data type by their offsets;
 *		sorts the members of an enum type by their values. This even
 *		works for locked data types since it doesn't change the value
 *		of the type.  MAP is an optional parallel integer array which
 *		is also swapped along with members of DT.
 *
 * Return:	Non-negative on success/Negative on failure
 *
 * Programmer:	Robb Matzke
 *		Wednesday, January  7, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5T_sort_value(H5T_t *dt, int *map)
{
    int		i, j, nmembs;
    size_t	size;
    hbool_t	swapped;
    uint8_t	tbuf[32];
    herr_t ret_value=SUCCEED;   /* Return value */

    FUNC_ENTER_NOAPI(H5T_sort_value, FAIL);

    /* Check args */
    assert(dt);
    assert(H5T_COMPOUND==dt->type || H5T_ENUM==dt->type);

    /* Use a bubble sort because we can short circuit */
    if (H5T_COMPOUND==dt->type) {
	if (H5T_SORT_VALUE!=dt->u.compnd.sorted) {
	    dt->u.compnd.sorted = H5T_SORT_VALUE;
	    nmembs = dt->u.compnd.nmembs;
	    for (i=nmembs-1, swapped=TRUE; i>0 && swapped; --i) {
		for (j=0, swapped=FALSE; j<i; j++) {
		    if (dt->u.compnd.memb[j].offset >
                            dt->u.compnd.memb[j+1].offset) {
                            H5T_cmemb_t tmp = dt->u.compnd.memb[j];
			dt->u.compnd.memb[j] = dt->u.compnd.memb[j+1];
			dt->u.compnd.memb[j+1] = tmp;
			if (map) {
			    int x = map[j];
			    map[j] = map[j+1];
			    map[j+1] = x;
			}
			swapped = TRUE;
		    }
		}
	    }
#ifndef NDEBUG
	    /* I never trust a sort :-) -RPM */
	    for (i=0; i<nmembs-1; i++) {
		assert(dt->u.compnd.memb[i].offset <
		       dt->u.compnd.memb[i+1].offset);
	    }
#endif
	}
    } else if (H5T_ENUM==dt->type) {
	if (H5T_SORT_VALUE!=dt->u.enumer.sorted) {
	    dt->u.enumer.sorted = H5T_SORT_VALUE;
	    nmembs = dt->u.enumer.nmembs;
	    size = dt->size;
	    assert(size<=sizeof(tbuf));
	    for (i=nmembs-1, swapped=TRUE; i>0 && swapped; --i) {
		for (j=0, swapped=FALSE; j<i; j++) {
		    if (HDmemcmp(dt->u.enumer.value+j*size,
				 dt->u.enumer.value+(j+1)*size,
				 size)>0) {
			/* Swap names */
			char *tmp = dt->u.enumer.name[j];
			dt->u.enumer.name[j] = dt->u.enumer.name[j+1];
			dt->u.enumer.name[j+1] = tmp;

			/* Swap values */
			HDmemcpy(tbuf, dt->u.enumer.value+j*size, size);
			HDmemcpy(dt->u.enumer.value+j*size,
				 dt->u.enumer.value+(j+1)*size, size);
			HDmemcpy(dt->u.enumer.value+(j+1)*size, tbuf, size);
			
			/* Swap map */
			if (map) {
			    int x = map[j];
			    map[j] = map[j+1];
			    map[j+1] = x;
			}

			swapped = TRUE;
		    }
		}
	    }
#ifndef NDEBUG
	    /* I never trust a sort :-) -RPM */
	    for (i=0; i<nmembs-1; i++) {
		assert(HDmemcmp(dt->u.enumer.value+i*size,
				dt->u.enumer.value+(i+1)*size,
				size)<0);
	    }
#endif
	}
    }
    
done:
    FUNC_LEAVE_NOAPI(ret_value);
}


/*-------------------------------------------------------------------------
 * Function:	H5T_sort_name
 *
 * Purpose:	Sorts members of a compound or enumeration data type by their
 *		names. This even works for locked data types since it doesn't
 *		change the value of the types.
 *
 * Return:	Success:	Non-negative
 *
 *		Failure:	Negative
 *
 * Programmer:	Robb Matzke
 *              Monday, January  4, 1999
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5T_sort_name(H5T_t *dt, int *map)
{
    int		i, j, nmembs;
    size_t	size;
    hbool_t	swapped;
    uint8_t	tbuf[32];
    herr_t ret_value=SUCCEED;   /* Return value */

    FUNC_ENTER_NOAPI(H5T_sort_name, FAIL);

    /* Check args */
    assert(dt);
    assert(H5T_COMPOUND==dt->type || H5T_ENUM==dt->type);

    /* Use a bubble sort because we can short circuit */
    if (H5T_COMPOUND==dt->type) {
	if (H5T_SORT_NAME!=dt->u.compnd.sorted) {
	    dt->u.compnd.sorted = H5T_SORT_NAME;
	    nmembs = dt->u.compnd.nmembs;
	    for (i=nmembs-1, swapped=TRUE; i>0 && swapped; --i) {
		for (j=0, swapped=FALSE; j<i; j++) {
		    if (HDstrcmp(dt->u.compnd.memb[j].name,
				 dt->u.compnd.memb[j+1].name)>0) {
			H5T_cmemb_t tmp = dt->u.compnd.memb[j];
			dt->u.compnd.memb[j] = dt->u.compnd.memb[j+1];
			dt->u.compnd.memb[j+1] = tmp;
			swapped = TRUE;
			if (map) {
			    int x = map[j];
			    map[j] = map[j+1];
			    map[j+1] = x;
			}
		    }
		}
	    }
#ifndef NDEBUG
	    /* I never trust a sort :-) -RPM */
	    for (i=0; i<nmembs-1; i++) {
		assert(HDstrcmp(dt->u.compnd.memb[i].name, 
				dt->u.compnd.memb[i+1].name)<0);
	    }
#endif
	}
    } else if (H5T_ENUM==dt->type) {
	if (H5T_SORT_NAME!=dt->u.enumer.sorted) {
	    dt->u.enumer.sorted = H5T_SORT_NAME;
	    nmembs = dt->u.enumer.nmembs;
	    size = dt->size;
	    assert(size<=sizeof(tbuf));
	    for (i=nmembs-1, swapped=TRUE; i>0 && swapped; --i) {
		for (j=0, swapped=FALSE; j<i; j++) {
		    if (HDstrcmp(dt->u.enumer.name[j],
				 dt->u.enumer.name[j+1])>0) {
			/* Swap names */
			char *tmp = dt->u.enumer.name[j];
			dt->u.enumer.name[j] = dt->u.enumer.name[j+1];
			dt->u.enumer.name[j+1] = tmp;

			/* Swap values */
			HDmemcpy(tbuf, dt->u.enumer.value+j*size, size);
			HDmemcpy(dt->u.enumer.value+j*size,
				 dt->u.enumer.value+(j+1)*size, size);
			HDmemcpy(dt->u.enumer.value+(j+1)*size, tbuf, size);

			/* Swap map */
			if (map) {
			    int x = map[j];
			    map[j] = map[j+1];
			    map[j+1] = x;
			}

			swapped = TRUE;
		    }
		}
	    }
#ifndef NDEBUG
	    /* I never trust a sort :-) -RPM */
	    for (i=0; i<nmembs-1; i++)
		assert(HDstrcmp(dt->u.enumer.name[i], dt->u.enumer.name[i+1])<0);
#endif
	}
    }
    
done:
    FUNC_LEAVE_NOAPI(ret_value);
}
