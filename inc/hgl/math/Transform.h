#pragma once
#include<hgl/type/TypeInfo.h>
#include<hgl/math/Matrix.h>
#include<hgl/type/ObjectList.h>
#include<hgl/type/VersionData.h>

namespace hgl
{
    /**
    * 变换基类
    */
    class TransformBase:public VersionData<Matrix4f>
    {
    protected:

        Vector3f WorldPosition;
        Vector3f WorldNormal;

    protected:

        virtual void MakeNewestData(Matrix4f &)=0;

    public:

        TransformBase():VersionData(Identity4f), WorldPosition(ZeroVector3f), WorldNormal(ZeroVector3f){}
        TransformBase(const TransformBase * const tb):VersionData(tb)
        {
            if(tb)
            {
                WorldPosition=tb->WorldPosition;
                WorldNormal=tb->WorldNormal;
            }
            else
            {
                WorldPosition=ZeroVector3f;
                WorldNormal=ZeroVector3f;
            }
        }
        virtual ~TransformBase()=default;

        virtual constexpr const size_t GetTypeHash()const=0;                ///<取得类型哈希值

        const uint32 GetMatrix(Matrix4f &mat,const Vector3f &wp,const Vector3f &wn)            ///<取得当前矩阵，并返回当前矩阵版本号
        {
            WorldPosition=wp;
            WorldNormal=wn;

            return GetNewestVersionData(mat);
        }

        virtual TransformBase *CloneSelf()const=0;                     ///<创建一个自身的复制

        virtual bool Update(){return false;}
    };//class TransformBase

    class TransformMatrix :public TransformBase
    {
        Matrix4f transform_matrix;

    protected:

        void MakeNewestData(Matrix4f &mat) override
        {
            mat=transform_matrix;
        }

    public:

        constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformMatrix>(); }

    public:

        TransformMatrix():TransformBase()
        {
            transform_matrix=Identity4f;
            UpdateVersion();
        }

        TransformMatrix(const Matrix4f &mat):TransformBase()
        {
            transform_matrix=mat;
            UpdateVersion();
        }

        TransformMatrix(const TransformMatrix *tm):TransformBase(tm)
        {
            transform_matrix=tm->transform_matrix;
            UpdateVersion();
        }

        TransformBase *CloneSelf()const override
        {
            return(new TransformMatrix(this));
        }

        const Matrix4f &GetTransformMatrix()const { return transform_matrix; }

