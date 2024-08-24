#pragma once
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

        virtual void MakeNewestData(Matrix4f &)=0;

    public:

        TransformBase():VersionData(Identity4f){}
        TransformBase(const TransformBase *tb):VersionData(tb){}
        virtual ~TransformBase()=default;

        virtual constexpr const size_t GetTypeHash()const=0;                ///<取得类型哈希值

        const uint32 GetMatrix(Matrix4f &mat)                               ///<取得当前矩阵，并返回当前矩阵版本号
        {
            return GetNewestVersionData(mat);
        }

        virtual TransformBase *CreateSelfCopy()const=0;                     ///<创建一个自身的复制
    };//class TransformBase

    class TransformMatrix :public TransformBase
    {
        Matrix4f transform_matrix;

    protected:

        virtual void MakeNewestData(Matrix4f &mat) override
        {
            mat=transform_matrix;
        }

    public:

        virtual constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformMatrix>(); }

    public:

        TransformMatrix():TransformBase()
        {
            transform_matrix=Identity4f;
        }

        TransformMatrix(const Matrix4f &mat):TransformBase()
        {
            transform_matrix=mat;
        }

        TransformMatrix(const TransformMatrix *tm):TransformBase(tm)
        {
            transform_matrix=tm->transform_matrix;
        }

        TransformBase *CreateSelfCopy()const
        {
            return(new TransformMatrix(this));
        }

        const Matrix4f &GetTransformMatrix()const { return transform_matrix; }

        void SetTransformMatrix(const Matrix4f &mat)
        {
            if (transform_matrix==mat)
                return;

            transform_matrix=mat;
            UpdateVersion();
        }
    };//class TransformMatrix

    class TransformTranslate3f :public TransformBase
    {
        Vector3f offset;

    protected:

        virtual void MakeNewestData(Matrix4f &mat) override
        {
            mat=translate(offset);
        }

    public:

        virtual constexpr const size_t GetTypeHash()const override{return hgl::GetTypeHash<TransformTranslate3f>();}

    public:

        TransformTranslate3f():TransformBase()
        {
            offset=ZeroVector3f;
        }

        TransformTranslate3f(const Vector3f &o):TransformBase()
        {
            offset=o;
        }

        TransformTranslate3f(const TransformTranslate3f *tt):TransformBase(tt)
        {
            offset=tt->offset;
        }

        TransformBase *CreateSelfCopy()const
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

        virtual void MakeNewestData(Matrix4f &mat) override
        {
            mat=ToMatrix(quat);
        }

    public:

        virtual constexpr const size_t GetTypeHash()const override{return hgl::GetTypeHash<TransformRotateQuat>();}

    public:

        TransformRotateQuat():TransformBase()
        {
            quat=IdentityQuatf;
        }

        TransformRotateQuat(const Quatf &q):TransformBase()
        {
            quat=q;
        }

        TransformRotateQuat(const TransformRotateQuat *trq):TransformBase(trq)
        {
            quat=trq->quat;
        }

        TransformBase *CreateSelfCopy()const
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

        virtual void MakeNewestData(Matrix4f &mat) override
        {
            mat=rotate(angle,axis);
        }

    public:

        virtual constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformRotateAxis>(); }

    public:

        TransformRotateAxis():TransformBase()
        {
            axis=ZeroVector3f;
            angle=0;
        }

        TransformRotateAxis(const Vector3f &a,float ang):TransformBase()
        {
            axis=a;
            angle=ang;
        }

        TransformRotateAxis(const TransformRotateAxis *tra):TransformBase(tra)
        {
            axis=tra->axis;
            angle=tra->angle;
        }

        TransformBase *CreateSelfCopy()const
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

        virtual void MakeNewestData(Matrix4f &mat) override
        {
            mat=glm::eulerAngleXYZ(euler.x,euler.y,euler.z);
        }

    public:

        virtual constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformRotateEuler>(); }

    public:

        TransformRotateEuler():TransformBase()
        {
            euler=ZeroVector3f;
        }

        TransformRotateEuler(const Vector3f &e):TransformBase()
        {
            euler=e;
        }

        TransformRotateEuler(const TransformRotateEuler *tre):TransformBase(tre)
        {
            euler=tre->euler;
        }

        TransformBase *CreateSelfCopy()const
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

        virtual void MakeNewestData(Matrix4f &mat) override
        {
            mat=scale(scale3f);
        }

    public:

        virtual constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformScale3f>(); }

    public:

        TransformScale3f():TransformBase()
        {
            scale3f=Vector3f(1,1,1);
        }

        TransformScale3f(const Vector3f &s):TransformBase()
        {
            scale3f=s;
        }

        TransformScale3f(const TransformScale3f *ts):TransformBase(ts)
        {
            scale3f=ts->scale3f;
        }

        TransformBase *CreateSelfCopy()const
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

        virtual void MakeNewestData(Matrix4f &mat) override
        {
            mat=lookat(eye,center,up);
        }

    public:

        virtual constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformLookAt>(); }

    public:

        TransformLookAt():TransformBase()
        {
            eye     =OneVector3f;
            center  =ZeroVector3f;
            up      =AxisVector::Z;
        }

        TransformLookAt(const Vector3f &e,const Vector3f &c,const Vector3f &u=AxisVector::Z)
        {
            eye     =e;
            center  =c;
            up      =u;
        }

        TransformLookAt(const TransformLookAt *tla):TransformBase(tla)
        {
            eye     =tla->eye;
            center  =tla->center;
            up      =tla->up;
        }

        TransformBase *CreateSelfCopy()const
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

            for (TransformBase *tb : transform_list)
            {
                tb->GetMatrix(TempMatrix);

                mat*=TempMatrix;
            }
        }
        
    public:

        virtual constexpr const size_t GetTypeHash()const override { return hgl::GetTypeHash<TransformManager>(); }

    public:

        TransformManager()=default;
        virtual ~TransformManager()=default;

        TransformManager(const TransformManager *tm):TransformBase(tm)
        {
            for(TransformBase *tb:tm->transform_list)
                AddTransform(tb->CreateSelfCopy());
        }

        TransformBase *CreateSelfCopy()const
        {
            TransformManager *tm=new TransformManager;

            for(TransformBase *tb:transform_list)
                tm->AddTransform(tb->CreateSelfCopy());

            return tm;
        }

        void operator = (const TransformManager &tm)
        {
            Clear();

            for(TransformBase *tb:tm.transform_list)
                AddTransform(tb->CreateSelfCopy());
        }

        void Clear()
        {
            transform_list.Clear();
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

            transform_list.DeleteMove(pos);     //使用DeleteMove是为了保持顺序(Delete可能会拿最后一个数据移到前面，而本类需要保持顺序)

            UpdateVersion();
        }
    };//class TransformManager
}//namespace hgl
