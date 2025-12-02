/**
 * Projection.h - 投影与视图变换
 * 
 * 包含：
 * - 正交投影矩阵 (OrthoMatrix)
 * - 透视投影矩阵 (PerspectiveMatrix)
 * - 视图矩阵 (LookAtMatrix)
 * - 屏幕坐标投影/反投影
 */

#pragma once

#include<hgl/math/VectorTypes.h>
#include<hgl/math/MatrixTypes.h>

namespace hgl
{
    // ==================== 正交投影矩阵 ====================
    
    /**
     * 生成一个正交投影矩阵（6参数版本）
     * @param left 左边界
     * @param right 右边界
     * @param bottom 底边界
     * @param top 顶边界
     * @param znear 近平面z值
     * @param zfar 远平面z值
     */
    Matrix4f OrthoMatrix( float left,
                          float right,
                          float bottom,
                          float top,
                          float znear,
                          float zfar );

    /**
     * 生成一个正交投影矩阵（4参数版本）
     * @param width 宽度
     * @param height 高度
     * @param znear 近平面z值
     * @param zfar 远平面z值
     */
    Matrix4f OrthoMatrix(float width,float height,float znear,float zfar);

    /**
     * 生成一个正交投影矩阵（2参数版本，近平面=0，远平面=1）
     * @param width 宽度
     * @param height 高度
     */
    Matrix4f OrthoMatrix(float width,float height);

    // ==================== 透视投影矩阵 ====================
    
    /**
     * 生成一个透视投影矩阵
     * @param field_of_view 视野角度（FOV，度数）
     * @param aspect_ratio 宽高比（width/height）
     * @param znear 近截面
     * @param zfar 远截面
     */
    Matrix4f PerspectiveMatrix( float field_of_view,
                                float aspect_ratio,                                
                                float znear,
                                float zfar);

    // ==================== 视图矩阵 ====================
    
    /**
     * 产生一个LookAt变换矩阵
     * @param eye 相机位置
     * @param target 目标位置
     * @param up 向上向量（默认为Z轴）
     */
    Matrix4f LookAtMatrix(const Vector3f &eye,const Vector3f &target,const Vector3f &up=AxisVector::Z);

    // ==================== 坐标投影 ====================
    
    /**
     * 将世界坐标投影到屏幕坐标
     * @param world_pos 世界坐标
     * @param view 视图矩阵
     * @param projection 投影矩阵
     * @param viewport_size 视口大小
     * @return 屏幕坐标
     */
    Vector2i ProjectToScreen(
        const Vector3f& world_pos,
        const Matrix4f& view,
        const Matrix4f& projection,
        const Vector2u& viewport_size);

    /**
     * 将屏幕坐标反投影到世界坐标
     * @param win_pos 屏幕坐标
     * @param view 视图矩阵
     * @param projection 投影矩阵
     * @param viewport_size 视口大小
     * @return 世界坐标
     */
    Vector3f UnProjectToWorld(
        const Vector2i &win_pos,
        const Matrix4f &view,
        const Matrix4f &projection,
        const Vector2u &viewport_size);
}//namespace hgl