        void SetTransformMatrix(const Matrix4f &mat)
        {
            if (IsNearlyEqual(transform_matrix, mat))
                return;

            transform_matrix=mat;
            UpdateVersion();
        }
    };//class TransformMatrix

    class TransformTranslate3f :public TransformBase
    {
        Vector3f offset;

    protected:

        void MakeNewestData(Matrix4f &mat) override
        {
            mat=TranslateMatrix(offset);
        }

    public:

        constexpr const size_t GetTypeHash()const override{return hgl::GetTypeHash<TransformTranslate3f>();}

    public:

        TransformTranslate3f():TransformBase()
        {
            offset=ZeroVector3f;
            UpdateVersion();
        }

        TransformTranslate3f(const Vector3f &o):TransformBase()
        {
            offset=o;
            UpdateVersion();
        }

        TransformTranslate3f(const TransformTranslate3f *tt):TransformBase(tt)
        {
            offset=tt->offset;
            UpdateVersion();
        }

        TransformBase *CloneSelf()const override
        {
            return(new TransformTranslate3f(this));
        }

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
    };//class TransformTranslate3f

    using TransformMove3f=TransformTranslate3f;

    class TransformRotateQuat :public TransformBase
    {
        Quatf quat;

    protected:

        void MakeNewestData(Matrix4f &mat) override
        {
            mat=ToMatrix(quat);
        }

    public:

        constexpr const size_t GetTypeHash()const override{return hgl::GetTypeHash<TransformRotateQuat>();}

    public:

        TransformRotateQuat():TransformBase()
        {
            quat=IdentityQuatf;
            UpdateVersion();
        }

        TransformRotateQuat(const Quatf &q):TransformBase()
        {
            quat=q;
            UpdateVersion();
        }

        TransformRotateQuat(const TransformRotateQuat *trq):TransformBase(trq)
        {
            quat=trq->quat;
            UpdateVersion();
        }

        TransformBase *CloneSelf()const override
        {
            return(new TransformRotateQuat(quat));
        }

        const Quatf &GetQuat()const { return quat; }
        void SetQuat(const Quatf &q)
        {
            if(IsNearlyEqual(quat,q))
                return;

            quat=q;
            UpdateVersion();
        }
    };//class TransformRotateQuat

    class TransformRotateAxis :public TransformBase
    {
        Vector3f axis;
        float angle;

    protected:

        void MakeNewestData(Matrix4f &mat) override
        {
            mat=AxisRotate(angle,axis);
        }

    public:

        constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformRotateAxis>(); }

    public:

        TransformRotateAxis():TransformBase()
        {
            axis=AxisVector::Z; // default to unit Z axis to avoid using zero vector
            angle=0;
            UpdateVersion();
        }

        TransformRotateAxis(const Vector3f &a,float ang):TransformBase()
        {
            axis=a;
            angle=ang;
            UpdateVersion();
        }

        TransformRotateAxis(const TransformRotateAxis *tra):TransformBase(tra)
        {
            axis=tra->axis;
            angle=tra->angle;
            UpdateVersion();
        }

        TransformBase *CloneSelf()const override
        {
            return(new TransformRotateAxis(axis,angle));
        }

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
    };//class TransformRotateAxis

    class TransformRotateEuler :public TransformBase
    {
        Vector3f euler;

    protected:

        void MakeNewestData(Matrix4f &mat) override
        {
            mat=glm::eulerAngleXYZ(euler.x,euler.y,euler.z);
        }

    public:

        constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformRotateEuler>(); }

    public:

        TransformRotateEuler():TransformBase()
        {
            euler=ZeroVector3f;
            UpdateVersion();
        }

        TransformRotateEuler(const Vector3f &e):TransformBase()
        {
            euler=e;
            UpdateVersion();
        }

        TransformRotateEuler(const TransformRotateEuler *tre):TransformBase(tre)
        {
            euler=tre->euler;
            UpdateVersion();
        }

        TransformBase *CloneSelf()const override
        {
            return(new TransformRotateEuler(euler));
        }

        const Vector3f &GetEuler()const { return euler; }

        const float     GetPitch()const { return euler.x; }
        const float     GetYaw  ()const { return euler.y; }
        const float     GetRoll ()const { return euler.z; }

        void SetEuler   (const Vector3f &e  ){if(IsNearlyEqual(euler,  e))return;euler  =e;UpdateVersion();}
        void SetPitch   (const float p      ){if(IsNearlyEqual(euler.x,p))return;euler.x=p;UpdateVersion();}
        void SetYaw     (const float y      ){if(IsNearlyEqual(euler.y,y))return;euler.y=y;UpdateVersion();}
        void SetRoll    (const float r      ){if(IsNearlyEqual(euler.z,r))return;euler.z=r;UpdateVersion();}
    };//class TransformRotateEuler

    class TransformScale3f :public TransformBase
    {
        Vector3f scale3f;

    protected:

        void MakeNewestData(Matrix4f &mat) override
        {
            mat=ScaleMatrix(scale3f);
        }

    public:

        constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformScale3f>(); }

    public:

        TransformScale3f():TransformBase()
        {
            scale3f=Vector3f(1,1,1);
            UpdateVersion();
        }

        TransformScale3f(const Vector3f &s):TransformBase()
        {
            scale3f=s;
            UpdateVersion();
        }

        TransformScale3f(const TransformScale3f *ts):TransformBase(ts)
        {
            scale3f=ts->scale3f;
            UpdateVersion();
        }

        TransformBase *CloneSelf()const override
        {
            return(new TransformScale3f(scale3f));
        }

        const Vector3f &GetScale()const { return scale3f; }
        void SetScale(const Vector3f &s)
        {
            if (IsNearlyEqual(scale3f,s))
                return;

            scale3f=s;
            UpdateVersion();
        }
    };//class TransformScale

    class TransformLookAt :public TransformBase
    {
        Vector3f eye;
        Vector3f center;
        Vector3f up;

    protected:

        void MakeNewestData(Matrix4f &mat) override
        {
            mat=LookAtMatrix(eye,center,up);
        }

    public:

        constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformLookAt>(); }

    public:

        TransformLookAt():TransformBase()
        {
            eye     =OneVector3f;
            center  =ZeroVector3f;
            up      =AxisVector::Z;
            UpdateVersion();
        }

        TransformLookAt(const Vector3f &e,const Vector3f &c,const Vector3f &u=AxisVector::Z)
        {
            eye     =e;
            center  =c;
            up      =u;
            UpdateVersion();
        }

        TransformLookAt(const TransformLookAt *tla):TransformBase(tla)
        {
            eye     =tla->eye;
            center  =tla->center;
            up      =tla->up;
            UpdateVersion();
        }

        TransformBase *CloneSelf()const override
        {
            return(new TransformLookAt(eye,center,up));
        }

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
    };//class TransformLookAt

    class TransformManager: public TransformBase
    {
        ObjectList<TransformBase> transform_list;

    protected:

        virtual void MakeNewestData(Matrix4f &mat) override
        {
            mat=Identity4f;

            if (transform_list.IsEmpty())
                return;
            
            Matrix4f TempMatrix;

            for(TransformBase *tb:transform_list)
            {
                tb->GetMatrix(TempMatrix,WorldPosition,WorldNormal);

                mat*=TempMatrix;
            }
        }
        
    public:

        virtual constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformManager>(); }

        const bool IsEmpty()const { return transform_list.IsEmpty(); }

    public:

        TransformManager()=default;
        virtual ~TransformManager()=default;

        TransformManager(const TransformManager *tm):TransformBase(tm)
        {
            for(const TransformBase *tb:tm->transform_list)
                AddTransform(tb->CloneSelf());

            UpdateVersion();
        }

        TransformBase *CloneSelf()const override
        {
            TransformManager *tm=new TransformManager;

            for(TransformBase *tb:transform_list)
                tm->AddTransform(tb->CloneSelf());

            return tm;
        }

        void operator = (const TransformManager &tm)
        {
            Clear();

            for(TransformBase *tb:tm.transform_list)
                AddTransform(tb->CloneSelf());
        }

        void Clear()
        {
            transform_list.Clear();
            UpdateVersion();
        }

        void AddTransform(TransformBase *tb)
        {
            transform_list.Add(tb);
            UpdateVersion();
        }

        TransformTranslate3f *AddTranslate(const Vector3f &v)
        {
            TransformTranslate3f *tt=new TransformTranslate3f(v);

            AddTransform(tt);

            return tt;
        }

        TransformMove3f *AddMove(const Vector3f &v){return AddTranslate(v); }

        TransformRotateAxis *AddRotateAxis(const Vector3f &axis,const float angle)
        {
            TransformRotateAxis *tra=new TransformRotateAxis(axis,angle);

            AddTransform(tra);

            return tra;
        }

        TransformRotateQuat *AddRotateQuat(const Quatf &q)
        {
            TransformRotateQuat *trq=new TransformRotateQuat(q);

            AddTransform(trq);

            return trq;
        }

        TransformRotateEuler *AddRotateEuler(const Vector3f &euler)
        {
            TransformRotateEuler *tre=new TransformRotateEuler(euler);

            AddTransform(tre);

            return tre;
        }

        TransformScale3f *AddScale(const Vector3f &v)
        {
            TransformScale3f *ts=new TransformScale3f(v);

            AddTransform(ts);

            return ts;
        }

        TransformLookAt *AddLookAt(const Vector3f &eye,const Vector3f &center,const Vector3f &up)
        {
            TransformLookAt *tla=new TransformLookAt;

            tla->SetLookAt(eye,center,up);

            AddTransform(tla);

            return tla;
        }

        TransformMatrix *AddMatrix(const Matrix4f &mat)
        {
            TransformMatrix *tm=new TransformMatrix(mat);

            AddTransform(tm);

            return tm;
        }

        void RemoveTransform(TransformBase *tb)
        {
            if(!tb)
                return;

            const int pos=transform_list.Find(tb);
            
            if(pos<0)
                return;

            transform_list.DeleteShift(pos);     //使用DeleteShift是为了保持顺序(Delete可能会拿最后一个数据移到前面，而本类需要保持顺序)

            UpdateVersion();
        }

        virtual bool Update() override
        {
            bool has_update=false;

            for(TransformBase *tb:transform_list)
                if(tb->UpdateNewestData())
                    has_update=true;

            if(has_update)
            {
                UpdateVersion();
                UpdateNewestData();
            }

            return has_update;
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
                return TranslateMatrix(translation_vector)*ToMatrix(rotation_quat)*ScaleMatrix(scale_vector);
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
                if(IsNearlyEqual(q, IdentityQuatf))
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
                if(IsNearlyZero(angle))
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
                if(IsNearlyZero(angle))
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
                if(IsNearlyZero(angle))
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
