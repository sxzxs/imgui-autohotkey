#ifndef __DXGICAPTURE_H__
#define __DXGICAPTURE_H___

extern "C" _declspec(dllexport) int dxgi_init(int time_out);
extern "C" _declspec(dllexport) int dxgi_pixelgetcolor(int x, int y, char out_str[6], void* ptr_data, void* ptr_bitmap_scan0, void* ptr_bitmap, int* is_time_out);
extern "C" _declspec(dllexport) int dxgi_save2file(char filename[100]);
#endif
