#pragma once
#pragma warning(disable:4996)
/*
离散傅里叶变换
图像高频部分代表了图像的细节、纹理信息；低频代表了图像的轮廓信息。
低通-》模糊
高通-》锐化
腐蚀和膨胀是针对白色部分（高亮部分）而言的。膨胀就是对图像高亮部分进行“领域扩张”，效果图拥有比原图更大的高亮区域；腐蚀是原图中的高亮区域被蚕食，效果图拥有比原图更小的高亮区域。
开运算：先腐蚀再膨胀，用来消除小物体
闭运算：先膨胀再腐蚀，用于排除小型黑洞
形态学梯度：就是膨胀图与俯视图之差，用于保留物体的边缘轮廓。
顶帽：原图像与开运算图之差，用于分离比邻近点亮一些的斑块。
黑帽：闭运算与原图像之差，用于分离比邻近点暗一些的斑块。
*/

void blog_demo2_create_mat();

void blog_demo2_threshold_binary();
void blog_demo2_erode();
void blog_demo2_blur();
void blog_demo2_canny();
void on_track(int, void*);
void blog_demo2_trackbar();
void blog_demo2_data_sctructure();
void blog_demo2_access_to_pixel();
void blog_demo2_equalize_hist();