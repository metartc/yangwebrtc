#ifndef YANGRESIZE_H
#define YANGRESIZE_H


class PicUtilBmp
{
public:
    PicUtilBmp(void);
    virtual ~PicUtilBmp(void);
    //unsigned char *in;
    //unsigned char *out;
    //int inWidth,inHeight,outWidth,outHeight;
    virtual void resize();
    virtual void init(int pinWidth,int pinHeight,int poutWidth,int poutHeight);
    virtual void initYuy2(int pinWidth,int pinHeight,int poutWidth,int poutHeight);
    virtual void initYuy2tonv12(int pinWidth,int pinHeight,int poutWidth,int poutHeight);
    virtual void initNv12(int pinWidth,int pinHeight,int poutWidth,int poutHeight);
    virtual void init420P(int pinWidth,int pinHeight,int poutWidth,int poutHeight);
    virtual void getAddr(unsigned char **p_in,unsigned char **p_out);
    virtual void close();
};



class YangResize
{
public:
    YangResize();
    virtual ~YangResize();
public:

    //unsigned char *in;
    //unsigned char *out;
    PicUtilBmp *pu;


protected:

private:
};


#endif // YANGRESIZE_H
