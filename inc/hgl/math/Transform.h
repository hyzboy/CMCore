#pragma once
#include<hgl/math/Matrix.h>
#include<hgl/type/ObjectList.h>

namespace hgl
{
    /**
    * 变换基类
    */
    class TransformBase
    {
        uint version;
        uint cur_version;

        Matrix4f CurMatrix;

    protected:

        virtual uint UpdateVersion()
        {
            //版本号只是为了记录变化，让程序知道和上次不一样，所以最大值是多少其实无所谓的
            version=(version >= 1000000)?0:++version;

            return version;
        }

        virtual void UpdateMatrix(Matrix4f &)=0;

    public:

        TransformBase()
        {
            version=0;
            cur_version=0;
            CurMatrix=Identity4f;
        }
        virtual ~TransformBase()=default;

        virtual constexpr const size_t GetTypeHash()const=0;                ///<取得类型哈希值

        const uint GetVersion()const{return version;}                       ///<取得当前数据版本号

        virtual const uint GetMatrix(Matrix4f &mat)                         ///<取得当前矩阵，并返回当前矩阵版本号
        {
            if (cur_version != version)
            {
                UpdateMatrix(CurMatrix);
                cur_version=version;
            }

            mat=CurMatrix;
            return cur_version;
        }
    };//class TransformBase

    class TransformMatrix :public TransformBase
    {
        Matrix4f transform_matrix;

    public:

        virtual constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformMatrix>(); }

        const Matrix4f &GetTransformMatrix()const { return transform_matrix; }

        void SetTransformMatrix(const Matrix4f &mat)
        {
            if (transform_matrix==mat)
                return;

            transform_matrix=mat;
            UpdateVersion();
        }

