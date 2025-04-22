#include<hgl/type/ConstStringSet.h>
#include<hgl/io/TextOutputStream.h>
#include<hgl/io/FileOutputStream.h>

namespace hgl
{
    template<typename SC> bool SaveToTextStream(io::TextOutputStream *tos,const ConstStringSet<SC> *css,bool output_id)
    {
        if(!tos||!css)
            return(false);

        if(css->GetCount()<=0)
            return(true);

        if(output_id)
        {
            SC id_str[16];
            const SC gap_str[4]={SC(' '),SC(':'),SC(' '),0};

            const ArrayList<ConstStringView<SC>> &csv_list=css->GetConstStringList();

            for(auto &csv:csv_list)
            {
                utos<SC>(id_str,sizeof(id_str),csv.id);

                tos->WriteChars(id_str,hgl::strlen(id_str));
                tos->WriteChars(gap_str,3);
                tos->WriteChars(csv.GetString(),csv.length);
                tos->WriteLineEnd();
            }

            return(true);
        }
        else
        {
            const DataArray<SC> &data_array=css->GetStringData();

            int length=data_array.GetCount();

            AutoDeleteArray<SC> text(length);

            hgl_cpy<SC>(text,data_array.data(),length);

            for(int i=0;i<length;i++)
            {
                if(text[i]==0)
                    text[i]=SC('\n');
            }

            tos->WriteChars(text,length);
            return(true);
        }
    }

    template<typename SC> bool SaveToTextFile(const OSString &filename,const ConstStringSet<SC> *css,bool output_id,bool output_bom)
    {
        if(!css)
            return(false);

        if(filename.IsEmpty())
            return(false);

        if(css->GetCount()<=0)
            return(true);

        io::OpenFileOutputStream fos(filename,io::FileOpenMode::CreateTrunc);

        if(!fos)
            return(false);

        AutoDelete<io::TextOutputStream> tos=io::CreateTextOutputStream<SC>(&fos);

        if(output_bom)
            tos->WriteBOM();

        return SaveToTextStream<SC>(tos,css,output_id);
    }

    //下面这些定义是为了实例化上面的函数，所以不要删掉

    //bool SaveToAnsiTextStream(hgl::io::TextOutputStream *tos,const ConstAnsiStringSet *css,bool output_id){return SaveToTextStream(tos,css,output_id);}
    bool SaveToWideTextStream(hgl::io::TextOutputStream *tos,const ConstWStringSet *css,bool output_id){return SaveToTextStream(tos,css,output_id);}
    bool SaveToUTF8TextStream(hgl::io::TextOutputStream *tos,const ConstU8StringSet *css,bool output_id){return SaveToTextStream(tos,css,output_id);}
    bool SaveToUTF16TextStream(hgl::io::TextOutputStream *tos,const ConstU16StringSet *css,bool output_id){return SaveToTextStream(tos,css,output_id);}

    //bool SaveToAnsiTextFile(const OSString &filename,const ConstAnsiStringSet *css,bool output_id,bool output_bom){return SaveToTextFile(filename,css,output_id,output_bom);}
    bool SaveToWideTextFile(const OSString &filename,const ConstWStringSet *css,bool output_id,bool output_bom){return SaveToTextFile(filename,css,output_id,output_bom);}
    bool SaveToUTF8TextFile(const OSString &filename,const ConstU8StringSet *css,bool output_id,bool output_bom){return SaveToTextFile(filename,css,output_id,output_bom);}
    bool SaveToUTF16TextFile(const OSString &filename,const ConstU16StringSet *css,bool output_id,bool output_bom){return SaveToTextFile(filename,css,output_id,output_bom);}
}//namespace hgl
