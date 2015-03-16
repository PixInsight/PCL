/*
 * error.h
 *
 *  Created on: May 29, 2014
 *      Author: klaus
 */

#ifndef ERROR_H_
#define ERROR_H_

#include "pcl/Exception.h"

#define ERR_MSG(message) \
		String().Format("Error in %s line %d: %s",__FILE__,__LINE__,message)

#define CHECK_POINTER(PTR) \
		if (PTR==NULL) {\
		  throw FatalError(String().Format("Error in %s line %d: Null pointer exception",__FILE__,__LINE__));\
		}


#endif /* ERROR_H_ */
