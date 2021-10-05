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
	virtual int32_t isValid()=0;
	virtual int32_t getSystemId()=0;
	virtual int32_t getIsDate()=0;
	virtual int32_t getUserCount()=0;
};

class YangReadDecFactory {
public:
	YangReadDecFactory();
	virtual ~YangReadDecFactory();
	YangReadDec *createReadDec();

};

#endif /* YANGREADDECFACTORY_H_ */
