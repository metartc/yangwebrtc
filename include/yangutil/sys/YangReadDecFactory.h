/*
 * YangReadDecFactory.h
 *
 *  Created on: 2020年9月9日
 *      Author: yang
 */

#ifndef YANGREADDECFACTORY_H_
#define YANGREADDECFACTORY_H_

class YangReadDec{
public:
	YangReadDec();
	virtual ~YangReadDec();
	virtual int isValid()=0;
	virtual int getSystemId()=0;
	virtual int getIsDate()=0;
	virtual int getUserCount()=0;
};

class YangReadDecFactory {
public:
	YangReadDecFactory();
	virtual ~YangReadDecFactory();
	YangReadDec *createReadDec();

};

#endif /* YANGREADDECFACTORY_H_ */
