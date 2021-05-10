#ifndef HGL_MATH_PHYSICS_CONST_INCLUDE
#define HGL_MATH_PHYSICS_CONST_INCLUDE

namespace hgl
{
    constexpr double HGL_GOLDEN_RATIO                       =0.61803398874989484820458683436563811772030917980576;//�ƽ����
    constexpr double HGL_SILVER_RATIO                       =2.4142135623730950488; //��������

    constexpr double HGL_SPEED_OF_SOUND                     =331.3f;                //����(��/��)
    constexpr double HGL_SPEED_OF_LIGHT                     =299792458;             //����(��/��)

    constexpr double HGL_ABSOLUTE_ZERO                      =-273.15f;              //�������

    constexpr double HGL_UNIVERSAL_GRAVITATION              =6.67384e-11;           //������������

    constexpr double HGL_EARTH_GRAVITATIONAL_ACCELERATION   =9.80665;               //�����ϵ��������ٶ�(ţ��)
    constexpr double HGL_EARTH_MASS                         =5.9722e+24;            //��������
    constexpr double HGL_EARTH_RADIUS                       =6371000;               //����뾶(��)

    /**
     * ����������������
     * @param m1 ��������
     * @param m2 ��������
     * @param length ���������ĵľ���
     */
    template<typename T>
    inline T UniversalGravitation(const T m1,const T m2,const T length)
    {
        return HGL_UNIVERSAL_GRAVITATION*((m1*m2)/(length*length));
    }

    /**
     * �����������ٶȼ���<br>
     * ������: (��������*������������)/(����뾶��ƽ��)=�����ϵ��������ٶ�
     * @param m ��������
     * @param raidus ����뾶
     * @return ������������ٶ�(ţ��)
     */
    template<typename T>
    inline T UniversalGravitation(const T m,const T radius)
    {
        return (HGL_UNIVERSAL_GRAVITATION*m)/(radius*radius);
    }

    /**
     * ����������
     * @param m ����
     * @param v �ٶ�
     * @param r ����
     */
    template<typename T>
    inline T CentripetalForce(const T m,const T v,const T r)
    {
        return (m*v*v)/r;
    }

    /**
     * ���ٶȼ���
     * @param power ����
     * @param weight ����
     */
    template<typename T>
    inline T AddSpeed(const T &power,const T &weight)
    {
        return power/weight;
    }

    /**
     * �����˶���������(�����˶�Խ�죬����Խ��)
     * @param m0 ��ֹ����
     * @param v �˶��ٶ�
     */
    template<typename T>
    inline T RunWeight(const T &m0,const T &v)
    {
        return m0/sqrt(1-(v*v)/(HGL_SPEED_OF_LIGHT*HGL_SPEED_OF_LIGHT));
    }
}//namespace hgl
#endif//HGL_MATH_PHYSICS_CONST_INCLUDE