        virtual void UpdateMatrix(Matrix4f &mat) override
        {
            mat=transform_matrix;
        }
    };//class TransformMatrix

    class TransformTranslate3f :public TransformBase
    {
        Vector3f offset;

    public:

        virtual constexpr const size_t GetTypeHash()const override{return hgl::GetTypeHash<TransformTranslate3f>();}

        const Vector3f &GetOffset()const { return offset; }
        void SetOffset(const Vector3f &o)
        {
            if(IsNearlyEqual(offset,o))
                return;

            offset=o;
            UpdateVersion();
        }

        void AddOffset(const Vector3f &o)
        {
            if (IsNearlyZero(o))
                return;

            offset+=o;
            UpdateVersion();
        }

        virtual void UpdateMatrix(Matrix4f &mat) override
        {
            mat=translate(offset);
        }
    };//class TransformTranslate

    class TransformRotateQuat :public TransformBase
    {
        Quatf quat;

    public:

        virtual constexpr const size_t GetTypeHash()const override{return hgl::GetTypeHash<TransformRotateQuat>();}

        const Quatf &GetQuat()const { return quat; }
        void SetQuat(const Quatf &q)
        {
            if(IsNearlyEqual(quat,q))
                return;

            quat=q;
            UpdateVersion();
        }

        virtual void UpdateMatrix(Matrix4f &mat) override
        {
            mat=ToMatrix(quat);
        }
    };//class TransformRotateQuat

    class TransformRotateAxis :public TransformBase
    {
        Vector3f axis;
        float angle;

    public:

        virtual constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformRotateAxis>(); }

        const Vector3f &GetAxis()const { return axis; }
        const float GetAngle()const { return angle; }

        void SetAxisAngle(const Vector3f &a,float ang)
        {
            if (IsNearlyEqual(axis,a) && IsNearlyEqual(angle,ang))
                return;

            axis=a;
            angle=ang;
            UpdateVersion();
        }

        void SetAxis(const Vector3f &a)
        {
            if (IsNearlyEqual(axis,a))
                return;

            axis=a;
            UpdateVersion();
        }

        void SetAngle(float a)
        {
            if (IsNearlyEqual(angle,a))
                return;

            angle=a;
            UpdateVersion();
        }

        virtual void UpdateMatrix(Matrix4f &mat) override
        {
            mat=rotate(angle,axis);
        }
    };//class TransformRotateAxis

    class TransformRotateEuler :public TransformBase
    {
        float pitch;        ///<绕X轴旋转弧度
        float yaw;          ///<绕Y轴旋转弧度
        float roll;         ///<绕Z轴旋转弧度

    public:

        virtual constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformRotateEuler>(); }

        const Vector3f &GetEuler()const { return Vector3f(pitch,yaw,roll); }

        const float GetPitch()const { return pitch; }
        const float GetYaw()const { return yaw; }
        const float GetRoll()const { return roll; }

        void SetEuler(const Vector3f &e)
        {
            if (IsNearlyEqual(e.x,pitch)
                &&IsNearlyEqual(e.y,yaw)
                &&IsNearlyEqual(e.z,roll))
                return;

            pitch=e.x;
            yaw=e.y;
            roll=e.z;

            UpdateVersion();
        }

        void SetPitch(float p)
        {
            if (IsNearlyEqual(pitch,p))
                return;

            pitch=p;
            UpdateVersion();
        }

        void SetYaw(float y)
        {
            if (IsNearlyEqual(yaw,y))
                return;

            yaw=y;
            UpdateVersion();
        }

        void SetRoll(float r)
        {
            if (IsNearlyEqual(roll,r))
                return;

            roll=r;
            UpdateVersion();
        }

        virtual void UpdateMatrix(Matrix4f &mat) override
        {
            mat=glm::eulerAngleXYZ(pitch,yaw,roll);
        }
    };//class TransformRotateEuler

    class TransformScale3f :public TransformBase
    {
        Vector3f scale3f;

    public:

        virtual constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformScale3f>(); }

        const Vector3f &GetScale()const { return scale3f; }
        void SetScale(const Vector3f &s)
        {
            if (IsNearlyEqual(scale3f,s))
                return;

            scale3f=s;
            UpdateVersion();
        }

        virtual void UpdateMatrix(Matrix4f &mat) override
        {
            mat=scale(scale3f);
        }
    };//class TransformScale

    class TransformLookAt :public TransformBase
    {
        Vector3f eye;
        Vector3f center;
        Vector3f up;

    public:

        virtual constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformLookAt>(); }

        const Vector3f &GetEye()const { return eye; }
        const Vector3f &GetCenter()const { return center; }
        const Vector3f &GetUp()const { return up; }

        void SetLookAt(const Vector3f &e,const Vector3f &c,const Vector3f &u)
        {
            if (IsNearlyEqual(eye,e) && IsNearlyEqual(center,c) && IsNearlyEqual(up,u))
                return;

            eye=e;
            center=c;
            up=u;
            UpdateVersion();
        }

        void SetEye(const Vector3f &pos)
        {
            if(IsNearlyEqual(eye,pos))
                return;

            eye=pos;
            UpdateVersion();
        }

        void SetCenter(const Vector3f &pos)
        {
            if (IsNearlyEqual(center,pos))
                return;

            center=pos;
            UpdateVersion();
        }

        void SetUp(const Vector3f &pos)
        {
            if (IsNearlyEqual(up,pos))
                return;

            up=pos;
            UpdateVersion();
        }

        virtual void UpdateMatrix(Matrix4f &mat) override
        {
            mat=lookat(eye,center,up);
        }
    };//class TransformLookAt

    class TransformManager
    {
        uint version;

        ObjectList<TransformBase> transform_list;

    public:

        TransformManager()=default;
        virtual ~TransformManager()=default;

        void Clear()
        {
            transform_list.Clear();
        }
      
        void AddTransform(TransformBase *tb)
        {
            transform_list.Add(tb);
        }

        void RemoveTransform(TransformBase *tb)
        {
            transform_list.DeleteByValue(tb);
        }


    };//class TransformManager

    /**
     * 变换矩阵<Br>
     * 便于分散管理平移、旋转、缩放等数值
     */
    class Transform
    {
        uint32 version;     ///<版本号（注：用于记录任何变化，而不是matrix）

        void UpdateVersion()
        {
            if(version>=0x70000000)
                version=0;
            else
                ++version;
        }

    protected:

        bool is_identity;
        bool is_zero_rotate;
        bool matrix_dirty;

        //matrix其实用的少，所以在不取matrix时，并不会计算。
        Matrix4f matrix;
        Matrix4f inverse_matrix;
        Matrix4f transpose_inverse_matrix;

        Vector3f translation_vector;
        Quatf rotation_quat;
        Vector3f rotation_axis;
        float rotate_angle;

        Vector3f scale_vector;

    protected:

        void UpdateQuat();

    public:

        void UpdateMatrix();

        const uint32    GetVersion  (){UpdateMatrix();return version;}
        const bool      IsIdentity  (){UpdateMatrix();return is_identity;}
        const Matrix4f &GetMatrix   (){UpdateMatrix();return matrix;}

        const Matrix4f GetMatrix()const     //不能执行UpdateMatrix时的获取
        {
            if(matrix_dirty)
                return translate(translation_vector)*ToMatrix(rotation_quat)*scale(scale_vector);
            else
                return matrix;
        }

        operator const Matrix4f &   (){return GetMatrix();}
        operator const Transform &  (){UpdateMatrix();return *this;}

        const Matrix4f &GetInverseMatrix()
        {
            UpdateMatrix();
            return inverse_matrix;
        }


        const Vector3f &GetTranslation  ()const{return translation_vector;}
        const Vector3f &GetScale        ()const{return scale_vector;}

        const Quatf &   GetRotationQuat ()const{return rotation_quat;}
        const Vector3f &GetRotationAxis ()const{return rotation_axis;}
        const float     GetRotateAngle  ()const{return rotate_angle;}

        void SetTranslation(const float x,const float y,const float z)
        {
            if(is_identity)
            {
                if(IsNearlyZero(x)
                 &&IsNearlyZero(y)
                 &&IsNearlyZero(z))
                    return;
            }

            is_identity=false;

            translation_vector.x=x;
            translation_vector.y=y;
            translation_vector.z=z;
            matrix_dirty=true;
        }

        void SetTranslation(const Vector3f &v)
        {
            if(is_identity)
            {
                if(IsNearlyZero(v.x)
                 &&IsNearlyZero(v.y)
                 &&IsNearlyZero(v.z))
                    return;
            }

            is_identity=false;

            translation_vector=v;
            matrix_dirty=true;
        }

        void ClearRotation()
        {
            rotation_quat=IdentityQuatf;
            rotation_axis=ZeroVector3f;
            rotate_angle=0;
            matrix_dirty=true;
        }

        void SetRotation(const Quatf &q)
        {
            if(is_identity)
            {
                if(q==IdentityQuatf)
                    return;
            }

            is_identity=false;

            rotation_quat=q;
            ExtractedQuat(q,rotation_axis,rotate_angle);
            matrix_dirty=true;
        }

        void SetRotation(const Vector3f &axis,const float angle)
        {
            if(is_identity)
            {
                if(IsNearlyZero(angle)==0)
                    return;
            }

            is_identity=false;

            rotation_axis=axis;
            rotate_angle=angle;
            UpdateQuat();
        }

        void SetRotation(const AXIS &axis,const float angle)
        {
            if(is_identity)
            {
                if(IsNearlyZero(angle)==0)
                    return;
            }

            is_identity=false;

            rotation_axis=GetAxisVector(axis);
            rotate_angle=angle;
            UpdateQuat();
        }

        void SetRotateAngle(float angle)
        {
            if(is_identity)
            {
                if(IsNearlyZero(angle)==0)
                    return;
            }

            is_identity=false;

            rotate_angle=angle;
            UpdateQuat();
        }

        void SetScale(const float &v)
        {
            if(is_identity)
            {
                if(IsNearlyEqual(v,1.0f))
                    return;
            }
            is_identity=false;

            scale_vector=Vector3f(v,v,v);
            matrix_dirty=true;
        }

        void SetScale(const float x,const float y,const float z)
        {
            if(is_identity)
            {
                if(IsNearlyEqual(x,1.0f)
                 &&IsNearlyEqual(y,1.0f)
                 &&IsNearlyEqual(z,1.0f))
                    return;
            }

            is_identity=false;

            scale_vector.x=x;
            scale_vector.y=y;
            scale_vector.z=z;

            matrix_dirty=true;
        }

        void SetScale(const Vector3f &v)
        {
            if(is_identity)
            {
                if(IsNearlyEqual(v.x,1.0f)
                 &&IsNearlyEqual(v.y,1.0f)
                 &&IsNearlyEqual(v.z,1.0f))
                    return;
            }

            is_identity=false;

            scale_vector=v;
            matrix_dirty=true;
        }

    public:

        Transform()
        {
            SetToIdentity();
            version=0;
        }

        Transform(const Matrix4f &m)
        {
            SetFromMatrix4f(m);
            version=0;
        }

        Transform(const Transform &t)
        {
            hgl_cpy(*this,t);
            version=0;
        }

        void SetToIdentity();

        const bool operator == (const Transform &t);

        void operator = (const Transform &t)
        {
            if(operator==(t))
                return;

            uint32 old_version=version;

            hgl_cpy(*this,t);

            version=++old_version;
            UpdateMatrix();
        }

        void SetFromMatrix4f(const Matrix4f &m);

        const bool IsIdentity()const{return is_identity;}

        const bool IsLastVersion()const{return !matrix_dirty;}

        inline Vector3f TransformPosition   (const Vector3f &v){UpdateMatrix();return Vector3f(matrix*Vector4f(v,1.0f));}
        inline Vector3f TransformDirection  (const Vector3f &v){UpdateMatrix();return Vector3f(matrix*Vector4f(v,0.0f));}
        inline Vector3f TransformNormal     (const Vector3f &v){UpdateMatrix();return normalize(Vector3f(transpose_inverse_matrix*Vector4f(v,0.0f)));}
        inline Matrix3f TransformMatrix     (const Matrix3f &child){UpdateMatrix();return Matrix3f(matrix*Matrix4f(child));}
        inline Matrix4f TransformMatrix     (const Matrix4f &child){UpdateMatrix();return matrix*child;}

        inline Vector3f InverseTransformPosition    (const Vector3f &v){UpdateMatrix();return Vector3f(inverse_matrix*Vector4f(v,1.0f));}
        inline Vector3f InverseTransformDirection   (const Vector3f &v){UpdateMatrix();return Vector3f(inverse_matrix*Vector4f(v,0.0f));}
        inline Vector3f InverseTransformNormal      (const Vector3f &v){UpdateMatrix();return normalize(Vector3f(transpose_inverse_matrix*Vector4f(v,0.0f)));}
        inline Matrix3f InverseTransformMatrix      (const Matrix3f &child){UpdateMatrix();return Matrix3f(inverse_matrix*Matrix4f(child));}
        inline Matrix4f InverseTransformMatrix      (const Matrix4f &child){UpdateMatrix();return inverse_matrix*child;}

        inline Transform TransformTransform(const Transform &child)
        {
            UpdateMatrix();
            const Matrix4f &child_matrix=child.GetMatrix();

            return Transform(matrix*child_matrix);
        }

        inline Transform TransformTransform(const Transform &child)const
        {
            const Matrix4f &cur_matrix=GetMatrix();
            const Matrix4f &child_matrix=child.GetMatrix();

            return Transform(cur_matrix*child_matrix);
        }
    };//Transform

    const Transform IdentityTransform;

    constexpr const size_t TransformMatrix4fLength=sizeof(Transform);

    Transform Lerp(const Transform &from,const Transform &to,const float t);
}//namespace hgl
