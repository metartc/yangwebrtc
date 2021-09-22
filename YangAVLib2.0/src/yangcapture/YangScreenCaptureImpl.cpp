/*
 * YangScreenCapture.cpp
 *
 *  Created on: 2020年8月30日
 *      Author: yang
 */

#include "YangScreenCaptureImpl.h"

#include "yangutil/sys/YangLog.h"
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <yangutil/yangavinfotype.h>
#ifndef _WIN32
#include <sys/ioctl.h>
#include <linux/fb.h>
int32_t YangScreenCaptureImpl::init(){

	struct fb_var_screeninfo fb_var_info;
	struct fb_fix_screeninfo fb_fix_info;



	// 打开framebuffer设备
	m_fd = open("/dev/fb0", O_RDONLY);
	if(m_fd < 0)
	{
		yang_error("can not open dev\n");
		return ERROR_SYS_Linux_ScreenDeviceOpenFailure;
	}

	// 获取LCD的可变参数
	ioctl(m_fd, FBIOGET_VSCREENINFO, &fb_var_info);
	// 一个像素多少位
	printf("bits_per_pixel: %d\n", fb_var_info.bits_per_pixel);
	// 一个像素多少位
	printf("bits_per_pixel: %d\n", fb_var_info.bits_per_pixel);
	// x分辨率
	printf("xres: %d\n", fb_var_info.xres);
	// y分辨率
	printf("yres: %d\n", fb_var_info.yres);
	// r分量长度(bit)
	printf("red_length: %d\n", fb_var_info.red.length);
	// g分量长度(bit)
	printf("green_length: %d\n", fb_var_info.green.length);
	// b分量长度(bit)
	printf("blue_length: %d\n", fb_var_info.blue.length);
	// t(透明度)分量长度(bit)
	printf("transp_length: %d\n", fb_var_info.transp.length);
	// r分量偏移
	printf("red_offset: %d\n", fb_var_info.red.offset);
	// g分量偏移
	printf("green_offset: %d\n", fb_var_info.green.offset);
	// b分量偏移
	printf("blue_offset: %d\n", fb_var_info.blue.offset);
	// t分量偏移
	printf("transp_offset: %d\n", fb_var_info.transp.offset);

	// 获取LCD的固定参数
	ioctl(m_fd, FBIOGET_FSCREENINFO, &fb_fix_info);
	// 一帧大小
	printf("smem_len: %d\n", fb_fix_info.smem_len);
	// 一行大小
	printf("line_length: %d\n", fb_fix_info.line_length);
	// 一帧大小
	m_bufLen = (fb_var_info.xres * fb_var_info.yres * fb_var_info.bits_per_pixel / 8);
	m_width=fb_var_info.xres;
	m_height=fb_var_info.yres;
	m_rgb = (uint8_t *)malloc(m_bufLen);
	//if(trgb == NULL)		exit(0);
	m_rgb = (uint8_t *)malloc(fb_var_info.xres * fb_var_info.yres * 3);
	//if(m_rgb == NULL)
	//{
	//	goto here;
	//}

	return Yang_Ok;
}
#else
#include <windows.h>
//#include <afxwin.h>
/**
int32_t YangScreenCaptureImpl::init(){
	CWnd *pDesktop = GetDesktopWindow();
	    CDC *pdeskdc = pDesktop->GetDC();
	    CRect re;
	    //获取窗口的大小
	    pDesktop->GetClientRect(&re);
	    CBitmap bmp;
	    bmp.CreateCompatibleBitmap(pdeskdc, re.Width(), re.Height());
	    //创建一个兼容的内存画板
	    CDC memorydc;
	    memorydc.CreateCompatibleDC(pdeskdc);
	    //选中画笔
	    CBitmap *pold = memorydc.SelectObject(&bmp);
	    //绘制图像
	    memorydc.BitBlt(0, 0, re.Width(), re.Height(), pdeskdc, 0, 0, SRCCOPY);
	    //获取鼠标位置，然后添加鼠标图像
	    CPoint32_t po;
	    GetCursorPos(&po);
	    HICON hinco = (HICON)GetCursor();
	    memorydc.DrawIcon(po.x - 10, po.y - 10, hinco);
	    //选中原来的画笔
	    memorydc.SelectObject(pold);
	    BITMAP bit;
	    bmp.GetBitmap(&bit);
	    //定义 图像大小（单位：byte）
	    DWORD size = bit.bmWidthBytes * bit.bmHeight;
	    LPSTR lpdata = (LPSTR)GlobalAlloc(GPTR, size);
	    //后面是创建一个bmp文件的必须文件头
	    BITMAPINFOHEADER pbitinfo;
	    pbitinfo.biBitCount = 24;
	    pbitinfo.biClrImportant = 0;
	    pbitinfo.biCompression = BI_RGB;
	    pbitinfo.biHeight = bit.bmHeight;
	    pbitinfo.biPlanes = 1;
	    pbitinfo.biSize = sizeof(BITMAPINFOHEADER);
	    pbitinfo.biSizeImage = size;
	    pbitinfo.biWidth = bit.bmWidth;
	    pbitinfo.biXPelsPerMeter = 0;
	    pbitinfo.biYPelsPerMeter = 0;
	    GetDIBits(pdeskdc->m_hDC, bmp, 0, pbitinfo.biHeight, lpdata, (BITMAPINFO*)
	        &pbitinfo, DIB_RGB_COLORS);
	    BITMAPFILEHEADER bfh;
	    bfh.bfReserved1 = bfh.bfReserved2 = 0;
	    bfh.bfType = ((WORD)('M' << 8) | 'B');
	    bfh.bfSize = size + 54;
	    bfh.bfOffBits = 54;
	    //写入文件
	    CFile file;
	    CString strFileName(GetAppPathW().c_str());
	    strFileName += _T("ScreenShot\\");
	    CreateDirectory((LPCTSTR)strFileName, NULL);
	    CTime t = CTime::GetCurrentTime();
	    CString tt = t.Format("%Y-%m-%d_%H-%M-%S");
	    strFileName += tt;
	    strFileName += _T(".bmp");
	    if (file.Open((LPCTSTR)strFileName, CFile::modeCreate | CFile::modeWrite))
	    {
	        file.Write(&bfh, sizeof(BITMAPFILEHEADER));
	        file.Write(&pbitinfo, sizeof(BITMAPINFOHEADER));
	        file.Write(lpdata, size);
	        file.Close();
	    }
	    GlobalFree(lpdata);
}
return Yang_Ok;

}**/
#endif
YangScreenCaptureImpl::YangScreenCaptureImpl() {
	m_width=0;
	m_height=0;
	m_rgb=NULL;
	m_bufLen=0;
	m_fd=0;
	m_out_videoBuffer=nullptr;
	isCapture=0;
}

YangScreenCaptureImpl::~YangScreenCaptureImpl() {

}

int32_t YangScreenCaptureImpl::captureFrame(uint8_t *p){
	if(read(m_fd, m_rgb, m_bufLen) < 0)		// 获取一帧数据
		{
			yang_error("read failed!\n");
			return ERROR_SYS_Linux_ScreenCaptureFailure;
		}
	return Yang_Ok;
}

void  YangScreenCaptureImpl::setVideoBuffer(YangVideoBuffer *pbuf){
	 m_out_videoBuffer=pbuf;
}
void YangScreenCaptureImpl::putBuffer(YangFrame* videoFrame){
	m_out_videoBuffer->putVideo(videoFrame);
}

