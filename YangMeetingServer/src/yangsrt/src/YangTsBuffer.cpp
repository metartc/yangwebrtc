#include <assert.h>
//#include <algorithm>
//#include <iterator>
#include <memory.h>
#include "../include/YangTsBuffer.h"
YangTsBuffer::YangTsBuffer()
    : _pos(0)
{
	curPos=0;
}

YangTsBuffer::YangTsBuffer(int32_t size, int8_t value)
    : _pos(0)
{
	curPos=0;
   // _data = std::vector<char>(size, value);
}

YangTsBuffer::~YangTsBuffer()
{
}

void YangTsBuffer::write_1byte(int8_t val)
{
	_data[curPos++]=val;

    //_data.push_back(val);
}

void YangTsBuffer::write_2bytes(int16_t val)
{
    char *p = (char *)&val;

    for (int i = 1; i >= 0; --i) {
    	_data[curPos++]=p[i];
        //_data.push_back(p[i]);
    }
}

void YangTsBuffer::write_3bytes(int32_t val)
{
    char *p = (char *)&val;

    for (int i = 2; i >= 0; --i) {
    	_data[curPos++]=p[i];
       // _data.push_back(p[i]);
    }
}

void YangTsBuffer::write_4bytes(int32_t val)
{
    char *p = (char *)&val;

    for (int i = 3; i >= 0; --i) {
    	_data[curPos++]=p[i];
        //_data.push_back(p[i]);
    }
}

void YangTsBuffer::write_8bytes(int64_t val)
{
    char *p = (char *)&val;

    for (int i = 7; i >= 0; --i) {
    	_data[curPos++]=p[i];
       // _data.push_back(p[i]);
    }
}
void YangTsBuffer::writeBytes(uint8_t* bytes,int size){
	memcpy(_data+curPos,bytes,size);
	   curPos+=size;
}
//void YangTsBuffer::write_string(std::string val)
//{
 //  memcpy(_data+curPos,val.c_str(),val.length());
 //  curPos+=val.length();
	//std::copy(val.begin(), val.end(), std::back_inserter(_data));
//}

void YangTsBuffer::append(const uint8_t* bytes, int size)
{
    if (!bytes || size <= 0)
        return;
    memcpy(_data+curPos,bytes,size);
    curPos+=size;
   // _data.insert(_data.end(), bytes, bytes + size);
}

int8_t YangTsBuffer::read_1byte()
{
    assert(require(1));

    int8_t val = _data[_pos];//_data.at(0 + _pos);
    _pos++;

    return val;
}

int16_t YangTsBuffer::read_2bytes()
{
    assert(require(2));

    int16_t val = 0;
    char *p = (char *)&val;

    for (int i = 1; i >= 0; --i) {
        p[i] = _data[_pos];//_data.at(0 + _pos);
        _pos++;
    }

    return val;
}

int32_t YangTsBuffer::read_3bytes()
{
    assert(require(3));

    int32_t val = 0;
    char *p = (char *)&val;

    for (int i = 2; i >= 0; --i) {
        p[i] =  _data[_pos];//_data.at(0 + _pos);
        _pos++;
    }

    return val;
}

int32_t YangTsBuffer::read_4bytes()
{
    assert(require(4));

    int32_t val = 0;
    char *p = (char *)&val;

    for (int i = 3; i >= 0; --i) {
        p[i] =  _data[_pos];//_data.at(0 + _pos);
        _pos++;
    }

    return val;
}

int64_t YangTsBuffer::read_8bytes()
{
    assert(require(8));

    int64_t val = 0;
    char *p = (char *)&val;

    for (int i = 7; i >= 0; --i) {
        p[i] =  _data[_pos];//_data.at(0 + _pos);
        _pos++;
    }

    return val;
}
void YangTsBuffer::readBytes(uint8_t *p,int len){
	memcpy(p,_data+_pos,len);
	_pos += len;
}
std::string YangTsBuffer::read_string(int len)
{
    assert(require(len));

    std::string val((char*)_data + _pos, len);
    _pos += len;

    return val;
}

void YangTsBuffer::skip(int size)
{
    _pos += size;
}

bool YangTsBuffer::require(int required_size)
{
    assert(required_size >= 0);

    return required_size <= curPos-_pos;//_data.size() - _pos;
}

bool YangTsBuffer::empty()
{
    return _pos >= curPos;//_data.size();
}

int YangTsBuffer::size()
{
    return curPos;//_data.size();
}

int YangTsBuffer::pos()
{
    return _pos;
}

uint8_t *YangTsBuffer::data()
{
    return (size() == 0) ? nullptr : _data;
}

void YangTsBuffer::clear()
{
    _pos = 0;
    curPos=0;
    //_data.clear();
}

void YangTsBuffer::set_data(int pos, const uint8_t *data, int len)
{
    if (!data)
        return;

    if (pos + len > size()) {
        return;
    }

    for (int i = 0; i < len; i++) {
        _data[pos + i] = data[i];
    }
}

std::string YangTsBuffer::to_string()
{
    return std::string(_data, _data+curPos);
}
